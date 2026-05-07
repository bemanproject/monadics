// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/monadics.hpp>

#include <concepts>
#include <cstdlib>
#include <expected>
#include <iostream>
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

// --- inspect and inspect_error ---

namespace beman::monadics {

template<typename Fn>
class inspect_t {
    Fn fn_;

  public:
    constexpr explicit inspect_t(Fn fn) : fn_(std::move(fn)) {}

    template<box Box>
    [[nodiscard]] friend constexpr auto operator|(Box&& box, inspect_t&& op) {
        using Traits = get_box_traits<Box>;
        if (Traits::has_value(box)) {
            if constexpr (std::is_void_v<typename Traits::value_type>) {
                op.fn_();
            } else {
                op.fn_(Traits::value(box));
            }
        }
        return std::forward<Box>(box);
    }
};

template<typename Fn>
class inspect_error_t {
    Fn fn_;

  public:
    constexpr explicit inspect_error_t(Fn fn) : fn_(std::move(fn)) {}

    template<box Box>
    [[nodiscard]] friend constexpr auto operator|(Box&& box, inspect_error_t&& op) {
        using Traits = get_box_traits<Box>;
        if (!Traits::has_value(box)) {
            if constexpr (has_error_channel<Box>) {
                op.fn_(Traits::error(box));
            } else {
                op.fn_();
            }
        }
        return std::forward<Box>(box);
    }
};

inline constexpr callable_adaptor<inspect_t> inspect{};
inline constexpr callable_adaptor<inspect_error_t> inspect_error{};

} // namespace beman::monadics

// --- main ---

