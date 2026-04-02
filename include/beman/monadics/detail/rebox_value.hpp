// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_REBOX_VALUE_HPP
#define BEMAN_MONADICS_DETAIL_REBOX_VALUE_HPP

#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/same_box.hpp>

#include <type_traits>
#include <utility>

namespace beman::monadics::detail {

template <typename NewBox, typename Box>
    requires same_box<NewBox, Box>
[[nodiscard]] constexpr decltype(auto) rebox_value(Box&& box) noexcept {
    using BoxTraits = get_box_traits<Box>;
    using NewBoxTraits = get_box_traits<NewBox>;

    if constexpr (std::is_void_v<typename NewBoxTraits::value_type>) {
        return NewBoxTraits::make();
    } else {
        return NewBoxTraits::make(BoxTraits::value(std::forward<Box>(box)));
    }
}

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_REBOX_VALUE_HPP
