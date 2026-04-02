// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_GET_MAKE_FN_HPP
#define BEMAN_MONADICS_DETAIL_GET_MAKE_FN_HPP

#include <beman/monadics/detail/utility.hpp>

#include <concepts>
#include <utility>

namespace beman::monadics::detail {

template <typename Box, typename Traits, typename T>
[[nodiscard]] consteval decltype(auto) get_make_fn() noexcept {
    if constexpr (requires { Traits::make(std::declval<T>()); }) {
        return [](auto&& v) -> decltype(auto) {
            return Traits::make(std::forward<decltype(v)>(v));
        };
    } else if constexpr (requires { Traits::make(std::declval<T&>()); }) {
        return [](auto& v) -> decltype(auto) {
            return Traits::make(v);
        };
    } else if constexpr (std::is_void_v<T>) {
        return []() -> decltype(auto) {
            return Box{};
        };
    } else if constexpr (std::constructible_from<Box, T>) {
        return [](auto&& v) -> decltype(auto) {
            return Box{std::forward<decltype(v)>(v)};
        };
    }
}

template <typename Box, typename Traits, typename T>
concept has_make_fn = requires {
    { get_make_fn<Box, Traits, T>() } -> deduced;
} || on_error<"provide Traits::make(T) or a Box{T} constructor">;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_GET_MAKE_FN_HPP
