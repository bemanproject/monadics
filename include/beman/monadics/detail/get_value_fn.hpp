// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_GET_VALUE_FN_HPP
#define BEMAN_MONADICS_DETAIL_GET_VALUE_FN_HPP

#include <beman/monadics/detail/utility.hpp>

#include <utility>

namespace beman::monadics::detail {

template<typename Box, typename Traits, typename V>
[[nodiscard]] consteval decltype(auto) get_value_fn() noexcept {
    if constexpr (requires { Traits::value(std::declval<Box>()); }) {
        return [](auto&& b) -> decltype(auto)
                   requires requires { Traits::value(std::forward<decltype(b)>(b)); }
        {
            return Traits::value(std::forward<decltype(b)>(b));
        };
    } else if constexpr (requires { std::declval<Box>().value(); }) {
        return [](auto&& b) -> decltype(auto)
                   requires requires { std::forward<decltype(b)>(b).value(); }
        {
            return std::forward<decltype(b)>(b).value();
        };
    }
}

template<typename Box, typename Traits, typename V>
concept has_value_fn = requires {
    { get_value_fn<Box, Traits, V>() } -> deduced;
} || on_error<"provide Traits::value(Box) or Box::value()">;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_GET_VALUE_FN_HPP
