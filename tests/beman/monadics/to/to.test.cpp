// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/monadics.hpp>

#include <catch2/catch_test_macros.hpp>

#include <expected>
#include <optional>

namespace beman::monadics {

template<typename T>
struct box_traits<std::optional<T>> {
    using error_type = unit;
};

template<typename T, typename E>
struct box_traits<std::expected<T, E>> {
    [[nodiscard]] static constexpr auto make_error(auto&& e) noexcept {
        return std::expected<T, E>{std::unexpect, std::forward<decltype(e)>(e)};
    }
};

} // namespace beman::monadics

namespace beman::monadics::tests {

namespace {

template<typename From, typename To>
concept converts = requires(From from) { from | to<To>(); };

template<typename From, typename To, typename Err>
concept converts_with_fallback = requires(From from, Err err) { from | to<To>(err); };

} // namespace

TEST_CASE("widening-needs-an-error-to-widen-into") {
    constexpr auto engaged = std::optional{15} | to<std::expected<int, int>>(20);
    constexpr auto empty = std::optional<int>{} | to<std::expected<int, int>>(20);

    STATIC_REQUIRE(engaged.value() == 15);
    STATIC_REQUIRE(empty.error() == 20);
}

TEST_CASE("the-destination-can-be-deduced") {
    constexpr auto engaged = std::optional{15} | to<std::expected>(20);

    STATIC_REQUIRE(std::same_as<decltype(engaged), const std::expected<int, int>>);
    STATIC_REQUIRE(engaged.value() == 15);
}

TEST_CASE("narrowing-drops-the-error") {
    constexpr auto engaged = std::expected<int, int>{15} | to<std::optional<int>>();
    constexpr auto failed = std::expected<int, int>{std::unexpect, 7} | to<std::optional<int>>();

    STATIC_REQUIRE(engaged.value() == 15);
    STATIC_REQUIRE_FALSE(failed.has_value());
}

TEST_CASE("a-carrier-crosses-into-a-family-without-a-fallback") {
    constexpr auto widened = just(15) | to<std::expected<int, int>>();

    STATIC_REQUIRE(std::same_as<decltype(widened), const std::expected<int, int>>);
    STATIC_REQUIRE(widened.value() == 15);
    STATIC_REQUIRE(converts<just_t<int>, std::expected<int, int>>);
}

TEST_CASE("widening-without-a-fallback-is-rejected") {
    STATIC_REQUIRE(converts_with_fallback<std::optional<int>, std::expected<int, int>, int>);
    STATIC_REQUIRE_FALSE(converts<std::optional<int>, std::expected<int, int>>);
}

TEST_CASE("a-fallback-is-rejected-where-an-error-already-exists") {
    STATIC_REQUIRE(converts<std::expected<int, int>, std::optional<int>>);
    STATIC_REQUIRE_FALSE(converts_with_fallback<std::expected<int, int>, std::optional<int>, int>);
}

} // namespace beman::monadics::tests
