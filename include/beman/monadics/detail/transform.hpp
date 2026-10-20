// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_TRANSFORM_HPP
#define BEMAN_MONADICS_DETAIL_TRANSFORM_HPP

#include <beman/monadics/detail/and_then.hpp>
#include <beman/monadics/detail/callable_adaptor.hpp>
#include <beman/monadics/detail/compose.hpp>
#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/invoke_with_value.hpp>
#include <beman/monadics/detail/just.hpp>
#include <beman/monadics/detail/same_box.hpp>
#include <beman/monadics/detail/to.hpp>

#include <type_traits>
#include <utility>

namespace beman::monadics::detail {

template<typename NewBox, typename NewValue>
concept buildable_from = requires {
    requires std::is_void_v<NewValue>;
    { get_box_traits<NewBox>::make() };
} || requires {
    { get_box_traits<NewBox>::make(std::declval<NewValue>()) };
};

template<typename Box,
         typename Fn,
         typename U = invoke_with_value_t<Fn, Box>,
         typename R = typename get_box_traits<Box>::template rebind<U>>
concept transformable_impl =
    object_result<U>
    && (same_box<Box, R> || on_error<"transform: fn must return a type compatible with rebind">)
    && and_thenable_return<R, Box>
    && (buildable_from<R, U> || on_error<"transform: the box cannot be built from what fn returns">);

template<typename Fn>
class transform_t {
  public:
    constexpr explicit transform_t(Fn fn) : fn_(std::move(fn)) {}

    template<box Box>
        requires transformable_impl<Box, Fn>
    [[nodiscard]] friend constexpr auto operator|(Box&& box, transform_t&& op) {
        using R = typename get_box_traits<Box>::template rebind<invoke_with_value_t<Fn, Box>>;

        return std::forward<Box>(box) | and_then([&op](auto&&... v) -> R {
                   return compose(just, std::move(op.fn_))(std::forward<decltype(v)>(v)...) | to<R>();
               });
    }

  private:
    Fn fn_;
};

inline constexpr callable_adaptor<transform_t> transform{};

template<typename Box, typename Fn>
concept transformable = requires(Box&& box, Fn&& fn) {
    { std::forward<Box>(box) | transform(std::forward<Fn>(fn)) };
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_TRANSFORM_HPP
