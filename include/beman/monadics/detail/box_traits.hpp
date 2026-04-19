// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_BOX_TRAITS_HPP
#define BEMAN_MONADICS_DETAIL_BOX_TRAITS_HPP

#include <beman/monadics/detail/utility.hpp>

namespace beman::monadics::detail {

template<typename T>
struct box_traits;

template<typename T>
concept has_box_traits =
    requires { sizeof(box_traits<T>) >= std::size_t{}; } || on_error<"Missing explicit box_traits<T> specialization">;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_BOX_TRAITS_HPP
