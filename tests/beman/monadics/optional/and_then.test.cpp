// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include "beman/monadics/detail/and_then.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include <helpers/move_only.hpp>

namespace beman::monadics::tests {

TEST_CASE("with-value") {
    constexpr auto result = std::optional{10} | and_then([](auto&& value) {
                                return std::optional{std::forward<decltype(value)>(value) * 2.0};
                            });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::optional<double>>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == 20.0);
}

TEST_CASE("with-nullopt") {
    constexpr auto result = std::optional<int>{} | and_then([](auto&& value) {
                                return std::optional{std::forward<decltype(value)>(value) * 2.0};
                            });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::optional<double>>);
    STATIC_REQUIRE(result.has_value() == false);
}

TEST_CASE("with-value-variadic") {
    constexpr auto result = std::optional<int>{10} | and_then([](auto&&... v) { return std::optional{sizeof...(v)}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::optional<std::size_t>>);
    STATIC_REQUIRE(result.has_value() == true);
    STATIC_REQUIRE(result.value() == 1);
}

TEMPLATE_TEST_CASE_SIG(
    "keep-value-category",
    "",
    ((typename Box, auto Fn, bool Expected), Box, Fn, Expected),
    (
        std::optional<int>&,
        [](int&) { return std::optional<int>{}; },
        true),
    (
        std::optional<int>&,
        [](int&&) { return std::optional<int>{}; },
        false),
    (
        std::optional<int>&&,
        [](int&&) { return std::optional<int>{}; },
        true),
    (
        std::optional<int>&&,
        [](int&) { return std::optional<int>{}; },
        false),
    (
        const std::optional<int>&,
        [](const int&) { return std::optional<int>{}; },
        true),
    (
        const std::optional<int>&,
        [](int&) { return std::optional<int>{}; },
        false),
    (std::optional<helpers::MoveOnly>&, [](helpers::MoveOnly) { return std::optional<helpers::MoveOnly>{}; }, false)) {
    STATIC_REQUIRE(and_thenable<Box, decltype(Fn)> == Expected);
}

} // namespace beman::monadics::tests
