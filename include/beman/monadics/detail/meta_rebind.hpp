// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_META_REBIND_HPP
#define BEMAN_MONADICS_DETAIL_META_REBIND_HPP

#include <beman/monadics/detail/decomposable.hpp>

#include <type_traits>

namespace beman::monadics::detail {

template<template<typename...> class Box, typename... Args>
struct meta_rebind {
    template<typename T>
    using rebind = Box<T, Args...>;
};

template<decomposable<1> Box>
[[nodiscard]] consteval auto get_meta_rebind() noexcept {
    return []<template<typename...> typename T, typename V, typename... Args>(T<V, Args...>*) {
        return std::type_identity<meta_rebind<T, Args...>>{};
    }(as_pointer<Box>);
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_META_REBIND_HPP
