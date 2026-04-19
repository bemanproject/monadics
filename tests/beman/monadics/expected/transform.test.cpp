// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("void-value-return-non-void-value") {
    constexpr auto result = std::expected<void, int>{} | transform([]() { return 2.0; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<double, int>>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == 2.0);
}

TEST_CASE("void-value-return-void-value") {
    constexpr auto result = std::expected<void, int>{} | transform([]() { return; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<void, int>>);
    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("void-value-with-error-not-called") {
    constexpr auto result = std::expected<void, int>{std::unexpected{3}} | transform([]() { return 2.0; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<double, int>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 3);
}

TEST_CASE("value-return-different-value") {
    constexpr auto result = std::expected<int, double>{10} | transform([](auto) { return std::string_view{"some"}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<std::string_view, double>>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == "some");
}

TEST_CASE("value-return-void-value") {
    constexpr auto result = std::expected<int, double>{10} | transform([](auto) { return; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<void, double>>);
    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("value-with-error-not-called") {
    constexpr auto result =
        std::expected<int, double>{std::unexpected{3.0}} | transform([](auto) { return std::string_view{"some"}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<std::string_view, double>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 3.0);
}

TEST_CASE("same-type-value-and-error-with-value") {
    using E = std::expected<int, int>;
    constexpr auto result = E{5} | transform([](int v) { return v * 2; });
    STATIC_REQUIRE(std::same_as<decltype(result), const E>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == 10);
}

TEST_CASE("same-type-value-and-error-with-error") {
    using E = std::expected<int, int>;
    constexpr auto result = E{std::unexpected{5}} | transform([](int v) { return v * 2; });
    STATIC_REQUIRE(std::same_as<decltype(result), const E>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 5);
}

TEMPLATE_TEST_CASE_SIG("keep-value-category",
                       "",
                       ((typename Box, auto Fn, bool Expected), Box, Fn, Expected),
                       (
                           std::expected<int, double>&,
                           [](int&) { return 0; },
                           true
                       ),
                       (
                           std::expected<int, double>&,
                           [](int&&) { return 0; },
                           false
                       ),
                       (
                           std::expected<int, double>&&,
                           [](int&&) { return 0; },
                           true
                       ),
                       (
                           std::expected<int, double>&&,
                           [](int&) { return 0; },
                           false
                       ),
                       (
                           const std::expected<int, double>&,
                           [](const int&) { return 0; },
                           true
                       ),
                       (const std::expected<int, double>&, [](int&) { return 0; }, false)) {
    STATIC_REQUIRE(transformable<Box, decltype(Fn)> == Expected);
}

} // namespace beman::monadics::tests
