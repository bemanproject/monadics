// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_INVOKE_WITH_VALUE_HPP
#define BEMAN_MONADICS_DETAIL_INVOKE_WITH_VALUE_HPP

#include "beman/monadics/detail/deduce_box_traits.hpp"

#include <concepts>
#include <utility>

namespace beman::monadics::detail {

template <typename Fn, is_box Box, typename BoxTraits = box_traits_for<Box>>
[[nodiscard]] static constexpr decltype(auto) invoke_with_value(Fn&& fn, Box&& box) noexcept
    requires requires {
        { BoxTraits::value(std::forward<Box>(box)) } -> std::same_as<void>;
        { fn() };
    } || requires {
        { fn(BoxTraits::value(std::forward<Box>(box))) };
    }
{
    if constexpr (std::is_void_v<typename BoxTraits::value_type> && std::invocable<Fn>) {
        // should just invoke Traits::value(box);
        return std::forward<Fn>(fn)();
    } else {
        return std::forward<Fn>(fn)(BoxTraits::value(std::forward<Box>(box)));
    }
}

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_INVOKE_WITH_VALUE_HPP
