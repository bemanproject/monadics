// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_GET_REBIND_ERROR_HPP
#define BEMAN_MONADICS_DETAIL_GET_REBIND_ERROR_HPP

#include <beman/monadics/detail/get_error_type.hpp>
#include <beman/monadics/detail/instance_of.hpp>
#include <beman/monadics/detail/meta_rebind_error.hpp>
#include <beman/monadics/detail/never.hpp>
#include <beman/monadics/detail/unit.hpp>
#include <beman/monadics/detail/utility.hpp>

#include <concepts>
#include <type_traits>

namespace beman::monadics::detail {

template<typename Box>
struct unit_error_only {
    template<std::same_as<unit> E>
    using rebind_error = Box;
};

template<typename Box, typename Traits>
concept error_is_never = requires { requires std::same_as<get_error_type_t<Box, Traits>, never>; };

template<typename Box>
struct never_error_only {
    template<std::same_as<never> E>
    using rebind_error = Box;
};

template<typename Box, typename Traits, typename E>
[[nodiscard]] consteval decltype(auto) get_rebind_error() noexcept {
    if constexpr (std::same_as<E, never> && error_is_never<Box, Traits>) {
        return std::type_identity<never_error_only<Box>>{};
    } else if constexpr (requires { typename Traits::template rebind_error<E>; }) {
        return std::type_identity<Traits>{};
    } else if constexpr (requires { typename Box::template rebind_error<E>; }) {
        return std::type_identity<Box>{};
    } else if constexpr (requires { get_meta_rebind_error<Box>(); }) {
        return get_meta_rebind_error<Box>();
    } else if constexpr (std::same_as<E, unit>) {
        return std::type_identity<unit_error_only<Box>>{};
    }
}

template<typename Box, typename Traits, typename E>
concept has_rebind_error =
    requires {
        { get_rebind_error<Box, Traits, E>() } -> instance_of<std::type_identity>;
    }
    || on_error<
        "provide Traits::template rebind_error<E>, Box::template rebind_error<E>, or a deducible template parameter"
    >;

template<typename Box, typename Traits, typename E>
    requires has_rebind_error<Box, Traits, E>
using get_rebind_error_t = typename decltype(get_rebind_error<Box, Traits, E>())::type;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_GET_REBIND_ERROR_HPP
