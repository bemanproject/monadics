// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_EITHER_HPP
#define BEMAN_MONADICS_DETAIL_EITHER_HPP

#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/instance_of.hpp>
#include <beman/monadics/detail/invoke_with_error.hpp>
#include <beman/monadics/detail/invoke_with_value.hpp>
#include <beman/monadics/detail/utility.hpp>

#include <concepts>
#include <type_traits>
#include <utility>

namespace beman::monadics::detail {

namespace _either {

template<typename OnValue, typename OnError, box Box>
consteval decltype(auto) common_result() noexcept {
    if constexpr (invocable_with_value<OnValue, Box> && invocable_with_error<OnError, Box>) {
        using R = invoke_with_value_t<OnValue, Box>;
        if constexpr (std::same_as<R, invoke_with_error_t<OnError, Box>>) {
            return std::type_identity<R>{};
        }
    }
}

} // namespace _either

template<typename OnValue, typename OnError, typename Box>
concept eitherable =
    box<Box>
    && (invocable_with_value<OnValue, Box>
        || on_error<"the handler must accept the box's value, or take nothing where it is void">)
    && (invocable_with_error<OnError, Box>
        || on_error<"the handler must accept the box's error, or take nothing where the failure carries nothing">)
    && (requires {
           { _either::common_result<OnValue, OnError, Box>() } -> instance_of<std::type_identity>;
       } || on_error<"both handlers must return exactly the same type">);

template<typename OnValue, typename OnError, typename Box>
    requires eitherable<OnValue, OnError, Box>
using either_result_t = typename decltype(_either::common_result<OnValue, OnError, Box>())::type;

struct either_t {
    template<box Box, typename OnValue, typename OnError>
        requires eitherable<OnValue, OnError, Box>
    [[nodiscard]] constexpr auto operator()(OnValue&& onValue, OnError&& onError, Box&& box) const {
        if (get_box_traits<Box>::has_value(box)) {
            return invoke_with_value(std::forward<OnValue>(onValue), std::forward<Box>(box));
        }

        return invoke_with_error(std::forward<OnError>(onError), std::forward<Box>(box));
    }
};

inline constexpr either_t either{};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_EITHER_HPP
