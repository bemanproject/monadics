// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include "beman/monadics/detail/and_then.hpp"

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("with-void-value-return-non-void-with-value") {
    constexpr auto result = stdx::expected<void, int>{} | and_then([]() { return stdx::expected<double, int>(2.0); });
    STATIC_REQUIRE(std::same_as<decltype(result), const stdx::expected<double, int>>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == 2.0);
}

TEST_CASE("with-void-value-return-non-void-with-error") {
    constexpr auto result = stdx::expected<void, int>{} | and_then([]() { return stdx::expected<double, int>(2); });

    STATIC_REQUIRE(std::same_as<decltype(result), const stdx::expected<double, int>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 2);
}

TEST_CASE("with-void-value-return-void-with-value") {
    constexpr auto result = stdx::expected<void, int>{} | and_then([]() -> stdx::expected<void, int> { return {}; });

    STATIC_REQUIRE(std::same_as<decltype(result), const stdx::expected<void, int>>);
    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("with-void-value-return-void-with-error") {
    constexpr auto result = stdx::expected<void, int>{} | and_then([]() -> stdx::expected<void, int> { return {5}; });

    STATIC_REQUIRE(std::same_as<decltype(result), const stdx::expected<void, int>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 5);
}

TEST_CASE("with-value-return-non-void-with-value") {
    constexpr auto result =
        stdx::expected<int, double>{10}
        | and_then([](auto) -> stdx::expected<std::string_view, double> { return std::string_view{"some"}; });

    STATIC_REQUIRE(std::same_as<decltype(result), const stdx::expected<std::string_view, double>>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == "some");
}

TEST_CASE("with-value-return-non-void-with-error") {
    constexpr auto result = stdx::expected<int, double>{10}
                          | and_then([](auto&& v) -> stdx::expected<std::string_view, double> { return v * 2.0; });

    STATIC_REQUIRE(std::same_as<decltype(result), const stdx::expected<std::string_view, double>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 20.0);
}

TEST_CASE("with-value-return-void-with-value") {
    constexpr auto result =
        stdx::expected<int, double>{10} | and_then([](auto) -> stdx::expected<void, double> { return {}; });

    STATIC_REQUIRE(std::same_as<decltype(result), const stdx::expected<void, double>>);
    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("with-value-return-void-with-error") {
    constexpr auto result =
        stdx::expected<int, double>{10} | and_then([](int&&) -> stdx::expected<void, double> { return {5.0}; });

    STATIC_REQUIRE(std::same_as<decltype(result), const stdx::expected<void, double>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 5.0);
}

TEMPLATE_TEST_CASE_SIG(
    "keep-value-category",
    "",
    ((typename Box, auto Fn, bool Expected), Box, Fn, Expected),
    (
        stdx::expected<int, double>&, [](int&) { return stdx::expected<int, double>{10}; }, true),
    (
        stdx::expected<int, double>&, [](int&&) { return stdx::expected<int, double>{10}; }, false),
    (
        stdx::expected<int, double>&&, [](int&&) { return stdx::expected<int, double>{10}; }, true),
    (
        stdx::expected<int, double>&&, [](int&) { return stdx::expected<int, double>{10}; }, false),
    (
        const stdx::expected<int, double>&, [](const int&) { return stdx::expected<int, double>{10}; }, true),
    (const stdx::expected<int, double>&, [](int&) { return stdx::expected<int, double>{10}; }, false)) {
    STATIC_REQUIRE(and_thenable<Box, decltype(Fn)> == Expected);
}

} // namespace beman::monadics::tests
