// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_INSPECT_ERROR_HPP
#define BEMAN_MONADICS_DETAIL_INSPECT_ERROR_HPP

#include <beman/monadics/detail/callable_adaptor.hpp>
#include <beman/monadics/detail/either.hpp>
#include <beman/monadics/detail/invoke_with_error.hpp>

#include <concepts>
#include <type_traits>
#include <utility>

namespace beman::monadics::detail {

template<typename Box, typename Fn>
concept inspect_errorable_impl =
    invocable_with_error<Fn, const std::remove_reference_t<Box>&>
    || on_error<"inspect_error: fn must accept the box's error by const reference, or take nothing">;

template<typename Fn>
class inspect_error_t {
  public:
    constexpr explicit inspect_error_t(Fn fn) : fn_(std::move(fn)) {}

    template<box Box>
        requires inspect_errorable_impl<Box, Fn>
    [[nodiscard]] friend constexpr Box operator|(Box&& box, inspect_error_t&& op) {
        either([](auto&&...) {},
               [&](auto&&... e)
                   requires std::invocable<Fn, decltype(e)...>
               { std::move(op.fn_)(std::forward<decltype(e)>(e)...); },
               std::as_const(box));

        return std::forward<Box>(box);
    }

  private:
    Fn fn_;
};

inline constexpr callable_adaptor<inspect_error_t> inspect_error{};

template<typename Box, typename Fn>
concept inspect_errorable = requires(Box&& box, Fn&& fn) {
    { std::forward<Box>(box) | inspect_error(std::forward<Fn>(fn)) };
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_INSPECT_ERROR_HPP
