// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <catch2/catch_test_macros.hpp>

#include <beman/monadics/detail/as_pointer.hpp>

namespace beman::monadics::detail::tests {

TEST_CASE("concept") {
    STATIC_REQUIRE(std::same_as<decltype(as_pointer<int>), int* const>);
    STATIC_REQUIRE(std::same_as<decltype(as_pointer<const int>), int* const>);
    STATIC_REQUIRE(std::same_as<decltype(as_pointer<int&>), int* const>);
    STATIC_REQUIRE(std::same_as<decltype(as_pointer<const int&>), int* const>);
    STATIC_REQUIRE(std::same_as<decltype(as_pointer<int&&>), int* const>);
}

TEST_CASE("value") {
    STATIC_REQUIRE(as_pointer<int> == nullptr);
    STATIC_REQUIRE(as_pointer<double> == nullptr);
}

} // namespace beman::monadics::detail::tests
