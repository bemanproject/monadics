// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("void-value-return-same-error") {
    constexpr auto result = Result<void, int>{} | or_else([](auto e) { return Result<void, int>(e * 2); });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<void, int>>);
    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("void-value-return-another-error") {
    constexpr auto result = Result<void, int>{} | or_else([](auto e) { return Result<void, double>(e * 2.0); });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<void, double>>);
    STATIC_REQUIRE(result.has_value() == true);
}

TEST_CASE("void-value-return-same-error-2") {
    constexpr auto result = Result<void, int>{20} | or_else([](auto e) { return Result<void, int>(e * 2); });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<void, int>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 40);
}

TEST_CASE("void-value-return-another-error-2") {
    constexpr auto result = Result<void, int>{1} | or_else([](auto e) { return Result<void, double>(e * 2.0); });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<void, double>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 2.0);
}

TEST_CASE("value-return-same-error") {
    constexpr auto result = Result<int, double>{} | or_else([](auto e) { return Result<int, double>(e + 1); });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<int, double>>);
    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("value-return-another-error") {
    constexpr auto result = Result<int, double>{} | or_else([](auto e) { return Result<int, float>(float(e)); });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<int, float>>);
    STATIC_REQUIRE(result.has_value() == true);
}

TEST_CASE("value-return-same-error-2") {
    constexpr auto result = Result<int, double>{2.0} | or_else([](auto e) { return Result<int, double>(e + 1); });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<int, double>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 3.0);
}

TEST_CASE("value-return-another-error-2") {
    constexpr auto result =
        Result<double, char>{'c'} | or_else([](auto e) { return Result<double, int>(static_cast<int>(e)); });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<double, int>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == static_cast<char>('c'));
}

TEMPLATE_TEST_CASE_SIG("keep-value-category",
                       "",
                       ((typename Box, auto Fn, bool Expected), Box, Fn, Expected),
                       (
                           Result<int, double>&,
                           [](double&) { return Result<int, double>{}; },
                           true
                       ),
                       (
                           Result<int, double>&,
                           [](double&) { return Result<void, double>{}; },
                           false
                       ),
                       (
                           Result<int, double>&,
                           [](double&&) { return Result<int, double>{}; },
                           false
                       ),
                       (
                           Result<int, double>&&,
                           [](double&&) { return Result<int, double>{}; },
                           true
                       ),
                       (
                           Result<int, double>&&,
                           [](double&) { return Result<int, double>{}; },
                           false
                       ),
                       (
                           const Result<int, double>&,
                           [](const double&) { return Result<int, double>{}; },
                           true
                       ),
                       (const Result<int, double>&, [](double&) { return Result<int, double>{}; }, false)) {
    STATIC_REQUIRE(or_elseable<Box, decltype(Fn)> == Expected);
}

} // namespace beman::monadics::tests
