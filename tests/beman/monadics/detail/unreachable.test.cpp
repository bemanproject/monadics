// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/unreachable.hpp>

#include <catch2/catch_test_macros.hpp>

#include <concepts>
#include <type_traits>

namespace beman::monadics::detail::tests {

TEST_CASE("stands-in-for-a-read-that-cannot-happen") {
    STATIC_REQUIRE(std::is_void_v<decltype(unreachable())>);

    constexpr auto stands_in = [](auto&&) -> int {
        unreachable();
    };
    STATIC_REQUIRE(std::same_as<std::invoke_result_t<decltype(stands_in), char>, int>);
}

} // namespace beman::monadics::detail::tests
