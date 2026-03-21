// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_TESTS_HELPERS_KEEPS_RETURN_VALUE_CATEGORY_HPP
#define BEMAN_MONADICS_TESTS_HELPERS_KEEPS_RETURN_VALUE_CATEGORY_HPP

#include <concepts>
#include <utility>

namespace beman::monadics::helpers {

template <typename Fn, typename Box, typename T>
concept keeps_return_value_category = requires(Fn fn, Box b, const Box cb) {
    { fn(b) } -> std::same_as<T&>;
    { fn(cb) } -> std::same_as<const T&>;
    { fn(std::move(b)) } -> std::same_as<T&&>;
    { fn(std::move(cb)) } -> std::same_as<const T&&>;
};

} // namespace beman::monadics::helpers

#endif // BEMAN_MONADICS_TESTS_HELPERS_KEEPS_RETURN_VALUE_CATEGORY_HPP
