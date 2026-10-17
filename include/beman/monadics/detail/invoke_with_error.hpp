// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_INVOKE_WITH_ERROR_HPP
#define BEMAN_MONADICS_DETAIL_INVOKE_WITH_ERROR_HPP

#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/unit.hpp>

#include <concepts>
#include <utility>

namespace beman::monadics::detail {

template<typename Fn, typename Box, typename Traits = get_box_traits<Box>>
concept invocable_with_error = requires { std::declval<Fn>()(Traits::error(std::declval<Box>())); }
                            || (std::same_as<typename Traits::error_type, unit> && std::invocable<Fn>);

template<typename Fn, box Box>
[[nodiscard]] constexpr decltype(auto) invoke_with_error(Fn&& fn, Box&& box)
    requires invocable_with_error<decltype(fn), decltype(box)>
{
    using Traits = get_box_traits<Box>;
    if constexpr (requires { std::forward<Fn>(fn)(Traits::error(std::forward<Box>(box))); }) {
        return std::forward<Fn>(fn)(Traits::error(std::forward<Box>(box)));
    } else {
        return std::forward<Fn>(fn)();
    }
}

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_INVOKE_WITH_ERROR_HPP
