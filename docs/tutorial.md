<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

# Tutorial

## Introduction

`beman.monadics` provides four monadic operations — `and_then`, `transform`, `or_else`,
`transform_error` — as free functions that work uniformly across any type that models a "box":
a container that either holds a value or signals absence/failure.

```cpp
#include <beman/monadics/monadics.hpp>
namespace bms = beman::monadics;
```

All code examples in this document assume this setup, plus the relevant standard library
header for the box type being demonstrated.

## Opting In a Type

Opting a type into the box abstraction is explicit: you must specialize
`beman::monadics::box_traits<T>`, even if the type already exposes a fully compatible interface.
This prevents types from being silently treated as boxes with incorrect behaviour.

An empty specialization is the minimum required opt-in. Once opted in, any members the type
already exposes are deduced automatically; you only need to supply what is missing.

### Case 1: Minimal opt-in (well-structured types)

If the type already exposes a compatible interface (`has_value()`, `value()`, `error()`,
nested `value_type` / `error_type`, and `rebind` / `rebind_error`), only the members the
deduction cascade cannot find need to be provided.

`std::expected` is the canonical example. It already exposes `has_value()`, `value()`,
`error()`, `value_type`, `error_type`, and its template parameters provide `rebind` and
`rebind_error`. However, `make_error` cannot be deduced: `std::expected` constructs error
states via `std::unexpect`-tagged construction, not from the error type directly. One member
suffices:

```cpp
#include <beman/monadics/monadics.hpp>
#include <expected>
#include <string>

template <typename T, typename E>
struct beman::monadics::box_traits<std::expected<T, E>> {
    [[nodiscard]] static constexpr auto make_error(auto&& e) {
        return std::expected<T, E>{std::unexpect, std::forward<decltype(e)>(e)};
    }
};
```

```cpp
using E = std::expected<int, std::string>;

auto result =
    E{42}
    | bms::and_then([](int v)            { return E{v * 2}; })
    | bms::transform([](int v)           { return v + 1; })
    | bms::or_else([](const std::string&){ return E{0}; })
    | bms::transform_error([](auto e)    { return "error: " + e; });
```

### Case 2: Partial customization

If the type is mostly well-structured but misses one capability, provide only that piece. The
rest is still deduced.

`std::optional` has `has_value()`, `value()`, and `value_type`, so the value side deduces, and
its `error_type` deduces to `unit` because nothing is stored on the failure side. What is left
is how to build an empty one, which no generic rule can guess. Provide that:

```cpp
#include <beman/monadics/monadics.hpp>
#include <optional>

template <typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    using error_type = unit;
};
```

`and_then`, `transform`, `or_else` and `inspect` now work. `transform_error` is unavailable
because there is no error to transform, and a handler on the failure side takes no argument
because there is nothing to pass it.

```cpp
#include <optional>
#include <string>

auto parse = [](const std::string& s) -> std::optional<int> {
    try { return std::stoi(s); } catch (...) { return {}; }
};

auto result =
    std::optional<std::string>{"42"}
    | bms::and_then(parse)
    | bms::transform([](int v) { return v * 2; })
    | bms::or_else([]()        { return std::optional{0}; }); // optional{84}
```

`std::shared_ptr` is another partial case: it has no `has_value()`, no `value_type`, and no
`rebind`, but each can be wired up. Its `error_type` deduces to `unit`, so only the empty
state has to be spelled out:

```cpp
#include <beman/monadics/monadics.hpp>
#include <memory>

template <typename T>
struct beman::monadics::box_traits<std::shared_ptr<T>> {
    [[nodiscard]] static bool has_value(const std::shared_ptr<T>& p) {
        return static_cast<bool>(p);
    }

    [[nodiscard]] static decltype(auto) value(auto&& p) {
        return *std::forward<decltype(p)>(p);
    }

    [[nodiscard]] static std::shared_ptr<T> make(T v) {
        return std::make_shared<T>(std::move(v));
    }
};
```

`and_then`, `transform`, `or_else` and `inspect` all work. `transform_error` is unavailable,
because there is no error to transform.

```cpp
#include <memory>

auto ptr = std::make_shared<int>(10);

auto result =
    ptr
    | bms::and_then([](int v)  { return std::make_shared<int>(v * 2); })
    | bms::transform([](int v) { return v + 3; })
    | bms::or_else([]()        { return std::make_shared<int>(0); }); // 23
```

### Case 3: Fully custom type

For types that expose no compatible interface at all, provide the full set of static members.

