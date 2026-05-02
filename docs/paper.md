<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

---
document: PxxxxR0
date: 2026-04-08
reply-to: TBD
audience: LEWG
---

# Generic Monadic Operations for Box Types

## Abstract

C++23 added monadic operations (`and_then`, `transform`, `or_else`, `transform_error`) as
member functions of `std::optional` and `std::expected`. These operations cannot be used
generically: there is no way to write an algorithm that works over both types, nor over any
third-party box type. This paper proposes a traits-based customization mechanism, `box_traits`,
that enables generic algorithms over any box type — including existing standard library types,
third-party types, and user-defined types — without modifying those types. A **box** is any
type that either holds a value or signals absence or failure. The four C++23 monadic operations
are then re-expressed as free functions over this abstraction.

---

## 1. Introduction

C++23 added monadic operations (`and_then`, `transform`, `or_else`, `transform_error`) to
`std::optional` and `std::expected` as member functions. These are a welcome addition — they
enable clean, composable pipelines for the two canonical vocabulary types:

```cpp
std::optional<int> opt = parse(input);
auto result = opt
    .and_then(validate)
    .transform(normalize)
    .or_else(recover);
```

The limitation is hard: these operations only work for `std::optional` and `std::expected`.
Any type that plays the same structural role — holds a value or signals absence/failure —
cannot participate, regardless of how compatible it actually is.

Consider a codebase that uses a legacy result type it cannot modify:

```cpp
template <typename T>
struct Result { bool ok; T data; int err_code; };
```

`Result<T>` carries exactly the same semantics as `std::expected<T, int>`. It holds a value
or an error, it can be flat-mapped, it can be transformed. But it has no `and_then` member,
and there is no way to give it one without modifying the type. The same is true for
`boost::optional`, `tl::expected`, `folly::Expected`, `std::shared_ptr<T>`, and any
third-party or proprietary type.

The monadic vocabulary introduced in C++23 is not a generic abstraction — it is two
type-specific APIs that happen to look similar. There is no shared concept, no way to write
an algorithm that works for both, and no extension point for types outside the standard
library.

---

## 2. Motivation

### 2.1 The generic algorithm problem

Consider a function that validates and transforms a value, propagating absence or failure:

```cpp
template <typename T>
??? process(T input) {
    return input
        .and_then(validate)
        .transform(normalize)
        .or_else(recover);
}
```

This does not compile for any `T`. `optional` and `expected` have incompatible member
interfaces (e.g., `or_else` takes a nullary callback for `optional` but a unary callback for
`expected`). There is no concept that both satisfy. A developer who wants this abstraction
today must either:

- Duplicate the logic for each type, or
- Write a macro, or
- Give up on genericity entirely.

### 2.2 Retroactive adaptation

Real codebases do not only use `std::optional` and `std::expected`. They use:

- `boost::optional`, which predates `std::optional` and exists in millions of lines of code
- `boost::outcome::result`, `tl::expected`, `folly::Expected`, and similar alternatives
- Qt's `std::optional`-like patterns via `std::optional` wrappers or custom types
- Legacy in-house result types that cannot be changed
- `std::shared_ptr<T>` and raw pointer `T*`, which represent nullable values structurally

Member functions cannot be added to any of these types retroactively.

### 2.3 Today vs. with this proposal

**Today** — writing a generic pipeline function is impossible:

```cpp
// Does not compile — no shared concept between optional and expected
template <typename Box>
Box pipeline(Box input) {
    return input
        .and_then(validate)
        .transform(normalize);
}
```

**With this proposal** — one function works for any adapted type:

```cpp
#include <box>
namespace bx = std::box;

template <bx::box Box>
Box pipeline(Box input) {
    return input
        | bx::and_then(validate)
        | bx::transform(normalize);
}

// Works for all of:
pipeline(std::optional{42});
pipeline(std::expected<int, std::string>{42});
pipeline(boost_optional_value);
pipeline(legacy_result_value);
```

A third-party or legacy type is adapted with a partial or full specialization of
`std::box::box_traits` — without touching the type itself.

---

## 3. Design

### 3.1 The `box_traits` customization point

The central mechanism is a traits struct template that users specialize to teach the library
about a type. The full interface is:

```cpp
namespace std::box {
    template <typename T>
    struct box_traits {};  // default: empty, triggers deduction cascade
}
```

