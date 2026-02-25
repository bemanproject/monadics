// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_DEDUCE_VALUE_HPP
#define BEMAN_MONADICS_DETAIL_DEDUCE_VALUE_HPP

#include <beman/monadics/detail/deduce_value_type.hpp>
#include <beman/monadics/detail/instance_of.hpp>

#include <concepts>
#include <utility>

namespace beman::monadics::detail {

namespace _deduce_value_fn {

template <typename Box, typename Traits>
consteval auto deduce() noexcept {
    if constexpr (requires { Traits::value(std::declval<Box>()); }) {
        return [](auto&& b) -> decltype(Traits::value(std::forward<decltype(b)>(b))) {
            return Traits::value(std::forward<decltype(b)>(b));
        };
    } else if constexpr (requires { std::declval<Box>().value(); }) {
        return [](auto&& b) { return std::forward<decltype(b)>(b).value(); };
    }
};

} // namespace _deduce_value_fn

template <typename Box, typename Traits>
concept has_value_fn = requires {
    { _deduce_value_fn::deduce<Box, Traits>() } -> std::invocable<Box>;
    requires has_value_type<Box, Traits>;
    {
        _deduce_value_fn::deduce<Box, Traits>()(std::declval<Box>())
    } -> std::convertible_to<deduce_value_type<Box, Traits>>;
};

template <typename Box, typename Traits>
    requires has_value_fn<Box, Traits>
inline constexpr auto deduce_value_fn = _deduce_value_fn::deduce<Box, Traits>();

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_DEDUCE_VALUE_HPP
