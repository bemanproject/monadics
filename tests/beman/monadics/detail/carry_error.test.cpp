// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/carry_error.hpp>
#include <beman/monadics/monadics.hpp>

#include <catch2/catch_test_macros.hpp>

#include <concepts>
#include <optional>

template<typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    using error_type = unit;
};

namespace beman::monadics::detail::tests {

namespace {

template<typename R>
concept carries_into = requires { carry_error.operator()<R>(unit{}); };

} // namespace

TEST_CASE("settles-the-error-into-the-result-it-is-told") {
    STATIC_REQUIRE(carry_error.operator()<std::optional<int>>(unit{}) == std::nullopt);
    STATIC_REQUIRE(carry_error.operator()<just_error_t<int>>(7) == just_error_t<int>{7});
}

TEST_CASE("names-no-result-of-its-own") {
    STATIC_REQUIRE_FALSE(std::invocable<carry_error_t, unit>);
}

TEST_CASE("refuses-a-result-that-is-not-a-box") {
    STATIC_REQUIRE(carries_into<std::optional<int>>);
    STATIC_REQUIRE_FALSE(carries_into<int>);
}

TEST_CASE("is-the-pass-through-handler-of-and_then") {
    STATIC_REQUIRE((std::optional<int>{} | and_then([](int v) { return std::optional{v * 2}; })) == std::nullopt);
    STATIC_REQUIRE(either([](int v) { return std::optional{v * 2}; }, carry_error, std::optional<int>{})
                   == std::nullopt);
}

} // namespace beman::monadics::detail::tests
