// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_TRANSFORM_ERROR_HPP
#define BEMAN_MONADICS_DETAIL_TRANSFORM_ERROR_HPP

#include <beman/monadics/detail/callable_adaptor.hpp>
#include <beman/monadics/detail/compose.hpp>
#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/invoke_with_error.hpp>
#include <beman/monadics/detail/just_error.hpp>
#include <beman/monadics/detail/or_else.hpp>
#include <beman/monadics/detail/same_box.hpp>
#include <beman/monadics/detail/to.hpp>

#include <utility>

namespace beman::monadics::detail {

template<typename NewError, typename Box>
concept transform_errorable_return = same_box<Box, typename get_box_traits<Box>::template rebind_error<NewError>>
                                  || on_error<"transform_error: fn must return a type compatible with rebind_error">;

template<typename NewBox, typename NewError>
concept buildable_from_error = requires {
    { get_box_traits<NewBox>::make_error(std::declval<NewError>()) };
};

template<typename Box,
         typename Fn,
         typename F = invoke_with_error_t<Fn, Box>,
         typename R = typename get_box_traits<Box>::template rebind_error<F>>
concept transform_errorable_result =
    object_result<F>
    && transform_errorable_return<F, Box>
    && or_elseable_return<R, Box>
    && (buildable_from_error<R, F> || on_error<"transform_error: the box cannot be built from what fn returns">);

template<typename Box, typename Fn>
concept transform_errorable_impl = (has_error_channel<Box> || on_error<"transform_error requires has_error_channel">)
                                && transform_errorable_result<Box, Fn>;

template<typename Fn>
class transform_error_t {
  public:
    constexpr explicit transform_error_t(Fn fn) : fn_(std::move(fn)) {}

    template<box Box>
        requires transform_errorable_impl<Box, Fn>
    [[nodiscard]] friend constexpr auto operator|(Box&& box, transform_error_t&& op) {
        using R = typename get_box_traits<Box>::template rebind_error<invoke_with_error_t<Fn, Box>>;

        return std::forward<Box>(box) | or_else([&op](auto&& e) -> R {
                   return compose(just_error, std::move(op.fn_))(std::forward<decltype(e)>(e)) | to<R>();
               });
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
