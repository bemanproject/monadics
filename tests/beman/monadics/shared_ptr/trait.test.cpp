// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("box-trait-for") {
    using Traits = get_box_traits<std::shared_ptr<int>>;
    STATIC_REQUIRE(std::same_as<Traits::value_type, int>);
    STATIC_REQUIRE(std::same_as<Traits::error_type, std::nullptr_t>);
    STATIC_REQUIRE(std::same_as<Traits::rebind<double>, std::shared_ptr<double>>);
    STATIC_REQUIRE(std::same_as<Traits::rebind_error<double>, std::shared_ptr<int>>);

    const auto sp1 = Traits::make(10);
    const auto sp2 = std::make_shared<int>(10);
    REQUIRE(sp1);
    REQUIRE(sp2);
    REQUIRE(*sp1 == *sp2);

    // REQUIRE(Traits::make_error() == std::shared_ptr<int>{});
    REQUIRE(Traits::value(std::make_shared<int>(10)) == 10);
}

template <typename Traits>
concept transformable = requires() {
    // { Traits::make(typename Traits::value_type{})  };
    { Traits::make(std::declval<typename Traits::value_type>()) };
};

TEST_CASE("makeable-value") {
    using Traits = get_box_traits<std::shared_ptr<int>>;
    STATIC_REQUIRE(transformable<Traits>);

    // STATIC_REQUIRE(std::same_as<decltype(std::declval<int>()), int&>);
}

TEST_CASE("has-error-channel") {
    STATIC_REQUIRE_FALSE(has_error_channel<std::shared_ptr<int>>);
}

} // namespace beman::monadics::tests
