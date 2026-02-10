// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_OR_ELSE_HPP
#define BEMAN_MONADICS_DETAIL_OR_ELSE_HPP

#include <beman/monadics/detail/pipe.hpp>
#include <beman/monadics/detail/make_with_value.hpp>
#include <beman/monadics/detail/invoke_with_error.hpp>

#include <type_traits>

namespace beman::monadics::detail {

namespace _or_else {

template <typename Fn, typename Box, typename BoxTraits>
consteval auto invoke_result() {

    if constexpr (requires { BoxTraits::error(std::declval<Box>()); }) {
        return std::type_identity<std::invoke_result_t<Fn, decltype(BoxTraits::error(std::declval<Box>()))>>{};
    } else {
        return std::type_identity<std::invoke_result_t<Fn>>{};
    }
}

template <typename Fn, typename Box, typename BoxTraits>
    requires requires {
        { BoxTraits::error() };
        // requires std::invocable<decltype(BoxTraits::error)>;
        // requires std::invocable<Fn>;
    } || requires {
        { BoxTraits::error(std::declval<Box>()) };
        // requires std::invocable<decltype(BoxTraits::error), Box>;
        // requires std::invocable<Fn, Value>;
    }
using invoke_result_t = decltype(invoke_result<Fn, Box, BoxTraits>())::type;

struct op_fn {
    template <typename BoxTraits, typename Box, typename Fn>
    [[nodiscard]] inline constexpr auto operator()(Box&& box, Fn&& fn) const noexcept {
        using NewBox       = decltype(invoke_with_error<BoxTraits>(std::forward<Fn>(fn), std::forward<Box>(box)));
        using NewBoxTraits = box_traits_for<NewBox>;

        if (!BoxTraits::has_value(box)) {
            return invoke_with_error<BoxTraits>(std::forward<Fn>(fn), std::forward<Box>(box));
        }

        return make_with_value<NewBoxTraits, BoxTraits>(std::forward<Box>(box));
    }
};

} // namespace _or_else

inline constexpr pipe_for<_or_else::op_fn> or_else{};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_OR_ELSE_HPP
