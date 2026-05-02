// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_CALLABLE_ADAPTOR_HPP
#define BEMAN_MONADICS_DETAIL_CALLABLE_ADAPTOR_HPP

#include <type_traits>
#include <utility>

namespace beman::monadics::detail {

template<template<typename> typename T>
struct callable_adaptor {
    template<typename Fn>
    [[nodiscard]] constexpr auto
        operator()(Fn&& fn) const noexcept(std::is_nothrow_constructible_v<T<std::decay_t<Fn>>, Fn>) {
        return T<std::decay_t<Fn>>{std::forward<Fn>(fn)};
    }
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_CALLABLE_ADAPTOR_HPP
