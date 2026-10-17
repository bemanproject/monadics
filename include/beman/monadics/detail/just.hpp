// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_JUST_HPP
#define BEMAN_MONADICS_DETAIL_JUST_HPP

#include <beman/monadics/detail/box_traits.hpp>
#include <beman/monadics/detail/never.hpp>

#include <concepts>
#include <type_traits>
#include <utility>

namespace beman::monadics::detail {

template<typename T>
class just_t {
  public:
    using value_type = std::remove_cvref_t<T>;

    template<typename U>
        requires std::constructible_from<T, U&&>
    constexpr explicit(false) just_t(U&& v) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : value_(std::forward<U>(v)) {}

    [[nodiscard]] constexpr T& value() & noexcept { return value_; }

    [[nodiscard]] constexpr const T& value() const& noexcept { return value_; }

    [[nodiscard]] constexpr T&& value() && noexcept { return std::forward<T>(value_); }

    [[nodiscard]] constexpr const T&& value() const&& noexcept { return std::forward<const T>(value_); }

    [[nodiscard]] constexpr bool has_value() const noexcept { return true; }

    [[nodiscard]] friend constexpr bool operator==(const just_t&, const just_t&)
        requires std::equality_comparable<value_type>
    = default;

  private:
    T value_;
};

template<typename V>
just_t(V) -> just_t<V>;

template<>
class just_t<void> {
  public:
    using value_type = void;

    constexpr just_t() noexcept = default;

    constexpr void value() const noexcept {}

    [[nodiscard]] constexpr bool has_value() const noexcept { return true; }

    [[nodiscard]] friend constexpr bool operator==(const just_t&, const just_t&) = default;
};

struct just_fn {
    [[nodiscard]] constexpr just_t<void> operator()() const noexcept { return {}; }

    template<typename V>
    [[nodiscard]] constexpr just_t<V> operator()(V&& v) const {
        return just_t<V>{std::forward<V>(v)};
    }
};

inline constexpr just_fn just{};

template<typename T>
struct box_traits<just_t<T>> {
    using error_type = never;
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_JUST_HPP
