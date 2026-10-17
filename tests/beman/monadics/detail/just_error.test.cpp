// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/just_error.hpp>
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

using E = just_error_t<int>;

template<typename Box, typename V>
concept rebindable_to = requires { typename get_box_traits<Box>::template rebind<V>; };

} // namespace

TEST_CASE("is-a-box-holding-only-an-error") {
    STATIC_REQUIRE(box<E>);
    STATIC_REQUIRE(error_only<E>);
    STATIC_REQUIRE(single_case<E>);
    STATIC_REQUIRE_FALSE(value_only<E>);
}

TEST_CASE("opting-in-is-naming-never-on-the-value-side") {
    STATIC_REQUIRE(std::same_as<get_box_traits<E>::value_type, never>);
    STATIC_REQUIRE(std::same_as<get_box_traits<E>::rebind_error<char>, just_error_t<char>>);
}

TEST_CASE("answers-for-itself-without-the-traits") {
    constexpr E e{7};
    STATIC_REQUIRE_FALSE(e.has_value());
    STATIC_REQUIRE(e.error() == 7);
}

TEST_CASE("never-holds-a-value") {
    STATIC_REQUIRE_FALSE(get_box_traits<E>::has_value(E{7}));
    STATIC_REQUIRE(get_box_traits<E>::error(E{7}) == 7);
    STATIC_REQUIRE(just_error(7) == E{7});
    STATIC_REQUIRE(E{7}.error() == 7);
}

TEST_CASE("carrying-an-lvalue-refers-to-it") {
    int e = 5;
    auto carried = just_error(e);
    STATIC_REQUIRE(std::same_as<decltype(carried), just_error_t<int&>>);
    REQUIRE(&carried.error() == &e);
}

TEST_CASE("reading-refers-to-the-error-it-holds") {
    STATIC_REQUIRE(std::same_as<decltype(std::declval<E&>().error()), int&>);
    STATIC_REQUIRE(std::same_as<decltype(std::declval<const E&>().error()), const int&>);
    STATIC_REQUIRE(std::same_as<decltype(std::declval<E&&>().error()), int&&>);
    STATIC_REQUIRE(std::same_as<decltype(std::declval<const E&&>().error()), const int&&>);
}

TEST_CASE("cannot-claim-a-value-it-has-no-room-for") {
    STATIC_REQUIRE(rebindable_to<E, never>);
    STATIC_REQUIRE(std::same_as<get_box_traits<E>::rebind<never>, E>);
    STATIC_REQUIRE_FALSE(rebindable_to<E, int>);
}

} // namespace beman::monadics::detail::tests
