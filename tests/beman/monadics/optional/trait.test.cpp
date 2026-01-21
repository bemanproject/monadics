// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <optional>
#include <type_traits>
#include <functional>

namespace beman::monadics::tests {

TEST_CASE("box-trait-for") {
    using Traits = box_traits_for<std::optional<int>>;
    STATIC_REQUIRE(std::same_as<Traits::value_type, int>);
    STATIC_REQUIRE(std::same_as<Traits::error_type, std::nullopt_t>);
    STATIC_REQUIRE(std::same_as<Traits::rebind<float>, std::optional<float>>);
    STATIC_REQUIRE(std::same_as<Traits::rebind_error<float>, std::optional<int>>);
    STATIC_REQUIRE(Traits::value(std::optional{5}) == 5);
    STATIC_REQUIRE(Traits::has_value(std::optional{5}));
    STATIC_REQUIRE(Traits::has_value(std::optional<int>{}) == false);
    STATIC_REQUIRE(std::same_as<decltype(Traits::error()), std::nullopt_t>);
    STATIC_REQUIRE(Traits::lift(10) == std::optional{10});
    STATIC_REQUIRE(Traits::lift_error(Traits::error()) == std::optional<int>{});
}

/*
namespace impl1 {

template<typename T, typename Traits = box_traits<T>>
concept is_box = requires {
  // value_type
  requires requires {
    typename T::value_type;
  } || requires {
    typename box_traits<T>::value_type;
  } || requires {
    // deducable T
    typename box_traits<T>::value_type;
  };
  { detail::_box_traits::valueType<T>() } -> std::same_as<void>;

  // typename T::template rebind<typename T::value_type>;
  // requires std::same_as<T, typename T::template rebind<typename T::value_type>>;
  // requires std::same_as<
    // typename T::value_type,
    // typename T::template rebind<typename T::value_type>::value_type
  // >;
  // requires std::same_as<
    // typename T::error_type,
    // typename T::template rebind<typename T::value_type>::error_type
  // >;

  // typename T::error_type;
  // typename T::template rebind_error<typename T::value_type>;
  // requires std::same_as<T, typename T::template rebind_error<typename T::value_type>>;
  // requires std::same_as<
    // typename T::value_type,
    // typename T::template rebind_error<typename T::error_type>::value_type
  // >;
  // requires std::same_as<
    // typename T::error_type,
    // typename T::template rebind_error<typename T::error_type>::error_type
  // >;

  requires requires {
    { std::declval<T>().has_value() } -> std::same_as<bool>;
    { std::declval<T>().value() } -> std::convertible_to<typename T::value_type>;
    { std::declval<T>().error() } -> std::convertible_to<typename T::error_type>;
  } || requires {
    typename T::box_type;
    { T::has_value(std::declval<typename T::box_type>()) } -> std::same_as<bool>;
    { T::value(std::declval<typename T::box_type>()) } -> std::convertible_to<typename T::value_type>;
    requires requires {
      { T::error(std::declval<typename T::box_type>()) } -> std::convertible_to<typename T::error_type>;
    } || requires {
      { T::error() } -> std::convertible_to<typename T::error_type>;
    };
  };

  // lift
  // lift_error
};


template <typename T, std::size_t N>
concept decomposable = requires {
    []<template <typename...> class U, typename... Args>
        requires (sizeof...(Args) >= N)
      (U<Args...>*) {
    }(static_cast<std::remove_cvref_t<T>*>(nullptr));
};

template <typename T>
concept False = false;

template <typename T>
struct ExtractValueType;

template <template <typename...> typename T, typename V, typename... Args>
struct ExtractValueType<T<V, Args...>> {
    using type = V;
};

template <typename Traits, typename Box>
concept HasValueType = requires {
  typename Traits::value_type;
} || requires {
  typename Box::value_type;
} || requires {
  requires decomposable<Box, 1>;
};

template <typename Traits, typename Box>
[[nodiscard]] consteval auto getValueType() noexcept {
    if constexpr (requires { typename Traits::value_type; }) {
        return std::type_identity<typename Traits::value_type>{};
    } else if constexpr (requires { typename Box::value_type; }) {
        return std::type_identity<typename Box::value_type>{};
    } else if constexpr (requires { typename ExtractValueType<Box>; }) {
        return std::type_identity<typename ExtractValueType<Box>::type>{};
    } else {
        static_assert(False<Box>, "Not able to extract value_type");
    }
}

template <typename Traits, typename Box>
  requires HasValueType<Traits, Box>
using ValueType = decltype(getValueType<Traits, Box>())::type;

template <typename Traits, typename Box>
concept HasErrorType = requires {
  typename Traits::error_type;
} || requires {
  typename Box::error_type;
} || requires {
  { Traits::error() };
// } || requires {
  // requires decomposable<Box, 2>;
};

template <typename Traits, typename Box>
consteval auto getErrorType() noexcept {
    if constexpr (requires { typename Traits::error_type; }) {
        return std::type_identity<typename Traits::error_type>{};
    } else if constexpr (requires { typename Box::error_type; }) {
        return std::type_identity<typename Box::error_type>{};
    } else if constexpr (requires { Traits::error(); }) {
        return std::type_identity<std::remove_cvref_t<decltype(Traits::error())>>{};
        // } else if constexpr (requires { typename ExtractErrorType<Box>; }) {
        // return std::type_identity<typename ExtractErrorType<Box>>{};
    } else {
        static_assert(False<Box>, "Not able to extract error_type");
    }
}

template <typename Traits, typename Box>
  requires HasErrorType<Traits, Box>
using ErrorType = decltype(getErrorType<Traits, Box>())::type;

template <typename Traits, typename Box>
concept HasRebind = requires {
  typename ValueType<Traits, Box>;
  requires requires {
    typename Traits::template rebind<ValueType<Traits, Box>>;
    requires std::same_as<
      typename Traits::template rebind<ValueType<Traits, Box>>,
      Box
    >;
  } || requires {
    typename Box::template rebind<ValueType<Traits, Box>>;
    requires std::same_as<
      typename Box::template rebind<ValueType<Traits, Box>>,
      Box
    >;
  } || requires {
    requires decomposable<Box, 1>;
  };
};

template <typename V, template <typename...> class Box, typename T, typename... Args>
auto metaRebind(Box<T, Args...>*) -> Box<V, Args...>;

template <typename Traits, typename Box, typename T = ValueType<Traits, Box>>
consteval auto rebindValue() noexcept {
    if constexpr (requires { typename Traits::template rebind<T>; }) {
        return std::type_identity<typename Traits::template rebind<T>>{};
    } else if constexpr (requires { typename Box::template rebind<T>; }) {
        return std::type_identity<typename Box::template rebind<T>>{};
    } else if constexpr (requires { metaRebind<T>(static_cast<Box*>(nullptr)); }) {
        return std::type_identity<decltype(metaRebind<T>(static_cast<Box*>(nullptr)))>{};
    } else {
        static_assert(False<Box>, "Not able to find rebind");
    }
}

template <typename Traits, typename Box>
  requires HasRebind<Traits, Box>
using Rebind = decltype(rebindValue<Traits, Box>())::type;

template <typename Traits, typename Box>
concept HasRebindError = requires {
  typename ValueType<Traits, Box>;
  requires requires {
    typename Traits::template rebind_error<ErrorType<Traits, Box>>;
    requires std::same_as<
      typename Traits::template rebind_error<ErrorType<Traits, Box>>,
      Box
    >;
  } || requires {
    typename Box::template rebind<ErrorType<Traits, Box>>;
    requires std::same_as<
      typename Box::template rebind<ErrorType<Traits, Box>>,
      Box
    >;
  } || requires {
    requires decomposable<Box, 2>;
  };
};

template <typename NewE,
          template <typename...> class Box,
          typename T, typename E, typename... Args>
auto metaRebindError(Box<T, E, Args...>*) -> Box<T, NewE, Args...>;

template <typename Traits, typename Box, typename E = ErrorType<Traits, Box>>
consteval auto rebindError() noexcept {
    if constexpr (requires { typename Traits::template rebind_error<E>; }) {
        return std::type_identity<typename Traits::template rebind_error<E>>{};
    } else if constexpr (requires { typename Box::template rebind_error<E>; }) {
        return std::type_identity<typename Box::template rebind_error<E>>{};
    } else if constexpr (requires { Traits::error(); }) {
        return std::type_identity<Box>{};
    } else if constexpr (requires { metaRebindError<E>(static_cast<Box*>(nullptr)); }) {
        return std::type_identity<decltype(metaRebindError<E>(static_cast<Box*>(nullptr)))>{};
    } else {
        // return Error<"Not able to find rebind_error">{};
        static_assert(False<Box>, "Not able to find rebind_error");
    }
}

template <typename Traits, typename Box>
  requires HasRebindError<Traits, Box>
using RebindError = decltype(rebindError<Traits, Box>())::type;

template <typename T, typename U>
concept SameIgnoreCVRef = std::same_as<
  std::remove_cvref_t<T>,
  std::remove_cvref_t<U>
>;

template <typename Traits, typename Box>
concept HasValueCheck = requires (Box box) {
  typename ValueType<Traits, Box>;
  requires requires {
    { Traits::has_value(box) } -> std::same_as<bool>;
  } || requires {
    { box.has_value() } -> std::same_as<bool>;
  } || requires {
    { static_cast<bool>(box) } -> std::same_as<bool>;
  };
};

template <typename Traits, typename Box>
  requires HasValueCheck<Traits, Box>
consteval auto hasValueCheck() noexcept {
    if constexpr (requires { Traits::has_value(std::declval<Box>()); }) {
        return &Traits::has_value;
    } else if (requires { std::declval<Box>().has_value(); }) {
        return [](const Box& b) { return b.has_value(); };
    } else {
      return [](const Box& b) { return static_cast<bool>(b); };
    }
};

template <typename Traits, typename Box>
concept HasValue = requires (Box box) {
  typename ValueType<Traits, Box>;
  requires requires {
    { Traits::value(box) } -> SameIgnoreCVRef<ValueType<Traits, Box>>;
  } || requires {
    { box.value() } -> SameIgnoreCVRef<ValueType<Traits, Box>>;
  } || requires {
    { *box } -> SameIgnoreCVRef<ValueType<Traits, Box>>;
  };
};

template <typename Traits, typename Box>
concept HasError = requires {
  typename ErrorType<Traits, Box>;
  requires requires {
    { Traits::error(std::declval<Box>()) } -> SameIgnoreCVRef<ErrorType<Traits, Box>>;
  } || requires {
    { Traits::error() } -> SameIgnoreCVRef<ErrorType<Traits, Box>>;
  } || requires {
    { std::declval<Box>().error() } -> SameIgnoreCVRef<ErrorType<Traits, Box>>;
  };
};

// || requires {
  // { Traits::value() };
// } || requires;

// template <typename Traits, typename Box>
// concept HasLift = requires {
  // typename ValueType<Traits, Box>;
  // requires std::is_void_v<ValueType<typename Traits, typename Box>
// };

template <typename Traits, typename Box, typename Value>
consteval auto liftValue() noexcept {
    if constexpr (requires { Traits::lift(); }) {
        return &Traits::lift;
    } else if constexpr (requires { Traits::lift(Traits::value(std::declval<Box>())); }) {
        return [](auto&& v) -> decltype(Traits::lift(std::forward<decltype(v)>(v))) {
            return Traits::lift(std::forward<decltype(v)>(v));
        };
        // return [](auto&& v) { return Traits::lift(std::forward<decltype(v)>(v)); };
    } else if constexpr (std::is_void_v<Value>) {
        return []() -> Box { return {}; };
    } else {
        return [](auto&& v) -> Box { return std::forward<decltype(v)>(v); };
    }
};

template <typename Traits, typename Box>
consteval auto liftError() noexcept {
    if constexpr (requires { Traits::lift_error(); }) {
        return &Traits::lift_error;
    } else if constexpr (requires { Traits::lift_error(Traits::error()); }) {
        return [](auto&& e) { return Traits::lift_error(std::forward<decltype(e)>(e)); };
    } else if constexpr (requires { Traits::error(); }) {
        return []() -> Box { return {}; };
    } else {
        return [](auto&& e) -> Box { return std::forward<decltype(e)>(e); };
    }
};

template <typename Traits, typename Box>
consteval auto getValue() noexcept {
    if constexpr (requires { std::declval<Box>().value(); }) {
        return [](auto&& b) { return std::forward<decltype(b)>(b).value(); };
    } else {
        return [](auto&& b) -> decltype(Traits::value(std::forward<decltype(b)>(b))) {
            return Traits::value(std::forward<decltype(b)>(b));
        };
    }
};

template <typename Traits, typename Box>
consteval auto getError() noexcept {
    if constexpr (requires { std::declval<Box>().error(); }) {
        return [](auto&& b) { return std::forward<decltype(b)>(b).error(); };
        // } else if constexpr (requires { *std::declval<Box>(); }) {
        // return [] (auto &&b) {
        // return *std::forward<decltype(b)>(b);
        // };
    } else if constexpr (requires { std::declval<Traits>().error(); }) {
        return &Traits::error;
    } else {
        return [](auto&& b) { return Traits::error(std::forward<decltype(b)>(b)); };
    }
};

template<typename T, typename Traits = box_traits<T>>
concept is_box2 = requires {
  // { valueType<T, Traits>() } -> same_as<type_identity>

  // typename ValueType<Traits, Box>;
  // typename ErrorType<Traits, Box>;
  // typename Rebind<Traits, Box>;
  // typename RebindError<Traits, Box>;
  // { hasValue<Traits, Box> } -> std::invocable<Box>
  // { value<Traits, Box> } -> std::convertible_to<Box>

  requires HasValueType<Traits, T>;
  requires HasErrorType<Traits, T>;
  requires HasRebind<Traits, T>;
  requires HasRebindError<Traits, T>;
  requires HasValueCheck<Traits, T>;
  // requires HasValue<Traits, T>;
  // requires HasError<Traits, T>;
};

} // namespace impl1

TEST_CASE("concept") {
  using Box = std::optional<int>;
  using Trait = box_traits_for<Box>;
  // STATIC_REQUIRE(is_box2<int> == false);
  // STATIC_REQUIRE(is_box2<Box>);
}
*/

namespace impl3 {

namespace detail {

// template <typename T, typename U = std::remove_cvref_t<T>>
// concept is_type_identity = requires {
// typename U::type;
// requires std::same_as<U, std::type_identity<typename U::type>>;
// };

template <typename T>
inline constexpr auto as_pointer = static_cast<std::remove_cvref_t<T>*>(nullptr);

template <typename T, std::size_t N>
concept decomposable = requires {
    []<template <typename...> class U, typename... Args>
        requires(sizeof...(Args) >= N)
    (U<Args...>*) {}(as_pointer<T>);
};

template <typename T, typename U>
concept same_template = requires {
    requires decomposable<T, 1>;
    requires decomposable<U, 1>;
    []<template <typename...> class C, typename... Ts, typename... Us>(C<Ts...>*, C<Us...>*) {}(as_pointer<T>,
                                                                                                as_pointer<U>);
};

template <typename T, template <typename...> class U>
concept instance_of = requires {
    requires decomposable<T, 1>;
    []<typename... Ts>(U<Ts...>*) {}(as_pointer<T>);
};

template <typename T>
struct ExtractValueType;

template <template <typename...> typename T, typename V, typename... Args>
struct ExtractValueType<T<V, Args...>> {
    using type = V;
};

template <typename Box, typename Traits>
[[nodiscard]] consteval auto get_value_type() noexcept {
    if constexpr (requires { typename Traits::value_type; }) {
        return std::type_identity<typename Traits::value_type>{};
    } else if constexpr (requires { typename Box::value_type; }) {
        return std::type_identity<typename Box::value_type>{};
    } else if constexpr (decomposable<Box, 1>) {
        return std::type_identity<typename ExtractValueType<Box>::type>{};
    }
}

template <typename Box, typename Traits = box_traits<Box>>
using value_type = typename decltype(get_value_type<Box, Traits>())::type;

template <typename Box, typename Traits>
consteval auto get_error_type() noexcept {
    if constexpr (requires { typename Traits::error_type; }) {
        return std::type_identity<typename Traits::error_type>{};
    } else if constexpr (requires { typename Box::error_type; }) {
        return std::type_identity<typename Box::error_type>{};
    } else if constexpr (requires { Traits::error(); }) {
        return std::type_identity<std::remove_cvref_t<decltype(Traits::error())>>{};
    }
    // else if constexpr (decomposable<Box, 2>) {
    // return std::type_identity<typename ExtractTemplateArg<Box, 1>::type>{};
    // }
}

template <typename Box, typename Traits = box_traits<Box>>
using error_type = typename decltype(get_error_type<Box, Traits>())::type;

template <template <typename...> class Box, typename... Args>
struct meta_rebind {
    template <typename T>
    using rebind = Box<T, Args...>;
};

template <template <typename...> class Box, typename T, typename... Args>
auto get_meta_rebind(Box<T, Args...>*) -> meta_rebind<Box, Args...>;

template <typename Box, typename Traits, typename T>
consteval auto get_rebind() noexcept {
    if constexpr (requires { typename Traits::template rebind<T>; }) {
        return std::type_identity<Traits>{};
    } else if constexpr (requires { typename Box::template rebind<T>; }) {
        return std::type_identity<Box>{};
    } else if constexpr (decomposable<Box, 1>) {
        return std::type_identity<decltype(get_meta_rebind(as_pointer<Box>))>{};
    }
}

template <typename T, typename Box>
concept is_same_box = requires {
    requires instance_of<T, std::type_identity>;
    requires std::same_as<typename T::type, Box>;
};

template <typename T, typename Box, typename Traits, template <typename...> class Get>
concept same_as_box_with = requires {
    requires instance_of<T, std::type_identity>;
    requires same_template<typename T::type, Box>;
    typename box_traits<typename T::type>;
    requires std::same_as<Get<typename T::type, box_traits<typename T::type>>, Get<Box, Traits>>;
};

// std::optional<int> box_traits<std::optional<int>>

// template <typename Box, typename Traits, typename T>
// using rebind = typename decltype(get_rebind<Box, Traits, T>())::type;

// template <typename Box, typename Traits, typename T>
// concept rebindable = requires {
// { get_rebind<Box, Traits, T>() } -> same_as_box_with<Box, Traits, error_type>;
// };

// template <typename Box, typename Traits, typename T>
// requires rebindable<Box, Traits, T>
// using rebind = typename decltype(get_rebind<Box, Traits, T>())::type;

template <typename Box, typename Traits, typename T>
    requires requires {
        { get_rebind<Box, Traits, T>() } -> instance_of<std::type_identity>;
    }
using rebind = typename decltype(get_rebind<Box, Traits, T>())::type;

template <typename Box, typename Traits, typename T>
concept rebindable = requires {
    typename rebind<Box, Traits, T>;
    requires same_template<rebind<Box, Traits, T>, Box>;
    requires std::same_as<error_type<rebind<Box, Traits, T>>, error_type<Box, Traits>>;
};

// template <typename T, typename Traits>
// concept rebindable2 = requires {
// typename Traits::template rebind<T>;
// requires is_instance_of<
// rebind<Box, Traits, T>,
// Box
// >;
// requires std::same_as<
// error_type<rebind<Box, Traits, T>>,
// error_type<Box, Traits>
// >;
// };

template <template <typename...> class Box, typename T, typename... Args>
struct meta_rebind_error {
    template <typename E>
    using rebind_error = Box<T, E, Args...>;
};

template <template <typename...> class Box, typename T, typename E, typename... Args>
auto get_meta_rebind_error(Box<T, E, Args...>*) -> meta_rebind_error<Box, T, Args...>;

template <typename Box>
struct no_rebind_error {
    template <typename>
    using rebind_error = Box;
};

template <typename Box, typename Traits, typename E = error_type<Box, Traits>>
consteval auto get_rebind_error() noexcept {
    if constexpr (requires { typename Traits::template rebind_error<E>; }) {
        return std::type_identity<Traits>{};
    } else if constexpr (requires { typename Box::template rebind_error<E>; }) {
        return std::type_identity<Box>{};
    } else if constexpr (requires { Traits::error(); }) {
        return std::type_identity<no_rebind_error<Box>>{};
    } else if constexpr (decomposable<Box, 2>) {
        return std::type_identity<decltype(get_meta_rebind_error(as_pointer<Box>))>{};
    }
}

template <typename Box, typename Traits, typename T>
concept rebindable_error = requires {
    { get_rebind_error<Box, Traits, T>() } -> same_as_box_with<Box, Traits, value_type>;
};

template <typename Box, typename Traits, typename E>
// requires rebindable_error<Box, Traits, E>
using rebind_error = typename decltype(get_rebind_error<Box, Traits, E>())::type;

template <typename Box, typename Traits>
consteval auto get_has_value() noexcept {
    if constexpr (requires { Traits::has_value(std::declval<Box>()); }) {
        return &Traits::has_value;
    } else if constexpr (requires { std::declval<Box>().has_value(); }) {
        return [](const Box& b) { return b.has_value(); };
    } else if constexpr (requires { static_cast<bool>(std::declval<Box>()); }) {
        return [](const Box& b) { return static_cast<bool>(b); };
    }
};

template <typename T, typename U>
concept is_same_unqualified = std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

template <typename Fn, typename Box, typename R>
concept is_invocable = requires {
    requires std::invocable<Fn, Box>;
    { std::invoke(std::declval<Fn>(), std::declval<Box>()) } -> is_same_unqualified<R>;
};

template <typename Box, typename Traits>
consteval auto get_value() noexcept {
    if constexpr (requires { Traits::value(std::declval<Box>()); }) {
        return [](auto&& b) -> decltype(Traits::value(std::forward<decltype(b)>(b))) {
            return Traits::value(std::forward<decltype(b)>(b));
        };
    } else if constexpr (requires { std::declval<Box>().value(); }) {
        return [](auto&& b) { return std::forward<decltype(b)>(b).value(); };
    }
};

template <typename Box, typename Traits>
consteval auto get_error() noexcept {
    if constexpr (requires { Traits::error(); }) {
        // return &Traits::error;
        return [](auto&&) { return Traits::error(); };
    } else if constexpr (requires { Traits::error(std::declval<Box>()); }) {
        return [](auto&& b) { return Traits::error(std::forward<decltype(b)>(b)); };
    } else if constexpr (requires { std::declval<Box>().error(); }) {
        return [](auto&& b) { return std::forward<decltype(b)>(b).error(); };
    };
};

// template<typename Box, typename Traits = box_traits<Box>>
// concept is_box = requires {
// { get_value_type<Box, Traits>() } -> is_type_identity;
// { get_error_type<Box, Traits>() } -> is_type_identity;
// { get_rebind<Box, Traits>() } -> is_same_box<Box>;
// { get_rebind_error<Box, Traits>() } -> is_same_box<Box>;
// { get_has_value<Box, Traits>() } -> is_invocable<Box, bool>;
// { get_value<Box, Traits>() } -> is_invocable<Box, value_type<Box, Traits>>;
// { get_error<Box, Traits>() } -> is_invocable<Box, error_type<Box, Traits>>;
// };

template <typename Box, typename Traits = box_traits<Box>>
concept is_box = requires {
    typename value_type<Box, Traits>;
    typename error_type<Box, Traits>;

    // { get_rebind<Box, Traits, T>() } -> instance_of<std::type_identity>;
    // requires instance_of<
    // rebind<Box, Traits, value_type<Box, Traits>>,
    // std::type_identity
    // >;
    typename rebind<Box, Traits, value_type<Box, Traits>>;
    typename rebind_error<Box, Traits, error_type<Box, Traits>>;

    { get_has_value<Box, Traits>() } -> is_invocable<Box, bool>;
    { get_value<Box, Traits>() } -> is_invocable<Box, value_type<Box, Traits>>;
    { get_error<Box, Traits>() } -> is_invocable<Box, error_type<Box, Traits>>;
};

} // namespace detail

template <typename Box>
concept is_box = requires {
    typename box_traits<Box>;
    requires detail::is_box<std::remove_cvref_t<Box>>;
};

template <typename Box, typename Traits = box_traits<Box>>
    requires detail::is_box<Box, Traits>
struct box_traits_for {
    using box_type   = Box;
    using value_type = detail::value_type<Box, Traits>;
    using error_type = detail::error_type<Box, Traits>;

