// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <catch2/catch_test_macros.hpp>

#include "trait.hpp"

namespace beman::monadics::tests {

template <typename T>
concept hm = requires {
    { box_traits<int*>::make(std::declval<T>()) };
};

TEST_CASE("box-trait-for") {
    using Traits = box_traits_for<int*>;
    STATIC_REQUIRE(std::same_as<Traits::value_type, int>);
    STATIC_REQUIRE(std::same_as<Traits::error_type, std::nullptr_t>);
    STATIC_REQUIRE(std::same_as<Traits::rebind<float>, float*>);
    STATIC_REQUIRE(std::same_as<Traits::rebind_error<float>, int*>);

    constexpr auto value = []() {
        int value = 10;
        return Traits::value(&value);
    }();
    STATIC_REQUIRE(value == 10);

    constexpr auto hasValue = []() {
        int value = 10;
        return Traits::has_value(&value);
    }();
    STATIC_REQUIRE(hasValue);

    constexpr auto noValue = []() { return Traits::has_value(static_cast<int*>(nullptr)); }();
    STATIC_REQUIRE(noValue == false);

    STATIC_REQUIRE(Traits::error() == nullptr);

    // should be ok
    // STATIC_REQUIRE(Traits::make_error() == static_cast<int*>(nullptr));

    constexpr auto liftValue = []() {
        int value = 10;
        return *Traits::make(value);
    }();
    STATIC_REQUIRE(liftValue == 10);
}

} // namespace beman::monadics::tests
