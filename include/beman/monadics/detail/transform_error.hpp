// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_TRANSFORM_ERROR_HPP
#define BEMAN_MONADICS_DETAIL_TRANSFORM_ERROR_HPP

#include <beman/monadics/detail/callable_adaptor.hpp>
#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/invoke_with_error.hpp>
#include <beman/monadics/detail/propagate_value.hpp>
#include <beman/monadics/detail/same_box.hpp>

#include <utility>

namespace beman::monadics::detail {

template<typename NewError, typename Box>
concept transform_errorable_return = same_box<Box, typename get_box_traits<Box>::template rebind_error<NewError>>
                                  || on_error<"transform_error: fn must return a type compatible with rebind_error">;

template<typename Box, typename Fn>
concept transform_errorable_impl =
    (has_error_channel<Box> || on_error<"transform_error requires has_error_channel">)
    && transform_errorable_return<decltype(invoke_with_error(std::declval<Fn>(), std::declval<Box>())), Box>;

template<typename Fn>
class transform_error_t {
  public:
    constexpr explicit transform_error_t(Fn fn) : fn_(std::move(fn)) {}

    template<box Box>
        requires transform_errorable_impl<Box, Fn>
    [[nodiscard]] friend constexpr decltype(auto) operator|(Box&& box, transform_error_t&& op) {
        using Traits = get_box_traits<Box>;
        using NewError = decltype(invoke_with_error(std::declval<Fn>(), std::declval<Box>()));
        using NewBox = typename Traits::template rebind_error<NewError>;
        using NewBoxTraits = get_box_traits<NewBox>;

        if (!Traits::has_value(box)) {
            return NewBoxTraits::make_error(invoke_with_error(std::move(op.fn_), std::forward<Box>(box)));
        }

        return propagate_value<NewBox>(std::forward<Box>(box));
    }

  private:
    Fn fn_;
};

inline constexpr callable_adaptor<transform_error_t> transform_error{};

template<typename Box, typename Fn>
concept transform_errorable = requires(Box&& box, Fn&& fn) {
    { std::forward<Box>(box) | transform_error(std::forward<Fn>(fn)) };
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_TRANSFORM_ERROR_HPP
