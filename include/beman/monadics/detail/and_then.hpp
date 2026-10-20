// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_AND_THEN_HPP
#define BEMAN_MONADICS_DETAIL_AND_THEN_HPP

#include <beman/monadics/detail/callable_adaptor.hpp>
#include <beman/monadics/detail/carry_error.hpp>
#include <beman/monadics/detail/common_payload.hpp>
#include <beman/monadics/detail/either.hpp>
#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/same_box.hpp>
#include <beman/monadics/detail/to.hpp>

#include <utility>

namespace beman::monadics::detail {

template<typename NewBox, typename OldBox>
concept and_thenable_return =
    object_result<NewBox>
    && (same_box<NewBox, OldBox> || on_error<"Should return the same type of Box">)
    && (has_common_payload<typename get_box_traits<OldBox>::error_type, typename get_box_traits<NewBox>::error_type>
        || on_error<"Should return the Box with same error_type, or specialize common_payload for the two">);

template<typename Box, typename Returned>
using and_then_result_t = typename get_box_traits<Returned>::template rebind_error<
    common_payload_t<typename get_box_traits<Box>::error_type, typename get_box_traits<Returned>::error_type>
>;

template<typename Box, typename Fn>
concept and_thenable_impl = invocable_with_value<Fn, Box>
                         && and_thenable_return<invoke_with_value_t<Fn, Box>, Box>
                         && eitherable<Fn, carry_error_t, Box>;

template<typename Fn>
class and_then_t {
  public:
    constexpr explicit and_then_t(Fn fn) : fn_(std::move(fn)) {}

    template<box Box>
        requires and_thenable_impl<Box, Fn>
    [[nodiscard]] friend constexpr auto operator|(Box&& box, and_then_t&& op) {
        using Returned = invoke_with_value_t<Fn, Box>;
        using R = and_then_result_t<Box, Returned>;

        if constexpr (std::same_as<R, Returned>) {
            return either(std::move(op.fn_), carry_error, std::forward<Box>(box));
        } else {
            return either(
                [&op](auto&&... v) -> R { return std::move(op.fn_)(std::forward<decltype(v)>(v)...) | to<R>(); },
                carry_error,
                std::forward<Box>(box)
            );
        }
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
