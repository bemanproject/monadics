// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("void-value") {
    constexpr auto fn = [](int v) {
        return v * 2.0;
    };

    SECTION("with-value") {
        constexpr auto result = std::expected<void, int>{} | transform_error(fn);
        STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<void, double>>);
        STATIC_REQUIRE(result.has_value());
    }

    SECTION("without-value") {
        constexpr auto result = std::expected<void, int>{std::unexpected{10}} | transform_error(fn);
        STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<void, double>>);
        STATIC_REQUIRE(result.has_value() == false);
        STATIC_REQUIRE(result.error() == 20.0);
    }
}

TEST_CASE("non-void-value") {
    constexpr auto fn = [](int v) {
        return v * 2.0;
    };

    SECTION("with-value") {
        constexpr auto result = std::expected<char, int>{'a'} | transform_error(fn);
        STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<char, double>>);
        STATIC_REQUIRE(result.has_value());
        STATIC_REQUIRE(result.value() == 'a');
    }

    SECTION("without-value") {
        constexpr auto result = std::expected<char, int>{std::unexpected{10}} | transform_error(fn);
        STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<char, double>>);
        STATIC_REQUIRE(result.has_value() == false);
        STATIC_REQUIRE(result.error() == 20.0);
    }
}

TEST_CASE("same-type-value-and-error") {
    constexpr auto fn = [](int e) {
        return e * 2;
    };

    SECTION("with-value") {
        constexpr auto result = std::expected<int, int>{5} | transform_error(fn);
        STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<int, int>>);
        STATIC_REQUIRE(result.has_value());
        STATIC_REQUIRE(result.value() == 5);
    }

    SECTION("without-value") {
        constexpr auto result = std::expected<int, int>{std::unexpected{5}} | transform_error(fn);
        STATIC_REQUIRE(std::same_as<decltype(result), const std::expected<int, int>>);
        STATIC_REQUIRE(result.has_value() == false);
        STATIC_REQUIRE(result.error() == 10);
    }
}

TEMPLATE_TEST_CASE_SIG("keep-value-category",
                       "",
                       ((typename Box, auto Fn, bool Expected), Box, Fn, Expected),
                       (
                           std::expected<int, double>&,
                           [](double&) { return 0.0; },
                           true
                       ),
                       (
                           std::expected<int, double>&,
                           [](double&&) { return 0.0; },
                           false
                       ),
                       (
                           std::expected<int, double>&&,
                           [](double&&) { return 0.0; },
                           true
                       ),
                       (
                           std::expected<int, double>&&,
                           [](double&) { return 0.0; },
                           false
                       ),
                       (
                           const std::expected<int, double>&,
                           [](const double&) { return 0.0; },
                           true
                       ),
                       (const std::expected<int, double>&, [](double&) { return 0.0; }, false)) {
    STATIC_REQUIRE(transform_errorable<Box, decltype(Fn)> == Expected);
}

} // namespace beman::monadics::tests
