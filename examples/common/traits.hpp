// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_EXAMPLES_COMMON_TRAITS_HPP
#define BEMAN_EXAMPLES_COMMON_TRAITS_HPP

#include <beman/monadics/monadics.hpp>

#include <concepts>
#include <expected>
#include <optional>

// --- std::optional ---

template<typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    [[nodiscard]] inline static constexpr auto error() noexcept { return std::nullopt; }
};

// --- std::expected ---

template<typename T, typename E>
struct beman::monadics::box_traits<std::expected<T, E>> {
    [[nodiscard]] inline static constexpr auto make_error(auto&& e) noexcept {
        return std::expected<T, E>{std::unexpect, std::forward<decltype(e)>(e)};
    }
};

// --- myopt: a minimal custom box ---

struct mynone {};

template<typename T>
class myopt {
  public:
    constexpr myopt(mynone) {}

    template<typename U>
        requires std::convertible_to<U, T>
    constexpr myopt(U&& u) : value_(std::forward<U>(u)), has_value_{true} {}

    constexpr T value() const noexcept { return value_; }
    constexpr bool has_value() const noexcept { return has_value_; }

  private:
    T value_{};
    bool has_value_{false};
};

template<typename T>
myopt(T) -> myopt<T>;

template<typename T>
struct beman::monadics::box_traits<myopt<T>> {
    [[nodiscard]] inline static constexpr auto error() noexcept { return mynone{}; }
};

// --- CURLcode: a C enum adapted as a box with void value and error channel ---

extern "C" {

typedef enum {
    CURLE_OK = 0,
    CURLE_UNSUPPORTED_PROTOCOL,
    CURLE_FAILED_INIT,
    CURLE_NOT_BUILT_IN,
} CURLcode;

}; // extern "C"

template<std::same_as<CURLcode> Box>
struct beman::monadics::box_traits<Box> {
    using value_type = void;
    using error_type = CURLcode;

    template<typename V>
    using rebind = Box;

    template<typename>
    using rebind_error = Box;

    [[nodiscard]] static constexpr bool has_value(const Box& box) noexcept { return box == CURLE_OK; }

    static constexpr value_type value(Box&&) noexcept {}

    [[nodiscard]] static constexpr decltype(auto) error(auto&& box) noexcept {
        return std::forward<decltype(box)>(box);
    }

    [[nodiscard]] static constexpr decltype(auto) make(auto&& v) noexcept { return std::forward<decltype(v)>(v); }

    [[nodiscard]] static constexpr decltype(auto) make_error(auto&& e) noexcept {
        return std::forward<decltype(e)>(e);
    }
};

#endif // BEMAN_EXAMPLES_COMMON_TRAITS_HPP
