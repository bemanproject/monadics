// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_SAME_BOX_HPP
#define BEMAN_MONADICS_DETAIL_SAME_BOX_HPP

#include <beman/monadics/detail/same_template.hpp>
#include <beman/monadics/detail/same_unqualified_as.hpp>
#include <beman/monadics/detail/deduce_box_traits.hpp>

#include <concepts>

namespace beman::monadics::detail {

namespace _same_box {

template <typename NewTraits, typename OldTraits>
using rebind =
    box_traits_for<typename OldTraits::template rebind<typename NewTraits::value_type>>::template rebind_error<
        typename NewTraits::error_type>;

} // namespace _same_box

template <typename T, typename U>
concept same_box = requires {
    requires is_box<T>;
    requires is_box<U>;

    requires std::same_as<std::remove_cvref_t<T>, _same_box::rebind<box_traits_for<T>, box_traits_for<U>>>;
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_SAME_BOX_HPP
