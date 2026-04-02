// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_test_macros.hpp>

#include <concepts>

namespace beman::monadics::tests {

TEST_CASE("box-trait-for") {
    using Traits = get_box_traits<std::optional<int>>;
    STATIC_REQUIRE(std::same_as<Traits::value_type, int>);
    STATIC_REQUIRE(std::same_as<Traits::error_type, std::nullopt_t>);
    STATIC_REQUIRE(std::same_as<Traits::rebind<float>, std::optional<float>>);
    STATIC_REQUIRE(std::same_as<Traits::rebind_error<float>, std::optional<int>>);
    STATIC_REQUIRE(Traits::value(std::optional{5}) == 5);
    STATIC_REQUIRE(Traits::has_value(std::optional{5}));
    STATIC_REQUIRE(Traits::has_value(std::optional<int>{}) == false);
    STATIC_REQUIRE(std::same_as<decltype(Traits::error()), std::nullopt_t>);
    STATIC_REQUIRE(Traits::make(10) == std::optional{10});
    STATIC_REQUIRE(Traits::make_error(Traits::error()) == std::optional<int>{});
}

TEST_CASE("has-error-channel") {
    STATIC_REQUIRE_FALSE(has_error_channel<std::optional<int>>);
}

} // namespace beman::monadics::tests
