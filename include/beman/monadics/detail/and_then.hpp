// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_AND_THEN_HPP
#define BEMAN_MONADICS_DETAIL_AND_THEN_HPP

#include <beman/monadics/detail/pipe.hpp>

#include <concepts>
#include <type_traits>
#include <functional>
#include <utility>

namespace beman::monadics::detail {

namespace _and_then {

template <typename Fn, typename Value>
[[nodiscard]] consteval decltype(auto) invoke_result() noexcept {
    if constexpr (std::is_void_v<Value>) {
        return std::type_identity<std::invoke_result_t<Fn>>{};
    } else if constexpr (!std::is_void_v<Value>) {
        return std::type_identity<std::invoke_result_t<Fn, Value>>{};
    }
}

template <typename Fn, typename Value>
[[nodiscard]] consteval decltype(auto) invoke() noexcept {
    if constexpr (std::is_void_v<Value>) {
        return std::type_identity<std::invoke_result_t<Fn>>{};
    } else if constexpr (!std::is_void_v<Value>) {
        return std::type_identity<std::invoke_result_t<Fn, Value>>{};
    }
}

template <typename Fn, typename Value>
concept invocable = requires {
    requires std::is_void_v<Value>;
    requires std::invocable<Fn>;
} || requires {
    requires !std::is_void_v<Value>;
    requires std::invocable<Fn, Value>;
};

template <typename Fn, typename Value>
// requires requires {
// requires std::is_void_v<Value>;
// requires std::invocable<Fn>;
// } || requires {
// requires !std::is_void_v<Value>;
// requires std::invocable<Fn, Value>;
// }
using invoke_result_t = decltype(invoke_result<Fn, Value>())::type;

// template<typename T, typename Trait>
// concept rebindable = requires {
// typename Trait::template rebind<T>;
// requires detail::same_template<
// typename Trait::template rebind<T>,
// typename Trait::box_type
// >;
// requires std::same_as<
// typename box_traits_for<typename Trait::template rebind<T>>::error_type,
// typename Trait::error_type
// >;
// };

// template <typename Traits, typename Fn, typename Box>
// [[nodiscard]] constexpr decltype(auto) invoke_with_value();

struct op_fn {
    template <typename Traits, typename Box, typename Fn>
    // requires requires {
    // { invoke_result<Fn, typename Traits::value_type>() } -> instance_of<std::type_identity>;
    // typename box_traits_for<invoke_result_t<Fn, typename Traits::value_type>>;
    // // requires same_unqualified_as<
    // // Box,
    // // typename box_traits_for<invoke_result_t<Fn, typename Traits::value_type>>::template rebind<typename
    // Traits::value_type>>
    // // >;
    // // requires same_template<Box, invoke_result_t<Fn, typename Traits::value_type>>;
    // requires std::same_as<
    // typename Traits::error_type,
    // typename box_traits_for<invoke_result_t<Fn, typename Traits::value_type>>::error_type>;
    // // requires std::invocable<Fn, typename Traits::value_type>; // cater for value_type void
    // [>
    // requires std::invoke_result_t<Fn, typename Traits::value_type>;
    // requires same_template<
    // Box,
    // std::invoke_result_t<Fn, typename Traits::value_type
    // >
    // */
    // }
    [[nodiscard]] inline constexpr decltype(auto) operator()(Box&& box, Fn&& fn) const noexcept {
        using NewBox =
            invoke_result_t<decltype(std::forward<Fn>(fn)), decltype(Traits::value(std::forward<Box>(box)))>;
        using NewBoxTraits = box_traits_for<NewBox>;

        if (Traits::has_value(box)) {
            return Traits::invoke_with_value(std::forward<Fn>(fn), std::forward<Box>(box));
        }

        return NewBoxTraits::lift_with_error(std::forward<Box>(box));
    }
};

} // namespace _and_then

inline constexpr pipe_for<_and_then::op_fn> and_then{};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_AND_THEN_HPP
