// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_REBOX_ERROR_HPP
#define BEMAN_MONADICS_DETAIL_REBOX_ERROR_HPP

#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/same_box.hpp>

#include <utility>

namespace beman::monadics::detail {

template <typename NewBox, typename Box>
    requires same_box<NewBox, Box>
[[nodiscard]] constexpr decltype(auto) rebox_error(Box&& box) noexcept {
    using BoxTraits    = get_box_traits<Box>;
    using NewBoxTraits = get_box_traits<NewBox>;

    if constexpr (requires { BoxTraits::error(std::forward<Box>(box)); }) {
        return NewBoxTraits::make_error(BoxTraits::error(std::forward<Box>(box)));
    } else {
        return NewBoxTraits::make_error(BoxTraits::error());
    }
}

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_REBOX_ERROR_HPP
