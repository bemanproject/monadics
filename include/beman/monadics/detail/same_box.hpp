// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_SAME_BOX_HPP
#define BEMAN_MONADICS_DETAIL_SAME_BOX_HPP

#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/same_template.hpp>
#include <beman/monadics/detail/same_unqualified_as.hpp>

#include <concepts>

namespace beman::monadics::detail {

namespace _same_box {

template <typename NewTraits, typename OldTraits>
using rebind = get_box_traits<typename OldTraits::template rebind<typename NewTraits::value_type>>::
    template rebind_error<typename NewTraits::error_type>;

} // namespace _same_box

template <typename T, typename U>
concept same_box = is_box<T>
                && is_box<U>
                && std::same_as<std::remove_cvref_t<T>, _same_box::rebind<get_box_traits<T>, get_box_traits<U>>>;

template <typename T, typename U>
concept same_box_and_value =
    same_box<T, U> && std::same_as<typename get_box_traits<T>::value_type, typename get_box_traits<U>::value_type>;

template <typename T, typename U>
concept same_box_and_error =
    same_box<T, U> && std::same_as<typename get_box_traits<T>::error_type, typename get_box_traits<U>::error_type>;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_SAME_BOX_HPP
