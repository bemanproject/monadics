// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/same_unqualified_as.hpp>

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::detail::tests {

TEMPLATE_TEST_CASE("identical-types", "", int, double) { STATIC_REQUIRE(same_unqualified_as<TestType, TestType>); }

TEMPLATE_TEST_CASE("strip-const", "", int, double) {
    STATIC_REQUIRE(same_unqualified_as<const TestType, TestType>);
    STATIC_REQUIRE(same_unqualified_as<TestType, const TestType>);
}

TEMPLATE_TEST_CASE("strip-lvalue-ref", "", int, double) {
    STATIC_REQUIRE(same_unqualified_as<TestType&, TestType>);
    STATIC_REQUIRE(same_unqualified_as<TestType, TestType&>);
}

TEMPLATE_TEST_CASE("strip-rvalue-ref", "", int, double) {
    STATIC_REQUIRE(same_unqualified_as<TestType&&, TestType>);
    STATIC_REQUIRE(same_unqualified_as<TestType, TestType>);
}

TEMPLATE_TEST_CASE("strip-combined-qualifiers", "", int, double) {
    STATIC_REQUIRE(same_unqualified_as<const TestType&, TestType>);
    STATIC_REQUIRE(same_unqualified_as<const TestType&&, TestType>);
    STATIC_REQUIRE(same_unqualified_as<volatile const TestType&&, TestType>);
}

TEST_CASE("different-base-types") {
    STATIC_REQUIRE_FALSE(same_unqualified_as<int, double>);
    STATIC_REQUIRE_FALSE(same_unqualified_as<const int&, double>);
}

} // namespace beman::monadics::detail::tests
