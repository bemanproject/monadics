// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include "beman/monadics/detail/transform.hpp"

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("with-value") {
    constexpr auto result = std::optional{10} | transform([](auto&& value) { return value * 2.0; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::optional<double>>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == 20.0);
}

TEST_CASE("with-nullopt") {
    constexpr auto result = std::optional<int>{} | transform([](auto&& value) { return (value - 1) * 0.5; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::optional<double>>);
    STATIC_REQUIRE(result.has_value() == false);
}

TEMPLATE_TEST_CASE_SIG("keep-value-category",
                       "",
                       ((typename Box, auto Fn, bool Expected), Box, Fn, Expected),
                       (
                           std::optional<int>&,
                           [](int&) { return 0; },
                           true
                       ),
                       (
                           std::optional<int>&,
                           [](int&&) { return 0; },
                           false
                       ),
                       (
                           std::optional<int>&&,
                           [](int&&) { return 0; },
                           true
                       ),
                       (
                           std::optional<int>&&,
                           [](int&) { return 0; },
                           false
                       ),
                       (
                           const std::optional<int>&,
                           [](const int&) { return 0; },
                           true
                       ),
                       (const std::optional<int>&, [](int&) { return 0; }, false)) {
    STATIC_REQUIRE(transformable<Box, decltype(Fn)> == Expected);
}

} // namespace beman::monadics::tests
