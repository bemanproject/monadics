// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_AND_THEN_HPP
#define BEMAN_MONADICS_DETAIL_AND_THEN_HPP

#include <beman/monadics/detail/callable_adaptor.hpp>
#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/invoke_with_value.hpp>
#include <beman/monadics/detail/propagate_error.hpp>
#include <beman/monadics/detail/same_box.hpp>

#include <utility>

namespace beman::monadics::detail {

template<typename NewBox, typename OldBox>
concept and_thenable_return =
    (same_box<NewBox, OldBox> || on_error<"Should return the same type of Box">)
    && (same_box_and_error<NewBox, OldBox> || on_error<"Should return the Box with same error_type">);

template<typename Box, typename Fn>
concept and_thenable_impl =
    and_thenable_return<decltype(invoke_with_value(std::declval<Fn>(), std::declval<Box>())), Box>;

template<typename Fn>
class and_then_t {
  public:
    constexpr explicit and_then_t(Fn fn) : fn_(std::move(fn)) {}

    template<box Box>
        requires and_thenable_impl<Box, Fn>
    [[nodiscard]] friend constexpr decltype(auto) operator|(Box&& box, and_then_t&& op) {
        using Traits = get_box_traits<Box>;
        using NewBox = decltype(invoke_with_value(std::declval<Fn>(), std::declval<Box>()));

        if (Traits::has_value(box)) {
            return invoke_with_value(std::move(op.fn_), std::forward<Box>(box));
        }

        return propagate_error<NewBox>(std::forward<Box>(box));
    }

  private:
    Fn fn_;
};

inline constexpr callable_adaptor<and_then_t> and_then{};

template<typename Box, typename Fn>
concept and_thenable = requires(Box&& box, Fn&& fn) {
    { std::forward<Box>(box) | and_then(std::forward<Fn>(fn)) };
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_AND_THEN_HPP
