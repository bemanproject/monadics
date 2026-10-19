// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_INVOKE_WITH_ERROR_HPP
#define BEMAN_MONADICS_DETAIL_INVOKE_WITH_ERROR_HPP

#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/unit.hpp>
#include <beman/monadics/detail/utility.hpp>

#include <concepts>
#include <type_traits>
#include <utility>

namespace beman::monadics::detail {

template<typename Fn, typename Box, typename Traits = get_box_traits<Box>>
concept invocable_with_error =
    requires { std::declval<Fn>()(Traits::error(std::declval<Box>())); }
    || (std::same_as<typename Traits::error_type, unit> && std::invocable<Fn>)
    || on_error<"the handler must accept the box's error, or take nothing where the failure carries nothing">;

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

namespace _invoke_with_error {

template<typename Fn, typename Box, typename Traits = get_box_traits<Box>>
[[nodiscard]] consteval decltype(auto) result() noexcept {
    if constexpr (requires { std::declval<Fn>()(Traits::error(std::declval<Box>())); }) {
        return std::type_identity<decltype(std::declval<Fn>()(Traits::error(std::declval<Box>())))>{};
    } else {
        return std::type_identity<decltype(std::declval<Fn>()())>{};
    }
}

} // namespace _invoke_with_error

template<typename Fn, typename Box>
    requires invocable_with_error<Fn, Box>
using invoke_with_error_t = typename decltype(_invoke_with_error::result<Fn, Box>())::type;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_INVOKE_WITH_ERROR_HPP
