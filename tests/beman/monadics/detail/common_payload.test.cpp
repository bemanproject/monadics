// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/common_payload.hpp>
#include <beman/monadics/monadics.hpp>

#include <catch2/catch_test_macros.hpp>

#include <concepts>
#include <utility>
#include <variant>

namespace beman::monadics::detail::tests {

namespace {

struct Io {};
struct Parse {};

template<typename... Ts>
struct normalizing {
    std::variant<Ts...> held;

    template<typename T>
        requires(std::same_as<std::remove_cvref_t<T>, Ts> || ...)
    constexpr explicit(false) normalizing(T&& t)
        : held(std::in_place_type<std::remove_cvref_t<T>>, std::forward<T>(t)) {}

    template<typename... Sub>
        requires(sizeof...(Sub) < sizeof...(Ts))
    constexpr explicit(false) normalizing(const normalizing<Sub...>& sub)
        : held(std::visit([](const auto& t) { return std::variant<Ts...>{t}; }, sub.held)) {}
};

template<typename... Ts>
struct ordered {
    std::variant<Ts...> held;

    template<typename T>
        requires(std::same_as<std::remove_cvref_t<T>, Ts> || ...)
    constexpr explicit(false) ordered(T&& t) : held(std::in_place_type<std::remove_cvref_t<T>>, std::forward<T>(t)) {}

    template<typename... Sub>
        requires(sizeof...(Sub) < sizeof...(Ts))
    constexpr explicit(false) ordered(const ordered<Sub...>&) : held() {}
};

struct Left {};
struct Right {};
struct Neither {};

} // namespace

} // namespace beman::monadics::detail::tests

namespace beman::monadics::detail {

namespace t = tests;

template<>
struct common_payload<t::normalizing<t::Io>, t::normalizing<t::Parse>> {
    using type = t::normalizing<t::Io, t::Parse>;
};
template<>
struct common_payload<t::normalizing<t::Parse>, t::normalizing<t::Io>> {
    using type = t::normalizing<t::Io, t::Parse>;
};

template<>
struct common_payload<t::normalizing<t::Io, t::Parse>, t::normalizing<t::Io>> {
    using type = t::normalizing<t::Io, t::Parse>;
};
template<>
struct common_payload<t::normalizing<t::Io>, t::normalizing<t::Io, t::Parse>> {
    using type = t::normalizing<t::Io, t::Parse>;
};
template<>
struct common_payload<t::normalizing<t::Io, t::Parse>, t::normalizing<t::Parse>> {
    using type = t::normalizing<t::Io, t::Parse>;
};
template<>
struct common_payload<t::normalizing<t::Parse>, t::normalizing<t::Io, t::Parse>> {
    using type = t::normalizing<t::Io, t::Parse>;
};

template<typename... As, typename... Bs>
struct common_payload<t::ordered<As...>, t::ordered<Bs...>> {
    using type = t::ordered<As..., Bs...>;
};

template<>
struct common_payload<t::Left, t::Right> {
    using type = t::Neither;
};
template<>
struct common_payload<t::Right, t::Left> {
    using type = t::Neither;
};

} // namespace beman::monadics::detail

namespace beman::monadics::detail::tests {

TEST_CASE("two-payloads-of-one-type-need-no-algebra") {
    STATIC_REQUIRE(has_common_payload<Io, Io>);
    STATIC_REQUIRE(std::same_as<common_payload_t<Io, Io>, Io>);
}

TEST_CASE("the-case-that-does-not-exist-is-the-identity") {
    STATIC_REQUIRE(has_common_payload<Io, never>);
    STATIC_REQUIRE(has_common_payload<never, Io>);
    STATIC_REQUIRE(std::same_as<common_payload_t<Io, never>, Io>);
    STATIC_REQUIRE(std::same_as<common_payload_t<never, Io>, Io>);
}

TEST_CASE("payloads-nobody-adapted-have-none") {
    STATIC_REQUIRE_FALSE(has_common_payload<Io, Parse>);
}

TEST_CASE("an-algebra-names-the-payload-that-holds-either") {
    using I = normalizing<Io>;
    using P = normalizing<Parse>;

    STATIC_REQUIRE(has_common_payload<I, P>);
    STATIC_REQUIRE(std::same_as<common_payload_t<I, P>, normalizing<Io, Parse>>);
    STATIC_REQUIRE(std::same_as<common_payload_t<P, I>, normalizing<Io, Parse>>);
}

TEST_CASE("an-answer-that-depends-on-the-order-is-refused") {
    STATIC_REQUIRE_FALSE(has_common_payload<ordered<Io>, ordered<Parse>>);
}

TEST_CASE("an-answer-that-grows-when-rejoined-is-refused") {
    STATIC_REQUIRE_FALSE(has_common_payload<ordered<Io, Parse>, ordered<Parse>>);
}

TEST_CASE("a-payload-that-holds-neither-is-refused") {
    STATIC_REQUIRE_FALSE(has_common_payload<Left, Right>);
}

} // namespace beman::monadics::detail::tests
