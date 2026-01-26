// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include "beman/monadics/detail/and_then.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("with-value") {
    constexpr auto result = std::optional{10} | and_then([](auto&& value) {
                                return std::optional{std::forward<decltype(value)>(value) * 2.0};
                            });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::optional<double>>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == 20.0);
}

TEST_CASE("with-nullopt") {
    constexpr auto result = std::optional<int>{} | and_then([](auto&& value) {
                                return std::optional{std::forward<decltype(value)>(value) * 2.0};
                            });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::optional<double>>);
    STATIC_REQUIRE(result.has_value() == false);
}

TEST_CASE("with-value-variadic") {
    constexpr auto result = std::optional<int>{10} | and_then([](auto&&... v) { return std::optional{sizeof...(v)}; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::optional<std::size_t>>);
    STATIC_REQUIRE(result.has_value() == true);
    STATIC_REQUIRE(result.value() == 1);
}

template <typename Box, auto op, auto fn>
concept monadic_invocable = requires (Box &&box) {
  { box | op(fn) };
};

TEST_CASE("with-value-variadic2") {
  STATIC_REQUIRE(monadic_invocable<std::optional<int>, and_then, [](int) { return std::optional{10}; }>);
  STATIC_REQUIRE(!monadic_invocable<std::optional<int>, and_then, []() { return std::optional{10}; }>);
    constexpr auto result = std::optional<int>{10} | and_then([](int) { return 10; });
    // STATIC_REQUIRE(std::same_as<decltype(result), const std::optional<std::size_t>>);
    // STATIC_REQUIRE(result.has_value() == true);
    // STATIC_REQUIRE(result.value() == 1);

}

} // namespace beman::monadics::tests
