// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/same_box.hpp>

#include <catch2/catch_template_test_macros.hpp>

#include <utility>
#include <variant>
#include <optional>

template <typename T, typename E>
struct Box : std::variant<T, E> {
    using std::variant<T, E>::variant;
};

namespace beman::monadics::detail {

template <typename T, typename E>
struct box_traits<Box<T, E>> {
    using value_type = T;
    using error_type = E;

    template <typename U>
    using rebind = Box<U, E>;

    template <typename F>
    using rebind_error = Box<T, F>;

    static constexpr bool has_value(const Box<T, E>& b) noexcept { return b.index() == 0; }
    static constexpr decltype(auto) value(auto&& b) { return std::get<0>(std::forward<decltype(b)>(b)); }
    static constexpr decltype(auto) error(auto&& b) { return std::get<1>(std::forward<decltype(b)>(b)); }
};

template <typename T>
struct box_traits<std::optional<T>> {
    static constexpr decltype(auto) error() { return std::nullopt; }
};

} // namespace beman::monadics::detail

namespace beman::monadics::detail::tests {

TEMPLATE_TEST_CASE("identical", "", (Box<int, double>), (std::optional<int>)) {
    STATIC_REQUIRE(same_box<TestType, TestType>);
}

TEST_CASE("rebind-value-type") {
    STATIC_REQUIRE(same_box<Box<float, double>, Box<int, double>>);
    STATIC_REQUIRE(same_box<std::optional<float>, std::optional<int>>);
}

TEST_CASE("rebinding-error-type") { STATIC_REQUIRE(same_box<Box<int, float>, Box<int, double>>); }

TEST_CASE("non-box") {
    STATIC_REQUIRE_FALSE(same_box<int, int>);
    STATIC_REQUIRE_FALSE(same_box<int, std::optional<int>>);
    STATIC_REQUIRE_FALSE(same_box<Box<int, double>, int>);
}

} // namespace beman::monadics::detail::tests
