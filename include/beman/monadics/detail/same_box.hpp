// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_SAME_BOX_HPP
#define BEMAN_MONADICS_DETAIL_SAME_BOX_HPP

#include <beman/monadics/detail/same_template.hpp>
#include <beman/monadics/detail/same_unqualified_as.hpp>
#include <beman/monadics/detail/deduce_box_traits.hpp>
#include <concepts>

namespace beman::monadics::detail {

template <typename T, typename U>
concept same_box = requires {
    requires is_box<T>;
    requires is_box<U>;

    requires std::same_as<std::remove_cvref_t<T>,
                          typename box_traits_for<U>::template rebind<typename box_traits_for<T>::value_type> >;
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_SAME_BOX_HPP