int main() {
    namespace bms = beman::monadics;

    // inspect: observe value in optional chain, box passes through unchanged
    {
        constexpr auto result = std::optional{42}
                              | bms::inspect([](int) { /* observe v */ })
                              | bms::transform([](int v) { return v * 2; });
        static_assert(result == std::optional{84});

        const auto rt = std::optional{42}
                      | bms::inspect([](int v) { std::cout << "  inspect: value = " << v << "\n"; })
                      | bms::transform([](int v) { return v * 2; });
        std::cout << "[optional] value path: result = " << rt.value() << "\n";
    }

    // inspect_error: observe absence in optional chain
    {
        constexpr auto result = std::optional<int>{}
                              | bms::inspect_error([]() { /* observe error */ })
                              | bms::or_else([]() { return std::optional{0}; });
        static_assert(result == std::optional{0});

        const auto rt = std::optional<int>{}
                      | bms::inspect_error([]() { std::cout << "  inspect_error: empty (nullopt)\n"; })
                      | bms::or_else([]() { return std::optional{0}; });
        std::cout << "[optional] empty path: result = " << rt.value() << "\n";
    }

    // inspect + inspect_error on expected with value
    {
        constexpr auto result = std::expected<int, int>{10}
                              | bms::inspect([](int) { /* observe v */ })
                              | bms::inspect_error([](int) { /* not called */ })
                              | bms::transform([](int v) { return v + 5; });
        static_assert(result == std::expected<int, int>{15});

        const auto rt = std::expected<int, std::string>{10}
                      | bms::inspect([](int v) { std::cout << "  inspect: value = " << v << "\n"; })
                      | bms::inspect_error([](const std::string&) { std::cout << "  inspect_error: not called\n"; })
                      | bms::transform([](int v) { return v + 5; });
        std::cout << "[expected] value path: result = " << rt.value() << "\n";
    }

    // inspect + inspect_error on expected with error
    {
        constexpr auto result = std::expected<int, int>{std::unexpected{7}}
                              | bms::inspect([](int) { /* not called */ })
                              | bms::inspect_error([](int) { /* observe e */ })
                              | bms::or_else([](int e) { return std::expected<int, int>{e + 1}; });
        static_assert(result == std::expected<int, int>{8});

        const auto rt = std::expected<int, std::string>{std::unexpected{"not found"}}
                      | bms::inspect([](int) { std::cout << "  inspect: not called\n"; })
                      | bms::inspect_error([](const std::string& e) {
                            std::cout << "  inspect_error: error = \"" << e << "\"\n";
                        })
                      | bms::transform_error([](const std::string& e) { return "wrapped: " + e; })
                      | bms::or_else([](const std::string&) { return std::expected<int, std::string>{-1}; });
        std::cout << "[expected] error path: result = " << rt.value() << "\n";
    }

    // inspect on myopt: user-defined box type
    {
        constexpr auto result =
            myopt{5} | bms::inspect([](int) { /* observe v */ }) | bms::transform([](int v) { return v + 10; });
        static_assert(result.value() == 15);

        const auto rt = myopt{5}
                      | bms::inspect([](int v) { std::cout << "  inspect: value = " << v << "\n"; })
                      | bms::transform([](int v) { return v + 10; });
        std::cout << "[myopt] value path: result = " << rt.value() << "\n";
    }

    // inspect_error on empty myopt
    {
        constexpr auto result = myopt<int>{mynone{}}
                              | bms::inspect_error([]() { /* observe error */ })
                              | bms::or_else([]() { return myopt{99}; });
        static_assert(result.value() == 99);

        const auto rt = myopt<int>{mynone{}}
                      | bms::inspect_error([]() { std::cout << "  inspect_error: empty (mynone)\n"; })
                      | bms::or_else([]() { return myopt{99}; });
        std::cout << "[myopt] empty path: result = " << rt.value() << "\n";
    }

    // inspect on CURLcode (void value type, error channel)
    {
        constexpr auto result = CURLcode{CURLE_OK}
                              | bms::inspect([]() { /* ok */ })
                              | bms::inspect_error([](CURLcode) { /* not called */ });
        static_assert(result == CURLE_OK);

        const auto rt = CURLcode{CURLE_OK}
                      | bms::inspect([]() { std::cout << "  inspect: ok\n"; })
                      | bms::inspect_error([](CURLcode) { std::cout << "  inspect_error: not called\n"; });
        std::cout << "[CURLcode] ok path: result = " << rt << "\n";
    }

    // inspect_error on CURLcode error
    {
        constexpr auto result = CURLcode{CURLE_FAILED_INIT}
                              | bms::inspect([]() { /* not called */ })
                              | bms::inspect_error([](CURLcode) { /* observe e */ })
                              | bms::or_else([](CURLcode e) { return e; });
        static_assert(result == CURLE_FAILED_INIT);

        const auto rt =
            CURLcode{CURLE_FAILED_INIT}
            | bms::inspect([]() { std::cout << "  inspect: not called\n"; })
            | bms::inspect_error([](CURLcode e) { std::cout << "  inspect_error: error code = " << e << "\n"; })
            | bms::transform_error([](CURLcode) { return CURLE_NOT_BUILT_IN; })
            | bms::or_else([](CURLcode e) { return e; });
        std::cout << "[CURLcode] error path: result = " << rt << "\n";
    }

    // chained: inspect mixed into a real pipeline
    {
        constexpr auto result = std::expected<int, int>{std::unexpected{3}}
                              | bms::inspect_error([](int) {})
                              | bms::transform_error([](int e) { return e * 10; })
                              | bms::inspect_error([](int) {})
                              | bms::or_else([](int e) { return std::expected<int, int>{e}; });
        static_assert(result == std::expected<int, int>{30});

        const auto rt =
            std::expected<int, std::string>{std::unexpected{"fail"}}
            | bms::inspect_error([](const std::string& e) { std::cout << "  inspect_error[1]: \"" << e << "\"\n"; })
            | bms::transform_error([](const std::string& e) { return "[" + e + "]"; })
            | bms::inspect_error([](const std::string& e) { std::cout << "  inspect_error[2]: \"" << e << "\"\n"; })
            | bms::or_else([](const std::string&) { return std::expected<int, std::string>{0}; });
        std::cout << "[chained] pipeline: result = " << rt.value() << "\n";
    }

    return EXIT_SUCCESS;
}