A full specialization provides any or all of:

| Member | Purpose |
|---|---|
| `using value_type = T` | the contained value type |
| `using error_type = E` | the error/sentinel type |
| `template <typename U> using rebind = ...` | same box template, different value type |
| `template <typename F> using rebind_error = ...` | same box template, different error type |
| `static bool has_value(const B&)` | whether the box holds a value |
| `static decltype(auto) value(auto&&)` | access the contained value |
| `static E error()` | return the absence sentinel (nullary — no error channel) |
| `static decltype(auto) error(auto&&)` | access the stored error (unary — real error channel) |
| `static B make(value_type)` | construct a box holding a value |
| `static B make_error(error_type)` | construct a box in the error state |

Specializing all members is only needed for fully opaque types. Most types already expose
enough — see §3.2.

### 3.2 Deduction cascade

The library does not require full specializations. For each capability, it runs a cascade:

1. Check the explicit `box_traits<T>` specialization.
2. Check the type's own members (`has_value()`, `value()`, `error()`, `value_type`, etc.).
3. Deduce from template parameters (for rebind).

The result is that the most well-structured types require no specialization at all:

**`std::expected<T, E>`** — already has `has_value()`, `value()`, `error()`, `value_type`,
`error_type`, and two template parameters. No specialization needed. All four operations
are immediately available.

**`std::optional<T>`** — has `has_value()`, `value()`, and `value_type`, but no `error()`
and no `error_type`. The absence sentinel `std::nullopt` is a compile-time constant, not
something stored inside the object. A one-member specialization suffices:

```cpp
template <typename T>
struct std::box::box_traits<std::optional<T>> {
    static constexpr auto error() noexcept { return std::nullopt; }
};
```

From this single function the library derives `error_type` (`std::nullopt_t`) and detects
that `optional` has no real error channel (see §3.4). Three operations (`and_then`,
`transform`, `or_else`) work; `transform_error` is automatically disabled.

**A fully custom type** — for types that expose nothing compatible, provide all members:

```cpp
// Legacy in-house result type — cannot be modified
template <typename T>
struct Result { bool ok; T data; int err_code; };

template <typename T>
struct std::box::box_traits<Result<T>> {
    using value_type = T;
    using error_type = int;
    template <typename U> using rebind       = Result<U>;
    template <typename F> using rebind_error = Result<T>;  // error is always int

    static bool           has_value(const Result<T>& r)    { return r.ok; }
    static decltype(auto) value(auto&& r)                   { return std::forward<decltype(r)>(r).data; }
    static int            error(const Result<T>& r)         { return r.err_code; }
    static Result<T>      make(T v)                         { return {true, std::move(v), 0}; }
    static Result<T>      make_error(int e)                 { return {false, {}, e}; }
};
```

All four operations are now available for `Result<T>`.

### 3.3 The four operations

The proposal re-expresses the four C++23 monadic operations as free functions over the
`box` concept:

#### `and_then(fn)` — flat-map over the value

When the box holds a value, calls `fn(value)` and returns its result. When the box
is empty or in an error state, propagates the empty/error state unchanged. `fn` must return
the same box template with the same error type.

```cpp
std::optional{42}
    | and_then([](int v) { return std::optional{v * 2}; });  // optional{84}

std::optional<int>{}
    | and_then([](int v) { return std::optional{v * 2}; });  // nullopt
```

#### `transform(fn)` — map over the value

When the box holds a value, calls `fn(value)` and wraps the result. `fn` returns a
plain value, not a box.

```cpp
std::optional{10}
    | transform([](int v) { return v + 1.5; });  // optional<double>{11.5}
```

#### `or_else(fn)` — recover from absence or error

When the box is empty or in an error state, calls `fn` and returns its result. The
value is passed through unchanged. `fn` must return the same box template with the same
value type. For boxes without a real error channel, `fn` takes no arguments; for boxes
with an error channel, `fn` receives the stored error.

```cpp
// Without error channel — fn is nullary
std::optional<int>{}
    | or_else([] { return std::optional{0}; });  // optional{0}

// With error channel — fn receives the error
std::expected<int, std::string>{std::unexpected, "fail"}
    | or_else([](const std::string&) { return std::expected<int, std::string>{-1}; });
```

#### `transform_error(fn)` — map over the error

