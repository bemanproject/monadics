// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <catch2/catch_test_macros.hpp>

#include "trait.hpp"

namespace beman::monadics::tests {

TEST_CASE("with-value") {
    const auto result = std::make_shared<int>(10) | or_else([]() { return std::make_shared<int>(1); });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<int>>);
    REQUIRE(static_cast<bool>(result));
    REQUIRE(*result == 10.0);
}

TEST_CASE("without-value") {
    const auto result = std::shared_ptr<int>{} | or_else([]() { return std::make_shared<int>(1); });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<int>>);
    REQUIRE(static_cast<bool>(result));
    REQUIRE(*result == 1);
}

TEST_CASE("change-value-type") {
    STATIC_REQUIRE(or_elseable<std::shared_ptr<int>, decltype([] { return std::shared_ptr<double>{}; })> == true);
}

} // namespace beman::monadics::tests
