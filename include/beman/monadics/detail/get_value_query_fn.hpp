// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_GET_VALUE_QUERY_FN_HPP
#define BEMAN_MONADICS_DETAIL_GET_VALUE_QUERY_FN_HPP

#include <beman/monadics/detail/utility.hpp>

#include <concepts>

namespace beman::monadics::detail {

template<typename Box, typename Traits>
[[nodiscard]] consteval decltype(auto) get_value_query_fn() noexcept {
    if constexpr (requires {
                      { Traits::has_value(std::declval<const Box&>()) } -> std::same_as<bool>;
                  }) {
        return [](const auto& box) {
            return Traits::has_value(box);
        };
    } else if constexpr (requires {
                             { std::declval<const Box&>().has_value() } -> std::same_as<bool>;
                         }) {
        return [](const auto& box) {
            return box.has_value();
        };
    }
}

template<typename Box, typename Traits>
concept has_value_query_fn = requires {
    { get_value_query_fn<Box, Traits>() } -> deduced;
} || on_error<"provide box_traits<Box>::has_value(const Box&) const, or Box::has_value() const">;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_GET_VALUE_QUERY_FN_HPP