    template <typename T>
    using rebind = detail::rebind<Box, Traits, value_type>::template rebind<T>;

    template <typename E>
    using rebind_error = detail::rebind_error<Box, Traits, error_type>::template rebind_error<E>;

    inline static constexpr auto value = detail::get_value<Box, Traits>();

    // template<typename T>
    // requires requires {
    //    is_same_box_type<detail::rebind_error<Box, Traits, T>, Box>;
    //    is_same_value_type<detail::rebind_error<Box, Traits, T>, Box>;
    // };
    // using rebind_error = detail::rebind_error<Box, Traits, T>;

    // { getErrorType<Box, Traits>() } -> is_type_identity;
    // { getRebind<Box, Traits>() } -> is_same_box<Box>;
    // { getRebindError<Box, Traits>() } -> is_same_box<Box>;
    // { get_has_value<Box, Traits>() } -> is_invocable<Box, bool>;
    // { get_value<Box, Traits>() } -> is_invocable<Box, value_type<Box, Traits>>;
    // { get_error<Box, Traits>() } -> is_invocable<Box, error_type<Box, Traits>>;
};

template <typename T, typename Trait>
concept rebindable = requires {
    typename Trait::template rebind<T>;
    requires detail::same_template<typename Trait::template rebind<T>, typename Trait::box_type>;
    requires std::same_as<typename box_traits_for<typename Trait::template rebind<T>>::error_type,
                          typename Trait::error_type>;
};

/*
struct op_fn {
    template <typename Box, typename Fn>
        requires requires {
            { std::invoke(fn, box) } -> value,
                                        Trait::rebind<deltype(value)> -> NewBox
                                                                         -> same_as_box_with<Trait, NewBox, value_type>
                                     -> rebindable<decltype(value), Trait>
                                     -> Trait::rebind
            std::invocable(fn, box);
            typename Trait::rebind<std::invoke_result_t<(fn, box)>>;
        }
    [[nodiscard]] inline constexpr auto operator()(Box&& box, Fn&& fn) const noexcept {
        using BoxTraits = box_traits_for<Box>;
        using NewValue =
            invoke_result_t<decltype(std::forward<Fn>(fn)), decltype(BoxTraits::value(std::forward<Box>(box)))>;
        using NewBoxTraits = box_traits_for<typename BoxTraits::template rebind<NewValue>>;

        return std::forward<Box>(box) | and_then(LiftedFn<decltype(fn), NewBoxTraits>{std::forward<Fn>(fn)});
    }
};
*/

} // namespace impl3

// TEST_CASE("impl2") {
// using Box   = std::optional<int>;
// using Trait = box_traits_for<Box>;

// // namespace impl2 = ::beman::monadics::detail::impl2;

// STATIC_REQUIRE(is_box<int> == false);
// STATIC_REQUIRE(is_box<Box>);

// // using Trait2 = impl2::box_traits_for<Box>;

// // STATIC_REQUIRE(std::same_as<Trait2::value_type, int>);
// // STATIC_REQUIRE(std::same_as<Trait2::error_type, std::nullopt_t>);
// // STATIC_REQUIRE(std::same_as<Trait2::rebind<float>, std::optional<float>>);
// // STATIC_REQUIRE(std::same_as<Trait2::rebind_error<float>, std::optional<int>>);
// // STATIC_REQUIRE(Trait2::value(std::optional{5}) == 5);
// // // STATIC_REQUIRE(std::same_as<Trait2::value(), std::optional<int>>);
// // // STATIC_REQUIRE(impl2::rebindable<void, Trait2> == false);
// // STATIC_REQUIRE(impl2::rebindable<int, Trait2>);

// // auto fn = [] (int v) { return v * 2.0; };

// // STATIC_REQUIRE(transformable<decltype(fn), Trait2>);

// // STATIC_REQUIRE(fooable<int>);

// // STATIC_REQUIRE(impl2::detail::rebindable<Box, Trait2, typename T>);

// // STATIC_REQUIRE(Traits::value(std::optional{5}) == 5);
// // STATIC_REQUIRE(Traits::has_value(std::optional{5}));
// // STATIC_REQUIRE(Traits::has_value(std::optional<int>{} == false));
// // STATIC_REQUIRE(std::same_as<decltype(Traits::error()), std::nullopt_t>);
// // STATIC_REQUIRE(Traits::lift(10) == std::optional{10});
// // STATIC_REQUIRE(Traits::lift_error() == std::optional<int>{});
// }

} // namespace beman::monadics::tests
