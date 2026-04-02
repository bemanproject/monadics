// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include "beman/monadics/detail/or_else.hpp"

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("void-value-return-same-error") {
    constexpr auto result =
        stdx::expected<void, int>{} | or_else([](auto e) { return stdx::expected<void, int>(e * 2); });

    STATIC_REQUIRE(std::same_as<decltype(result), const stdx::expected<void, int>>);
    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("void-value-return-another-error") {
    constexpr auto result =
        stdx::expected<void, int>{} | or_else([](auto e) { return stdx::expected<void, double>(e * 2.0); });

    STATIC_REQUIRE(std::same_as<decltype(result), const stdx::expected<void, double>>);
    STATIC_REQUIRE(result.has_value() == true);
}

TEST_CASE("void-value-return-same-error-2") {
    constexpr auto result =
        stdx::expected<void, int>{20} | or_else([](auto e) { return stdx::expected<void, int>(e * 2); });

    STATIC_REQUIRE(std::same_as<decltype(result), const stdx::expected<void, int>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 40);
}

TEST_CASE("void-value-return-another-error-2") {
    constexpr auto result =
        stdx::expected<void, int>{1} | or_else([](auto e) { return stdx::expected<void, double>(e * 2.0); });

    STATIC_REQUIRE(std::same_as<decltype(result), const stdx::expected<void, double>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 2.0);
}

TEST_CASE("value-return-same-error") {
    constexpr auto result =
        stdx::expected<int, double>{} | or_else([](auto e) { return stdx::expected<int, double>(e + 1); });

    STATIC_REQUIRE(std::same_as<decltype(result), const stdx::expected<int, double>>);
    STATIC_REQUIRE(result.has_value());
}

TEST_CASE("value-return-another-error") {
    constexpr auto result =
        stdx::expected<int, double>{} | or_else([](auto e) { return stdx::expected<int, float>(float(e)); });

    STATIC_REQUIRE(std::same_as<decltype(result), const stdx::expected<int, float>>);
    STATIC_REQUIRE(result.has_value() == true);
}

TEST_CASE("value-return-same-error-2") {
    constexpr auto result =
        stdx::expected<int, double>{2.0} | or_else([](auto e) { return stdx::expected<int, double>(e + 1); });

    STATIC_REQUIRE(std::same_as<decltype(result), const stdx::expected<int, double>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == 3.0);
}

TEST_CASE("value-return-another-error-2") {
    constexpr auto result = stdx::expected<double, char>{'c'}
                          | or_else([](auto e) { return stdx::expected<double, int>(static_cast<int>(e)); });

    STATIC_REQUIRE(std::same_as<decltype(result), const stdx::expected<double, int>>);
    STATIC_REQUIRE(result.has_value() == false);
    STATIC_REQUIRE(result.error() == static_cast<char>('c'));
}

TEMPLATE_TEST_CASE_SIG(
    "keep-value-category",
    "",
    ((typename Box, auto Fn, bool Expected), Box, Fn, Expected),
    (
        stdx::expected<int, double>&,
        [](double&) { return stdx::expected<int, double>{}; },
        true
    ),
    (
        stdx::expected<int, double>&,
        [](double&) { return stdx::expected<void, double>{}; },
        false
    ),
    (
        stdx::expected<int, double>&,
        [](double&&) { return stdx::expected<int, double>{}; },
        false
    ),
    (
        stdx::expected<int, double>&&,
        [](double&&) { return stdx::expected<int, double>{}; },
        true
    ),
    (
        stdx::expected<int, double>&&,
        [](double&) { return stdx::expected<int, double>{}; },
        false
    ),
    (
        const stdx::expected<int, double>&,
        [](const double&) { return stdx::expected<int, double>{}; },
        true
    ),
    (const stdx::expected<int, double>&, [](double&) { return stdx::expected<int, double>{}; }, false)
) {
    STATIC_REQUIRE(or_elseable<Box, decltype(Fn)> == Expected);
}

} // namespace beman::monadics::tests
