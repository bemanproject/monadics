// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/make_value.hpp>
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

template<typename R, typename T>
concept makes_from = requires { get_box_traits<R>::make(std::declval<T>()); };

} // namespace

TEST_CASE("is-the-traits-make-with-the-empty-value-folded-in") {
    STATIC_REQUIRE(make_value<std::optional<int>>(4) == get_box_traits<std::optional<int>>::make(4));
    STATIC_REQUIRE(make_value<just_t<void>>().has_value());
    STATIC_REQUIRE_FALSE(makes_from<just_t<void>, int>);
}

} // namespace beman::monadics::detail::tests
