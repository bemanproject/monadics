// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_TRANSFORM_HPP
#define BEMAN_MONADICS_DETAIL_TRANSFORM_HPP

#include <beman/monadics/detail/and_then.hpp>

#include <type_traits>
#include <utility>

namespace beman::monadics::detail {

namespace _transform {

template <typename Fn, typename Value>
consteval auto invoke_result() {
    if constexpr (std::is_void_v<Value>) {
        return std::type_identity<std::invoke_result_t<Fn>>{};
    } else {
        return std::type_identity<std::invoke_result_t<Fn, Value>>{};
    }
}

template <typename Fn, typename Value>
    requires requires {
        requires std::is_void_v<Value>;
        requires std::invocable<Fn>;
    } || requires { requires std::invocable<Fn, Value>; }
using invoke_result_t = decltype(invoke_result<Fn, Value>())::type;

template <typename Fn, typename NewTraits>
struct LiftedFn {
    Fn fn;

    [[nodiscard]] inline constexpr decltype(auto) operator()(auto&&... v) const noexcept {
        if constexpr (std::is_void_v<typename NewTraits::value_type>) {
            std::forward<Fn>(fn)(std::forward<decltype(v)>(v)...);
            return NewTraits::lift();
        } else {
            return NewTraits::lift(std::forward<Fn>(fn)(std::forward<decltype(v)>(v)...));
        }
    }
};

template <typename T>
concept False = false;

struct op_fn {
    template <typename Traits, typename Box, typename Fn>
    [[nodiscard]] inline constexpr auto operator()(Box&& box, Fn&& fn) const noexcept {
        using NewValue  = decltype(Traits::invoke_with_value(std::forward<Fn>(fn), std::forward<Box>(box)));
        using NewTraits = box_traits_for<typename Traits::template rebind<NewValue>>;

        if (Traits::has_value(box)) {
            if constexpr (std::is_void_v<typename NewTraits::value_type>) {
                Traits::invoke_with_value(std::forward<Fn>(fn), std::forward<Box>(box));
                return NewTraits::lift();
            } else {
                return NewTraits::lift(Traits::invoke_with_value(std::forward<Fn>(fn), std::forward<Box>(box)));
            }
        }

        return NewTraits::lift_with_error(std::forward<Box>(box));

        // return std::forward<Box>(box)
        // | and_then([f = std::forward<Fn>(fn)](auto&&... v) mutable -> typename NewTraits::box_type {
        // if constexpr (sizeof...(v) == 1) {
        // return NewTraits::lift(std::forward<decltype(f)>(f)(std::forward<decltype(v)>(v)...));
        // } else if constexpr (std::is_void_v<typename NewTraits::value_type>) {
        // std::forward<decltype(f)>(f)();
        // return NewTraits::lift();
        // } else if constexpr (std::is_void_v<typename Traits::value_type>) {
        // return NewTraits::lift(std::forward<decltype(f)>(f)());
        // } else {
        // static_assert(False<Box>, "something went wrong");
        // }
        // });
    }
};

} // namespace _transform

inline constexpr pipe_for<_transform::op_fn> transform{};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_TRANSFORM_HPP
