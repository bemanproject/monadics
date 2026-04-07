// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <beman/monadics/detail/and_then.hpp>

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("with-value") {
    constexpr auto result = CURLcode{CURLE_OK} | and_then([] { return CURLE_OK; });

    STATIC_REQUIRE(result == CURLE_OK);
}

TEST_CASE("with-error") {
    constexpr auto result = CURLcode{CURLE_COULDNT_RESOLVE_PROXY} | and_then([] { return CURLE_OK; });

    STATIC_REQUIRE(result == CURLE_COULDNT_RESOLVE_PROXY);
}

TEST_CASE("experiment") {
    /*

    CURL *curl = curl_easy_init();

    curl | and_then(curl_easy_setopt, CURLOPT_URL, "https://example.com")
         | and_then(curl_easy_setopt, ..., ...)
         | and_then(curl_easy_perform)

    CURLE_OK | and_then(curl, curl_easy_setopt)
             | and_then(curl_easy_setopt(curl)

    curl_easy_setopt
    // constexpr auto result = CURLcode{CURLE_COULDNT_RESOLVE_PROXY}
        // | and_then([] {
          // return CURLE_OK;
        // });

    STATIC_REQUIRE(result == CURLE_COULDNT_RESOLVE_PROXY);
    */
}

} // namespace beman::monadics::tests
