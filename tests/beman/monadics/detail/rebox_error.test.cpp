// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/rebox_error.hpp>

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

    static constexpr bool           has_value(const Box<T, E>& b) noexcept { return b.index() == 0; }
    static constexpr decltype(auto) value(auto&& b) { return std::get<0>(std::forward<decltype(b)>(b)); }
    static constexpr decltype(auto) error(auto&& b) { return std::get<1>(std::forward<decltype(b)>(b)); }
};

template <typename T>
struct box_traits<std::optional<T>> {
    static constexpr decltype(auto) error() { return std::nullopt; }
};

} // namespace beman::monadics::detail

namespace beman::monadics::detail::tests {

TEST_CASE("with-error-channel") {
    constexpr auto result = []() {
        Box<int, float> src{1.0f};
        return rebox_error<Box<int, double>>(std::move(src));
    }();

    STATIC_REQUIRE(std::get<1>(result) == 1.0);
}

TEST_CASE("without-error-channel") {
    constexpr auto result = []() {
        std::optional<int> src{std::nullopt};
        return rebox_error<std::optional<double>>(std::move(src));
    }();

    STATIC_REQUIRE_FALSE(result.has_value());
}

} // namespace beman::monadics::detail::tests
