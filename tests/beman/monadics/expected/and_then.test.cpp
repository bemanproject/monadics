// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("with-void-value-return-non-void-with-value") {
    constexpr auto result = std::expected<void, int>{} | and_then([]() { return std::expected<double, int>{2.0}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<double, int>>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == 2.0);
}

TEST_CASE("with-void-value-return-non-void-with-error") {
    constexpr auto result =
        std::expected<void, int>{} | and_then([]() { return std::expected<double, int>{std::unexpected{2}}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<double, int>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 2);
}

TEST_CASE("with-void-value-return-void-with-value") {
    constexpr auto result = std::expected<void, int>{} | and_then([]() -> std::expected<void, int> { return {}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<void, int>>);
    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("with-void-value-return-void-with-error") {
    constexpr auto result =
        std::expected<void, int>{} | and_then([]() -> std::expected<void, int> { return {std::unexpected{5}}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<void, int>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 5);
}

TEST_CASE("with-value-return-non-void-with-value") {
    constexpr auto result =
        std::expected<int, double>{10}
        | and_then([](auto) -> std::expected<std::string_view, double> { return std::string_view{"some"}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<std::string_view, double>>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == "some");
}

TEST_CASE("with-value-return-non-void-with-error") {
    constexpr auto result =
        std::expected<int, double>{10}
        | and_then([](auto&& v) -> std::expected<std::string_view, double> { return {std::unexpected{v * 2.0}}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<std::string_view, double>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 20.0);
}

TEST_CASE("with-value-return-void-with-value") {
    constexpr auto result =
        std::expected<int, double>{10} | and_then([](auto) -> std::expected<void, double> { return {}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<void, double>>);
    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("with-value-return-void-with-error") {
    constexpr auto result = std::expected<int, double>{10}
                          | and_then([](int&&) -> std::expected<void, double> { return {std::unexpected{5.0}}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<void, double>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 5.0);
}

TEST_CASE("same-type-value-and-error-propagates-value") {
    using E = std::expected<int, int>;
    constexpr auto result = E{5} | and_then([](int v) { return E{v * 2}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const E>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == 10);
}

TEST_CASE("same-type-value-and-error-propagates-error") {
    using E = std::expected<int, int>;
    constexpr auto result = E{std::unexpected{5}} | and_then([](int v) { return E{v * 2}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const E>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 5);
}

struct ChangedError {};

TEMPLATE_TEST_CASE_SIG(
    "keep-value-category",
    "",
    ((typename Box, auto Fn, bool Expected), Box, Fn, Expected),
    (
        std::expected<int, double>&,
        [](int&) { return std::expected<int, double>{10}; },
        true
    ),
    (
        std::expected<int, double>&,
        [](int&&) { return std::expected<int, double>{10}; },
        false
    ),
    (
        std::expected<int, double>&&,
        [](int&&) { return std::expected<int, double>{10}; },
        true
    ),
    (
        std::expected<int, double>&&,
        [](int&&) { return std::expected<int, ChangedError>{10}; },
        false
    ),
    (
        std::expected<int, double>&&,
        [](int&&) { return std::expected<int, float>{10}; },
        false
    ),
    (
        std::expected<int, double>&&,
        [](int&) { return std::expected<int, double>{10}; },
        false
    ),
    (
        const std::expected<int, double>&,
        [](const int&) { return std::expected<int, double>{10}; },
        true
    ),
    (const std::expected<int, double>&, [](int&) { return std::expected<int, double>{10}; }, false)
) {
    STATIC_REQUIRE(and_thenable<Box, decltype(Fn)> == Expected);
}

} // namespace beman::monadics::tests
