# beman.monadics: A Beman Library Monadics

<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable-next-line line-length -->
![Library Status](https://raw.githubusercontent.com/bemanproject/beman/refs/heads/main/images/badges/beman_badge-beman_library_under_development.svg) ![Continuous Integration Tests](https://github.com/bemanproject/monadics/actions/workflows/ci_tests.yml/badge.svg) ![Lint Check (pre-commit)](https://github.com/bemanproject/monadics/actions/workflows/pre-commit-check.yml/badge.svg) [![Coverage](https://coveralls.io/repos/github/bemanproject/monadics/badge.svg?branch=main)](https://coveralls.io/github/bemanproject/monadics?branch=main) ![Standard Target](https://github.com/bemanproject/beman/blob/main/images/badges/cpp29.svg)

`beman.monadics` is a Beman library that generalizes the monadic vocabulary to any type that models a "box".

**Implements**: Monadics operations as free functions

**Status**: [Under development and not yet ready for production use.](https://github.com/bemanproject/beman/blob/main/docs/beman_library_maturity_model.md#under-development-and-not-yet-ready-for-production-use)

This library generalizes the monadic vocabulary to any type that models a "box".

---

## License

`beman.monadics` is distributed under the [Apache License v2.0 with LLVM Exceptions](LICENSE).

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

#include <optional>
#include <cstdlib>

#include <beman/monadics/monadics.hpp>

template <typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    [[nodiscard]] static constexpr auto error() noexcept { return std::nullopt; }
};

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

## Dependencies

### Build Environment

This project requires at least the following to build:

* A C++ compiler that conforms to the C++20 standard or greater
* CMake 3.30 or later
* (Test Only) Catch2

You can disable building tests by setting CMake option `BEMAN_MONADICS_BUILD_TESTS` to
`OFF` when configuring the project.

### Supported Platforms

| Compiler   | Version | C++ Standards | Standard Library  |
|------------|---------|---------------|-------------------|
| GCC        | 15-13   | C++26-C++20   | libstdc++         |
| GCC        | 12-11   | C++23, C++20  | libstdc++         |
| Clang      | 22-19   | C++26-C++20   | libstdc++, libc++ |
| Clang      | 18      | C++26-C++20   | libc++            |
| Clang      | 18      | C++23-C++20   | libstdc++         |
| Clang      | 17      | C++26-C++20   | libc++            |
| Clang      | 17      | C++20         | libstdc++         |
| AppleClang | latest  | C++26-C++20   | libc++            |
| MSVC       | latest  | C++23         | MSVC STL          |

## Development

See the [Contributing Guidelines](CONTRIBUTING.md).

## Integrate beman.monadics into your project

### Build

You can build monadics using a CMake workflow preset:

```bash
cmake --workflow --preset gcc-release
```

To list available workflow presets, you can invoke:

```bash
cmake --list-presets=workflow
```

For details on building beman.monadics without using a CMake preset, refer to the
[Contributing Guidelines](CONTRIBUTING.md).

### Installation

To install beman.monadics globally after building with the `gcc-release` preset, you can
run:

```bash
sudo cmake --install build/gcc-release
```

Alternatively, to install to a prefix, for example `/opt/beman`, you can run:

```bash
sudo cmake --install build/gcc-release --prefix /opt/beman
```

This will generate the following directory structure:

```txt
/opt/beman
├── include
│   └── beman
│       └── monadics
│           ├── monadics.hpp
│           └── ...
└── lib
    └── cmake
        └── beman.monadics
            ├── beman.monadics-config-version.cmake
            ├── beman.monadics-config.cmake
            └── beman.monadics-targets.cmake
```

### CMake Configuration

If you installed beman.monadics to a prefix, you can specify that prefix to your CMake
project using `CMAKE_PREFIX_PATH`; for example, `-DCMAKE_PREFIX_PATH=/opt/beman`.

You need to bring in the `beman.monadics` package to define the `beman::monadics` CMake
target:

```cmake
find_package(beman.monadics REQUIRED)
```

You will then need to add `beman::monadics` to the link libraries of any libraries or
executables that include `beman.monadics` headers.

```cmake
target_link_libraries(yourlib PUBLIC beman::monadics)
```

### Using beman.monadics

To use `beman.monadics` in your C++ project,
include an appropriate `beman.monadics` header from your source code.

```c++
#include <beman/monadics/monadics.hpp>
```

> [!NOTE]
>
> `beman.monadics` headers are to be included with the `beman/monadics/` prefix.
> Altering include search paths to spell the include target another way (e.g.
> `#include <monadics.hpp>`) is unsupported.