```cpp
#include <beman/monadics/monadics.hpp>

template <typename T>
struct Result {
    bool ok;
    T    data;
    int  err;
};

template <typename T>
struct beman::monadics::box_traits<Result<T>> {
    using value_type = T;
    using error_type = int;

    template <typename U> using rebind       = Result<U>;
    template <typename E> using rebind_error = Result<T>;

    [[nodiscard]] static bool has_value(const Result<T>& r) { return r.ok; }

    [[nodiscard]] static decltype(auto) value(auto&& r) {
        return std::forward<decltype(r)>(r).data;
    }

    [[nodiscard]] static int error(const Result<T>& r) { return r.err; }

    [[nodiscard]] static Result<T> make(T v)       { return {true,  std::move(v), 0}; }
    [[nodiscard]] static Result<T> make_error(int e) { return {false, {},          e}; }
};
```

All four operations are now available:

```cpp
constexpr Result<int> r{true, 10, 0};

constexpr auto result =
    r
    | bms::transform([](int v)  { return v * 2; })          // Result{true, 20, 0}
    | bms::and_then([](int v)   { return Result<int>{true, v + 1, 0}; }) // Result{true, 21, 0}
    | bms::transform_error([](int e) { return e + 100; });  // error unchanged (has value)
```

### Case 4: A type with only one of the two cases

Some types hold a value that is always there, or an error that already happened. Name `never` on
the side that does not exist and the library supplies everything that side would have needed,
since none of it can be reached:

```cpp
template <typename T>
struct Certain {
    using value_type = T;
    using error_type = bms::never;

    template <std::same_as<bms::never>>
    using rebind_error = Certain<T>;

    T v;
    constexpr const T& value() const { return v; }
};

template <typename T>
struct beman::monadics::box_traits<Certain<T>> {};
```

```cpp
Certain<int>{4} | bms::transform([](int x) { return x * 2; });   // Certain<int>{8}
Certain<int>{4} | bms::to<std::optional<int>>();                 // std::optional{4}
```

`or_else` and `transform_error` are unavailable for such a type — there is nothing to recover
from. `value_only`, `error_only` and `single_case` report which case a box has, and
`no_value_case` / `no_error_case` ask about one side on its own.

## Operations Reference

### `and_then` — flat-map over the value

Calls `fn(value)` when the box is non-empty; propagates absence/error unchanged. `fn` must
return the same box template with the same error type.

```cpp
#include <optional>

constexpr auto result = std::optional{42}
    | bms::and_then([](int v) { return std::optional{v * 2}; }); // optional{84}

constexpr auto empty = std::optional<int>{}
    | bms::and_then([](int v) { return std::optional{v * 2}; }); // nullopt
```

### `transform` — map over the value

Calls `fn(value)` and rewraps the result. `fn` returns a plain value, not a box.

```cpp
#include <optional>

constexpr auto result = std::optional{10}
    | bms::transform([](int v) { return v + 1.5; }); // optional<double>{11.5}
```

### `or_else` — recover from absence/error

Calls `fn` when the box is empty or in an error state; passes through the value unchanged.
`fn` must return the same box template with the same value type.

For boxes without an error channel, `fn` takes no arguments:

```cpp
#include <optional>

constexpr auto result = std::optional<int>{}
    | bms::or_else([]() { return std::optional{0}; }); // optional{0}
```

For boxes with an error channel, `fn` receives the error:

```cpp
#include <expected>
#include <string>

using E = std::expected<int, std::string>;
auto result = E{std::unexpected, "oops"}
    | bms::or_else([](const std::string&) { return E{-1}; }); // expected{-1}
```

### `transform_error` — map over the error

Calls `fn(error)` and replaces the error. Only available for box types with a real error
channel (`std::expected`-like types; not `std::optional` or pointers).

```cpp
#include <expected>

using E = std::expected<int, int>;
constexpr auto result = E{std::unexpected, 42}
    | bms::transform_error([](int e) { return e * 2; }); // expected{unexpected, 84}
```

### `inspect` and `inspect_error` — look without taking apart

Calls `fn` on the value or the failure and hands the box back unchanged. The box is passed as
const, so a handler that would consume the value is rejected rather than moving out of a box
you still hold.

```cpp
#include <optional>

auto logged = std::optional{42}
    | bms::inspect([](int v) { std::println("got {}", v); })
    | bms::transform([](int v) { return v * 2; });   // optional{84}
```

Where nothing is stored on the failure side, the handler takes nothing:

```cpp
auto traced = std::optional<int>{}
    | bms::inspect_error([] { std::println("empty"); });   // nullopt
```

### `to` — move a result between box types

