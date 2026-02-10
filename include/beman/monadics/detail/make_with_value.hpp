// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_MAKE_WITH_VALUE_HPP
#define BEMAN_MONADICS_DETAIL_MAKE_WITH_VALUE_HPP

#include <type_traits>
#include <utility>

namespace beman::monadics::detail {

template <typename NewBoxTraits, typename BoxTraits, typename Box>
[[nodiscard]] constexpr decltype(auto) make_with_value(Box&& box) noexcept {
    if constexpr (std::is_void_v<typename NewBoxTraits::value_type>) {
        return NewBoxTraits::make();
    } else {
        return NewBoxTraits::make(BoxTraits::value(std::forward<Box>(box)));
    }
}

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_MAKE_WITH_VALUE_HPP
