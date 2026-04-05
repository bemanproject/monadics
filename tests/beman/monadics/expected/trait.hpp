// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_EXPECTED_TRAIT_HPP
#define BEMAN_MONADICS_EXPECTED_TRAIT_HPP

#include <beman/monadics/monadics.hpp>

#include <type_traits>

namespace stdx {

template<typename T, typename E>
    requires(!std::is_void_v<E>)
class [[nodiscard("expected value should be handled")]] expected {
    struct Void {};

    static constexpr bool IsVoid = std::is_void_v<T>;
    using Value = std::conditional_t<IsVoid, Void, T>;

  public:
    using value_type = T;
    using error_type = E;

    template<typename Value>
    using rebind = expected<Value, error_type>;

    template<typename Error>
    using rebind_error = expected<value_type, Error>;

    constexpr expected()
        requires std::default_initializable<Value>
        : value_{}, has_value_(true) {}

    template<typename U = value_type>
        requires std::same_as<std::remove_cvref_t<U>, value_type>
    constexpr expected(U&& u)
        requires(!IsVoid)
        : value_(std::forward<U>(u)), has_value_(true) {}

    template<typename U = error_type>
        requires std::same_as<std::remove_cvref_t<U>, error_type>
    constexpr expected(U&& u) : error_(std::forward<U>(u)), has_value_(false) {}

    constexpr expected(const expected&) = default;
    constexpr expected(expected&&) = default;

    [[nodiscard]] constexpr bool has_value() const noexcept { return has_value_; }

    [[nodiscard]] constexpr decltype(auto) value() & noexcept {
        if constexpr (!IsVoid) {
            return static_cast<value_type&>(value_);
        }
    }

    [[nodiscard]] constexpr decltype(auto) value() const& noexcept {
        if constexpr (!IsVoid) {
            return static_cast<const value_type&>(value_);
        }
    }

    [[nodiscard]] constexpr decltype(auto) value() && noexcept {
        if constexpr (!IsVoid) {
            return std::move(value_);
        }
    }

    [[nodiscard]] constexpr decltype(auto) value() const&& noexcept {
        if constexpr (!IsVoid) {
            return std::move(value_);
        }
    }

    [[nodiscard]] constexpr error_type& error() & noexcept { return error_; }

    [[nodiscard]] constexpr const error_type& error() const& noexcept { return error_; }

    [[nodiscard]] constexpr error_type&& error() && noexcept { return std::move(error_); }

    [[nodiscard]] constexpr const error_type&& error() const&& noexcept { return std::move(error_); }

    [[nodiscard]] friend constexpr bool operator==(const expected& lhs, const expected& rhs) noexcept {
        if (lhs.has_value_ != rhs.has_value_) {
            return false;
        }

        if (lhs.has_value_ != false) {
            if constexpr (!IsVoid) {
                return lhs.value_ == rhs.value_;
            }
        }

        return true;
    }

  private:
    union {
        Value value_;
        E error_;
    };

    bool has_value_{false};
};

} // namespace stdx

#endif // BEMAN_MONADICS_EXPECTED_TRAIT_HPP
