// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_OR_ELSE_HPP
#define BEMAN_MONADICS_DETAIL_OR_ELSE_HPP

#include <beman/monadics/detail/pipe.hpp>

#include <type_traits>

namespace beman::monadics::detail {

namespace _or_else {

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
        using NewBox = decltype(Traits::invoke_with_error(std::forward<Fn>(fn), std::forward<Box>(box)));
        // using NewBox    = invoke_result_t<decltype(std::forward<Fn>(fn)), decltype(std::forward<Box>(box)), Traits>;
        using NewBoxTraits = box_traits_for<NewBox>;

        if (!Traits::has_value(box)) {
            return Traits::invoke_with_error(std::forward<Fn>(fn), std::forward<Box>(box));
        }

        return NewBoxTraits::lift_with_value(std::forward<Box>(box));

        // if (!std::invoke(&BoxTraits::has_value, box)) {
        // if (!Traits::has_value(box)) {
        // return Traits::template invoke_with<&Traits::error>();

        // if constexpr (requires { BoxTraits::error(std::forward<Box>(box)); }) {
        // // if constexpr (std::is_void_v<typename BoxTraits::value_type>) {
        // return std::forward<Fn>(fn)(BoxTraits::error(std::forward<Box>(box)));
        // } else {
        // return std::forward<Fn>(fn)();
        // }
        // }

        // return NewBoxTraits::lift_with<&Traits::value>(std::forward<Box>(box));
        // if constexpr (requires {
        // { BoxTraits::value(std::forward<Box>(box)) } -> std::same_as<void>;
        // }) {
        // return NewBoxTraits::lift();
        // } else {
        // return NewBoxTraits::lift(BoxTraits::value(std::forward<Box>(box)));
        // // return NewBoxTraits::lift(BoxTraits::value());
        // }
    }
};

} // namespace _or_else

inline constexpr pipe_for<_or_else::op_fn> or_else{};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_OR_ELSE_HPP
