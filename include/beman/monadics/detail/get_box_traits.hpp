// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_GET_BOX_TRAITS_HPP
#define BEMAN_MONADICS_DETAIL_GET_BOX_TRAITS_HPP

#include <type_traits>

#include <beman/monadics/detail/box_traits.hpp>
#include <beman/monadics/detail/get_error_fn.hpp>
#include <beman/monadics/detail/get_error_type.hpp>
#include <beman/monadics/detail/get_make_error_fn.hpp>
#include <beman/monadics/detail/get_make_fn.hpp>
#include <beman/monadics/detail/get_rebind.hpp>
#include <beman/monadics/detail/get_rebind_error.hpp>
#include <beman/monadics/detail/get_value_fn.hpp>
#include <beman/monadics/detail/get_value_query_fn.hpp>
#include <beman/monadics/detail/get_value_type.hpp>

namespace beman::monadics::detail {

namespace _get_box_traits {

template<typename Box, typename Traits>
concept box = has_box_traits<Box>
           && has_value_type<Box, Traits>
           && has_error_type<Box, Traits>
           && has_rebind<Box, Traits, get_value_type_t<Box, Traits>>
           && has_rebind_error<Box, Traits, get_error_type_t<Box, Traits>>
           && has_value_query_fn<Box, Traits>
           && has_value_fn<Box, Traits>
           && has_error_fn<Box, Traits>
           && has_make_fn<Box, Traits, get_value_type_t<Box, Traits>>
           && has_make_error_fn<Box, Traits, get_error_type_t<Box, Traits>>;

template<typename Box, typename Traits>
struct traits {
    using box_type = Box;
    using value_type = get_value_type_t<Box, Traits>;
    using error_type = get_error_type_t<Box, Traits>;

    template<typename T>
    using rebind = get_rebind_t<Box, Traits, value_type>::template rebind<T>;

    template<typename E>
    using rebind_error = get_rebind_error_t<Box, Traits, error_type>::template rebind_error<E>;

    static constexpr auto has_value = get_value_query_fn<Box, Traits>();
    static constexpr auto value = get_value_fn<Box, Traits>();
    static constexpr auto error = get_error_fn<Box, Traits>();

    static constexpr auto make = get_make_fn<Box, Traits, value_type>();
    static constexpr auto make_error = get_make_error_fn<Box, Traits, error_type>();
};

} // namespace _get_box_traits

template<typename Box>
concept box = _get_box_traits::box<std::remove_cvref_t<Box>, // maybe should preserve qualifiers?
                                   box_traits<std::remove_cvref_t<Box>>>;

template<box T>
using get_box_traits = _get_box_traits::traits<std::remove_cvref_t<T>, box_traits<std::remove_cvref_t<T>>>;

template<typename Box>
concept has_error_channel = requires {
    { get_box_traits<Box>::error(std::declval<Box>()) };
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_GET_BOX_TRAITS_HPP
