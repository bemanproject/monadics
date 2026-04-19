// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("with-error-then-else-transform") {
    using Expected = std::expected<int, double>;

    constexpr int value = []() {
        const auto result = Expected{std::unexpected{-1.0}}
                          | and_then([](auto&& v) { return Expected{v + 10}; })
                          | or_else([](auto&& e) { return Expected{int(e)}; })
                          | transform([](auto&& v) { return v + 50; });
        return result.value();
    }();

    STATIC_REQUIRE(value == 49);
}

TEST_CASE("with-value-then-else-transform") {
    using Expected = std::expected<int, double>;

    constexpr int value = []() {
        const auto result = Expected{0}
                          | and_then([](auto&& v) { return Expected{v + 10}; })
                          | or_else([](auto&& e) { return Expected{int(e)}; })
                          | transform([](auto&& v) { return v + 50; });
        return result.value();
    }();

    STATIC_REQUIRE(value == 60);
}

TEST_CASE("same-type-value-and-error-composition") {
    using E = std::expected<int, int>;

    SECTION("value-path") {
        constexpr int value = []() {
            const auto result = E{5}
                              | and_then([](int v) { return E{v * 2}; })
                              | transform([](int v) { return v + 1; })
                              | or_else([](int e) { return E{std::unexpected{e - 1}}; });
            return result.value();
        }();
        STATIC_REQUIRE(value == 11);
    }

    SECTION("error-path") {
        constexpr int err = []() {
            const auto result = E{std::unexpected{5}}
                              | and_then([](int v) { return E{v * 2}; })
                              | transform([](int v) { return v + 1; })
                              | or_else([](int e) { return E{std::unexpected{e * 3}}; });
            return result.error();
        }();
        STATIC_REQUIRE(err == 15);
    }
}

} // namespace beman::monadics::tests
