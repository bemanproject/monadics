// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/just.hpp>
#include <beman/monadics/monadics.hpp>

#include <catch2/catch_test_macros.hpp>

#include <concepts>
#include <optional>
#include <utility>

template<typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    using error_type = unit;
};

namespace beman::monadics::detail::tests {

namespace {

using J = just_t<int>;

} // namespace

TEST_CASE("is-a-box-holding-only-a-value") {
    STATIC_REQUIRE(box<J>);

    STATIC_REQUIRE(value_only<J>);
    STATIC_REQUIRE(single_case<J>);
    STATIC_REQUIRE_FALSE(error_only<J>);
    STATIC_REQUIRE_FALSE(single_case<std::optional<int>>);
}

TEST_CASE("opting-in-is-naming-never-on-the-error-side") {
    STATIC_REQUIRE(std::same_as<get_box_traits<J>::error_type, never>);
    STATIC_REQUIRE(std::same_as<get_box_traits<J>::rebind<double>, just_t<double>>);
    STATIC_REQUIRE(std::same_as<get_box_traits<J>::rebind_error<never>, J>);
}

TEST_CASE("answers-for-itself-without-the-traits") {
    constexpr J j{4};
    STATIC_REQUIRE(j.has_value());
    STATIC_REQUIRE(j.value() == 4);
    STATIC_REQUIRE(just_t<void>{}.has_value());
}

TEST_CASE("always-holds-its-value") {
    STATIC_REQUIRE(get_box_traits<J>::has_value(J{4}));
    STATIC_REQUIRE(get_box_traits<J>::value(J{4}) == 4);
    STATIC_REQUIRE(just(4) == J{4});
    STATIC_REQUIRE(J{4}.value() == 4);
}

TEST_CASE("carrying-an-lvalue-refers-to-it") {
    int v = 5;
    auto carried = just(v);
    STATIC_REQUIRE(std::same_as<decltype(carried), just_t<int&>>);
    REQUIRE(&carried.value() == &v);
}

TEST_CASE("reading-refers-to-the-value-it-holds") {
    STATIC_REQUIRE(std::same_as<decltype(std::declval<J&>().value()), int&>);
    STATIC_REQUIRE(std::same_as<decltype(std::declval<const J&>().value()), const int&>);
    STATIC_REQUIRE(std::same_as<decltype(std::declval<J&&>().value()), int&&>);
    STATIC_REQUIRE(std::same_as<decltype(std::declval<const J&&>().value()), const int&&>);
}

TEST_CASE("a-value-that-carries-nothing") {
    STATIC_REQUIRE(box<just_t<void>>);
    STATIC_REQUIRE(value_only<just_t<void>>);
    STATIC_REQUIRE(std::is_void_v<get_box_traits<just_t<void>>::value_type>);
    STATIC_REQUIRE(just() == just_t<void>{});
}

TEST_CASE("is-the-identity-monad") {
    STATIC_REQUIRE((J{4} | transform([](int v) { return v * 2; })) == J{8});
    STATIC_REQUIRE((J{4} | and_then([](int v) { return just_t<double>{v / 2.0}; })) == just_t<double>{2.0});
}

TEST_CASE("has-nothing-to-recover-from") {
    STATIC_REQUIRE_FALSE(or_elseable<J, decltype([] { return J{0}; })>);
    STATIC_REQUIRE_FALSE(transform_errorable<J, decltype([](auto&&) { return 1; })>);
    STATIC_REQUIRE_FALSE(has_error_channel<J>);
}

TEST_CASE("is-its-own-family") {
    STATIC_REQUIRE_FALSE(and_thenable<J, decltype([](int v) { return std::optional{v}; })>);
}

TEST_CASE("crosses-into-a-family-through-to") {
    STATIC_REQUIRE(std::same_as<decltype(J{4} | to<std::optional<int>>()), std::optional<int>>);
    STATIC_REQUIRE((J{4} | to<std::optional<int>>()) == std::optional{4});
}

} // namespace beman::monadics::detail::tests
