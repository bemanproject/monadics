// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_TRANSFORM_ERROR_HPP
#define BEMAN_MONADICS_DETAIL_TRANSFORM_ERROR_HPP

#include <beman/monadics/detail/or_else.hpp>

#include <utility>

namespace beman::monadics::detail {

struct transform_error_t {
    template <typename Fn>
    struct action {
        Fn fn;

        template <is_box Box, same_unqualified_as<action> A, typename BoxTraits = box_traits_for<Box>>
        [[nodiscard]] friend inline constexpr decltype(auto) operator|(Box&& box, A&& a) noexcept
            requires requires {
                requires same_box<Box,
                                  typename BoxTraits::template rebind_error<decltype(invoke_with_error(
                                      std::forward<A>(a).fn, std::forward<Box>(box)))>>;
            }
        {
            {

                using NewError     = decltype(invoke_with_error(std::forward<A>(a).fn, std::forward<Box>(box)));
                using NewBox       = typename BoxTraits::template rebind_error<NewError>;
                using NewBoxTraits = box_traits_for<NewBox>;

                // transform_error does not make sense if you don't have error
                return std::forward<Box>(box) | or_else([f = std::forward<A>(a).fn](auto&& e) {
                           return NewBoxTraits::make_error(f(std::forward<decltype(e)>(e)));
                       });
            }
        }
    };

    template <typename Fn>
    [[nodiscard]] constexpr decltype(auto) operator()(Fn&& fn) const noexcept {
        return action<decltype(fn)>{std::forward<Fn>(fn)};
    }
};

inline constexpr transform_error_t transform_error{};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_TRANSFORM_ERROR_HPP
