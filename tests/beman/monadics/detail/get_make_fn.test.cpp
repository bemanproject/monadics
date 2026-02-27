// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_make_fn.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::detail::tests {

namespace {

template <typename T>
struct box_traits {};

// Branch 1: Traits::make(T)
struct TraitsMakeBox {
    int val{};
};

template <>
struct box_traits<TraitsMakeBox> {
    static constexpr TraitsMakeBox make(int v) noexcept { return TraitsMakeBox{v}; }
};

// Branch 3: std::is_void_v<T> — default-construct Box
struct VoidBox {};

// Branch 4: std::constructible_from<Box, T>
struct ConstructibleBox {
    int val{};
};
// No box_traits specialization — relies on Box{T} constructor.

// Nothing works
struct NonConstructibleBox {
    struct tag {};

    NonConstructibleBox() = delete;
    explicit NonConstructibleBox(struct tag) {}
};

} // namespace

TEMPLATE_TEST_CASE_SIG("concept",
                       "",
                       ((typename Box, typename T, bool Has), Box, T, Has),
                       (TraitsMakeBox, int, true),
                       (VoidBox, void, true),
                       (ConstructibleBox, int, true),
                       (NonConstructibleBox, int, false)) {
    using Traits = box_traits<Box>;
    if constexpr (Has) {
        STATIC_REQUIRE(has_make_fn<Box, Traits, T>);
    } else {
        STATIC_REQUIRE_FALSE(has_make_fn<Box, Traits, T>);
    }
}

TEST_CASE("traits-branch") {
    constexpr auto fn = get_make_fn<TraitsMakeBox, box_traits<TraitsMakeBox>, int>();
    REQUIRE(fn(7).val == 7);
}

TEST_CASE("void-T-branch") {
    constexpr auto           fn     = get_make_fn<VoidBox, box_traits<VoidBox>, void>();
    [[maybe_unused]] VoidBox result = fn();
}

TEST_CASE("constructible-branch") {
    constexpr auto fn = get_make_fn<ConstructibleBox, box_traits<ConstructibleBox>, int>();
    REQUIRE(fn(7).val == 7);
}

TEST_CASE("traits-wins-over-constructible-when-both-present") {
    // TraitsMakeBox is also constructible from int via aggregate init,
    // but Traits::make is checked first and wins.
    constexpr auto fn = get_make_fn<TraitsMakeBox, box_traits<TraitsMakeBox>, int>();
    REQUIRE(fn(7).val == 7);
}

} // namespace beman::monadics::detail::tests
