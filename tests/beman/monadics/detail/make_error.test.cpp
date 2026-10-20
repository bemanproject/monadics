// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/make_error.hpp>
#include <beman/monadics/monadics.hpp>

#include <catch2/catch_test_macros.hpp>

#include <optional>
#include <utility>

template<typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    using error_type = unit;
};

namespace beman::monadics::detail::tests {

namespace {

template<typename R, typename E>
concept makes_error_from = requires { get_box_traits<R>::make_error(std::declval<E>()); };

} // namespace

TEST_CASE("is-the-traits-make_error-with-the-empty-failure-folded-in") {
    STATIC_REQUIRE(make_error<just_error_t<int>>(7) == get_box_traits<just_error_t<int>>::make_error(7));
    STATIC_REQUIRE(make_error<std::optional<int>>('e') == std::nullopt);
    STATIC_REQUIRE_FALSE(makes_error_from<std::optional<int>, char>);
}

} // namespace beman::monadics::detail::tests
