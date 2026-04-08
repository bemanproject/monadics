// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <catch2/catch_test_macros.hpp>

#include "trait.hpp"

namespace beman::monadics::tests {

TEST_CASE("with-null-then-else-transform") {
    constexpr int value = []() {
        const auto result = std::optional<int>{}
                          | and_then([](auto&& v) { return std::optional{v + 10}; })
                          | or_else([]() { return std::optional{-10}; })
                          | transform([](auto&& v) { return v + 50; });
        return *result;
    }();

    STATIC_REQUIRE(value == 40);
}

TEST_CASE("with-value-then-else-transform") {
    constexpr int value = []() {
        const auto result = std::optional<int>{10}
                          | and_then([](auto&& v) { return std::optional{v + 10}; })
                          | or_else([]() { return std::optional{-10}; })
                          | transform([](auto&& v) { return v + 50; });
        return *result;
    }();

    STATIC_REQUIRE(value == 70);
}

} // namespace beman::monadics::tests
