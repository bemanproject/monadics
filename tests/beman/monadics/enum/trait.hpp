// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_RAW_PTR_TRAIT_HPP
#define BEMAN_MONADICS_RAW_PTR_TRAIT_HPP

#include <beman/monadics/box_traits.hpp>

#include <type_traits>

extern "C" {

typedef enum {
    CURLE_OK = 0,
    CURLE_UNSUPPORTED_PROTOCOL,  /* 1 */
    CURLE_FAILED_INIT,           /* 2 */
    CURLE_URL_MALFORMAT,         /* 3 */
    CURLE_NOT_BUILT_IN,          /* 4 - [was obsoleted in August 2007 for
                                    7.17.0, reused in April 2011 for 7.21.5] */
    CURLE_COULDNT_RESOLVE_PROXY, /* 5 */
    CURLE_COULDNT_RESOLVE_HOST,  /* 6 */
    // ...
    CURL_LAST /* never use! */
} CURLcode;

}; // extern "C"

template <std::same_as<CURLcode> Box>
struct beman::monadics::box_traits<Box> {
    using value_type = void;
    using error_type = CURLcode;

    template <typename V>
    using rebind = Box;

    template <typename>
    using rebind_error = Box;

    [[nodiscard]] inline static constexpr bool has_value(const Box& box) noexcept { return box == CURLE_OK; }

    inline static constexpr value_type value(Box&&) noexcept {}

    [[nodiscard]] inline static constexpr decltype(auto) error(auto&& box) noexcept {
        return std::forward<decltype(box)>(box);
    }

    [[nodiscard]] inline static constexpr decltype(auto) lift(auto&& v) noexcept {
        return std::forward<decltype(v)>(v);
    }

    [[nodiscard]] inline static constexpr decltype(auto) lift_error(auto&& e) noexcept {
        return std::forward<decltype(e)>(e);
    }
};

#endif // BEMAN_MONADICS_RAW_PTR_TRAIT_HPP
