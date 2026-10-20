// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("with-null-then-else-then") {
    constexpr int value = []() {
        int held{10};
        int* p = nullptr;

        auto result = p
                    | and_then([&held](auto&& v) {
                          held = v;
                          held += 100;
                          return &held;
                      })
                    | or_else([&held]() { return &held; })
                    | and_then([](auto&& v) {
                          v += 10;
                          return &v;
                      });
        return *result;
    }();

    STATIC_REQUIRE(value == 20);
}

TEST_CASE("with-value-else-then-else-then") {
    constexpr int value = []() {
        int held{10};
        int* p = &held;

        auto result = p
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
        return *result;
    }();

    STATIC_REQUIRE(value == 21);
}

} // namespace beman::monadics::tests
