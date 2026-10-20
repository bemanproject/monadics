// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_GET_VALUE_HPP
#define BEMAN_MONADICS_DETAIL_GET_VALUE_HPP

#include <beman/monadics/detail/get_value_fn.hpp>
#include <beman/monadics/detail/get_value_type.hpp>
#include <beman/monadics/detail/utility.hpp>

#include <type_traits>

namespace beman::monadics::detail {

template<typename Box, typename Traits>
concept value_read_agrees =
    reads_as<Box, decltype(get_value_fn<Box, Traits, get_value_type_t<Box, Traits>>()), get_value_type_t<Box, Traits>>
    || std::is_void_v<get_value_type_t<Box, Traits>>
    || on_error<"Traits::value must yield the value_type the box names">;

template<typename Box, typename Traits>
concept has_value_access = has_value_type<Box, Traits>
                        && has_value_fn<Box, Traits, get_value_type_t<Box, Traits>>
                        && value_read_agrees<Box, Traits>;

template<typename Box, typename Traits>
    requires has_value_access<Box, Traits>
using get_value_t = get_value_type_t<Box, Traits>;

template<typename Box, typename Traits>
    requires has_value_access<Box, Traits>
[[nodiscard]] consteval decltype(auto) get_value() noexcept {
    return get_value_fn<Box, Traits, get_value_t<Box, Traits>>();
}

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_GET_VALUE_HPP
