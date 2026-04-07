// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_PROPAGATE_ERROR_HPP
#define BEMAN_MONADICS_DETAIL_PROPAGATE_ERROR_HPP

#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/same_box.hpp>

#include <utility>

namespace beman::monadics::detail {

template<typename NewBox, typename Box>
concept propagatable_error = same_box<NewBox, Box> && (requires {
    requires has_error_channel<Box>;
    { get_box_traits<NewBox>::make_error(get_box_traits<Box>::error(std::declval<Box>())) };
} || requires {
    requires !has_error_channel<Box>;
    { get_box_traits<NewBox>::make_error(get_box_traits<Box>::error()) };
});

template<typename NewBox, typename Box>
    requires propagatable_error<NewBox, Box>
[[nodiscard]] constexpr decltype(auto) propagate_error(Box&& box) {
    using BoxTraits = get_box_traits<Box>;
    using NewBoxTraits = get_box_traits<NewBox>;

    if constexpr (has_error_channel<Box>) {
        return NewBoxTraits::make_error(BoxTraits::error(std::forward<Box>(box)));
    } else {
        return NewBoxTraits::make_error(BoxTraits::error());
    }
}

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_PROPAGATE_ERROR_HPP
