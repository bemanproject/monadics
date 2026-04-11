// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_PIPE_ADAPTOR_HPP
#define BEMAN_MONADICS_DETAIL_PIPE_ADAPTOR_HPP

#include <beman/monadics/detail/access_key.hpp>

#include <type_traits>
#include <utility>

namespace beman::monadics::detail {

template<typename T>
struct pipe_adaptor {
    template<typename Fn>
    [[nodiscard]] constexpr decltype(auto)
        operator()(Fn&& fn) const noexcept(std::is_nothrow_constructible_v<std::decay_t<Fn>, Fn>) {
        using Callable = std::decay_t<Fn>;

        struct closure : public T {
            friend T;

            // msvc bug does not allow to use Callable in noexcept
            constexpr explicit closure(Fn&& f) noexcept(std::is_nothrow_constructible_v<std::decay_t<Fn>, Fn>)
                : identity_(std::forward<Fn>(f)) {}

            constexpr Callable& identity(access_key<T>) & noexcept { return identity_; }
            constexpr const Callable& identity(access_key<T>) const& noexcept { return identity_; }
            constexpr Callable&& identity(access_key<T>) && noexcept { return std::move(identity_); }
            constexpr const Callable&& identity(access_key<T>) const&& noexcept { return std::move(identity_); }

          private:
            Callable identity_;
        };

        return closure{std::forward<Fn>(fn)};
    }
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_PIPE_ADAPTOR_HPP
