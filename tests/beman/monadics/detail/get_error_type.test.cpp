// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_error_type.hpp>

#include <catch2/catch_template_test_macros.hpp>

#include <concepts>

namespace beman::monadics::detail::tests {

namespace {

template<typename T>
struct box_traits {};

struct TraitsErrorType {};

template<>
struct box_traits<TraitsErrorType> {
    using error_type = int;
};

struct BoxErrorType {
    using error_type = int;
};

struct TraitsErrorFn {};

template<>
struct box_traits<TraitsErrorFn> {
    static constexpr int error() noexcept { return -1; }
};

struct BothErrorType {
    using error_type = double;
};

template<>
struct box_traits<BothErrorType> {
    using error_type = int;
};

} // namespace

TEMPLATE_TEST_CASE_SIG("concept",
                       "",
                       ((typename Box, bool Has), Box, Has),
                       (int, false),
                       (TraitsErrorType, true),
                       (BoxErrorType, true),
                       (TraitsErrorFn, true),
                       (BothErrorType, true)) {
    using Traits = box_traits<Box>;
    if constexpr (Has) {
        STATIC_REQUIRE(has_error_type<Box, Traits>);
    } else {
        STATIC_REQUIRE_FALSE(has_error_type<Box, Traits>);
    }
}

TEMPLATE_TEST_CASE("get", "", TraitsErrorType, BoxErrorType, TraitsErrorFn, BothErrorType) {
    using Box = TestType;
    using Traits = box_traits<Box>;
    STATIC_REQUIRE(std::same_as<get_error_type_t<Box, Traits>, int>);
}

} // namespace beman::monadics::detail::tests
