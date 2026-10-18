// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_COMMON_PAYLOAD_HPP
#define BEMAN_MONADICS_DETAIL_COMMON_PAYLOAD_HPP

#include <beman/monadics/detail/never.hpp>
#include <beman/monadics/detail/utility.hpp>

#include <concepts>
#include <type_traits>

namespace beman::monadics::detail {

template<typename P1, typename P2>
struct common_payload;

namespace _common_payload {

template<typename P1, typename P2>
[[nodiscard]] consteval auto computed() noexcept {
    if constexpr (std::same_as<P1, P2>) {
        return std::type_identity<P1>{};
    } else if constexpr (std::same_as<P2, never>) {
        return std::type_identity<P1>{};
    } else if constexpr (std::same_as<P1, never>) {
        return std::type_identity<P2>{};
    } else {
        return std::type_identity<typename common_payload<P1, P2>::type>{};
    }
}

template<typename P1, typename P2>
concept structural = std::same_as<P1, P2> || std::same_as<P1, never> || std::same_as<P2, never>;

template<typename P1, typename P2>
concept named = requires { typename common_payload<P1, P2>::type; };

template<typename P1, typename P2>
concept commutative = requires {
    requires named<P2, P1>;
    requires std::same_as<typename common_payload<P1, P2>::type, typename common_payload<P2, P1>::type>;
};

template<typename P1, typename P2>
concept injects = requires {
    requires std::constructible_from<typename common_payload<P1, P2>::type, P1>;
    requires std::constructible_from<typename common_payload<P1, P2>::type, P2>;
};

} // namespace _common_payload

template<typename P1, typename P2>
concept has_common_payload =
    _common_payload::structural<P1, P2>
    || ((_common_payload::named<P1, P2>
         || on_error<"specialize common_payload for these two payloads to name the type that holds either">)
        && (_common_payload::commutative<P1, P2>
            || on_error<"specialize common_payload for both orders, or normalize so the orders answer alike">)
        && (_common_payload::injects<P1, P2>
            || on_error<"give the type common_payload names a converting constructor from each payload">));

template<typename P1, typename P2>
    requires has_common_payload<P1, P2>
using common_payload_t = typename decltype(_common_payload::computed<P1, P2>())::type;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_COMMON_PAYLOAD_HPP
