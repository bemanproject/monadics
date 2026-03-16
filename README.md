# beman.monadics: A Beman Library Monadics

<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable-next-line line-length -->
`beman.monadics` is a minimal C++ library conforming to [The Beman Standard](https://github.com/bemanproject/beman/blob/main/docs/beman_standard.md).

**Implements**: Monadics operations as free functions

**Status**: [Under development and not yet ready for production use.](https://github.com/bemanproject/beman/blob/main/docs/beman_library_maturity_model.md#under-development-and-not-yet-ready-for-production-use)

This library generalizes the monadic vocabulary to any type that models a “box”.

---

## Features

- Free-function monadic operations
- Pipe (`|`) syntax for composition
- Extensible via `box_traits`
- Value-category preserving
- Concept-constrained APIs
- Semantics aligned with standard monadic operations

---

## Provided Operations (Initial Scope)

The initial operation set mirrors the C++23/C++26 monadic vocabulary:

- `and_then`
- `transform`
- `or_else`
- `transform_error`

Semantics follow `std::expected` / `std::optional`.

The goal is to validate the unifying abstraction first.

---

## Quick Example

```cpp
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/monadics.hpp>
#include <optional>
#include <cstdlib>

int main() {
    namespace bms = beman::monadics;

    const auto result =
        std::optional{10}
        | bms::and_then([](auto&& v) {
              return std::optional{v * 2.0};
          })
        | bms::transform([](double v) {
              return static_cast<int>(v);
          })
        | bms::and_then([](int) {
              return std::optional<char>{};  // failure
          })
        | bms::or_else([] {
              return std::optional{EXIT_SUCCESS};
          });

    return result.value_or(EXIT_FAILURE);
}
```

## `std::shared_ptr`

`std::shared_ptr` can act as a box: it may contain a value or be empty. Unlike `std::optional`, it has no error type, so `transform_error` is disabled. We can still use `and_then`, `transform`, and `or_else`.

```cpp
#include <beman/monadics/monadics.hpp>
#include <memory>

template <typename T>
struct beman::monadics::box_traits<std::shared_ptr<T>> {
    [[nodiscard]] static constexpr bool has_value(const auto& box) noexcept {
        return static_cast<bool>(box);
    }

    [[nodiscard]] static constexpr decltype(auto) value(auto&& box) noexcept {
        return *std::forward<decltype(box)>(box);
    }

    [[nodiscard]] static constexpr auto error() noexcept { return nullptr; }

    [[nodiscard]] static constexpr decltype(auto) make(auto&& value) noexcept {
        return std::make_shared<T>(std::forward<decltype(value)>(value));
    }
};

int main() {
    namespace bms = beman::monadics;

    auto ptr = std::make_shared<int>(10);

    auto result =
        ptr
        | bms::and_then([](int v) { return std::make_shared<int>(v * 2); })
        | bms::transform([](int v) { return v + 3; })
        | bms::or_else([]() { return std::make_shared<int>(0); });

    if (result) {
        std::cout << "Result: " << *result << "\n"; // prints 23
    }

    return 0;
}
```

## Build Requirements

### Supported Platforms

| Compiler   | Version | C++ Standards | Standard Library  |
|------------|---------|---------------|-------------------|
| GCC        | 15-13   | C++26-C++20   | libstdc++         |
| GCC        | 12-11   | C++23, C++20  | libstdc++         |
| Clang      | 22-19   | C++26-C++20   | libstdc++, libc++ |
| Clang      | 18-17   | C++26-C++20   | libc++            |
| Clang      | 18-17   | C++20         | libstdc++         |
| AppleClang | latest  | C++26-C++20   | libc++            |
| MSVC       | latest  | C++23         | MSVC STL          |
