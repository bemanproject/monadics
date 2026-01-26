// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_PIPE_HPP
#define BEMAN_MONADICS_DETAIL_PIPE_HPP

#include <beman/monadics/detail/same_unqualified_as.hpp>
#include <beman/monadics/detail/deduce_box_traits.hpp>

#include <utility>

namespace beman::monadics::detail {

template <typename CPO>
struct pipe_for {
    inline static constexpr CPO cpo{};

    template <typename Fn>
    struct action {
        Fn fn;

        template <typename Box, typename Traits = box_traits_for<Box>>
        [[nodiscard]] inline constexpr decltype(auto) operator()(Box&& box) noexcept
            requires requires {
              { cpo.template operator()<Traits>(std::forward<Box>(box), std::forward<Fn>(fn)) };
            }
        {
            // static_assert(std::same_as<decltype(box), Boo*&&>);
            return cpo.template operator()<Traits>(std::forward<Box>(box), std::forward<Fn>(fn));
        }

        template <is_box Box, same_unqualified_as<action> A>
        [[nodiscard]] friend inline constexpr decltype(auto) operator|(Box&& box, A&& a) noexcept
            requires requires {
              { std::forward<A>(a)(std::forward<Box>(box)) };
            }
        {
            return std::forward<A>(a)(std::forward<Box>(box));
        }
    };

    template <typename Fn>
    [[nodiscard]] constexpr decltype(auto) operator()(Fn&& fn) const noexcept {
        return action<decltype(fn)>{std::forward<Fn>(fn)};
    }

    // template <typename Fn>
    // [[nodiscard]] constexpr decltype(auto) operator()() const noexcept {
    // return action<decltype(fn)>{std::forward<Fn>(fn)};
    // }
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_PIPE_HPP
