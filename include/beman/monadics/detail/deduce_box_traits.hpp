// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_DEDUCE_BOX_TRAITS_HPP
#define BEMAN_MONADICS_DETAIL_DEDUCE_BOX_TRAITS_HPP

#include <type_traits>

#include <beman/monadics/detail/as_pointer.hpp>
#include <beman/monadics/detail/box_traits.hpp>
#include <beman/monadics/detail/decomposable.hpp>
#include <beman/monadics/detail/deduce_error_fn.hpp>
#include <beman/monadics/detail/deduce_error_type.hpp>
#include <beman/monadics/detail/deduce_make_error_fn.hpp>
#include <beman/monadics/detail/deduce_make_fn.hpp>
#include <beman/monadics/detail/deduce_rebind.hpp>
#include <beman/monadics/detail/deduce_rebind_error.hpp>
#include <beman/monadics/detail/deduce_value_fn.hpp>
#include <beman/monadics/detail/deduce_value_query_fn.hpp>
#include <beman/monadics/detail/deduce_value_type.hpp>
#include <beman/monadics/detail/instance_of.hpp>
#include <beman/monadics/detail/same_template.hpp>
#include <beman/monadics/detail/same_unqualified_as.hpp>

namespace beman::monadics::detail {

namespace _deduce_box_traits {

template <typename Box, typename Traits = std::remove_cvref_t<Box>>
concept is_box = requires {
    requires has_value_type<Box, Traits>;
    requires has_error_type<Box, Traits>;
    requires has_rebind<Box, Traits, deduce_value_type<Box, Traits>>;
    requires has_rebind_error<Box, Traits, deduce_error_type<Box, Traits>>;

    requires has_value_query_fn<Box, Traits>;
    requires has_value_fn<Box, Traits>;
    requires has_error_fn<Box, Traits>;

    requires has_make_fn<Box, Traits, deduce_value_type<Box, Traits>>;
    requires has_make_error_fn<Box, Traits, deduce_error_type<Box, Traits>>;
};

template <typename Box, typename Traits>
struct traits {
    using box_type   = Box;
    using value_type = deduce_value_type<Box, Traits>;
    using error_type = deduce_error_type<Box, Traits>;

    template <typename T>
    using rebind = deduce_rebind<Box, Traits, value_type>::template rebind<T>;

    template <typename E>
    using rebind_error = deduce_rebind_error<Box, Traits, error_type>::template rebind_error<E>;

    inline static constexpr auto has_value = deduce_value_query_fn<Box, Traits>;
    inline static constexpr auto value     = deduce_value_fn<Box, Traits>;
    inline static constexpr auto error     = deduce_error_fn<Box, Traits>;

    inline static constexpr auto make       = deduce_make_fn<Box, Traits, value_type>;
    inline static constexpr auto make_error = deduce_make_error_fn<Box, Traits, error_type>;
};

} // namespace _deduce_box_traits

template <typename Box>
concept is_box = requires {
    typename box_traits<std::remove_cvref_t<Box>>;
    requires detail::_deduce_box_traits::is_box<std::remove_cvref_t<Box>, // maybe should preserve qualifiers?
                                                box_traits<std::remove_cvref_t<Box>>>;
};

template <is_box T>
using box_traits_for = detail::_deduce_box_traits::traits<std::remove_cvref_t<T>, box_traits<std::remove_cvref_t<T>>>;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_DEDUCE_BOX_TRAITS_HPP
