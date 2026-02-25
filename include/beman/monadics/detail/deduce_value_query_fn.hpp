// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_DEDUCE_VALUE_QUERY_HPP
#define BEMAN_MONADICS_DETAIL_DEDUCE_VALUE_QUERY_HPP

#include <beman/monadics/detail/instance_of.hpp>

#include <concepts>
#include <utility>

namespace beman::monadics::detail {

namespace _deduce_value_query_fn {

template <typename Box, typename Trait>
consteval auto deduce() noexcept {
    if constexpr (requires { Trait::has_value(std::declval<Box>()); }) {
        return [](const auto& box) { return Trait::has_value(box); };
    } else if constexpr (requires { std::declval<Box>().has_value(); }) {
        return [](const auto& box) { return box.has_value(); };
    }
};

} // namespace _deduce_value_query_fn

template <typename Box, typename Traits>
concept has_value_query_fn = requires {
    { _deduce_value_query_fn::deduce<Box, Traits>() } -> std::invocable<Box>;
    { _deduce_value_query_fn::deduce<Box, Traits>()(std::declval<Box>()) } -> std::same_as<bool>;
};

template <typename Box, typename Traits>
    requires has_value_query_fn<Box, Traits>
inline constexpr auto deduce_value_query_fn = _deduce_value_query_fn::deduce<Box, Traits>();

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_DEDUCE_VALUE_QUERY_HPP
