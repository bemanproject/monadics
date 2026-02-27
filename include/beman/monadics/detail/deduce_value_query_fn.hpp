// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_DEDUCE_VALUE_QUERY_HPP
#define BEMAN_MONADICS_DETAIL_DEDUCE_VALUE_QUERY_HPP

#include <beman/monadics/detail/instance_of.hpp>

#include <concepts>
#include <utility>

namespace beman::monadics::detail {

namespace _deduce_value_query_fn {

template <typename Box, typename Trait>
consteval auto deduce() noexcept {
    if constexpr (requires { Trait::has_value(std::declval<Box>()); }) {
        return [](const auto& box) { return Trait::has_value(box); };
    } else if constexpr (requires { std::declval<Box>().has_value(); }) {
        return [](const auto& box) { return box.has_value(); };
    }
};

} // namespace _deduce_value_query_fn

template <typename Box, typename Traits>
concept has_value_query_fn = requires {
    { _deduce_value_query_fn::deduce<Box, Traits>() } -> std::invocable<Box>;
    { _deduce_value_query_fn::deduce<Box, Traits>()(std::declval<Box>()) } -> std::same_as<bool>;
};

template <typename Box, typename Traits>
    requires has_value_query_fn<Box, Traits>
inline constexpr auto deduce_value_query_fn = _deduce_value_query_fn::deduce<Box, Traits>();

} // namespace beman::monadics::detail

// has_value_query.hpp
template <typename Box, typename Traits>
concept has_traits_value_query_fn = requires {
    { Traits::has_value(std::declval<Box>()) } -> std::same_as<bool>;
};

template <typename Box>
concept has_box_value_query_fn = requires {
    { std::declval<Box>().has_value() } -> std::same_as<bool>;
};

template <typename Box, typename Traits>
concept has_value_query = has_traits_value_query_fn<Box, Traits> || has_box_value_query_fn<Box>;

// deduce_value_query_fn.hpp
template <typename Box, typename Traits>
    requires has_value_query<Box, Traits>
[[nodiscard]] consteval auto get_value_query_fn() {
    if constexpr (has_traits_value_query_fn<Box, Traits>) {
        return [](const auto& box) { return Traits::has_value(box); };
    } else {
        return [](const auto& box) { return box.has_value(); };
    }
}

/*

#include <concepts>
#include <utility>
#include <array>
#include <functional>
#include <optional>


template<typename T>
concept has_value_query_fn2 = requires {
  requires std::invocable<decltype(&T::has_value), T>;
  { std::invoke(&T::has_value, std::declval<T>()) } -> std::same_as<bool>;
};


static_assert(has_value_query_fn2<int> == false);
static_assert(has_value_query_fn2<std::optional<int>>);


template<std::size_t N>
struct err {
    char msg[N];

    consteval err(const char (&m)[N]) {
        std::copy(std::begin(m), std::end(m), std::begin(msg));
    }
};

template<std::size_t N>
err(const char (&str)[N]) -> err<N>;

template<typename T, err>
concept is_err = !std::is_void_v<T>;

template<typename T>
concept is_valid = !std::is_void_v<T>;

template<typename Box, typename Traits>
class get_value_query_fn3_t {
    [[nodiscard]] static consteval decltype(auto) deduce() noexcept {
        if constexpr (requires { Traits::has_value(std::declval<Box>()); }) {
            return [](const auto& box) { return Traits::has_value(box); };
        } else if constexpr (requires { std::declval<Box>().has_value(); }) {
            return [](const auto& box) { return box.has_value(); };
        }
    };

public:
//   template<typename D = int>
//   requires requires {
//       { deduce() } -> is_err<"Box or box_traits does not provide has_value">;
//     }
//   [[nodiscard]] consteval decltype(auto) operator()() const noexcept

//   {
//       return deduce();
//   }

    [[nodiscard]] consteval decltype(auto) operator()() const noexcept
        requires requires {
            { deduce() } -> std::same_as<void>;
        }
    = delete("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

    [[nodiscard]] consteval decltype(auto) operator()() const noexcept
        requires requires {
            { deduce() } -> is_valid;
        }
    {
        return deduce();
    }
};


template<typename Box, typename Traits>
inline constexpr get_value_query_fn3_t<Box, Traits> get_value_query_fn3;

template<typename Box, typename Traits>
concept has_value_query3 = requires {
    { get_value_query_fn3<Box, Traits>() };
};


// template<typename Box, typename Traits>
// concept has_value_query = requires {
//     { get_value_query_fn<Box, Traits>() };
// };

// struct get_value_query_fn_t {

//   // somehow add message here
//   template<typename Box, typename Traits>
//   [[nodiscard]] consteval auto operator()() const noexcept
//     requires requires {
//       { get_value_query_fn2<Box, Traits>() } -> is_err<"upppss">;
//     } = delete;

//   template<typename Box, typename Traits>
//   [[nodiscard]] consteval decltype(auto) operator()() const noexcept
//   {
//       return get_value_query_fn2<Box, Traits>();
//   }
// };


// inline constexpr get_value_query_fn_t get_value_query_fn;

// template<typename Box, typename Traits>
// concept has_value_query = has_value_query_fn<Traits> || has_value_query_fn<Box>;

struct Foo {
    template<typename T>
    static constexpr bool has_value(const std::optional<T> &opt) noexcept {
        return opt.has_value();
    }
};


template<typename T>
    requires has_value_query3<T, T>
void foo(T) {

}

template<typename T>
concept is_box = requires {
    { get_value_query_fn3<T, T>() };
    // requires has_value_query3<T, T>;
};

template<is_box T>
void boo(T t)
{
  //  foo(t);
}


int main() {
    //static_assert(has_value_query_fn<int, int> == false);
    //static_assert(has_value_query_fn<Foo, int> == true);

    static_assert(has_value_query3<int, int> == false);
    static_assert(has_value_query3<std::optional<int>, int>);
    // static_assert(has_value_query3<std::optional<int>, Foo>);

//    foo(std::optional{1});
    boo(1);
}
https://godbolt.org/z/s4EEMzKcY
*/
#endif // BEMAN_MONADICS_DETAIL_DEDUCE_VALUE_QUERY_HPP