When the box is in an error state, calls `fn(error)` and replaces the error. The value
is passed through unchanged. Only available for boxes with a real error channel (see §3.4).

```cpp
std::expected<int, int>{std::unexpected, 42}
    | transform_error([](int e) { return e * 2; });  // expected{unexpected, 84}
```

### 3.4 Error channel detection

Not all boxes carry a typed error. `std::optional` signals absence structurally —
there is no stored error value. The library detects the presence of a real error channel
without any explicit tag or trait member:

- If the assembled `error` function is **unary** (callable as `error(box)`), the box
  has a real error channel.
- If `error` is **nullary** (callable as `error()`, returning a sentinel constant without
  reading the box), the box has no error channel.

This distinction is derived automatically from the `box_traits` specialization.
For `std::optional`, the provided `error()` is nullary; the library detects this and
disables `transform_error`. No extra configuration is required.

### 3.5 Value-category preservation

All four operations preserve the value category of the box. An rvalue box forwards
the contained value as an rvalue to the callback, enabling move semantics through a chain:

```cpp
std::optional<std::string>{"hello"}
    | and_then([](std::string&& s) {  // receives rvalue
          return std::optional{std::move(s)};
      });
```

### 3.6 Pipe syntax

The `|` operator is provided as a convenience for chaining. It is **not** a globally
overloaded operator; it is defined as a hidden friend within each operation object and is
only found via argument-dependent lookup when the right-hand side is one of the four
operation adaptors. There is no conflict with ranges `operator|` because there is no global
overload — only type-specific hidden friends.

The free-function call syntax is always available as an alternative:

```cpp
// Pipe syntax (chaining convenience)
auto r1 = opt | and_then(f) | transform(g);

// Equivalent free-function call
auto r2 = transform(and_then(opt, f), g);
```

### 3.7 Extensibility: writing new generic operations

`std::box::box_traits` and the `std::box::box` concept are public. Any third party can write
new generic operations that work over all adapted types:

```cpp
#include <box>
namespace bx = std::box;

// value_or: works for any adapted type
template <bx::box B>
constexpr auto value_or(B&& b, typename bx::get_box_traits<B>::value_type fallback) {
    using Traits = bx::get_box_traits<B>;
    return Traits::has_value(b) ? Traits::value(std::forward<B>(b)) : fallback;
}

// flatten: works for any adapted type whose value type is itself a box
template <bx::box B>
    requires bx::box<typename bx::get_box_traits<B>::value_type>
constexpr auto flatten(B&& b) {
    using Traits = bx::get_box_traits<B>;
    if (!Traits::has_value(b))
        return bx::propagate_error<typename Traits::value_type>(std::forward<B>(b));
    return Traits::value(std::forward<B>(b));
}
```

These algorithms work immediately for any type adapted via `box_traits`, including
types defined after this paper is implemented.

---

## 4. Impact on the Standard

This proposal adds:

- A new header `<box>` (or additions to `<utility>` or `<functional>` — TBD)
- `std::box::box_traits<T>` — the customization point
- `std::box::box` — the concept (following the `std::ranges::range` precedent)
- `std::box::get_box_traits<T>` — alias that assembles traits with deduction applied
- `std::box::and_then`, `std::box::transform`, `std::box::or_else`, `std::box::transform_error` — the four operations

Built-in specializations of `std::box::box_traits` for `std::optional` and `std::expected`
are provided by the standard library. No changes are required to `std::optional` or
`std::expected` themselves.

This proposal does not deprecate the member-function APIs added in C++23.

---

## 5. Alternatives Considered

### 5.1 Customization point objects (CPOs)

CPOs are the preferred mechanism in C++20 ranges for dispatching to member functions or
ADL-found free functions. A CPO for `and_then` would dispatch to `obj.and_then(fn)` or
`and_then(obj, fn)` (ADL).

The fundamental limitation of CPOs for this use case is that they can only *dispatch* — they
cannot *supply* missing functionality. `std::shared_ptr<T>` has no `and_then` member and no
ADL `and_then`. A CPO cannot give it one. A `std::box::box_traits` specialization can.

The traits approach additionally allows supplying `error()`, `make()`, and `make_error()` for
types that have no such concept — which a CPO cannot do.

### 5.2 Member functions only

