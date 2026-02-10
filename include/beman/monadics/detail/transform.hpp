// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_TRANSFORM_HPP
#define BEMAN_MONADICS_DETAIL_TRANSFORM_HPP

#include <beman/monadics/detail/invoke_with_value.hpp>
#include <beman/monadics/detail/rebox_error.hpp>
#include <beman/monadics/detail/and_then.hpp>
#include "beman/monadics/detail/same_box.hpp"

#include <type_traits>
#include <utility>

namespace beman::monadics::detail {

struct transform_t {
    template <typename Fn>
    struct action {
        Fn fn;

        template <is_box Box, same_unqualified_as<action> A, typename Traits = box_traits_for<Box>>
        [[nodiscard]] friend inline constexpr decltype(auto) operator|(Box&& box, A&& a) noexcept
            requires requires {
                requires same_box<Box,
                                  typename Traits::template rebind<decltype(invoke_with_value(
                                      std::forward<A>(a).fn, std::forward<Box>(box)))>>;
            }
        /*
            requires {
              { box | and_then(...) } -> same_box2<Box>
            }
        */
        {
            using NewValue     = decltype(invoke_with_value(std::forward<A>(a).fn, std::forward<Box>(box)));
            using NewBox       = typename Traits::template rebind<NewValue>;
            using NewBoxTraits = box_traits_for<NewBox>;

            if (Traits::has_value(box)) {
                if constexpr (std::is_void_v<typename NewBoxTraits::value_type>) {
                    invoke_with_value(std::forward<A>(a).fn, std::forward<Box>(box));
                    return NewBoxTraits::make();
                } else {
                    return NewBoxTraits::make(invoke_with_value(std::forward<A>(a).fn, std::forward<Box>(box)));
                }
            }

            return rebox_error<NewBox>(std::forward<Box>(box));
        }
    };

    template <typename Fn>
    [[nodiscard]] constexpr decltype(auto) operator()(Fn&& fn) const noexcept {
        return action<decltype(fn)>{std::forward<Fn>(fn)};
    }
};

inline constexpr transform_t transform{};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_TRANSFORM_HPP
