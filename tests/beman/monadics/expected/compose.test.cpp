// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <catch2/catch_test_macros.hpp>

#include "trait.hpp"

namespace beman::monadics::tests {

TEST_CASE("with-null-then-else-transform") {
    using Expected = stdx::expected<int, double>;

    constexpr int value = []() {
        const auto result = Expected{-1.0}
                          | and_then([](auto&& v) { return Expected{v + 10}; })
                          | or_else([](auto&& e) { return Expected{int(e)}; })
                          | transform([](auto&& v) { return v + 50; });
        return result.value();
    }();

    STATIC_REQUIRE(value == 49);
}

TEST_CASE("with-value-then-else-transform") {
    using Expected = stdx::expected<int, double>;

    constexpr int value = []() {
        const auto result = Expected{0}
                          | and_then([](auto&& v) { return Expected{v + 10}; })
                          | or_else([](auto&& e) { return Expected{int(e)}; })
                          | transform([](auto&& v) { return v + 50; });
        return result.value();
    }();

    STATIC_REQUIRE(value == 60);
}

} // namespace beman::monadics::tests
