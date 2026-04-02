// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/invoke_with_error.hpp>

#include <catch2/catch_test_macros.hpp>

#include <utility>

template <typename T, typename E>
struct Box {
    bool has_val;
    T val{};
    E err{};
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

    static constexpr bool has_value(const auto& b) noexcept { return b.has_val; }

    static constexpr decltype(auto) value(auto&& b) { return std::move(b.val); }

    static constexpr decltype(auto) error(auto&& b) { return std::move(b.err); }

    static constexpr Box<T, E> make(T v) { return {true, std::move(v), {}}; }

    static constexpr Box<T, E> make_error(E e) { return {false, {}, std::move(e)}; }
};

} // namespace beman::monadics::detail

namespace beman::monadics::detail::tests {

TEST_CASE("with-error-channel") {
    constexpr auto result = []() {
        Box<int, double> box{false, 0, 3.5};
        return invoke_with_error([](double e) -> int { return e * 2; }, std::move(box));
    }();
    STATIC_REQUIRE(result == 7);
}

} // namespace beman::monadics::detail::tests
