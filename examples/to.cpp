// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/monadics.hpp>

#include <concepts>
#include <cstdlib>
#include <expected>
#include <optional>

// --- box_traits specializations for this example ---

template<typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    using error_type = unit;
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
    using error_type = unit;

    [[nodiscard]] inline static constexpr auto make_error(unit) noexcept { return myopt<T>{mynone{}}; }
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
