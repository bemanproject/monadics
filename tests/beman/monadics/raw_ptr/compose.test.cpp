// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("with-null-then-else-then") {
    constexpr int result = []() {
        int value{10};
        int* p = nullptr;

        auto chained = p
                     | and_then([&value](auto&& v) {
                           value = v;
                           value += 100;
                           return &value;
                       })
                     | or_else([&value]() { return &value; })
                     | and_then([](auto&& v) {
                           v += 10;
                           return &v;
                       });
        return *chained;
    }();

    STATIC_REQUIRE(result == 20);
}

TEST_CASE("with-value-else-then-else-then") {
    constexpr int result = []() {
        int value{10};
        int* p = &value;

        auto chained = p
                     | or_else([]() {
                           static int fallback = 1;
                           return &fallback;
                       })
                     | and_then([](auto&& v) {
                           v += 1;
                           return &v;
                       })
                     | or_else([]() {
                           int* none = nullptr;
                           return none;
                       })
                     | and_then([](auto&& v) {
                           v += 10;
                           return &v;
                       });
        return *chained;
    }();

    STATIC_REQUIRE(result == 21);
}

} // namespace beman::monadics::tests
