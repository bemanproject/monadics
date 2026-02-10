// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_DEDUCE_MAKE_FN_HPP
#define BEMAN_MONADICS_DETAIL_DEDUCE_MAKE_FN_HPP

#include <beman/monadics/detail/instance_of.hpp>

#include <concepts>
#include <utility>

namespace beman::monadics::detail {

namespace _deduce_make_fn {

template <typename Box, typename Traits, typename T>
consteval auto deduce() noexcept {
    if constexpr (requires { Traits::make(std::declval<T>()); }) {
        return [](auto&& v) { return Traits::make(std::forward<decltype(v)>(v)); };
    } else if constexpr (requires { &Traits::make; }) {
        // return &Traits::make;
        return [](auto& v) { return Traits::make(std::forward<decltype(v)>(v)); };
    } else if constexpr (std::is_void_v<T>) {
        return []() { return Box{}; };
    } else if constexpr (std::constructible_from<Box, T>) {
        return [](auto&& v) { return Box{std::forward<decltype(v)>(v)}; };
    }
};

} // namespace _deduce_make_fn

template <typename Box, typename Traits, typename T>
concept has_make_fn = requires {
    requires requires {
        { _deduce_make_fn::deduce<Box, Traits, T>() } -> std::invocable<T&>; // T& for raw_ptr value
        // { deduce_make_fn<Box, Traits, T>()(std::declval<T>()) } -> same_unqualified_as<Box>;
        // { deduce_make_fn<Box, Traits, T>()(std::declval<T>()) } -> same_unqualified_as<Box>;
    } || requires {
        requires std::is_void_v<T>;
        { _deduce_make_fn::deduce<Box, Traits, T>() } -> std::invocable;
        // { deduce_make_fn<Box, Traits, T>() } -> same_unqualified_as<Box>;
    };
};

template <typename Box, typename Traits, typename T>
    requires has_make_fn<Box, Traits, T>
inline constexpr auto deduce_make_fn = _deduce_make_fn::deduce<Box, Traits, T>();

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_DEDUCE_MAKE_FN_HPP
