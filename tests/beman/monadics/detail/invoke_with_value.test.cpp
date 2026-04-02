// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/invoke_with_value.hpp>

#include <catch2/catch_test_macros.hpp>

#include <variant>
#include <utility>

template <typename T, typename E>
struct Box : std::variant<std::conditional_t<std::is_void_v<T>, std::monostate, T>, E> {};

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
    static constexpr decltype(auto) value(auto&& b) {
        if constexpr (std::is_void_v<T>) {
            return;
        } else {
            return std::get<0>(std::forward<decltype(b)>(b));
        }
    }
    static constexpr decltype(auto) error(auto&& b) { return std::get<1>(std::forward<decltype(b)>(b)); }

    static constexpr Box<T, E> make(auto... v) { return {std::move(v)...}; }
    static constexpr Box<T, E> make_error(E e) { return {std::move(e)}; }
};

} // namespace beman::monadics::detail

namespace beman::monadics::detail::tests {

TEST_CASE("with-non-void-value") {
    constexpr auto result = []() {
        Box<int, double> box{7};
        return invoke_with_value([](int v) { return v * 2; }, std::move(box));
    }();
    STATIC_REQUIRE(result == 14);
}

TEST_CASE("with-void-value") {
    constexpr auto result = []() {
        Box<void, double> box{};
        return invoke_with_value([]() { return 10; }, std::move(box));
    }();
    STATIC_REQUIRE(result == 10);
}

} // namespace beman::monadics::detail::tests
