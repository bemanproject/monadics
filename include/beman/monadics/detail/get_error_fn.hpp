// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_GET_ERROR_FN_HPP
#define BEMAN_MONADICS_DETAIL_GET_ERROR_FN_HPP

#include <beman/monadics/detail/utility.hpp>

#include <utility>

namespace beman::monadics::detail {

template <typename Box, typename Traits>
[[nodiscard]] consteval decltype(auto) get_error_fn() noexcept {
    if constexpr (requires { Traits::error(); }) {
        return []() -> decltype(auto) {
            return Traits::error();
        };
    } else if constexpr (requires { Traits::error(std::declval<Box>()); }) {
        return [](auto&& b) -> decltype(auto) {
            return Traits::error(std::forward<decltype(b)>(b));
        };
    } else if constexpr (requires { std::declval<Box>().error(); }) {
        return [](auto&& b) -> decltype(auto) {
            return std::forward<decltype(b)>(b).error();
        };
    }
}

template <typename Box, typename Traits>
concept has_error_fn = requires {
    { get_error_fn<Box, Traits>() } -> deduced;
} || on_error<"provide Traits::error(), Traits::error(Box), or Box::error()">;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_GET_ERROR_FN_HPP
