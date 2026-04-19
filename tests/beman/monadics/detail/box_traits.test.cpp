// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/box_traits.hpp>

#include <catch2/catch_template_test_macros.hpp>

struct NoSpecialization {};

struct EmptySpecialization {};

template<>
struct beman::monadics::detail::box_traits<EmptySpecialization> {};

struct NonEmptySpecialization {};

template<>
struct beman::monadics::detail::box_traits<NonEmptySpecialization> {
    using value_type = int;
};

namespace beman::monadics::detail::tests {

TEMPLATE_TEST_CASE_SIG("concept",
                       "",
                       ((typename T, bool Result), T, Result),
                       (NoSpecialization, false),
                       (EmptySpecialization, true),
                       (NonEmptySpecialization, true)) {
    STATIC_REQUIRE(has_box_traits<T> == Result);
}

} // namespace beman::monadics::detail::tests
