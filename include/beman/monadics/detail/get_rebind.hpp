// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_GET_REBIND_HPP
#define BEMAN_MONADICS_DETAIL_GET_REBIND_HPP

#include <beman/monadics/detail/get_value_type.hpp>
#include <beman/monadics/detail/instance_of.hpp>
#include <beman/monadics/detail/meta_rebind.hpp>
#include <beman/monadics/detail/never.hpp>
#include <beman/monadics/detail/utility.hpp>

#include <concepts>
#include <type_traits>

namespace beman::monadics::detail {

template<typename Box, typename Traits>
concept value_is_never = requires { requires std::same_as<get_value_type_t<Box, Traits>, never>; };

template<typename Box>
struct never_value_only {
    template<std::same_as<never> T>
    using rebind = Box;
};

template<typename Box, typename Traits, typename T>
[[nodiscard]] consteval decltype(auto) get_rebind() noexcept {
    if constexpr (std::same_as<T, never> && value_is_never<Box, Traits>) {
        return std::type_identity<never_value_only<Box>>{};
    } else if constexpr (requires { typename Traits::template rebind<T>; }) {
        return std::type_identity<Traits>{};
    } else if constexpr (requires { typename Box::template rebind<T>; }) {
        return std::type_identity<Box>{};
    } else if constexpr (requires { get_meta_rebind<Box>(); }) {
        return get_meta_rebind<Box>();
    }
}

template<typename Box, typename Traits, typename T>
concept has_rebind = requires {
    { get_rebind<Box, Traits, T>() } -> instance_of<std::type_identity>;
} || on_error<"provide Traits::template rebind<T>, Box::template rebind<T>, or a deducible template parameter">;

template<typename Box, typename Traits, typename T>
    requires has_rebind<Box, Traits, T>
using get_rebind_t = typename decltype(get_rebind<Box, Traits, T>())::type;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_GET_REBIND_HPP
