// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_GET_REBIND_HPP
#define BEMAN_MONADICS_DETAIL_GET_REBIND_HPP

#include <beman/monadics/detail/instance_of.hpp>
#include <beman/monadics/detail/meta_rebind_value.hpp>
#include <beman/monadics/detail/utility.hpp>

#include <type_traits>

namespace beman::monadics::detail {

template <typename Box, typename Traits, typename T>
[[nodiscard]] consteval decltype(auto) get_rebind() noexcept {
    if constexpr (requires { typename Traits::template rebind<T>; }) {
        return std::type_identity<Traits>{};
    } else if constexpr (requires { typename Box::template rebind<T>; }) {
        return std::type_identity<Box>{};
    } else if constexpr (requires { get_meta_rebind<Box>(); }) {
        return get_meta_rebind<Box>();
    }
}

template <typename Box, typename Traits, typename T>
concept has_rebind = requires {
    { get_rebind<Box, Traits, T>() } -> instance_of<std::type_identity>;
} || on_error<"provide Traits::template rebind<T>, Box::template rebind<T>, or a deducible template parameter">;

template <typename Box, typename Traits, typename T>
    requires has_rebind<Box, Traits, T>
using get_rebind_t = typename decltype(get_rebind<Box, Traits, T>())::type;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_GET_REBIND_HPP
