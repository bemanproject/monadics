// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("void-value-return-non-void-value") {
    constexpr auto result = Result<void, int>{} | transform([]() { return 2.0; });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<double, int>>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == 2.0);
}

TEST_CASE("void-value-return-void-value") {
    constexpr auto result = Result<void, int>{} | transform([]() { return; });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<void, int>>);
    STATIC_REQUIRE(result.has_value() == true);
}

TEST_CASE("value-return-different-value") {
    constexpr auto result = Result<int, double>{10} | transform([](auto) { return std::string_view{"some"}; });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<std::string_view, double>>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == "some");
}

TEST_CASE("value-return-void-value") {
    constexpr auto result = Result<int, double>{10} | transform([](auto) { return; });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<void, double>>);
    STATIC_REQUIRE(result.has_value());
}

TEMPLATE_TEST_CASE_SIG("keep-value-category",
                       "",
                       ((typename Box, auto Fn, bool Expected), Box, Fn, Expected),
                       (
                           Result<int, double>&,
                           [](int&) { return 0; },
                           true
                       ),
                       (
                           Result<int, double>&,
                           [](int&&) { return 0; },
                           false
                       ),
                       (
                           Result<int, double>&&,
                           [](int&&) { return 0; },
                           true
                       ),
                       (
                           Result<int, double>&&,
                           [](int&) { return 0; },
                           false
                       ),
                       (
                           const Result<int, double>&,
                           [](const int&) { return 0; },
                           true
                       ),
                       (const Result<int, double>&, [](int&) { return 0; }, false)) {
    STATIC_REQUIRE(transformable<Box, decltype(Fn)> == Expected);
}

} // namespace beman::monadics::tests
