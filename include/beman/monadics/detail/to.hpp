// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_TO_HPP
#define BEMAN_MONADICS_DETAIL_TO_HPP

#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/never.hpp>
#include <beman/monadics/detail/unit.hpp>
#include <beman/monadics/detail/utility.hpp>

#include <type_traits>
#include <utility>

namespace beman::monadics {

namespace detail::_to {

template<box To, box From>
[[nodiscard]] constexpr To make_to_result(From&& from, auto&& error_fn) {
    using FromTraits = get_box_traits<From>;
    using ToTraits = get_box_traits<To>;

    if constexpr (no_case<From>) {
        unreachable();
    } else if constexpr (error_only<From>) {
        return ToTraits::make_error(error_fn());
    } else {
        if (FromTraits::has_value(from)) {
            if constexpr (std::is_void_v<typename ToTraits::value_type>) {
                return ToTraits::make();
            } else {
                return ToTraits::make(FromTraits::value(std::forward<From>(from)));
            }
        }

        if constexpr (value_only<From>) {
            unreachable();
        } else {
            return ToTraits::make_error(error_fn());
        }
    }
}

template<box To>
class to_closure {
  public:
    template<box From>
        requires(!has_error_channel<To> || has_error_channel<From> || no_error_case<From>)
             || on_error<"From has no error channel: provide a fallback error">
    [[nodiscard]] friend constexpr auto operator|(From&& from, to_closure&&) {
        return make_to_result<To>(std::forward<From>(from), [&] {
            if constexpr (has_error_channel<From> && has_error_channel<To>) {
                return get_box_traits<From>::error(std::forward<From>(from));
            } else {
                return unit{};
            }
        });
    }
};

template<template<typename...> typename To, typename From>
consteval auto deduce_to() {
    using FromTraits = get_box_traits<From>;
    if constexpr (has_error_channel<From>
                  && requires { typename To<typename FromTraits::value_type, typename FromTraits::error_type>; }) {
        return std::type_identity<To<typename FromTraits::value_type, typename FromTraits::error_type>>{};
    } else {
        return std::type_identity<To<typename FromTraits::value_type>>{};
    }
}

template<template<typename...> typename To, typename From>
using deduce_to_t = decltype(deduce_to<To, std::remove_cvref_t<From>>())::type;

template<template<typename...> typename To>
class deducing_to_closure {
  public:
    template<box From>
        requires requires { requires box<deduce_to_t<To, From>>; }
    [[nodiscard]] friend constexpr auto operator|(From&& from, deducing_to_closure&&) {
        return std::forward<From>(from) | to_closure<deduce_to_t<To, From>>{};
    }
};

template<box To, typename Err>
class fallback_to_closure {
    Err err_;

  public:
    constexpr explicit fallback_to_closure(Err err) : err_(std::move(err)) {}

    template<box From>
        requires(!has_error_channel<From>) || on_error<"From already has an error channel: no fallback needed">
    [[nodiscard]] friend constexpr auto operator|(From&& from, fallback_to_closure&& op) {
        return make_to_result<To>(std::forward<From>(from), [&] { return std::move(op.err_); });
    }
};

template<template<typename...> typename To, typename Err>
class deducing_fallback_to_closure {
    Err err_;

  public:
    constexpr explicit deducing_fallback_to_closure(Err err) : err_(std::move(err)) {}

    template<box From>
        requires requires { requires box<To<typename get_box_traits<From>::value_type, Err>>; }
    [[nodiscard]] friend constexpr auto operator|(From&& from, deducing_fallback_to_closure&& op) {
        using ToBox = To<typename get_box_traits<From>::value_type, Err>;
        return std::forward<From>(from) | fallback_to_closure<ToBox, Err>{std::move(op.err_)};
    }
};

} // namespace detail::_to

template<detail::box To>
[[nodiscard]] constexpr auto to() {
    return detail::_to::to_closure<To>{};
}

template<template<typename...> typename To>
[[nodiscard]] constexpr auto to() {
    return detail::_to::deducing_to_closure<To>{};
}

template<detail::box To, typename Err>
    requires detail::has_error_channel<To>
[[nodiscard]] constexpr auto to(Err&& err) {
    return detail::_to::fallback_to_closure<To, std::decay_t<Err>>{std::forward<Err>(err)};
}

template<template<typename...> typename To, typename Err>
[[nodiscard]] constexpr auto to(Err&& err) {
    return detail::_to::deducing_fallback_to_closure<To, std::decay_t<Err>>{std::forward<Err>(err)};
}

} // namespace beman::monadics

#endif // BEMAN_MONADICS_DETAIL_TO_HPP
