// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <catch2/catch_test_macros.hpp>

#include <optional>
#include <utility>
#include <vector>

#include <beman/monadics/detail/instance_of.hpp>

namespace beman::monadics::detail::tests {

TEST_CASE("concept") {
    STATIC_REQUIRE(instance_of<std::optional<int>, std::optional>);
    STATIC_REQUIRE(instance_of<std::vector<int>, std::vector>);
    STATIC_REQUIRE(instance_of<std::pmr::vector<int>, std::vector>);
    STATIC_REQUIRE(instance_of<std::pair<int, double>, std::pair>);

    STATIC_REQUIRE_FALSE(instance_of<int, std::optional>);
    STATIC_REQUIRE_FALSE(instance_of<std::optional<int>, std::vector>);
}

} // namespace beman::monadics::detail::tests
