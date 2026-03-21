// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include "beman/monadics/detail/transform_error.hpp"

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("void-value") {
    auto fn = [](int v) { return v * 2.0; };

    SECTION("with-value") {
        constexpr auto result = stdx::expected<void, int>{} | transform_error(fn);

        STATIC_REQUIRE(std::same_as<decltype(result), const stdx::expected<void, double>>);
        STATIC_REQUIRE(result.has_value());
    }

    SECTION("without-value") {
        constexpr auto result = stdx::expected<void, int>{10} | transform_error(fn);

        STATIC_REQUIRE(std::same_as<decltype(result), const stdx::expected<void, double>>);
        STATIC_REQUIRE(result.has_value() == false);
        STATIC_REQUIRE(result.error() == 20.0);
    }
}

TEST_CASE("non-void-value") {
    auto fn = [](int v) { return v * 2.0; };

    SECTION("with-value") {
        constexpr auto result = stdx::expected<char, int>{} | transform_error(fn);

        STATIC_REQUIRE(std::same_as<decltype(result), const stdx::expected<char, double>>);
        STATIC_REQUIRE(result.has_value());
    }

    SECTION("without-value") {
        constexpr auto result = stdx::expected<char, int>{10} | transform_error(fn);

        STATIC_REQUIRE(std::same_as<decltype(result), const stdx::expected<char, double>>);
        STATIC_REQUIRE(result.has_value() == false);
        STATIC_REQUIRE(result.error() == 20.0);
    }
}

TEMPLATE_TEST_CASE_SIG("keep-value-category",
                       "",
                       ((typename Box, auto Fn, bool Expected), Box, Fn, Expected),
                       (
                           stdx::expected<int, double>&, [](double&) { return 0.0; }, true),
                       (
                           stdx::expected<int, double>&, [](double&&) { return 0.0; }, false),
                       (
                           stdx::expected<int, double>&&, [](double&&) { return 0.0; }, true),
                       (
                           stdx::expected<int, double>&&, [](double&) { return 0.0; }, false),
                       (
                           const stdx::expected<int, double>&, [](const double&) { return 0.0; }, true),
                       (const stdx::expected<int, double>&, [](double&) { return 0.0; }, false)) {
    STATIC_REQUIRE(transform_errorable<Box, decltype(Fn)> == Expected);
}

} // namespace beman::monadics::tests
