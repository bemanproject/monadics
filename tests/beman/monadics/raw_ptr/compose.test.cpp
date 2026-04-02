// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("with-null-then-else-then") {
    constexpr int value = []() {
        int value{10};
        int* p = nullptr;

        auto result = p
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
        return *result;
    }();

    STATIC_REQUIRE(value == 20);
}

TEST_CASE("with-value-else-then-else-then") {
    constexpr int value = []() {
        int value{10};
        int* p = &value;

        auto result = p
                    | or_else([]() {
                          static int value = 1;
                          return &value;
                      })
                    | and_then([](auto&& v) {
                          v += 1;
                          return &v;
                      })
                    | or_else([]() {
                          int* p = nullptr;
                          return p;
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
