// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_META_REBIND_ERROR_HPP
#define BEMAN_MONADICS_DETAIL_META_REBIND_ERROR_HPP

#include <beman/monadics/detail/decomposable.hpp>

#include <type_traits>

namespace beman::monadics::detail {

template<template<typename...> class Box, typename T, typename... Args>
struct meta_rebind_error {
    template<typename E>
    using rebind_error = Box<T, E, Args...>;
};

template<decomposable<2> Box>
[[nodiscard]] consteval auto get_meta_rebind_error() noexcept {
    return []<template<typename...> typename T, typename V, typename E, typename... Args>(T<V, E, Args...>*) {
        return std::type_identity<meta_rebind_error<T, V, Args...>>{};
    }(as_pointer<Box>);
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_META_REBIND_ERROR_HPP
