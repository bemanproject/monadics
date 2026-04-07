// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_TRANSFORM_ERROR_HPP
#define BEMAN_MONADICS_DETAIL_TRANSFORM_ERROR_HPP

#include <beman/monadics/detail/or_else.hpp>
#include <beman/monadics/detail/pipe_adaptor.hpp>

#include <utility>

namespace beman::monadics::detail {

template<typename NewError, typename Box>
concept transform_errorable_return = same_box<Box, typename get_box_traits<Box>::template rebind_error<NewError>>
                                  || on_error<"transform_error: fn must return a type compatible with rebind_error">;

template<typename Box, typename Fn>
concept transform_errorable_impl =
    (has_error_channel<Box> || on_error<"transform_error requires has_error_channel">)
    && transform_errorable_return<decltype(invoke_with_error(std::declval<Fn>(), std::declval<Box>())), Box>;

class transform_error_t {
    inline static constexpr access_key<transform_error_t> key{};

    template<box Box, std::derived_from<transform_error_t> Op>
    [[nodiscard]] friend constexpr decltype(auto) operator|(Box&& box, Op&& op)
        requires transform_errorable_impl<decltype(box), decltype(std::forward<Op>(op).callable(key))>
    {
        using Traits = get_box_traits<Box>;
        using NewError = decltype(invoke_with_error(std::forward<Op>(op).callable(key), std::forward<Box>(box)));
        using NewBox = typename Traits::template rebind_error<NewError>;
        using NewBoxTraits = get_box_traits<NewBox>;

        if (!Traits::has_value(box)) {
            return NewBoxTraits::make_error(invoke_with_error(std::forward<Op>(op).callable(key),
                                                              std::forward<Box>(box)));
        }

        return propagate_value<NewBox>(std::forward<Box>(box));

        // gcc11/12 internal crash with pipe_adaptor_t
        // return std::forward<Box>(box) | or_else([f = std::forward<Op>(op).callable(key)](auto&& e) {
        //            return NewBoxTraits::make_error(f(std::forward<decltype(e)>(e)));
        //        });
    }
};

inline constexpr pipe_adaptor<transform_error_t> transform_error{};

template<typename Box, typename Fn>
concept transform_errorable =
    requires(Box&& box, Fn&& fn) { std::forward<Box>(box) | transform_error(std::forward<Fn>(fn)); };

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_TRANSFORM_ERROR_HPP
