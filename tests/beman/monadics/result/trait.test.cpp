// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("box-trait-for") {
    using Expected = Result<void, int>;
    using Traits = get_box_traits<Expected>;
    STATIC_REQUIRE(std::same_as<Traits::value_type, void>);
    STATIC_REQUIRE(std::same_as<Traits::error_type, int>);
    STATIC_REQUIRE(std::same_as<Traits::rebind<double>, Result<double, int>>);
    STATIC_REQUIRE(std::same_as<Traits::rebind_error<double>, Result<void, double>>);
    STATIC_REQUIRE(std::same_as<decltype(Traits::value(Expected{10})), void>);
    STATIC_REQUIRE(Traits::error(Expected{10}) == 10);
    STATIC_REQUIRE(Traits::make() == Expected{});
    STATIC_REQUIRE(Traits::make_error(1) == Expected{1});
}

TEST_CASE("box-trait-for-non-void-value") {
    using Expected = Result<char, int>;
    using Traits = get_box_traits<Expected>;
    STATIC_REQUIRE(std::same_as<Traits::value_type, char>);
    STATIC_REQUIRE(std::same_as<Traits::error_type, int>);
    STATIC_REQUIRE(std::same_as<Traits::rebind<double>, Result<double, int>>);
    STATIC_REQUIRE(std::same_as<Traits::rebind_error<double>, Result<char, double>>);
    STATIC_REQUIRE(Traits::value(Expected{'a'}) == 'a');
    STATIC_REQUIRE(Traits::error(Expected{10}) == 10);
    STATIC_REQUIRE(Traits::make('b') == Expected{'b'});
    STATIC_REQUIRE(Traits::make_error(1) == Expected{1});
}

TEST_CASE("has-error-channel") {
    STATIC_REQUIRE(has_error_channel<Result<int, double>>);
}

} // namespace beman::monadics::tests
