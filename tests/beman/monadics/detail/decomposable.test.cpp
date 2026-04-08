// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <catch2/catch_test_macros.hpp>

#include <optional>
#include <utility>
#include <vector>

#include <beman/monadics/detail/decomposable.hpp>

namespace beman::monadics::detail::tests {

TEST_CASE("concept") {
    STATIC_REQUIRE(decomposable<std::pair<int, double>, 1>);
    STATIC_REQUIRE(decomposable<std::pair<int, double>, 2>);
    STATIC_REQUIRE(decomposable<std::vector<int>, 1>);

    STATIC_REQUIRE_FALSE(decomposable<int, 1>);
    STATIC_REQUIRE_FALSE(decomposable<std::optional<int>, 2>);
    STATIC_REQUIRE_FALSE(decomposable<std::pair<int, double>, 3>);
}

TEST_CASE("ignore-cvref") {
    STATIC_REQUIRE(decomposable<const std::optional<int>&, 1>);
    STATIC_REQUIRE(decomposable<std::optional<int>&&, 1>);
}

} // namespace beman::monadics::detail::tests
