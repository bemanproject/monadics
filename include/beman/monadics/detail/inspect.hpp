// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_INSPECT_HPP
#define BEMAN_MONADICS_DETAIL_INSPECT_HPP

#include <beman/monadics/detail/callable_adaptor.hpp>
#include <beman/monadics/detail/either.hpp>
#include <beman/monadics/detail/invoke_with_value.hpp>

#include <concepts>
#include <type_traits>
#include <utility>

namespace beman::monadics::detail {

template<typename Box, typename Fn>
concept inspectable_impl = invocable_with_value<Fn, const std::remove_reference_t<Box>&>
                        || on_error<"inspect: fn must accept the box's value by const reference">;

template<typename Fn>
class inspect_t {
  public:
    constexpr explicit inspect_t(Fn fn) : fn_(std::move(fn)) {}

    template<box Box>
        requires inspectable_impl<Box, Fn>
    [[nodiscard]] friend constexpr Box operator|(Box&& box, inspect_t&& op) {
        either([&](auto&&... v)
                   requires std::invocable<Fn, decltype(v)...>
               { std::move(op.fn_)(std::forward<decltype(v)>(v)...); },
               [](auto&&...) {},
               std::as_const(box));

        return std::forward<Box>(box);
    }

  private:
    Fn fn_;
};

inline constexpr callable_adaptor<inspect_t> inspect{};

template<typename Box, typename Fn>
concept inspectable = requires(Box&& box, Fn&& fn) {
    { std::forward<Box>(box) | inspect(std::forward<Fn>(fn)) };
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_INSPECT_HPP
