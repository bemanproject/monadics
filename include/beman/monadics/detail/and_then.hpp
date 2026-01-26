// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_AND_THEN_HPP
#define BEMAN_MONADICS_DETAIL_AND_THEN_HPP

// #include "beman/monadics/detail/same_box.hpp"
#include "beman/monadics/detail/deduce_box_traits.hpp"
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

template <typename NewBox,
          typename Traits,
          typename NewBoxTraits = box_traits_for<NewBox>>
concept same_box = requires {
  requires std::same_as<
    typename NewBoxTraits::template rebind<typename Traits::value_type>,
    typename Traits::box_type
  >;
};

struct and_then_t {
    // template <typename Traits, typename Box, typename Fn>
    // [[nodiscard]] inline constexpr decltype(auto) operator()(Box&& box, Fn&& fn) const noexcept
      // requires requires {
        // { Traits::value(std::forward<Box>(box)) } -> std::same_as<void>;
        // { std::forward<Fn>(fn)() } -> same_box<Traits>;
      // } || requires {
        // { std::forward<Fn>(fn)(Traits::value(std::forward<Box>(box))) } -> same_box<Traits>;
      // }
    // {

        // using NewBox =
            // invoke_result_t<decltype(std::forward<Fn>(fn)), decltype(Traits::value(std::forward<Box>(box)))>;
        // using NewBoxTraits = box_traits_for<NewBox>;

        // if (Traits::has_value(box)) {
            // return Traits::invoke_with_value(std::forward<Fn>(fn), std::forward<Box>(box));
        // }

        // return NewBoxTraits::lift_with_error(std::forward<Box>(box));
    // }

    template <typename Fn>
    struct action {
        Fn fn;

        template <is_box Box, same_unqualified_as<action> A, typename Traits = box_traits_for<Box>>
        [[nodiscard]] friend inline constexpr decltype(auto) operator|(Box&& box, A&& a) noexcept
          requires requires {
            { Traits::value(std::forward<Box>(box)) } -> std::same_as<void>;
            { std::forward<A>(a).fn() } -> same_box<Traits>;
          } || requires {
            { std::forward<A>(a).fn(Traits::value(std::forward<Box>(box))) } -> same_box<Traits>;
          }
        {
            using NewBox =
                invoke_result_t<decltype(std::forward<A>(a).fn), decltype(Traits::value(std::forward<Box>(box)))>;
            using NewBoxTraits = box_traits_for<NewBox>;

            if (Traits::has_value(box)) {
                return Traits::invoke_with_value(std::forward<A>(a).fn, std::forward<Box>(box));
            }

            return NewBoxTraits::lift_with_error(std::forward<Box>(box));
        }
    };

    template <typename Fn>
    [[nodiscard]] constexpr decltype(auto) operator()(Fn&& fn) const noexcept {
        return action<decltype(fn)>{std::forward<Fn>(fn)};
    }

};

} // namespace _and_then

inline constexpr _and_then::and_then_t and_then{};
// inline constexpr pipe_for<_and_then::op_fn> and_then{};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_AND_THEN_HPP
