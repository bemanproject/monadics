// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_DEDUCE_MAKE_ERROR_FN_HPP
#define BEMAN_MONADICS_DETAIL_DEDUCE_MAKE_ERROR_FN_HPP

#include <beman/monadics/detail/instance_of.hpp>
#include <beman/monadics/detail/same_unqualified_as.hpp>

#include <concepts>
#include <utility>

namespace beman::monadics::detail {

namespace _deduce_make_error_fn {

template <typename Box, typename Traits, typename E>
consteval auto deduce() noexcept {
    if constexpr (requires { Traits::make_error(std::declval<E>()); }) {
        return [](auto&& e) { return Traits::make_error(std::forward<decltype(e)>(e)); };
    } else if constexpr (std::constructible_from<Box, E>) {
        return [](auto&& e) { return Box{std::forward<decltype(e)>(e)}; };
    }
};

} // namespace _deduce_make_error_fn

template <typename Box, typename Traits, typename E>
concept has_make_error_fn = requires {
    requires requires {
        { _deduce_make_error_fn::deduce<Box, Traits, E>() } -> std::invocable<E>;
        { _deduce_make_error_fn::deduce<Box, Traits, E>()(std::declval<E>()) } -> same_unqualified_as<Box>;
    } || requires {
        { _deduce_make_error_fn::deduce<Box, Traits, E>() } -> std::invocable;
        { _deduce_make_error_fn::deduce<Box, Traits, E>()() } -> same_unqualified_as<Box>;
    };
};

template <typename Box, typename Traits, typename E>
    requires has_make_error_fn<Box, Traits, E>
inline constexpr auto deduce_make_error_fn = _deduce_make_error_fn::deduce<Box, Traits, E>();

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_DEDUCE_MAKE_ERROR_FN_HPP
