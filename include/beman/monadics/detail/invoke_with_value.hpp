// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_INVOKE_WITH_VALUE_HPP
#define BEMAN_MONADICS_DETAIL_INVOKE_WITH_VALUE_HPP

#include <concepts>
#include <utility>

namespace beman::monadics::detail {

template <typename BoxTraits, typename Fn, typename Box>
// requires requires
// requires std::is_void_v<value_type>;
// requires std::invocable<Fn>;
// } || requires { requires std::invocable<Fn, value_type>; }
[[nodiscard]] static constexpr decltype(auto) invoke_with_value(Fn&& fn, Box&& box) noexcept {
    if constexpr (std::is_void_v<typename BoxTraits::value_type> && std::invocable<Fn>) {
        // should just invoke Traits::value(box);
        return std::forward<Fn>(fn)();
    } else {
        return std::forward<Fn>(fn)(BoxTraits::value(std::forward<Box>(box)));
    }
}

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_INVOKE_WITH_VALUE_HPP
