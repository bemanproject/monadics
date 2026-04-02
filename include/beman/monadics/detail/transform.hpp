// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_TRANSFORM_HPP
#define BEMAN_MONADICS_DETAIL_TRANSFORM_HPP

#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/invoke_with_value.hpp>
#include <beman/monadics/detail/pipe_adaptor.hpp>
#include <beman/monadics/detail/rebox_error.hpp>
#include <beman/monadics/detail/same_box.hpp>

#include <type_traits>
#include <utility>

namespace beman::monadics::detail {

class transform_t {
    inline static constexpr access_key<transform_t> key{};

    template <is_box Box, std::derived_from<transform_t> Op, typename Traits = get_box_traits<Box>>
    [[nodiscard]] friend constexpr decltype(auto) operator|(Box&& box, Op&& op)
        requires requires {
            requires same_box<Box,
                              typename Traits::template rebind<decltype(invoke_with_value(
                                  std::forward<Op>(op).callable(key), std::forward<Box>(box)))>>;
        }
    {
        using NewValue = decltype(invoke_with_value(std::forward<Op>(op).callable(key), std::forward<Box>(box)));
        using NewBox = typename Traits::template rebind<NewValue>;
        using NewBoxTraits = get_box_traits<NewBox>;

        if (Traits::has_value(box)) {
            if constexpr (std::is_void_v<NewValue>) {
                invoke_with_value(std::forward<Op>(op).callable(key), std::forward<Box>(box));
                return NewBoxTraits::make();
            } else {
                return NewBoxTraits::make(
                    invoke_with_value(std::forward<Op>(op).callable(key), std::forward<Box>(box)));
            }
        }

        return rebox_error<NewBox>(std::forward<Box>(box));
    }
};

inline constexpr pipe_adaptor<transform_t> transform{};

template <typename Box, typename Fn>
concept transformable = requires(std::remove_reference_t<Box> box, std::remove_reference_t<Fn> fn) {
    std::forward<Box>(box) | transform(std::forward<Fn>(fn));
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_TRANSFORM_HPP
