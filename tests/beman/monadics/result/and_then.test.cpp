// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("with-void-value-return-non-void-with-value") {
    constexpr auto result = Result<void, int>{} | and_then([]() { return Result<double, int>(2.0); });
    STATIC_REQUIRE(std::same_as<decltype(result), const Result<double, int>>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == 2.0);
}

TEST_CASE("with-void-value-return-non-void-with-error") {
    constexpr auto result = Result<void, int>{} | and_then([]() { return Result<double, int>(2); });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<double, int>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 2);
}

TEST_CASE("with-void-value-return-void-with-value") {
    constexpr auto result = Result<void, int>{} | and_then([]() -> Result<void, int> { return {}; });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<void, int>>);
    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("with-void-value-return-void-with-error") {
    constexpr auto result = Result<void, int>{} | and_then([]() -> Result<void, int> { return {5}; });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<void, int>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 5);
}

TEST_CASE("with-value-return-non-void-with-value") {
    constexpr auto result = Result<int, double>{10} | and_then([](auto) -> Result<std::string_view, double> {
                                return std::string_view{"some"};
                            });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<std::string_view, double>>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == "some");
}

TEST_CASE("with-value-return-non-void-with-error") {
    constexpr auto result =
        Result<int, double>{10} | and_then([](auto&& v) -> Result<std::string_view, double> { return v * 2.0; });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<std::string_view, double>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 20.0);
}

TEST_CASE("with-value-return-void-with-value") {
    constexpr auto result = Result<int, double>{10} | and_then([](auto) -> Result<void, double> { return {}; });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<void, double>>);
    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("with-value-return-void-with-error") {
    constexpr auto result = Result<int, double>{10} | and_then([](int&&) -> Result<void, double> { return {5.0}; });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<void, double>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 5.0);
}

struct ChangedError {};

TEMPLATE_TEST_CASE_SIG("keep-value-category",
                       "",
                       ((typename Box, auto Fn, bool Expected), Box, Fn, Expected),
                       (
                           Result<int, double>&,
                           [](int&) { return Result<int, double>{10}; },
                           true
                       ),
                       (
                           Result<int, double>&,
                           [](int&&) { return Result<int, double>{10}; },
                           false
                       ),
                       (
                           Result<int, double>&&,
                           [](int&&) { return Result<int, double>{10}; },
                           true
                       ),
                       (
                           Result<int, double>&&,
                           [](int&&) { return Result<int, ChangedError>{10}; },
                           false
                       ),
                       (
                           Result<int, double>&&,
                           [](int&&) { return Result<int, float>{10}; },
                           false
                       ),
                       (
                           Result<int, double>&&,
                           [](int&) { return Result<int, double>{10}; },
                           false
                       ),
                       (
                           const Result<int, double>&,
                           [](const int&) { return Result<int, double>{10}; },
                           true
                       ),
                       (const Result<int, double>&, [](int&) { return Result<int, double>{10}; }, false)) {
    STATIC_REQUIRE(and_thenable<Box, decltype(Fn)> == Expected);
}

} // namespace beman::monadics::tests
