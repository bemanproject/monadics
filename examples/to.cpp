// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/monadics.hpp>

#include <concepts>
#include <cstdlib>
#include <expected>
#include <optional>

// --- box_traits specializations for this example ---

template<typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    [[nodiscard]] inline static constexpr auto error() noexcept { return std::nullopt; }
};

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

// --- to ---

namespace beman::monadics {

namespace detail::_to {

template<box To, box From>
[[nodiscard]] constexpr auto make_to_result(From&& from, auto&& error_fn) {
    using FromTraits = get_box_traits<From>;
    using ToTraits = get_box_traits<To>;

    if (FromTraits::has_value(from)) {
        if constexpr (std::is_void_v<typename ToTraits::value_type>) {
            return ToTraits::make();
        } else {
            return ToTraits::make(FromTraits::value(std::forward<From>(from)));
        }
    }
    return ToTraits::make_error(error_fn());
}

template<box To>
class to_closure {
  public:
    template<box From>
        requires(!has_error_channel<To> || has_error_channel<From>)
             || on_error<"From has no error channel: provide a fallback error">
    [[nodiscard]] friend constexpr auto operator|(From&& from, to_closure) {
        return make_to_result<To>(std::forward<From>(from), [&] {
            if constexpr (has_error_channel<From> && has_error_channel<To>) {
                return get_box_traits<From>::error(std::forward<From>(from));
            } else {
                return get_box_traits<To>::error();
            }
        });
    }
};

template<template<typename...> class To, typename From>
consteval auto deduce_to() {
    using FromTraits = get_box_traits<From>;
    if constexpr (has_error_channel<From>
                  && requires { typename To<typename FromTraits::value_type, typename FromTraits::error_type>; }) {
        return std::type_identity<To<typename FromTraits::value_type, typename FromTraits::error_type>>{};
    } else {
        return std::type_identity<To<typename FromTraits::value_type>>{};
    }
}

template<template<typename...> class To, typename From>
using deduce_to_t = decltype(deduce_to<To, std::remove_cvref_t<From>>())::type;

template<template<typename...> class To>
class deducing_to_closure {
  public:
    template<box From>
        requires requires { requires box<deduce_to_t<To, From>>; }
    [[nodiscard]] friend constexpr auto operator|(From&& from, deducing_to_closure) {
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

template<template<typename...> class To, typename Err>
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

template<box To>
[[nodiscard]] constexpr auto to() {
    return detail::_to::to_closure<To>{};
}

template<template<typename...> class To>
[[nodiscard]] constexpr auto to() {
    return detail::_to::deducing_to_closure<To>{};
}

template<box To, typename Err>
    requires has_error_channel<To>
[[nodiscard]] constexpr auto to(Err&& err) {
    return detail::_to::fallback_to_closure<To, std::decay_t<Err>>{std::forward<Err>(err)};
}

template<template<typename...> class To, typename Err>
[[nodiscard]] constexpr auto to(Err&& err) {
    return detail::_to::deducing_fallback_to_closure<To, std::decay_t<Err>>{std::forward<Err>(err)};
}

} // namespace beman::monadics

// --- main ---

int main() {
    namespace bms = beman::monadics;

    // optional(value) -> myopt: value preserved, custom box type
    {
        constexpr std::optional opt{15};
        static_assert((opt | bms::to<myopt>()).value() == 15);
        static_assert((opt | bms::to<myopt<int>>()).value() == 15);
    }

    // optional(value) -> expected: value preserved, fallback error unused
    {
        constexpr std::optional opt{15};
        static_assert((opt | bms::to<std::expected>(20)).value() == 15);
        static_assert((opt | bms::to<std::expected<int, int>>(20)).value() == 15);
    }

    // expected(value) -> optional: value preserved, error channel dropped
    {
        constexpr std::expected<int, int> exp{15};
        static_assert((exp | bms::to<std::optional>()).value() == 15);
        static_assert((exp | bms::to<std::optional<int>>()).value() == 15);
    }

    // expected(error) -> optional: error channel dropped, result is empty
    {
        constexpr std::expected<int, int> exp{std::unexpected{10}};
        static_assert(!(exp | bms::to<std::optional>()).has_value());
        static_assert(!(exp | bms::to<std::optional<int>>()).has_value());
    }

    // empty optional -> optional: identity-like, both lack error channel
    {
        constexpr std::optional<int> empty{};
        static_assert(!(empty | bms::to<std::optional>()).has_value());
        static_assert(!(empty | bms::to<std::optional<int>>()).has_value());
    }

    // empty optional -> expected: fallback error used
    {
        constexpr std::optional<int> empty{};
        constexpr auto result = empty | bms::to<std::expected<int, int>>(40);
        static_assert(result.error() == 40);
    }

    // expected(error) -> expected: error propagated
    {
        constexpr std::expected<int, int> exp{std::unexpected{10}};
        static_assert((exp | bms::to<std::expected>()).error() == 10);
        static_assert((exp | bms::to<std::expected<int, int>>()).error() == 10);
    }

    // CURLcode(error) -> CURLcode: error propagated, void value type
    {
        constexpr auto result = CURLcode{CURLE_NOT_BUILT_IN} | bms::to<CURLcode>();
        static_assert(result == CURLE_NOT_BUILT_IN);
    }

    // CURLcode(ok) -> CURLcode: value preserved, void value type identity
    {
        constexpr auto result = CURLcode{CURLE_OK} | bms::to<CURLcode>();
        static_assert(result == CURLE_OK);
    }

    // expected(value) -> myopt: value preserved, custom box target
    {
        constexpr std::expected<int, int> exp{15};
        static_assert((exp | bms::to<myopt>()).value() == 15);
        static_assert((exp | bms::to<myopt<int>>()).value() == 15);
    }

    // expected(error) -> myopt: error channel dropped, result is empty
    {
        constexpr std::expected<int, int> exp{std::unexpected{10}};
        static_assert(!(exp | bms::to<myopt>()).has_value());
        static_assert(!(exp | bms::to<myopt<int>>()).has_value());
    }

    // myopt(value) -> expected: value preserved, fallback error unused
    {
        constexpr auto val = myopt{15};
        static_assert((val | bms::to<std::expected>(20)).value() == 15);
        static_assert((val | bms::to<std::expected<int, int>>(20)).value() == 15);
    }

    // myopt(empty) -> expected: fallback error used
    {
        constexpr myopt<int> empty{mynone{}};
        static_assert((empty | bms::to<std::expected>(40)).error() == 40);
        static_assert((empty | bms::to<std::expected<int, int>>(40)).error() == 40);
    }

    constexpr auto r2 = myopt{10} | bms::and_then([](auto v) { return myopt{v}; });
    return r2.value();
}
