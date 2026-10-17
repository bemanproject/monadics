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

template<typename R, typename Fn, typename Box, typename Traits = get_box_traits<Box>>
concept told_the_value = requires {
    requires std::is_void_v<typename Traits::value_type>;
    requires requires(Fn fn) { fn.template operator()<R>(); };
} || requires(Fn fn) { fn.template operator()<R>(Traits::value(std::declval<Box>())); };

template<typename R, typename Fn, typename Box, typename Traits = get_box_traits<Box>>
concept told_the_error = requires(Fn fn) { fn.template operator()<R>(Traits::error(std::declval<Box>())); };

template<typename R, typename Fn, box Box>
    requires told_the_value<R, Fn, Box>
[[nodiscard]] constexpr R invoke_with_value_as(Fn&& fn, Box&& box) {
    using Traits = get_box_traits<Box>;
    if constexpr (std::is_void_v<typename Traits::value_type>) {
        return std::forward<Fn>(fn).template operator()<R>();
    } else {
        return std::forward<Fn>(fn).template operator()<R>(Traits::value(std::forward<Box>(box)));
    }
}

template<typename R, typename Fn, box Box>
    requires told_the_error<R, Fn, Box>
[[nodiscard]] constexpr R invoke_with_error_as(Fn&& fn, Box&& box) {
    return std::forward<Fn>(fn).template operator()<R>(get_box_traits<Box>::error(std::forward<Box>(box)));
}

template<typename OnValue, typename OnError, box Box>
consteval decltype(auto) common_result() noexcept {
    if constexpr (invocable_with_value<OnValue, Box>) {
        using R = invoke_with_value_t<OnValue, Box>;
        if constexpr (invocable_with_error<OnError, Box>) {
            if constexpr (std::same_as<R, invoke_with_error_t<OnError, Box>>) {
                return std::type_identity<R>{};
            }
        } else if constexpr (told_the_error<R, OnError, Box>) {
            return std::type_identity<R>{};
        }
    } else if constexpr (invocable_with_error<OnError, Box>) {
        using R = invoke_with_error_t<OnError, Box>;
        if constexpr (told_the_value<R, OnValue, Box>) {
            return std::type_identity<R>{};
        }
    }
}

template<typename OnValue, typename OnError, typename Box>
concept one_names_its_result = invocable_with_value<OnValue, Box> || invocable_with_error<OnError, Box>;

template<typename OnValue, typename OnError, typename Box>
concept value_side_callable =
    invocable_with_value<OnValue, Box>
    || (invocable_with_error<OnError, Box> && told_the_value<invoke_with_error_t<OnError, Box>, OnValue, Box>);

template<typename OnValue, typename OnError, typename Box>
concept error_side_callable =
    invocable_with_error<OnError, Box>
    || (invocable_with_value<OnValue, Box> && told_the_error<invoke_with_value_t<OnValue, Box>, OnError, Box>);

} // namespace _either

template<typename OnValue, typename OnError, typename Box>
concept eitherable =
    box<Box>
    && (_either::one_names_its_result<OnValue, OnError, Box> || on_error<"at least one handler must name its result">)
    && (_either::value_side_callable<OnValue, OnError, Box>
        || on_error<"the handler must accept the box's value, or take nothing where it is void">)
    && (_either::error_side_callable<OnValue, OnError, Box>
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
        using R = either_result_t<OnValue, OnError, Box>;

        if (get_box_traits<Box>::has_value(box)) {
            if constexpr (invocable_with_value<OnValue, Box>) {
                return invoke_with_value(std::forward<OnValue>(onValue), std::forward<Box>(box));
            } else {
                return _either::invoke_with_value_as<R>(std::forward<OnValue>(onValue), std::forward<Box>(box));
            }
        }

        if constexpr (invocable_with_error<OnError, Box>) {
            return invoke_with_error(std::forward<OnError>(onError), std::forward<Box>(box));
        } else {
            return _either::invoke_with_error_as<R>(std::forward<OnError>(onError), std::forward<Box>(box));
        }
    }
};

inline constexpr either_t either{};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_EITHER_HPP
