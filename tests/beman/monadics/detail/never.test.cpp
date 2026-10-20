// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/never.hpp>
#include <beman/monadics/detail/unit.hpp>

#include <catch2/catch_test_macros.hpp>

#include <concepts>
#include <type_traits>

namespace beman::monadics::detail::tests {

TEST_CASE("no-value-can-be-made") {
    STATIC_REQUIRE_FALSE(std::is_default_constructible_v<never>);
    STATIC_REQUIRE(std::is_empty_v<never>);
}

TEST_CASE("distinct-from-the-payload-that-carries-nothing") {
    STATIC_REQUIRE_FALSE(std::same_as<never, unit>);
    STATIC_REQUIRE_FALSE(std::convertible_to<unit, never>);
    STATIC_REQUIRE_FALSE(std::convertible_to<never, unit>);
}

} // namespace beman::monadics::detail::tests