Narrowing drops the failure. Widening needs one supplied, because the source has none to carry
across, and supplying one where the source already stores an error is rejected.

```cpp
#include <expected>
#include <optional>

auto narrowed = std::expected<int, int>{std::unexpect, 7}
    | bms::to<std::optional<int>>();                      // nullopt

auto widened = std::optional<int>{}
    | bms::to<std::expected<int, int>>(20);               // unexpected{20}
```

The destination may be named exactly, or left to deduce from the source's case types:

```cpp
auto deduced = std::optional{15} | bms::to<std::expected>(20);   // expected<int, int>{15}
```

### `either` — the primitive the rest are built from

Reads the tag and hands the inhabited case to the matching handler. One handler names the
result type, and the other either returns that same type or is written to be told it.

```cpp
constexpr int n = bms::either([](int v) { return v; },
                                  [](bms::unit) { return -1; },
                                  std::optional{42});   // 42
```

### `just` and `just_error` — one case, carried on its own

Boxes in their own right: a `just` holds a value and has no failure case, a `just_error` holds an
error and has no value case. Each is the identity monad, so the operations apply inside its own
family, and crossing to another family is `to`'s job:

```cpp
constexpr auto j = bms::just(42);                    // just_t<int>
constexpr auto doubled = j | bms::transform([](int v) { return v * 2; });   // just_t<int>{84}
constexpr auto o = bms::just(42) | bms::to<std::optional<int>>();           // optional<int>{42}
```

Because a `just` cannot fail, it gets no `or_else` and no `transform_error` — there is nothing to
recover from.

To put a value straight into a box of a family you name, `make_value<R>` and `make_error<R>` are
the carriers and `to` in one call, and they accept the payloads the box's own construction
cannot: a value side that carries nothing takes no argument, and a failure that carries nothing
accepts a typed error and drops it.

```cpp
constexpr auto x = bms::make_error<std::expected<int, char>>('e');   // unexpected{'e'}
constexpr auto n = bms::make_error<std::optional<int>>('e');         // nullopt
```

Their other use is inside an operation's handlers, and the library ships that use as
`carry_error` and `carry_value`, which are those two waiting to be told the result. Each settles
its carrier into the result it is told, so an operation hands one over and names nothing itself:

```cpp
// and_then(fn) and or_else(fn), in full
bms::either(fn, bms::carry_error, std::forward<Box>(box));
bms::either(bms::carry_value, fn, std::forward<Box>(box));
```

### `compose` — one callable after another

`compose(g, f)` calls `f`, then `g` on its result. Its one special rule: when `f` returns `void`,
`g` is called with no argument, so a callable that only has a side effect composes like any other.
`compose(just, fn)` turns a value-to-value callable into a continuation that carries a value,
which is the whole of `transform`:

```cpp
// transform(fn), in full, where R is the box rebound around what fn returned
std::forward<Box>(box) | bms::and_then([&](auto&&... v) -> R {
    return bms::compose(bms::just, fn)(v...) | bms::to<R>();
});
```

## Pipe Composition

All operations compose with `|`:

```cpp
#include <optional>

constexpr auto result =
    std::optional{10}
    | bms::and_then([](int v)    { return std::optional{v * 2.0}; })
    | bms::transform([](double v){ return static_cast<int>(v); })
    | bms::and_then([](int)      { return std::optional<char>{}; }) // fails here
    | bms::or_else([]()          { return std::optional{0}; });     // recovers
```

## Compile-time Evaluation

All operations are `constexpr`. Any chain where the box type's `box_traits` methods and
all provided callables are also `constexpr` evaluates entirely at compile time. This works
out of the box for `std::optional` and `std::expected`:

```cpp
#include <beman/monadics/monadics.hpp>
#include <optional>
namespace bms = beman::monadics;

constexpr auto result =
    std::optional{6}
    | bms::and_then([](int v) { return std::optional{v * 7}; })
    | bms::transform([](int v) { return v - 0; });

static_assert(result == std::optional{42});
```

Types that require runtime allocation (e.g. `std::shared_ptr`) remain runtime-only — the
library imposes no restriction either way.

## Writing Your Own Operations

Because `box_traits`, `get_box_traits`, `either`, `just`, `just_error`, `make_value`,
`make_error`, `carry_value`, `carry_error` and `compose` are public, you can write new operations
that work across all adapted types without touching the library or the box types. `either` is
usually the place to start, since it removes the need to test the tag and project the value
yourself, and `carry_error` / `carry_value` are the pass-through handlers, so your operation
writes only the one it cares about. See
[Design Rationale — Extensibility](design_rationale.md#extensibility) for details and examples.
