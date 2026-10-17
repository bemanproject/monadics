// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_GET_ERROR_FN_HPP
#define BEMAN_MONADICS_DETAIL_GET_ERROR_FN_HPP

#include <beman/monadics/detail/unit.hpp>
#include <beman/monadics/detail/utility.hpp>

#include <concepts>
#include <utility>

namespace beman::monadics::detail {

template<typename Box, typename Traits, typename E>
[[nodiscard]] consteval decltype(auto) get_error_fn() noexcept {
    if constexpr (requires { Traits::error(std::declval<Box>()); }) {
        return [](auto&& b) -> decltype(auto) {
            return Traits::error(std::forward<decltype(b)>(b));
        };
    } else if constexpr (requires { std::declval<Box>().error(); }) {
        return [](auto&& b) -> decltype(auto) {
            return std::forward<decltype(b)>(b).error();
        };
    } else if constexpr (std::same_as<E, unit>) {
        return [](auto&&) {
            return unit{};
        };
    }
}

template<typename Box, typename Traits, typename E>
concept has_error_fn = requires {
    { get_error_fn<Box, Traits, E>() } -> deduced;
} || on_error<"provide Traits::error(Box) or Box::error(); a box storing nothing there needs neither">;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_GET_ERROR_FN_HPP