The C++23 approach of adding member functions to `std::optional` and `std::expected` provides
excellent ergonomics for those two types. It cannot be extended to types that already exist
and cannot be modified. It also does not enable the generic algorithm pattern (§2.1).

### 5.3 CRTP base class

A CRTP base (`box_base<Derived>`) could provide default implementations.
CRTP requires inheritance, which is:

- Impossible for `final` classes
- Impossible for types outside the developer's control (`std::shared_ptr`, third-party types)
- Retroactively inapplicable to existing types

The traits pattern follows the established precedent of `std::iterator_traits`,
`std::char_traits`, and `std::numeric_limits`: it separates the type from its traits without
requiring any modification to the type.

### 5.4 `tag_invoke`

`tag_invoke` was proposed in P1895 as a universal mechanism for customization points.
`std::box::box_traits` does not conflict with or replace `tag_invoke`; the traits struct
serves a different role — it supplies *structural members* (types, factories, accessors) in
addition to dispatching operations. If `tag_invoke` is eventually standardized, the two
mechanisms are complementary.

---

## 6. Proposed Wording

> **Note:** Wording is preliminary and will be refined based on design feedback. Normative
> wording targeting the working draft will be provided in a subsequent revision.

### 6.1 Header `<box>` synopsis

```cpp
namespace std::box {

    // Customization point
    template <typename T>
    struct box_traits {};

    // Assembled traits (applies deduction cascade)
    template <typename T>
    using get_box_traits = /* exposition-only assembled traits */;

    // Concept
    template <typename T>
    concept box = /* see below */;

    // Operations
    inline constexpr /* unspecified */ and_then        = /* unspecified */;
    inline constexpr /* unspecified */ transform       = /* unspecified */;
    inline constexpr /* unspecified */ or_else         = /* unspecified */;
    inline constexpr /* unspecified */ transform_error = /* unspecified */;

    // Helper algorithms
    template <box B, box V>
    constexpr auto propagate_error(B&&) noexcept(/* see below */);

    template <box B, box V>
    constexpr auto propagate_value(B&&) noexcept(/* see below */);

} // namespace std::box
```

### 6.2 `std::box::box` concept

A type `B` models `std::box::box` if `get_box_traits<B>` provides at minimum:
- `value_type` — the type of the contained value
- `has_value(b)` — returns `bool` indicating whether `b` holds a value
- `value(b)` — accesses the contained value
- `error()` or `error(b)` — provides the error/sentinel
- `make(v)` — constructs a `B` holding value `v`

The concept shares its name with the enclosing namespace, following the precedent of
`std::ranges::range`.

> Full wording TBD.

### 6.3 `std::box::and_then`

**Effects:** Given a box `b` and callable `fn`:
- If `get_box_traits<B>::has_value(b)` is `true`, returns
  `fn(get_box_traits<B>::value(std::forward<B>(b)))`.
- Otherwise, returns a box of type `result_type` in the error/absent state carrying the
  same error as `b`.

**Constraints:** `fn` must return a type that models `std::box::box` with the same box
template and the same `error_type` as `B`.

> Full wording for `transform`, `or_else`, and `transform_error` follows the same pattern;
> TBD in subsequent revision.

---

## 7. Future Work

The following are not proposed here but are natural follow-on directions:

- **Built-in specializations** for additional standard types: `std::shared_ptr`,
  `std::unique_ptr`, `std::variant`.
- **`flatten`** — collapsing nested boxes; expressible as a user algorithm today (see §3.7).
- **`sequence` / `zip`** — combining multiple boxes; out of scope for this paper.
- **Reflection integration (C++26)** — `std::box::box_traits` defaults could eventually be
  derived via reflection, reducing or eliminating the need for explicit specializations for
  well-structured types.

---

## 8. Implementation Experience

A complete, conforming implementation is available as
[beman.monadics](https://github.com/bemanproject/beman.monadics), a Beman Project library.
The implementation:

- Passes the full test suite under GCC 11–15, Clang 17–21, AppleClang, and MSVC
- Is tested in C++20, C++23, and C++26 modes
- Includes Debug, Release, MaxSan (Address + Leak + UB sanitizers), and TSan configurations

---

## 9. Acknowledgements

> TBD.

---

## References

> TBD — to be populated with relevant prior papers (P0798, P2505, P1859, etc.) and
> implementation references.
