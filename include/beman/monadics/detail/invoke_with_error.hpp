// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_INVOKE_WITH_ERROR_HPP
#define BEMAN_MONADICS_DETAIL_INVOKE_WITH_ERROR_HPP

#include <concepts>
#include <utility>

namespace beman::monadics::detail {

template <typename BoxTraits, typename Fn, typename Box>
// requires requires {
// { error(std::declval<Box>()) };
// requires std::invocable<Fn, error_type>;
// } || requires { requires std::invocable<Fn>; }
[[nodiscard]] static constexpr decltype(auto) invoke_with_error(Fn&& fn, Box&& box) noexcept {
    if constexpr (requires { BoxTraits::error(std::forward<Box>(box)); }) {
        return std::forward<Fn>(fn)(BoxTraits::error(std::forward<Box>(box)));
    } else {
        return std::forward<Fn>(fn)();
    }
}

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_INVOKE_WITH_ERROR_HPP
