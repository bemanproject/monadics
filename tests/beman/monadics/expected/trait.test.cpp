// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("box-trait-for-void-value") {
    using Expected = std::expected<void, int>;
    using Traits = get_box_traits<Expected>;

    STATIC_REQUIRE(std::same_as<Traits::value_type, void>);
    STATIC_REQUIRE(std::same_as<Traits::error_type, int>);
    STATIC_REQUIRE(std::same_as<Traits::rebind<double>, std::expected<double, int>>);
    STATIC_REQUIRE(std::same_as<Traits::rebind_error<double>, std::expected<void, double>>);
    STATIC_REQUIRE(std::same_as<decltype(Traits::value(Expected{})), void>);
    STATIC_REQUIRE(Traits::error(Expected{std::unexpected{10}}) == 10);
    STATIC_REQUIRE(Traits::make() == Expected{});
    STATIC_REQUIRE(Traits::make_error(1) == Expected{std::unexpected{1}});
}

TEST_CASE("box-trait-for-non-void-value") {
    using Expected = std::expected<char, int>;
    using Traits = get_box_traits<Expected>;

    STATIC_REQUIRE(std::same_as<Traits::value_type, char>);
    STATIC_REQUIRE(std::same_as<Traits::error_type, int>);
    STATIC_REQUIRE(std::same_as<Traits::rebind<double>, std::expected<double, int>>);
    STATIC_REQUIRE(std::same_as<Traits::rebind_error<double>, std::expected<char, double>>);
    STATIC_REQUIRE(Traits::value(Expected{'a'}) == 'a');
    STATIC_REQUIRE(Traits::error(Expected{std::unexpected{10}}) == 10);
    STATIC_REQUIRE(Traits::make('b') == Expected{'b'});
    STATIC_REQUIRE(Traits::make_error(1) == Expected{std::unexpected{1}});
}

TEST_CASE("box-trait-for-same-value-error-type") {
    using Expected = std::expected<int, int>;
    using Traits = get_box_traits<Expected>;

    STATIC_REQUIRE(std::same_as<Traits::value_type, int>);
    STATIC_REQUIRE(std::same_as<Traits::error_type, int>);
    STATIC_REQUIRE(std::same_as<Traits::rebind<double>, std::expected<double, int>>);
    STATIC_REQUIRE(std::same_as<Traits::rebind_error<double>, std::expected<int, double>>);
    STATIC_REQUIRE(Traits::value(Expected{5}) == 5);
    STATIC_REQUIRE(Traits::error(Expected{std::unexpected{5}}) == 5);

    constexpr auto with_value = Traits::make(5);
    STATIC_REQUIRE(with_value.has_value());
    STATIC_REQUIRE(with_value.value() == 5);

    constexpr auto with_error = Traits::make_error(5);
    STATIC_REQUIRE(!with_error.has_value());
    STATIC_REQUIRE(with_error.error() == 5);
}

TEST_CASE("has-error-channel") {
    STATIC_REQUIRE(has_error_channel<std::expected<int, double>>);
    STATIC_REQUIRE(has_error_channel<std::expected<void, int>>);
    STATIC_REQUIRE(has_error_channel<std::expected<int, int>>);
}

} // namespace beman::monadics::tests
