// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_INVOKE_WITH_VALUE_HPP
#define BEMAN_MONADICS_DETAIL_INVOKE_WITH_VALUE_HPP

#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/utility.hpp>

#include <concepts>
#include <utility>

namespace beman::monadics::detail {

template<typename Fn, typename Box, typename Traits = get_box_traits<Box>>
concept invocable_with_value = requires {
    requires std::is_void_v<typename Traits::value_type>;
    requires std::invocable<Fn>;
} || requires {
    { std::declval<Fn>()(Traits::value(std::declval<Box>())) };
} || on_error<"the handler must accept the box's value, or take nothing where it is void">;

template<typename Fn, box Box>
[[nodiscard]] constexpr decltype(auto) invoke_with_value(Fn&& fn, Box&& box)
    requires invocable_with_value<decltype(fn), decltype(box)>
{
    using Traits = get_box_traits<Box>;
    if constexpr (std::is_void_v<typename Traits::value_type> && std::invocable<Fn>) {
        // should just invoke Traits::value(box);
        return std::forward<Fn>(fn)();
    } else {
        return std::forward<Fn>(fn)(Traits::value(std::forward<Box>(box)));
    }
}

template<typename Fn, typename Box>
    requires invocable_with_value<Fn, Box>
using invoke_with_value_t = decltype(invoke_with_value(std::declval<Fn>(), std::declval<Box>()));

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_INVOKE_WITH_VALUE_HPP
