// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include "beman/monadics/detail/transform.hpp"

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("with-value") {
    const auto result = std::make_shared<int>(10) | transform([](auto&& value) { return value * 2.0; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<double>>);
    REQUIRE(static_cast<bool>(result));
    REQUIRE(*result == 20.0);
}

TEST_CASE("with-nullptr") {
    const auto result = std::shared_ptr<int>{} | transform([](auto&& value) { return (value - 1) * 0.5; });
    REQUIRE(std::same_as<decltype(result), const std::shared_ptr<double>>);
    REQUIRE(static_cast<bool>(result) == false);
}

// shared_ptr::operator* has no && overload — *std::move(sp) still yields T&.
// Shared ownership means the managed object is never moved out implicitly.
TEMPLATE_TEST_CASE_SIG("value-is-always-lvalue-ref",
                       "",
                       ((typename Box, auto Fn, bool Expected), Box, Fn, Expected),
                       (
                           std::shared_ptr<int>&,
                           [](int&) { return 0; },
                           true),
                       (
                           std::shared_ptr<int>&&,
                           [](int&) { return 0; },
                           true),
                       (
                           std::shared_ptr<int>&,
                           [](int&&) { return 0; },
                           false),
                       (std::shared_ptr<int>&&, [](int&&) { return 0; }, false)) {
    STATIC_REQUIRE(transformable<Box, decltype(Fn)> == Expected);
}

} // namespace beman::monadics::tests
