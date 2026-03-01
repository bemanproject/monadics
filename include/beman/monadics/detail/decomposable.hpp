// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_DECOMPOSABLE_HPP
#define BEMAN_MONADICS_DETAIL_DECOMPOSABLE_HPP

#include <beman/monadics/detail/as_pointer.hpp>

namespace beman::monadics::detail {

namespace _decomposable {

template <typename T, std::size_t N>
struct impl : std::false_type {};

template <template <class...> typename U, typename... Args, std::size_t N>
struct impl<U<Args...>, N> : std::bool_constant<(sizeof...(Args) >= N)> {};

} // namespace _decomposable

template <typename T, std::size_t N>
concept decomposable = _decomposable::impl<std::remove_cvref_t<T>, N>::value;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_DECOMPOSABLE_HPP
