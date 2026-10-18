// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_OR_ELSE_HPP
#define BEMAN_MONADICS_DETAIL_OR_ELSE_HPP

#include <beman/monadics/detail/callable_adaptor.hpp>
#include <beman/monadics/detail/carry_value.hpp>
#include <beman/monadics/detail/common_payload.hpp>
#include <beman/monadics/detail/either.hpp>
#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/same_box.hpp>
#include <beman/monadics/detail/to.hpp>

#include <utility>

namespace beman::monadics::detail {

template<typename NewBox, typename OldBox>
concept or_elseable_return =
    object_result<NewBox>
    && (same_box<NewBox, OldBox> || on_error<"Should return the same type of Box">)
    && (has_common_payload<typename get_box_traits<OldBox>::value_type, typename get_box_traits<NewBox>::value_type>
        || on_error<"Should return the Box with same value_type, or specialize common_payload for the two">);

template<typename Box, typename Returned>
using or_else_result_t = typename get_box_traits<Returned>::template rebind<
    common_payload_t<typename get_box_traits<Box>::value_type, typename get_box_traits<Returned>::value_type>
>;

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
        using Returned = invoke_with_error_t<Fn, Box>;
        using R = or_else_result_t<Box, Returned>;

        if constexpr (std::same_as<R, Returned>) {
            return either(carry_value, std::move(op.fn_), std::forward<Box>(box));
        } else {
            return either(
                carry_value,
                [&op](auto&&... e) -> R { return std::move(op.fn_)(std::forward<decltype(e)>(e)...) | to<R>(); },
                std::forward<Box>(box)
            );
        }
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
