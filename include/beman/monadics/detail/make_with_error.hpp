// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_MAKE_WITH_ERROR_HPP
#define BEMAN_MONADICS_DETAIL_MAKE_WITH_ERROR_HPP

#include <utility>

namespace beman::monadics::detail {

template <typename NewBoxTraits, typename BoxTraits, typename Box>
[[nodiscard]] static constexpr decltype(auto) make_with_error(Box&& box) noexcept {
    if constexpr (requires { BoxTraits::error(std::forward<Box>(box)); }) {
        return NewBoxTraits::lift_error(BoxTraits::error(std::forward<Box>(box)));
    } else {
        return NewBoxTraits::lift_error(BoxTraits::error());
    }
}

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_MAKE_WITH_ERROR_HPP
