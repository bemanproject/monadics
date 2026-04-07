// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_INVOKE_WITH_ERROR_HPP
#define BEMAN_MONADICS_DETAIL_INVOKE_WITH_ERROR_HPP

#include <beman/monadics/detail/get_box_traits.hpp>

#include <concepts>
#include <utility>

namespace beman::monadics::detail {

template<typename Fn, typename Box, typename Traits = get_box_traits<Box>>
[[nodiscard]] constexpr decltype(auto) invoke_with_error(Fn&& fn, Box&& box) noexcept
    requires requires {
        { Traits::error(std::forward<Box>(box)) };
        { std::forward<Fn>(fn)(Traits::error(std::forward<Box>(box))) };
    } || requires { requires std::invocable<Fn>; }
{
    if constexpr (requires { Traits::error(std::forward<Box>(box)); }) {
        return std::forward<Fn>(fn)(Traits::error(std::forward<Box>(box)));
    } else {
        return std::forward<Fn>(fn)();
    }
}

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_INVOKE_WITH_ERROR_HPP
