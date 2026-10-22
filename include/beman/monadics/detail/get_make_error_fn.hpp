// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_GET_MAKE_ERROR_FN_HPP
#define BEMAN_MONADICS_DETAIL_GET_MAKE_ERROR_FN_HPP

#include <beman/monadics/detail/never.hpp>
#include <beman/monadics/detail/unit.hpp>
#include <beman/monadics/detail/unreachable.hpp>
#include <beman/monadics/detail/utility.hpp>

#include <concepts>
#include <utility>

namespace beman::monadics::detail {

template<typename Traits, typename E>
concept supplies_make_error = requires { Traits::make_error(std::declval<E>()); };

template<typename Box, typename Traits, typename E>
[[nodiscard]] consteval decltype(auto) get_make_error_fn() noexcept {
    if constexpr (std::same_as<E, never>) {
        return [](never) -> Box {
            unreachable();
        };
    } else if constexpr (supplies_make_error<Traits, E>) {
        return [](auto&& e)
            requires requires { Traits::make_error(std::forward<decltype(e)>(e)); }
        {
            return Traits::make_error(std::forward<decltype(e)>(e));
        };
    } else if constexpr (!std::constructible_from<Box, E> && std::constructible_from<Box, E&>) {
        return [](auto& e)
            requires requires { Box{e}; }
        {
            return Box{e};
        };
    } else if constexpr (std::constructible_from<Box, E>) {
        return [](auto&& e)
            requires requires { Box{std::forward<decltype(e)>(e)}; }
        {
            return Box{std::forward<decltype(e)>(e)};
        };
    } else if constexpr (std::same_as<E, unit> && std::default_initializable<Box>) {
        return [](unit) {
            return Box{};
        };
    }
}

template<typename Box, typename Traits, typename E>
concept has_make_error_fn =
    (requires {
        { get_make_error_fn<Box, Traits, E>() } -> deduced;
    } || on_error<"provide Traits::make_error(E) or a Box{E} constructor">)
    && (!std::same_as<E, never>
        || !supplies_make_error<Traits, E>
        || on_error<"an error named never is synthesized, so box_traits must not supply make_error">);

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_GET_MAKE_ERROR_FN_HPP
