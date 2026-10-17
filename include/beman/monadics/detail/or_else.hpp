// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_OR_ELSE_HPP
#define BEMAN_MONADICS_DETAIL_OR_ELSE_HPP

#include <beman/monadics/detail/callable_adaptor.hpp>
#include <beman/monadics/detail/carry_value.hpp>
#include <beman/monadics/detail/either.hpp>
#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/same_box.hpp>

#include <utility>

namespace beman::monadics::detail {

template<typename NewBox, typename OldBox>
concept or_elseable_return =
    object_result<NewBox>
    && (same_box<NewBox, OldBox> || on_error<"Should return the same type of Box">)
    && (same_box_and_value<NewBox, OldBox> || on_error<"Should return the Box with same value_type">);

template<typename Box, typename Fn>
concept or_elseable_impl = invocable_with_error<Fn, Box>
                        && or_elseable_return<invoke_with_error_t<Fn, Box>, Box>
                        && eitherable<carry_value_t, Fn, Box>;

template<typename Fn>
class or_else_t {
  public:
    constexpr explicit or_else_t(Fn fn) : fn_(std::move(fn)) {}

    template<box Box>
        requires or_elseable_impl<Box, Fn>
    [[nodiscard]] friend constexpr auto operator|(Box&& box, or_else_t&& op) {
        return either(carry_value, std::move(op.fn_), std::forward<Box>(box));
    }

  private:
    Fn fn_;
};

inline constexpr callable_adaptor<or_else_t> or_else{};

template<typename Box, typename Fn>
concept or_elseable = requires(Box&& box, Fn&& fn) {
    { std::forward<Box>(box) | or_else(std::forward<Fn>(fn)) };
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_OR_ELSE_HPP
