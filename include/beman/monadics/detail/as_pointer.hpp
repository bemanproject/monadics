// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_AS_POINTER_HPP
#define BEMAN_MONADICS_DETAIL_AS_POINTER_HPP

#include <type_traits>

namespace beman::monadics::detail {

template<typename T>
inline constexpr auto as_pointer = static_cast<std::remove_cvref_t<T>*>(nullptr);

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_AS_POINTER_HPP
