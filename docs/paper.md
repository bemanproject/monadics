audience:
  - LEWG
author:
  - name: Mikhail Svetkin
    email: TBD
toc: true
---

# Abstract

Monadic operations (`and_then`, `transform`, `or_else`, `transform_error`) were added to
`std::optional` [@P0798R8] and `std::expected` [@P2505R5] in C++23. These operations are
tied to specific types: they cannot be extended, they do not compose generically, and
third-party types cannot participate. This paper proposes `box_traits`, a traits-based
customization point, and `box`, a concept that any adapted type satisfies. The four C++23
operations are re-expressed as free functions that work over any box type, and users can
write their own generic operations on top - without modifying existing types and without a
new paper for each one.

# Introduction

Monadic operations (`and_then`, `transform`, `or_else`, `transform_error`) were added to
`std::optional` [@P0798R8] and `std::expected` [@P2505R5] in C++23. They enable clean,
composable pipelines:

```cpp
using time_expected = std::expected<boost::posix_time::ptime, std::string>;

time_expected from_iso_str(std::string time);

auto d = from_iso_str(ts)
    .and_then(validate)
    .transform(next_day);
```

A natural next step is to observe the value inside the pipeline - log it, record metrics,
debug an intermediate state:

```cpp
void print_value(boost::posix_time::ptime t) { std::cout << t << "\n"; }

auto d = from_iso_str(ts)
    .and_then(print_value)   // does not compile - print_value must return expected<T, E>
    .transform(next_day);
```

`and_then` requires its callable to return an `expected<T, E>`, but `print_value` returns
`void`. The value cannot be observed without breaking the chain.

The same problem appears on the error path:

```cpp
void print_error(std::string err) { std::cerr << err << "\n"; }

auto d = from_iso_str(ts)
    .and_then(validate)
    .or_else(print_error)   // does not compile - print_error must return expected<T, E>
    .transform(next_day);
```

`or_else` requires its callable to return `expected<T, E>`, but `print_error` returns `void`.
There is no operation in the standard that observes without consuming. `inspect_error` does
not exist. Neither does `inspect`. To add both, they would need to be proposed separately for
`std::optional` and `std::expected`, and repeated again for every new type in the future.

There is no way to extend the monadic operation pipeline, and users may want to provide their
own custom operations useful for their specific domain. This is not a new problem. The ranges
library faced the same challenge - how to define operations that work generically across any
sequence type - and solved it by defining `std::ranges::range` as a concept and providing an
extension point. Anyone can write a custom view that composes with the rest of the library.
User-defined ranges work seamlessly alongside standard ones. This paper brings a similar
approach to monadic operations.

Monadic operations are not unique to `std::optional` and `std::expected`. The same pattern
appears in `boost::optional`, `absl::StatusOr`, `folly::Expected`, and countless in-house
result types. All of them represent the same concept - a value or an absence - but none of
them can participate in the pipeline. They have no `and_then`, `transform`, `or_else`, or
`transform_error` as members, and there is no way to add them. The only options are to wrap
the third-party type, duplicate the logic, or abandon the pipeline style entirely.

This paper proposes `box_traits`, a traits-based customization point, and `box`, a concept
that any adapted type satisfies. The four C++23 operations are re-expressed as free functions,
and users can write their own generic operations on top. `inspect_error` does not need a new
paper - a user can write it once and it works for any box type:

```cpp
auto d = from_iso_str(ts)
    | inspect_error(print_error)   // user-provided, works for any box type
    | std::box::transform(next_day);
```

Third-party types participate with a single `box_traits` specialization, and a generic
function works over any adapted type without a separate overload per type.

# Design

## The `box` abstraction

A type models `box` if it represents a value that is either present or absent, optionally
with an associated error. When a value is present, `has_value` returns `true` and `value`
provides access to it. When absent, the box either carries a typed error or signals absence
with a sentinel. Operations act on the value path or the absent path - absence or error
propagates unchanged unless explicitly transformed.

