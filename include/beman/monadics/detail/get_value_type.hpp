// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_GET_VALUE_TYPE_HPP
#define BEMAN_MONADICS_DETAIL_GET_VALUE_TYPE_HPP

#include <beman/monadics/detail/instance_of.hpp>
#include <beman/monadics/detail/meta_extract_value_type.hpp>
#include <beman/monadics/detail/utility.hpp>

#include <type_traits>

namespace beman::monadics::detail {

template <typename Box, typename Traits>
[[nodiscard]] consteval decltype(auto) get_value_type() noexcept {
    if constexpr (requires { typename Traits::value_type; }) {
        return std::type_identity<typename Traits::value_type>{};
    } else if constexpr (requires { typename Box::value_type; }) {
        return std::type_identity<typename Box::value_type>{};
    } else if constexpr (requires { meta_extract_value_type<Box>(); }) {
        return meta_extract_value_type<Box>();
    }
}

template <typename Box, typename Traits>
concept has_value_type = requires {
    { get_value_type<Box, Traits>() } -> instance_of<std::type_identity>;
} || on_error<"provide Traits::value_type, Box::value_type, or a deducible template parameter">;

template <typename Box, typename Traits>
    requires has_value_type<Box, Traits>
using get_value_type_t = typename decltype(get_value_type<Box, Traits>())::type;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_GET_VALUE_TYPE_HPP
