// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("void-value") {
    constexpr auto fn = [](int v) {
        return v * 2.0;
    };

    SECTION("with-value") {
        constexpr auto result = Result<void, int>{} | transform_error(fn);

        STATIC_REQUIRE(std::same_as<decltype(result), const Result<void, double>>);
        STATIC_REQUIRE(result.has_value());
    }

    SECTION("without-value") {
        constexpr auto result = Result<void, int>{10} | transform_error(fn);

        STATIC_REQUIRE(std::same_as<decltype(result), const Result<void, double>>);
        STATIC_REQUIRE(result.has_value() == false);
        STATIC_REQUIRE(result.error() == 20.0);
    }
}

TEST_CASE("non-void-value") {
    constexpr auto fn = [](int v) {
        return v * 2.0;
    };

    SECTION("with-value") {
        constexpr auto result = Result<char, int>{} | transform_error(fn);

        STATIC_REQUIRE(std::same_as<decltype(result), const Result<char, double>>);
        STATIC_REQUIRE(result.has_value());
    }

    SECTION("without-value") {
        constexpr auto result = Result<char, int>{10} | transform_error(fn);

        STATIC_REQUIRE(std::same_as<decltype(result), const Result<char, double>>);
        STATIC_REQUIRE(result.has_value() == false);
        STATIC_REQUIRE(result.error() == 20.0);
    }
}

TEMPLATE_TEST_CASE_SIG("keep-value-category",
                       "",
                       ((typename Box, auto Fn, bool Expected), Box, Fn, Expected),
                       (
                           Result<int, double>&,
                           [](double&) { return 0.0; },
                           true
                       ),
                       (
                           Result<int, double>&,
                           [](double&&) { return 0.0; },
                           false
                       ),
                       (
                           Result<int, double>&&,
                           [](double&&) { return 0.0; },
                           true
                       ),
                       (
                           Result<int, double>&&,
                           [](double&) { return 0.0; },
                           false
                       ),
                       (
                           const Result<int, double>&,
                           [](const double&) { return 0.0; },
                           true
                       ),
                       (const Result<int, double>&, [](double&) { return 0.0; }, false)) {
    STATIC_REQUIRE(transform_errorable<Box, decltype(Fn)> == Expected);
}

} // namespace beman::monadics::tests
