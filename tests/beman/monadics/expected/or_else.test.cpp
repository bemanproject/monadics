// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("void-value-return-same-error-has-value") {
    constexpr auto result =
        std::expected<void, int>{} | or_else([](auto e) { return std::expected<void, int>{std::unexpected{e * 2}}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<void, int>>);
    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("void-value-return-another-error-has-value") {
    constexpr auto result = std::expected<void, int>{}
                          | or_else([](auto e) { return std::expected<void, double>{std::unexpected{e * 2.0}}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<void, double>>);
    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("void-value-return-same-error-has-error") {
    constexpr auto result = std::expected<void, int>{std::unexpected{20}}
                          | or_else([](auto e) { return std::expected<void, int>{std::unexpected{e * 2}}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<void, int>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 40);
}

TEST_CASE("void-value-return-another-error-has-error") {
    constexpr auto result = std::expected<void, int>{std::unexpected{1}}
                          | or_else([](auto e) { return std::expected<void, double>{std::unexpected{e * 2.0}}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<void, double>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 2.0);
}

TEST_CASE("value-return-same-error-has-value") {
    constexpr auto result = std::expected<int, double>{}
                          | or_else([](auto e) { return std::expected<int, double>{std::unexpected{e + 1}}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<int, double>>);
    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("value-return-another-error-has-value") {
    constexpr auto result = std::expected<int, double>{}
                          | or_else([](auto e) { return std::expected<int, float>{std::unexpected{float(e)}}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<int, float>>);
    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("value-return-same-error-has-error") {
    constexpr auto result = std::expected<int, double>{std::unexpected{2.0}}
                          | or_else([](auto e) { return std::expected<int, double>{std::unexpected{e + 1}}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<int, double>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 3.0);
}

TEST_CASE("value-return-another-error-has-error") {
    constexpr auto result = std::expected<double, char>{std::unexpected{'c'}} | or_else([](auto e) {
                                return std::expected<double, int>{std::unexpected{static_cast<int>(e)}};
                            });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<double, int>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == static_cast<int>('c'));
}

TEST_CASE("same-type-value-and-error-propagates-value") {
    using E = std::expected<int, int>;
    constexpr auto result = E{5} | or_else([](int e) { return E{std::unexpected{e * 2}}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const E>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == 5);
}

TEST_CASE("same-type-value-and-error-propagates-error") {
    using E = std::expected<int, int>;
    constexpr auto result = E{std::unexpected{5}} | or_else([](int e) { return E{std::unexpected{e * 2}}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const E>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 10);
}

TEMPLATE_TEST_CASE_SIG(
    "keep-value-category",
    "",
    ((typename Box, auto Fn, bool Expected), Box, Fn, Expected),
    (
        std::expected<int, double>&,
        [](double&) { return std::expected<int, double>{}; },
        true
    ),
    (
        std::expected<int, double>&,
        [](double&) { return std::expected<void, double>{}; },
        false
    ),
    (
        std::expected<int, double>&,
        [](double&&) { return std::expected<int, double>{}; },
        false
    ),
    (
        std::expected<int, double>&&,
        [](double&&) { return std::expected<int, double>{}; },
        true
    ),
    (
        std::expected<int, double>&&,
        [](double&) { return std::expected<int, double>{}; },
        false
    ),
    (
        const std::expected<int, double>&,
        [](const double&) { return std::expected<int, double>{}; },
        true
    ),
    (const std::expected<int, double>&, [](double&) { return std::expected<int, double>{}; }, false)
) {
    STATIC_REQUIRE(or_elseable<Box, decltype(Fn)> == Expected);
}

} // namespace beman::monadics::tests
