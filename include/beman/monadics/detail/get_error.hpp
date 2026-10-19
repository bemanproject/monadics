// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_GET_ERROR_HPP
#define BEMAN_MONADICS_DETAIL_GET_ERROR_HPP

#include <beman/monadics/detail/get_error_fn.hpp>
#include <beman/monadics/detail/get_error_type.hpp>
#include <beman/monadics/detail/utility.hpp>

namespace beman::monadics::detail {

template<typename Box, typename Traits>
concept error_read_agrees =
    reads_as<Box, decltype(get_error_fn<Box, Traits, get_error_type_t<Box, Traits>>()), get_error_type_t<Box, Traits>>
    || on_error<"Traits::error must yield the error_type the box names">;

template<typename Box, typename Traits>
concept has_error_access = has_error_type<Box, Traits>
                        && has_error_fn<Box, Traits, get_error_type_t<Box, Traits>>
                        && error_read_agrees<Box, Traits>;

template<typename Box, typename Traits>
    requires has_error_access<Box, Traits>
using get_error_t = get_error_type_t<Box, Traits>;

template<typename Box, typename Traits>
    requires has_error_access<Box, Traits>
[[nodiscard]] consteval decltype(auto) get_error() noexcept {
    return get_error_fn<Box, Traits, get_error_t<Box, Traits>>();
}

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_GET_ERROR_HPP
