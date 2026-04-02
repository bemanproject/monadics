// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_GET_ERROR_TYPE_HPP
#define BEMAN_MONADICS_DETAIL_GET_ERROR_TYPE_HPP

#include <beman/monadics/detail/instance_of.hpp>
#include <beman/monadics/detail/utility.hpp>

#include <type_traits>

namespace beman::monadics::detail {

template<typename Box, typename Traits>
[[nodiscard]] consteval decltype(auto) get_error_type() noexcept {
    if constexpr (requires { typename Traits::error_type; }) {
        return std::type_identity<typename Traits::error_type>{};
    } else if constexpr (requires { typename Box::error_type; }) {
        return std::type_identity<typename Box::error_type>{};
    } else if constexpr (requires { Traits::error(); }) {
        return std::type_identity<std::remove_cvref_t<decltype(Traits::error())>>{};
    }
}

template<typename Box, typename Traits>
concept has_error_type = requires {
    { get_error_type<Box, Traits>() } -> instance_of<std::type_identity>;
} || on_error<"provide Traits::error_type, Box::error_type, or Traits::error()">;

template<typename Box, typename Traits>
    requires has_error_type<Box, Traits>
using get_error_type_t = typename decltype(get_error_type<Box, Traits>())::type;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_GET_ERROR_TYPE_HPP
