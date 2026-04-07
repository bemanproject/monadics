// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_INVOKE_WITH_VALUE_HPP
#define BEMAN_MONADICS_DETAIL_INVOKE_WITH_VALUE_HPP

#include <beman/monadics/detail/get_box_traits.hpp>

#include <concepts>
#include <utility>

namespace beman::monadics::detail {

template<typename Fn, box Box, typename Traits = get_box_traits<Box>>
[[nodiscard]] constexpr decltype(auto) invoke_with_value(Fn&& fn, Box&& box) noexcept
    requires requires {
        { Traits::value(std::forward<Box>(box)) } -> std::same_as<void>;
        { fn() };
    } || requires {
        { fn(Traits::value(std::forward<Box>(box))) };
    }
{
    if constexpr (std::is_void_v<typename Traits::value_type> && std::invocable<Fn>) {
        // should just invoke Traits::value(box);
        return std::forward<Fn>(fn)();
    } else {
        return std::forward<Fn>(fn)(Traits::value(std::forward<Box>(box)));
    }
}

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_INVOKE_WITH_VALUE_HPP
