// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_GET_MAKE_FN_HPP
#define BEMAN_MONADICS_DETAIL_GET_MAKE_FN_HPP

#include <beman/monadics/detail/never.hpp>
#include <beman/monadics/detail/unreachable.hpp>
#include <beman/monadics/detail/utility.hpp>

#include <concepts>
#include <utility>

namespace beman::monadics::detail {

template<typename Traits, typename T>
concept supplies_make = requires { Traits::make(std::declval<T>()); };

template<typename Box, typename Traits, typename T>
[[nodiscard]] consteval decltype(auto) get_make_fn() noexcept {
    if constexpr (std::same_as<T, never>) {
        return [](never) -> Box {
            unreachable();
        };
    } else if constexpr (std::is_void_v<T>) {
        if constexpr (requires { Traits::make(); }) {
            return []() -> decltype(auto) {
                return Traits::make();
            };
        } else {
            return []() -> decltype(auto) {
                return Box{};
            };
        }
    } else if constexpr (supplies_make<Traits, T>) {
        return [](auto&& v) -> decltype(auto)
                   requires requires { Traits::make(std::forward<decltype(v)>(v)); }
        {
            return Traits::make(std::forward<decltype(v)>(v));
        };
    } else if constexpr (supplies_make<Traits, T&>) {
        return [](auto& v) -> decltype(auto)
                   requires requires { Traits::make(v); }
        {
            return Traits::make(v);
        };
    } else if constexpr (!std::constructible_from<Box, T> && std::constructible_from<Box, T&>) {
        return [](auto& v) -> decltype(auto)
                   requires requires { Box{v}; }
        {
            return Box{v};
        };
    } else if constexpr (std::constructible_from<Box, T>) {
        return [](auto&& v) -> decltype(auto)
                   requires requires { Box{std::forward<decltype(v)>(v)}; }
        {
            return Box{std::forward<decltype(v)>(v)};
        };
    }
}

template<typename Box, typename Traits, typename T>
concept has_make_fn = (requires {
                          { get_make_fn<Box, Traits, T>() } -> deduced;
                      } || on_error<"provide Traits::make(T) or a Box{T} constructor">)
                   && (!std::same_as<T, never>
                       || !supplies_make<Traits, T>
                       || on_error<"a value named never is synthesized, so box_traits must not supply make">);

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_GET_MAKE_FN_HPP
