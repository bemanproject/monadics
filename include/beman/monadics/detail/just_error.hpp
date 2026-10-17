// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_JUST_ERROR_HPP
#define BEMAN_MONADICS_DETAIL_JUST_ERROR_HPP

#include <beman/monadics/detail/box_traits.hpp>
#include <beman/monadics/detail/never.hpp>

#include <concepts>
#include <type_traits>
#include <utility>

namespace beman::monadics::detail {

template<typename T>
class just_error_t {
  public:
    using error_type = std::remove_cvref_t<T>;

    template<typename F>
    using rebind_error = just_error_t<F>;

    template<typename U>
        requires std::constructible_from<T, U&&>
    constexpr explicit(false) just_error_t(U&& e) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : error_(std::forward<U>(e)) {}

    [[nodiscard]] constexpr T& error() & noexcept { return error_; }

    [[nodiscard]] constexpr const T& error() const& noexcept { return error_; }

    [[nodiscard]] constexpr T&& error() && noexcept { return std::forward<T>(error_); }

    [[nodiscard]] constexpr const T&& error() const&& noexcept { return std::forward<const T>(error_); }

    [[nodiscard]] constexpr bool has_value() const noexcept { return false; }

    [[nodiscard]] friend constexpr bool operator==(const just_error_t&, const just_error_t&)
        requires std::equality_comparable<error_type>
    = default;

  private:
    T error_;
};

template<typename E>
just_error_t(E) -> just_error_t<E>;

struct just_error_fn {
    template<typename E>
    [[nodiscard]] constexpr just_error_t<E> operator()(E&& e) const {
        return just_error_t<E>{std::forward<E>(e)};
    }
};

inline constexpr just_error_fn just_error{};

template<typename T>
struct box_traits<just_error_t<T>> {
    using value_type = never;
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_JUST_ERROR_HPP
