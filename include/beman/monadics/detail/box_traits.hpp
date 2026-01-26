// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_BOX_TRAITS_HPP
#define BEMAN_MONADICS_DETAIL_BOX_TRAITS_HPP

namespace beman::monadics::detail {

template <typename T>
struct box_traits {
    using box_type = T;
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_BOX_TRAITS_HPP
