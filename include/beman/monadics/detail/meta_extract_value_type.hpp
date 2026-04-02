// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_META_EXTRACT_VALUE_TYPE_HPP
#define BEMAN_MONADICS_DETAIL_META_EXTRACT_VALUE_TYPE_HPP

#include <beman/monadics/detail/decomposable.hpp>

#include <type_traits>

namespace beman::monadics::detail {

template<decomposable<1> Box>
[[nodiscard]] consteval auto meta_extract_value_type() noexcept {
    return []<template<typename...> typename T, typename V, typename... Args>(T<V, Args...>*) {
        return std::type_identity<V>{};
    }(as_pointer<Box>);
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_META_EXTRACT_VALUE_TYPE_HPP
