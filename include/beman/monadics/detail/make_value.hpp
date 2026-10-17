// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_MAKE_VALUE_HPP
#define BEMAN_MONADICS_DETAIL_MAKE_VALUE_HPP

#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/just.hpp>
#include <beman/monadics/detail/to.hpp>

#include <utility>

namespace beman::monadics::detail {

template<box R>
[[nodiscard]] constexpr R make_value(auto&&... v) {
    return just(std::forward<decltype(v)>(v)...) | to<R>();
}

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_MAKE_VALUE_HPP