This covers a wide range of types which share the same structural protocol: `std::optional`
(value or absence), `std::expected` (value or typed error), and any third-party or
user-defined type that follows the same pattern.

## The `box_traits` customization point

The central mechanism is a traits struct template that users specialize to teach the library
about a type:

```cpp
namespace std::box {
    template <typename T>
    struct box_traits;  // must be specialized for each box type
}
```

No default definition is provided - a specialization is required. The deduction cascade
(see [#deduction-cascade]) minimizes what needs to be explicitly provided. A full
specialization may provide any or all of:

| Member | Purpose |
|---|---|
| `using value_type = T` | the contained value type |
| `using error_type = E` | the error/sentinel type |
| `template <typename U> using rebind = ...` | same box template, different value type |
| `template <typename F> using rebind_error = ...` | same box template, different error type |
| `static bool has_value(const B&)` | whether the box holds a value |
| `static decltype(auto) value(auto&&)` | access the contained value |
| `static E error()` | *(no error channel)* return the absence sentinel |
| `static decltype(auto) error(auto&&)` | *(error channel)* access the stored error |
| `static B make(value_type)` | construct a box holding a value |
| `static B make_error(error_type)` | construct a box in the error state |

Specializing all members is only needed for fully opaque types. Most types already expose
enough - see [#deduction-cascade].

## Deduction cascade and examples {#deduction-cascade}

The library does not require full specializations. For each capability, it tries the following
sources in order, stopping at the first that succeeds:

| Capability | Deduction order |
|---|---|
| `value_type` | `box_traits<T>::value_type`, then `Box::value_type`, then template parameter |
| `error_type` | `box_traits<T>::error_type`, then `Box::error_type`, then return type of `error()` |
| `rebind` | `box_traits<T>::rebind`, then `Box::rebind`, then rebind via template parameters |
| `rebind_error` | `box_traits<T>::rebind_error`, then `Box::rebind_error`, then rebind_error via template parameters |
| `has_value` | `box_traits<T>::has_value`, then `Box::has_value()` |
| `value` | `box_traits<T>::value`, then `Box::value()` |
| `error` | `box_traits<T>::error`, then `Box::error()` |
| `make` | `box_traits<T>::make`, then `Box::make`, then `Box::Box(value_type)` |
| `make_error` | `box_traits<T>::make_error`, then `Box::make_error`, then `Box::Box(error_type)` |

The result is that the most well-structured types require minimal specialization.

`std::expected<T, E>` already has `has_value()`, `value()`, `error()`, `value_type`,
`error_type`, and two template parameters. One member suffices:

```cpp
template <typename T, typename E>
struct std::box::box_traits<std::expected<T, E>> {
    static std::expected<T, E> make_error(auto&& e) {
        return {std::unexpect, std::forward<decltype(e)>(e)};
    }
    // everything else deduced
};
```

`std::optional<T>` needs only `error()` - `std::nullopt` is a compile-time constant, not
something stored inside the object:

```cpp
template <typename T>
struct std::box::box_traits<std::optional<T>> {
    static constexpr auto error() { return std::nullopt; }
};
```

`boost::optional` is structurally close enough to `std::optional` that the same
one-line specialization suffices:

```cpp
template <typename T>
struct std::box::box_traits<boost::optional<T>> {
    static constexpr auto error() { return boost::none; }
};
```

With that, the pipeline from the Introduction now works:

```cpp
auto result = parse(input)
    | std::box::and_then(validate)
    | std::box::transform(normalize);  // works for boost::optional
```

`absl::StatusOr` uses different names (`.ok()`, `.status()`) - three lines bridge the naming gap:

```cpp
template <typename T>
struct std::box::box_traits<absl::StatusOr<T>> {
    using error_type = absl::Status;
    static bool         has_value(const absl::StatusOr<T>& s) { return s.ok(); }
    static absl::Status error(const absl::StatusOr<T>& s)     { return s.status(); }
    // value(), make(), rebind - deduced
};
```

And the same pipeline works for `absl::StatusOr`:

```cpp
auto result = parse(input)
    | std::box::and_then(validate)
    | std::box::transform(normalize);  // works for absl::StatusOr
```

For types that expose nothing compatible, all members are provided explicitly:

```cpp
// Legacy in-house result type - cannot be modified
template <typename T>
struct Result { bool ok; T data; int err_code; };

template <typename T>
struct std::box::box_traits<Result<T>> {
    using value_type = T;
    using error_type = int;
    template <typename U> using rebind       = Result<U>;
    template <typename F> using rebind_error = Result<T>;

    static bool           has_value(const Result<T>& r) { return r.ok; }
    static decltype(auto) value(auto&& r)                { return std::forward<decltype(r)>(r).data; }
    static int            error(const Result<T>& r)      { return r.err_code; }
    static Result<T>      make(T v)                      { return {true, std::move(v), 0}; }
    static Result<T>      make_error(int e)              { return {false, {}, e}; }
};
```

## Pipe syntax

The `|` operator is provided as a convenience for chaining. It is **not** a globally
overloaded operator; it is defined as a hidden friend within each operation's closure type
and is only found via argument-dependent lookup when the right-hand side is one of the four
operation adaptors. There is no conflict with ranges `operator|` because there is no global
overload - only type-specific hidden friends.

The free-function call syntax is always available as an alternative:

```cpp
// Pipe syntax (chaining convenience)
auto r1 = box | std::box::and_then(f) | std::box::transform(g);

// Equivalent free-function call
auto r2 = std::box::transform(std::box::and_then(box, f), g);
```

## The four operations

### `and_then(fn)` - compose a chain of operations returning a box

When the box holds a value, calls `fn(value)` and returns its result. When the box is empty
or in an error state, propagates the state unchanged. `fn` must return the same box template
with the same error type.

```cpp
auto d = from_iso_str(ts)
    | std::box::and_then(validate);  // validate returns time_expected
```

### `transform(fn)` - apply a function to change the value

When the box holds a value, calls `fn(value)` and wraps the result. `fn` returns a plain
value, not a box.

```cpp
auto d = from_iso_str(ts)
    | std::box::transform(next_day);  // next_day returns ptime
```

### `or_else(fn)` - pass through the value, otherwise call a function with the error

When the box is empty or in an error state, calls `fn` and returns its result. The value is
passed through unchanged. For boxes without an error channel, `fn` takes no arguments; for
boxes with an error channel, `fn` receives the stored error.

```cpp
auto d = from_iso_str(ts)
    | std::box::or_else([](const std::string& err) {
          return time_expected{std::unexpected, "default"};
      });
```

### `transform_error(fn)` - apply a function to change the error

When the box is in an error state, calls `fn(error)` and replaces the error. The value is
passed through unchanged. Only available for boxes with an error channel.

```cpp
auto d = from_iso_str(ts)
    | std::box::transform_error([](const std::string& err) {
          return "parse failed: " + err;
      });
```

## Error channel detection {#error-channel-detection}

Not all boxes carry a typed error. `std::optional` signals absence structurally - there is
no stored error value. The library detects the presence of a real error channel automatically,
without any explicit tag or trait member:

| `error` signature | Error channel | Effect |
|---|---|---|
| `error(box)` - unary | yes | `transform_error` available |
| `error()` - nullary | no | `transform_error` disabled |

This distinction is derived from the `box_traits` specialization. For `std::optional`, the
provided `error()` is nullary - the library detects this and disables `transform_error`
automatically. No extra configuration is required.

## User-defined and third-party box types

### Raw pointers - the nullable pointer chain

From the [Unreal Community Wiki](https://unrealcommunity.wiki/), the recommended pattern for
navigating deep object graphs is:

```cpp
// Recommended shortcut - crashes if any pointer is null
FArmorStruct* TheArmor = &GetGalaxy()
    ->GetSolarSystem()
    ->GetPlanet()
    ->GetMainCharacter()
    ->Armor;
```

The safe version is correct but nobody writes it:

```cpp
auto galaxy = GetGalaxy();
if (!galaxy) return;
auto solarSystem = galaxy->GetSolarSystem();
if (!solarSystem) return;
auto planet = solarSystem->GetPlanet();
if (!planet) return;
auto character = planet->GetMainCharacter();
if (!character) return;
auto TheArmor = &character->Armor;
```

<!-- TODO: add box_traits<T*> specialization -->

With a single `box_traits<T*>` specialization, `T*` becomes a box. The pipeline is as
concise as the unsafe one-liner and as safe as the version nobody writes:

```cpp
auto TheArmor = GetGalaxy()
    | std::box::and_then(&Galaxy::GetSolarSystem)
    | std::box::and_then(&SolarSystem::GetPlanet)
    | std::box::and_then(&Planet::GetMainCharacter)
    | std::box::and_then(&ACharacter::Armor);
```

Every step is null-checked automatically. If `GetGalaxy()` returns null, the rest of the
chain never executes.

### C API return codes - libcurl

`CURLcode` is an integer enum where `CURLE_OK` (zero) means success and anything else is a
failure. The official libcurl examples discard return values:

```c
curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");       // return discarded
curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_3); // return discarded
```

The correct version duplicates cleanup at every exit path - which is why nobody writes it.

<!-- TODO: add box_traits<CURLcode> specialization -->

With a `box_traits<CURLcode>` specialization, `CURLcode` becomes a box with `value_type =
void` and `error_type = CURLcode`. Error propagation becomes automatic:

```cpp
CURLcode result = curl_global_init(CURL_GLOBAL_ALL);

result = curl_easy_setopt(curl, CURLOPT_URL, "https://example.com")
    | std::box::and_then([curl] {
          return curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_3);
      })
    | std::box::and_then([curl] {
          return curl_easy_perform(curl);
      });

curl_easy_cleanup(curl);
curl_global_cleanup();
```

If any step fails, the rest of the chain is skipped and the failing `CURLcode` propagates as
the final result. Cleanup happens once at the end regardless of where failure occurred.

## Extensibility: writing new generic operations

`std::box::box_traits` and the `std::box::box` concept are public. Any third party can write
new generic operations that work over all adapted types without modifying the library or
the types themselves.

### `inspect` and `inspect_error`

In the Introduction, `print_value` and `print_error` could not be used in the pipeline
because `and_then` and `or_else` require their callables to return a box. A user can solve
this once with `inspect` and `inspect_error`:

```cpp
template <std::box::box Box, typename Fn>
auto inspect(Box&& box, Fn&& fn) {
    using Traits = std::box::get_box_traits<Box>;
    if (Traits::has_value(box))
        fn(Traits::value(box));
    return std::forward<Box>(box);
}

template <std::box::box Box, typename Fn>
auto inspect_error(Box&& box, Fn&& fn) {
    using Traits = std::box::get_box_traits<Box>;
    if (!Traits::has_value(box))
        fn(Traits::error(box));
    return std::forward<Box>(box);
}
```

Both pass the box through unchanged. The pipeline from the Introduction now compiles:

```cpp
auto d = from_iso_str(ts)
    | inspect(print_value)          // observes the value, chain continues
    | inspect_error(print_error)    // observes the error, chain continues
    | std::box::transform(next_day);
```

Both operations work for any adapted type - `std::optional`, `std::expected`,
`boost::optional`, `absl::StatusOr`, or any user-defined box type.

### `value_or`

```cpp
template <std::box::box Box>
constexpr auto value_or(Box&& box, typename std::box::get_box_traits<Box>::value_type fallback) {
    using Traits = std::box::get_box_traits<Box>;
    return Traits::has_value(box) ? Traits::value(std::forward<Box>(box)) : fallback;
}

auto v1 = from_iso_str(ts) | value_or(boost::posix_time::ptime{});
```

This subsumes per-type proposals such as [@P3413R0] (`value_or_else` for `std::optional`
only) - the same function works for every adapted type.

# Alternatives Considered

## Customization point objects (CPOs)

CPOs are the preferred mechanism in C++20 ranges for dispatching to member functions or
ADL-found free functions. A CPO for `and_then` would dispatch to `obj.and_then(fn)` or
`and_then(obj, fn)` (ADL).

The fundamental limitation of CPOs for this use case is that they can only *dispatch* - they
cannot *supply* missing functionality. `absl::StatusOr<T>` has no `and_then` member and no
ADL `and_then`. A CPO cannot give it one. Each of the nine required capabilities would need
its own customization point. The problem is moved, not solved.

## Injection-based approaches

Both member functions and `tag_invoke` [@P1895R0] require injection into the type. Member
functions must be declared inside the class definition. `tag_invoke` relies on ADL - a free
function must be findable in the type's associated namespaces. Neither can be provided for a
type you don't own. `absl::StatusOr` cannot be given `and_then` as a member, and its
namespace cannot be extended with an ADL `tag_invoke` overload. Traits are the only mechanism
that works from outside the type.

## CRTP base class

A CRTP base (`box_base<Derived>`) could provide default implementations of the monadic
operations. The fundamental problem is that `box_base<boost::optional<T>>` cannot inspect
what the derived `box_traits` specialization has already provided. It cannot conditionally
generate only the missing members - it either provides everything, causing ambiguity with
user-supplied members, or provides nothing, which is no better than not having a base class.

The deduction cascade solves this precisely: each capability is resolved in priority order,
checking `box_traits<T>` first, then the type's own members, then deduction from template
parameters. The base class pattern has no equivalent mechanism.

# Impact on the Standard

This proposal adds a new header `<box>` (exact placement TBD - could be a standalone header
or additions to `<utility>` or `<functional>`):

```cpp
namespace std::box {

    // Customization point
    template <typename T>
    struct box_traits;

    // Assembled traits with deduction cascade applied
    template <typename T>
    using get_box_traits = /* exposition-only */;

    // Concept
    template <typename T>
    concept box = /* see below */;

    // Error channel detection
    template <typename T>
    concept has_error_channel = /* see below */;

    // Operations
    inline constexpr /* unspecified */ and_then        = /* unspecified */;
    inline constexpr /* unspecified */ transform       = /* unspecified */;
    inline constexpr /* unspecified */ or_else         = /* unspecified */;
    inline constexpr /* unspecified */ transform_error = /* unspecified */;

    // Extension point for user-defined pipe-composable operations
    template<template<typename> typename T>
    struct callable_adaptor;

} // namespace std::box
```

Built-in specializations of `std::box::box_traits` for `std::optional` and `std::expected`
are provided by the standard library. No changes are required to `std::optional` or
`std::expected` themselves.

This proposal does not deprecate the member-function APIs added in C++23 [@P0798R8] and
[@P2505R5].

# Proposed Wording

::: note
Wording is preliminary and will be refined based on design feedback. Normative wording
targeting the working draft will be provided in a subsequent revision.
:::

## Header `<box>` synopsis

```cpp
namespace std::box {

    // Customization point
    template <typename T>
    struct box_traits;

    // Assembled traits with deduction cascade applied
    template <typename T>
    using get_box_traits = /* exposition-only */;

    // Concepts
    template <typename T>
    concept box = /* see below */;

    template <typename T>
    concept has_error_channel = /* see below */;

    // Operations
    inline constexpr /* unspecified */ and_then        = /* unspecified */;
    inline constexpr /* unspecified */ transform       = /* unspecified */;
    inline constexpr /* unspecified */ or_else         = /* unspecified */;
    inline constexpr /* unspecified */ transform_error = /* unspecified */;

    // Extension point for user-defined pipe-composable operations
    template<template<typename> typename T>
    struct callable_adaptor;

} // namespace std::box
```

## `std::box::box` concept

A type `B` models `std::box::box` if `get_box_traits<B>` provides at minimum `value_type`,
`has_value`, `value`, `error`, and `make`.

::: note
Full wording TBD.
:::

## `std::box::and_then`

Calls `fn` with the contained value if the box holds one, and returns the result. Otherwise
propagates the absent or error state unchanged.

::: note
Full normative wording in a subsequent revision.
:::

## `std::box::transform`

Calls `fn` with the contained value if the box holds one, wraps the result in the same box
template, and returns it. Otherwise propagates the absent or error state unchanged.

::: note
Full normative wording in a subsequent revision.
:::

## `std::box::or_else`

Returns the box unchanged if it holds a value. Otherwise calls `fn` and returns its result.
For boxes with an error channel, `fn` receives the stored error; for boxes without an error
channel, `fn` takes no arguments.

::: note
Full normative wording in a subsequent revision.
:::

## `std::box::transform_error`

Returns the box unchanged if it holds a value. Otherwise calls `fn` with the stored error,
replaces the error with the result, and returns the box. Only available for boxes that satisfy
`has_error_channel`.

::: note
Full normative wording in a subsequent revision.
:::

# Future Work

- **Partial application for all operations** - extend all four operations to accept additional
  arguments that are partially applied to the callable, eliminating lambda boilerplate for
  common patterns:

  ```cpp
  // Today
  curl_global_init(CURL_GLOBAL_ALL)
      | std::box::and_then([curl] { return curl_easy_setopt(curl, CURLOPT_URL, "https://example.com"); });

  // With partial application
  curl_global_init(CURL_GLOBAL_ALL)
      | std::box::and_then(curl_easy_setopt, curl, CURLOPT_URL, "https://example.com");
  ```

  This is enabled by `callable_adaptor`, which is exposed as a public extension point for
  this purpose.

- **`inspect` and `inspect_error` as standard operations** - currently user-provided, these
  are common enough to warrant standardization alongside the four core operations.

- **Built-in specializations** for additional standard types: `std::shared_ptr`,
  `std::unique_ptr`, and `std::variant` (two-alternative only). `std::variant` with exactly
  two alternatives and distinct types satisfies the structural requirements. Multi-alternative
  and duplicate-type variants do not admit an unambiguous success channel and are deliberately
  excluded.

- **C++26 reflection** - `std::box::box_traits` defaults could be derived via reflection
  [@P2996R8], reducing or eliminating the need for explicit specializations for well-structured
  types. The diagnostic pattern used in the current implementation would be superseded by
  `delete("reason")` [@P2573R2], already accepted into C++26.

# Implementation Experience

A complete, conforming implementation is available as
[beman.monadics](https://github.com/bemanproject/beman.monadics), a Beman Project library.
The implementation:

- Passes the full test suite under GCC 11-15, Clang 17-21, AppleClang, and MSVC
- Is tested in C++20, C++23, and C++26 modes
- Includes Debug, Release, MaxSan (Address + Leak + UB sanitizers), and TSan configurations

# Acknowledgements

TBD.

# References

- [@P0798R8] Sy Brand. *Monadic operations for std::optional*. 2019.
- [@P2505R5] Jeff Garland. *Monadic Functions for std::expected*. 2022.
- [@P1895R0] Lewis Baker, Eric Niebler, Kirk Shoop. *tag_invoke: A general pattern for supporting customisable functions*. 2019.
- [@P3413R0] TBD. *value_or_else for std::optional*. TBD.
- [@P2996R8] Barry Revzin et al. *Reflection for C++26*. 2024.
- [@P2573R2] Ville Voutilainen. *= delete("should have a reason")*. 2023.
