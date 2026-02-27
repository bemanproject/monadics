// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_make_error_fn.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::detail::tests {

namespace {

template <typename T>
struct box_traits {};

// Branch 1: Traits::make_error(E)
struct TraitsMakeErrorBox {
    int err{};
};

template <>
struct box_traits<TraitsMakeErrorBox> {
    static constexpr TraitsMakeErrorBox make_error(int e) noexcept { return TraitsMakeErrorBox{e}; }
};

// Branch 2: std::constructible_from<Box, E>
struct ConstructibleErrorBox {
    int err{};
};
// No box_traits specialization — relies on Box{E} constructor.

// Nothing works
struct NonConstructibleErrorBox {
    struct tag {};
    NonConstructibleErrorBox() = delete;
    explicit NonConstructibleErrorBox(struct tag) {}
};

} // namespace

TEMPLATE_TEST_CASE_SIG("concept",
                       "",
                       ((typename Box, typename E, bool Has), Box, E, Has),
                       (TraitsMakeErrorBox, int, true),
                       (ConstructibleErrorBox, int, true),
                       (NonConstructibleErrorBox, int, false)) {
    using Traits = box_traits<Box>;
    if constexpr (Has) {
        STATIC_REQUIRE(has_make_error_fn<Box, Traits, E>);
    } else {
        STATIC_REQUIRE_FALSE(has_make_error_fn<Box, Traits, E>);
    }
}

TEST_CASE("traits-branch") {
    constexpr auto fn = get_make_error_fn<TraitsMakeErrorBox, box_traits<TraitsMakeErrorBox>, int>();
    REQUIRE(fn(-1).err == -1);
}

TEST_CASE("constructible-branch") {
    constexpr auto fn = get_make_error_fn<ConstructibleErrorBox, box_traits<ConstructibleErrorBox>, int>();
    REQUIRE(fn(-1).err == -1);
}

TEST_CASE("traits-wins-over-constructible-when-both-present") {
    // TraitsMakeErrorBox is also constructible from int via aggregate init,
    // but Traits::make_error is checked first and wins.
    constexpr auto fn = get_make_error_fn<TraitsMakeErrorBox, box_traits<TraitsMakeErrorBox>, int>();
    REQUIRE(fn(-1).err == -1);
}

} // namespace beman::monadics::detail::tests
