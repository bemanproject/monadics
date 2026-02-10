// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("box-trait-for") {
    using Expected = stdx::expected<void, int>;
    using Traits   = box_traits_for<Expected>;
    STATIC_REQUIRE(std::same_as<Traits::value_type, void>);
    STATIC_REQUIRE(std::same_as<Traits::error_type, int>);
    STATIC_REQUIRE(std::same_as<Traits::rebind<double>, stdx::expected<double, int>>);
    STATIC_REQUIRE(std::same_as<Traits::rebind_error<double>, stdx::expected<void, double>>);
    STATIC_REQUIRE(std::same_as<decltype(Traits::value(Expected{10})), void>);
    STATIC_REQUIRE(Traits::error(Expected{10}) == 10);
    STATIC_REQUIRE(Traits::make() == Expected{});
    STATIC_REQUIRE(Traits::make_error(1) == Expected{1});
}

TEST_CASE("box-trait-for-non-void-value") {
    using Expected = stdx::expected<char, int>;
    using Traits   = box_traits_for<Expected>;
    STATIC_REQUIRE(std::same_as<Traits::value_type, char>);
    STATIC_REQUIRE(std::same_as<Traits::error_type, int>);
    STATIC_REQUIRE(std::same_as<Traits::rebind<double>, stdx::expected<double, int>>);
    STATIC_REQUIRE(std::same_as<Traits::rebind_error<double>, stdx::expected<char, double>>);
    STATIC_REQUIRE(Traits::value(Expected{'a'}) == 'a');
    STATIC_REQUIRE(Traits::error(Expected{10}) == 10);
    STATIC_REQUIRE(Traits::make('b') == Expected{'b'});
    STATIC_REQUIRE(Traits::make_error(1) == Expected{1});
}

} // namespace beman::monadics::tests
