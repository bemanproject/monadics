// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_PIPE_ADAPTOR_HPP
#define BEMAN_MONADICS_DETAIL_PIPE_ADAPTOR_HPP

#include "beman/monadics/detail/access_key.hpp"

#include <type_traits>
#include <utility>

namespace beman::monadics::detail {

template <typename T>
struct pipe_adaptor {
    template <typename Fn>
    [[nodiscard]] constexpr decltype(auto)
        operator()(Fn&& fn) const noexcept(std::is_nothrow_constructible_v<std::decay_t<Fn>, Fn>) {
        using Callable = std::decay_t<Fn>;

        struct closure : public T {
            friend T;

            // msvc bug does not allow to use Callable in noexcept
            constexpr explicit closure(Fn&& f) noexcept(std::is_nothrow_constructible_v<std::decay_t<Fn>, Fn>)
                : callable_(std::forward<Fn>(f)) {}

            constexpr Callable& callable(access_key<T>) & noexcept { return callable_; }
            constexpr const Callable& callable(access_key<T>) const& noexcept { return callable_; }
            constexpr Callable&& callable(access_key<T>) && noexcept { return std::move(callable_); }
            constexpr const Callable&& callable(access_key<T>) const&& noexcept { return std::move(callable_); }

          private:
            Callable callable_;
        };

        return closure{std::forward<Fn>(fn)};
    }
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_PIPE_ADAPTOR_HPP
