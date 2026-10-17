// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/carry_value.hpp>
#include <beman/monadics/monadics.hpp>

#include <catch2/catch_test_macros.hpp>

#include <concepts>
#include <optional>

template<typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    using error_type = unit;
};

namespace beman::monadics::detail::tests {

namespace {} // namespace

TEST_CASE("settles-the-value-into-the-result-it-is-told") {
    STATIC_REQUIRE(carry_value.operator()<std::optional<int>>(4) == std::optional{4});
    STATIC_REQUIRE(carry_value.operator()<just_t<int>>(4) == just_t<int>{4});
}

TEST_CASE("takes-nothing-where-the-value-carries-nothing") {
    STATIC_REQUIRE(carry_value.operator()<just_t<void>>().has_value());
}

TEST_CASE("names-no-result-of-its-own") {
    STATIC_REQUIRE_FALSE(std::invocable<carry_value_t, int>);
}

TEST_CASE("is-the-pass-through-handler-of-or_else") {
    STATIC_REQUIRE((std::optional{4} | or_else([] { return std::optional{0}; })) == std::optional{4});
    STATIC_REQUIRE(either(carry_value, [] { return std::optional{0}; }, std::optional{4}) == std::optional{4});
}

} // namespace beman::monadics::detail::tests
