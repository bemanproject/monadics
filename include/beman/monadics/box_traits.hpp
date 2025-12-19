// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_BOX_TRAITS_HPP
#define BEMAN_MONADICS_BOX_TRAITS_HPP

#include <concepts>
#include <type_traits>
#include <utility>
#include <functional>

#include <beman/monadics/detail/as_pointer.hpp>
#include <beman/monadics/detail/decomposable.hpp>
#include <beman/monadics/detail/deduce_error_fn.hpp>
#include <beman/monadics/detail/deduce_error_type.hpp>
#include <beman/monadics/detail/deduce_lift_error_fn.hpp>
#include <beman/monadics/detail/deduce_lift_fn.hpp>
#include <beman/monadics/detail/deduce_rebind.hpp>
#include <beman/monadics/detail/deduce_rebind_error.hpp>
#include <beman/monadics/detail/deduce_value_fn.hpp>
#include <beman/monadics/detail/deduce_value_query_fn.hpp>
#include <beman/monadics/detail/deduce_value_type.hpp>
#include <beman/monadics/detail/instance_of.hpp>
#include <beman/monadics/detail/same_template.hpp>
#include <beman/monadics/detail/same_unqualified_as.hpp>

namespace beman::monadics {

namespace detail::_box_traits {

template <typename Box, typename Traits>
concept is_box = requires {
    requires has_value_type<Box, Traits>;
    requires has_error_type<Box, Traits>;
    requires has_rebind<Box, Traits, deduce_value_type<Box, Traits>>;
    requires has_rebind_error<Box, Traits, deduce_error_type<Box, Traits>>;

    requires has_value_query_fn<Box, Traits>;
    requires has_value_fn<Box, Traits>;
    requires has_error_fn<Box, Traits>;

    requires has_lift_fn<Box, Traits, deduce_value_type<Box, Traits>>;
    requires has_lift_error_fn<Box, Traits, deduce_error_type<Box, Traits>>;
};

template <typename Box, typename Traits>
struct deduce_traits {
    using box_type   = Box;
    using value_type = deduce_value_type<Box, Traits>;
    using error_type = deduce_error_type<Box, Traits>;

    template <typename T>
    using rebind = deduce_rebind<Box, Traits, value_type>::template rebind<T>;

    template <typename E>
    using rebind_error = deduce_rebind_error<Box, Traits, error_type>::template rebind_error<E>;

    inline static constexpr auto has_value = deduce_value_query_fn<Box, Traits>;
    inline static constexpr auto value     = deduce_value_fn<Box, Traits>;
    inline static constexpr auto error     = deduce_error_fn<Box, Traits>;

    inline static constexpr auto lift       = deduce_lift_fn<Box, Traits, value_type>;
    inline static constexpr auto lift_error = deduce_lift_error_fn<Box, Traits, error_type>;

    template <typename Fn, typename B>
    // requires requires {
    // requires std::is_void_v<value_type>;
    // requires std::invocable<Fn>;
    // } || requires { requires std::invocable<Fn, value_type>; }
    [[nodiscard]] static constexpr decltype(auto) invoke_with_value(Fn&& fn, B&& box) noexcept {
        if constexpr (std::is_void_v<value_type> && std::invocable<Fn>) {
            // should just invoke Traits::value(box);
            return std::forward<Fn>(fn)();
        } else {
            return std::forward<Fn>(fn)(value(std::forward<B>(box)));
        }
    }

    template <typename Fn, typename B>
        requires requires {
            { error(std::declval<Box>()) };
            requires std::invocable<Fn, error_type>;
        } || requires { requires std::invocable<Fn>; }
    [[nodiscard]] static constexpr decltype(auto) invoke_with_error(Fn&& fn, B&& box) noexcept {
        if constexpr (requires { error(std::forward<B>(box)); }) {
            return std::forward<Fn>(fn)(error(std::forward<B>(box)));
        } else {
            return std::forward<Fn>(fn)();
        }
    }

    template <typename B>
    [[nodiscard]] static constexpr decltype(auto) lift_with_value(B&& box) noexcept {
        if constexpr (std::is_void_v<value_type>) {
            return lift();
        } else {
            return lift(value(std::forward<B>(box)));
        }
    }

    template <typename B>
    [[nodiscard]] static constexpr decltype(auto) lift_with_error(B&& box) noexcept {
        if constexpr (requires { error(std::forward<B>(box)); }) {
            return lift_error(error(std::forward<B>(box)));
            // } else if constexpr (requires { error(); }) {
            // return lift_error();
        } else {
            return lift_error(error());
        }
    }
};

} // namespace detail::_box_traits

template <typename T>
struct box_traits {
    using box_type = T;
};

template <typename Box>
concept is_box = requires {
    typename box_traits<std::remove_cvref_t<Box>>;
    requires detail::_box_traits::is_box<std::remove_cvref_t<Box>, // maybe should preserve qualifiers?
                                         box_traits<std::remove_cvref_t<Box>>>;
};

template <is_box T>
using box_traits_for = detail::_box_traits::deduce_traits<std::remove_cvref_t<T>, box_traits<std::remove_cvref_t<T>>>;

} // namespace beman::monadics

#endif // BEMAN_MONADICS_BOX_TRAITS_HPP
