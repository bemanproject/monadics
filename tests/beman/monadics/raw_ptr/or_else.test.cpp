// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include "beman/monadics/detail/or_else.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("with-nullptr") {
    constexpr int value = []() {
        int  value{10};
        int* p = nullptr;

        auto result = p | or_else([&value]() { return &value; });
        return *result;
    }();

    STATIC_REQUIRE(value == 10);
}

TEST_CASE("with-value") {
    constexpr int value = []() {
        int v1{10};
        int v2{15};

        auto result = &v1 | or_else([&v2]() { return &v2; });
        return *result;
    }();

    STATIC_REQUIRE(value == 10);
}

// test to don't allow rebind error

} // namespace beman::monadics::tests
