// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_AND_THEN_HPP
#define BEMAN_MONADICS_DETAIL_AND_THEN_HPP

#include "beman/monadics/detail/deduce_box_traits.hpp"
#include "beman/monadics/detail/rebox_error.hpp"
#include "beman/monadics/detail/invoke_with_value.hpp"
#include "beman/monadics/detail/same_box.hpp"

#include <concepts>
#include <utility>

namespace beman::monadics::detail {

struct and_then_t {
    template <typename Fn>
    struct action {
        Fn fn;

        template <is_box Box, same_unqualified_as<action> A, typename Traits = box_traits_for<Box>>
        [[nodiscard]] friend inline constexpr decltype(auto) operator|(Box&& box, A&& a) noexcept
            requires requires {
                { Traits::value(std::forward<Box>(box)) } -> std::same_as<void>;
                { std::forward<A>(a).fn() } -> same_box<Box>;
            } || requires {
                { std::forward<A>(a).fn(Traits::value(std::forward<Box>(box))) } -> same_box<Box>;
            }
        {
            if (Traits::has_value(box)) {
                return invoke_with_value(std::forward<A>(a).fn, std::forward<Box>(box));
            }

            using NewBox       = decltype(invoke_with_value(std::forward<A>(a).fn, std::forward<Box>(box)));
            using NewBoxTraits = box_traits_for<NewBox>;

            return rebox_error<NewBox>(std::forward<Box>(box));
        }
    };

    template <typename Fn>
    [[nodiscard]] constexpr decltype(auto) operator()(Fn&& fn) const noexcept {
        return action<decltype(fn)>{std::forward<Fn>(fn)};
    }
};

inline constexpr and_then_t and_then{};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_AND_THEN_HPP
