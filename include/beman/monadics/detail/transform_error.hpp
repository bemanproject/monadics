// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_TRANSFORM_ERROR_HPP
#define BEMAN_MONADICS_DETAIL_TRANSFORM_ERROR_HPP

#include <beman/monadics/detail/or_else.hpp>

#include <type_traits>
#include <utility>

namespace beman::monadics::detail {

namespace _transform_error {

template <typename Fn, typename Box, typename BoxTraits>
consteval auto invoke_result() {

    if constexpr (requires { BoxTraits::error(std::declval<Box>()); }) {
        return std::type_identity<std::invoke_result_t<Fn, decltype(BoxTraits::error(std::declval<Box>()))>>{};
    } else {
        return std::type_identity<std::invoke_result_t<Fn>>{};
    }
}

template <typename Fn, typename Box, typename BoxTraits>
    requires requires {
        { BoxTraits::error() };
        // requires std::invocable<decltype(BoxTraits::error)>;
        // requires std::invocable<Fn>;
    } || requires {
        { BoxTraits::error(std::declval<Box>()) };
        // requires std::invocable<decltype(BoxTraits::error), Box>;
        // requires std::invocable<Fn, Value>;
    }
using invoke_result_t = decltype(invoke_result<Fn, Box, BoxTraits>())::type;

struct op_fn {
    template <typename Traits, typename Box, typename Fn>
    [[nodiscard]] inline constexpr auto operator()(Box&& box, Fn&& fn) const noexcept {
        using NewError     = invoke_result_t<decltype(std::forward<Fn>(fn)), decltype(std::forward<Box>(box)), Traits>;
        using NewBoxTraits = box_traits_for<typename Traits::template rebind_error<NewError>>;

        // transform_error does not make sense if you don't have error
        return std::forward<Box>(box)
             | or_else([&fn](auto&& v) { return NewBoxTraits::lift_error(fn(std::forward<decltype(v)>(v))); });
    }
};

} // namespace _transform_error

inline constexpr pipe_for<_transform_error::op_fn> transform_error{};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_TRANSFORM_ERROR_HPP
