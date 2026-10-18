<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

# Design Rationale

## Goals

`beman.monadics` has two related goals:

1. **Validate a unifying abstraction.** Demonstrate that the monadic vocabulary (`and_then`,
   `transform`, `or_else`, `transform_error`) can be expressed as free functions over a single
   "box" abstraction that covers `std::optional`, `std::expected`, smart pointers, raw pointers,
   and arbitrary user types. This is intended to inform a future C++ standardization paper.

2. **Provide an open extension point.** Give users a foundation on which they can build
   additional operations — `value_or`, `sequence`, and others — that automatically work for
   every adapted box type, without modifying the library or the box type itself.

## Why Free Functions?

C++23 adds monadic operations as member functions of `std::optional` and `std::expected`. This
works for those types, but member functions cannot be added retroactively to existing types
(`std::shared_ptr`, raw pointers, third-party types), and they cannot be added by users at all
without modifying the type.

Free functions in a namespace make the operations open for extension:

```cpp
// works with any adapted type — no changes to the type required
ptr | and_then(f) | transform(g) | or_else(h);
```

## The Box Abstraction

A "box" is any type that can hold a value or signal absence/failure. Its capabilities are
expressed as a traits class users specialize: `box_traits<T>`.

A box's two sides are described independently, and each side is one of three kinds: it carries
a payload, it carries nothing, or it does not exist at all.

| | carries a payload | carries nothing | does not exist |
|---|---|---|---|
| value side | `T` | `void` | `never` |
| error side | `E` | `unit` | `never` |

`unit` and `never` are different statements, and the difference decides which operations a box
gets. An empty `std::optional` *is* a case, it just holds nothing, so `error_type` is `unit`. A
box that cannot fail has no failure case to be in, so its `error_type` is `never`.

`1` is spelled `void` on the value side and `unit` on the error side. That asymmetry is
inherited rather than chosen: `std::expected<void, E>` puts `void` there and rebinding has to
hand the standard type back, while the standard has no `expected<T, void>` to constrain the
other side.

### Without an error channel

Types like `std::optional` and `std::shared_ptr` signal absence structurally — nothing is
stored on the failure side. Their `error_type` is `unit`, the payload of a case that carries
nothing:

```cpp
template <typename T>
struct beman::monadics::box_traits<MyBox<T>> {
    // The type of the held value.
    using value_type = T;

    // Nothing is stored on the failure side, so the failure payload is unit.
    // A box says this rather than leaving it to be inferred.
    using error_type = unit;

    // Produces a box holding U instead of T.
    template <typename U>
    using rebind = MyBox<U>;

    // The failure side has nowhere to put anything else, so it can only ever be
    // rebound to unit.
    template <std::same_as<unit>>
    using rebind_error = MyBox<T>;

    // Reports whether the box holds a value.
    [[nodiscard]] static bool           has_value(const MyBox<T>&);

    // Accesses the value; forwards the box's value category.
    [[nodiscard]] static decltype(auto) value(auto&& box);

    // Constructs a box holding a value.
    [[nodiscard]] static MyBox<T> make(value_type v);

    // Constructs an empty box. Needed only when the empty state is not the
    // default-constructed box, which is what naming unit otherwise synthesizes.
    [[nodiscard]] static MyBox<T> make_error(unit);
};
```

There is no `error` member here. `error(box)` is synthesized and yields `unit`, which is what
lets the library call it the same way for every box rather than nullary for some and unary for
others. `transform_error` is unavailable because there is no error to transform, and a handler
on the failure side takes no argument because there is nothing to pass it. See
[Error Channel Detection](#error-channel-detection).

### With an error channel

Types like `std::expected` store the error inside the box alongside the value type:

```cpp
template <typename T, typename E>
struct beman::monadics::box_traits<MyBox<T, E>> {
    // The type of the held value.
    using value_type = T;

    // The type of the stored error.
    using error_type = E;

    // Produces a box holding U instead of T, keeping the same error type E.
    template <typename U> using rebind       = MyBox<U, E>;

    // Produces a box holding the same T but with error type F instead of E.
    template <typename F> using rebind_error = MyBox<T, F>;

    // Reports whether the box holds a value.
    [[nodiscard]] static bool           has_value(const MyBox<T, E>&);

    // Accesses the value; forwards the box's value category.
    [[nodiscard]] static decltype(auto) value(auto&& box);

    // Accesses the stored error; forwards the box's value category.
    [[nodiscard]] static decltype(auto) error(auto&& box);

    // Constructs a box holding a value.
    [[nodiscard]] static MyBox<T, E> make(value_type v);

    // Constructs a box holding an error. Enables transform_error.
    [[nodiscard]] static MyBox<T, E> make_error(error_type e);
};
```

All four operations — including `transform_error` — are available for these types.

### With only one of the two cases

A box can also hold one case and have no way to reach the other: a value that is always there,
or an error that already happened. Naming `never` on the absent side is the whole of saying so,
and the library supplies everything that side would have needed — the read, the construction,
the rebind, and the tag query — because none of it can be reached:

```cpp
namespace bms = beman::monadics;

template <typename T>
struct Certain {
    using value_type = T;
    using error_type = bms::never;      // there is no failure case

    template <std::same_as<bms::never>>
    using rebind_error = Certain<T>;

    T v;
    constexpr const T& value() const { return v; }
};

template <typename T>
struct beman::monadics::box_traits<Certain<T>> {};   // nothing left to supply
```

That is a complete box. It is the identity monad, so `and_then` is application and `transform`
maps the value, while `or_else` and `transform_error` are unavailable — there is nothing to
recover from:

```cpp
Certain<int>{4} | bms::transform([](int x) { return x * 2; });   // Certain<int>{8}
Certain<int>{4} | bms::to<std::optional<int>>();                 // std::optional{4}
```

Concepts report which case a box has, all read from the traits rather than from the type.
`value_only`, `error_only` and `single_case` describe a box that has exactly one of the two.
`no_value_case` and `no_error_case` ask about one side on its own, which is what an operation
building a result needs to know, and `no_case` is the box with neither, the shape a handler takes
over a box that cannot fail. `unreachable` is where a missing case would have been read, for the
rare specialization that writes such a member by hand.

The library's own `just` and `just_error` are instances of this: a `just` holds a value and has
no failure case, a `just_error` holds an error and has no value case. They are what an
operation's pass-through handler builds, settled into the operation's result by `to`. See
[Carrying a case into the result](#carrying-a-case-into-the-result).

## Adapting an Existing Type

Most real types already expose part of this interface. The library runs a deduction cascade
for each capability: it checks the explicit specialization first, then the box type's own
members, then the template parameters. You only need to supply what it cannot find on its own.

### When almost everything is deducible - minimal specialization

Opting a type into the box abstraction is always explicit: a `box_traits<T>` specialization is
required even if the type already exposes a fully compatible interface. This prevents types from
being silently treated as boxes with incorrect behaviour. For example, a type that satisfies
almost everything but lacks `make_error` would otherwise be accepted and produce wrong results.

`std::expected<T, E>` already has `has_value()`, `value()`, `error()`, nested `value_type`
and `error_type`, and two template parameters that the library uses for rebind deduction.
However, `make_error` cannot be deduced: `std::expected` constructs error states via
`std::unexpect`-tagged construction, not from the error type directly. One member suffices:

```cpp
template <typename T, typename E>
struct beman::monadics::box_traits<std::expected<T, E>> {
    [[nodiscard]] static constexpr auto make_error(auto&& e) {
        return std::expected<T, E>{std::unexpect, std::forward<decltype(e)>(e)};
    }
};
```

All four operations are available after this one-member opt-in.

### When a few pieces are missing - minimal specialization

`std::optional<T>` has `has_value()`, `value()`, and `value_type`, so the value side deduces.
Its `error_type` deduces to `unit`, since nothing is stored on the failure side. What the
library cannot derive is how to build an empty one: `std::optional<T>{}` is not something a
generic rule can reach for, because value-initialising a box means the engaged state for some
types and the empty state for others. One member is enough:

```cpp
template <typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    using error_type = unit;
};
```

Its argument is the `unit` payload, which carries nothing and is ignored. Because `error_type`
is `unit`, `has_error_channel<optional<T>>` is false, `transform_error` is unavailable, and
`rebind_error` accepts only `unit` rather than claiming that `optional` could carry an error of
any type.

## Why a Traits Class, Not a Concept or CRTP Base?

The central requirement is that the library must work retroactively — for types that already
exist and cannot be modified. Consider `std::shared_ptr<T>`. It has no `has_value()`, no
`value_type`, and no way to build an empty one that a generic rule could find. A concept-gated free function can constrain usage but cannot
supply missing members. A CRTP base requires inheritance, which is impossible for standard
library types and breaks for any `final` class.

A traits specialization adds the missing pieces without touching `std::shared_ptr` at all.
Neither `error` nor `make_error` is among them: naming `unit` on the failure side synthesizes
the read, which yields `unit`, and the build, which is the default-constructed box.

```cpp
template <typename T>
struct beman::monadics::box_traits<std::shared_ptr<T>> {
    using error_type = unit;

    static bool          has_value(const std::shared_ptr<T>& p) { return static_cast<bool>(p); }
    static decltype(auto) value(auto&& p) { return *std::forward<decltype(p)>(p); }
    static std::shared_ptr<T> make(T v) { return std::make_shared<T>(std::move(v)); }
};
```

| Approach | Problem |
|---|---|
| Member-function concept | Requires the type to already expose the right interface; cannot retrofit |
| CRTP base | Requires inheritance; impossible for `final` classes and third-party types |
| **Traits specialization** | **Non-intrusive, retroactively applicable to any type** |

The traits pattern follows the precedent of `std::iterator_traits`, `std::char_traits`, and
`std::numeric_limits`. It separates the "what" (the box type) from the "how" (the trait
implementation) without touching the type itself.

## Extensibility

The second goal — an open extension point — is what distinguishes this library from simply
adding more member functions to standard types.

`get_box_traits<Box>` is a public type alias that exposes the full assembled traits for any
adapted box: `value_type`, `error_type`, `has_value`, `value`, `error`, `make`, `make_error`,
`rebind`, and `rebind_error`. Anyone can use it to write a new operation that works immediately
for every existing and future box type.

`either` is usually the better starting point. It reads the tag and hands the inhabited
case to the matching continuation, which is the whole of what the four shipped operations do,
so a new operation written against it needs no `has_value` and `value` dance of its own. It is
joined by `just`, `just_error`, `compose`, `make_value` / `make_error` and the two pass-through
handlers `carry_value` / `carry_error`, which are what the shipped operations are made of:

```cpp
template <beman::monadics::box Box>
constexpr auto to_string(Box&& box) {
    return beman::monadics::either([](const auto& v) { return std::format("{}", v); },
                                       [](const auto&) { return std::string{"<none>"}; },
                                       std::forward<Box>(box));
}
```

### Carrying a case into the result

A continuation often has to produce *the other case* of a box it cannot name. `and_then`'s error
handler has to rebuild the error inside whatever box the callable returned, and only the callable
knows what that is. `just(v)` and `just_error(e)` solve this: each is a box holding exactly one
case and lacking the other, so a handler can build the case it carries without committing to a
family.

```cpp
namespace bms = beman::monadics;

auto j = bms::just(42);                       // just_t<int>: a value, no failure case
auto e = bms::just_error(bms::unit{});        // just_error_t<unit>: a failure, no value case
```

Settling a carrier into a named box is `make_value<R>(v)` and `make_error<R>(e)`, which is what
`get_box_traits<R>::make` does where that call exists, and more where it does not: a value
side carrying nothing takes no argument, and a failure carrying nothing accepts a typed error
and drops it.

Because the carrier still has to end up in the operation's result, the library ships that step
as two handlers as well, `carry_error` and `carry_value`. Each is the matching `make` waiting to
be told its result, and `either` supplies the type from the handler that names one:

```cpp
and_then(fn):   either(fn, bms::carry_error, std::forward<Box>(box));
or_else(fn):    either(bms::carry_value, fn, std::forward<Box>(box));
```

One handler has to name the result, since otherwise there is nothing to tell the other. The one
that names it either declares its return type or is an ordinary callable like `fn`, which matters
beyond readability: `std::invocable` can be answered for a handler that declares its return type
without instantiating its body.

`compose(g, f)` is `g` after `f`, with one rule that earns its place here: if `f` returns `void`,
`g` is called with no argument. That is what lets `transform` accept a callable that only has a
side effect, without every operation repeating the check.

### The four operations are derived, not primitive

With those pieces the shipped operations are the textbook definitions, where `into R` is the
`| to<R>()` that settles a carrier into the result the operation names:

| operation | definition | in the literature |
|---|---|---|
| `and_then(fn)` | `either(fn, carry_error, box)` | `bind m f = [f, inr] m` |
| `or_else(fn)` | `either(carry_value, fn, box)` | `handle m g = [inl, g] m` |
| `transform(fn)` | `box \| and_then(compose(just, fn) into R)` | `fmap f = bind (return . f)` |
| `transform_error(fn)` | `box \| or_else(compose(just_error, fn) into R)` | its mirror |

The first two read as the literature does, because `carry_error` is `inr` and `carry_value` is
`inl`. The other two name their own `R`, the box rebound around what `fn` returned, which they
compute for their own constraint anyway.

Only the first two reach `either` directly. `transform` is `bind (return . f)` as written, an
`and_then` whose continuation applies `fn` and settles the result into `R`, so the two cannot
disagree on any box and the functor laws follow from the monad laws. It also inherits
`and_then`'s rule about what a continuation may return, which is why `transform` refuses a box
whose `rebind` would change the error type to one the two cannot share, something neither
`std::optional` nor `std::expected` permits. An operation you write against the same pieces
inherits the same guarantee.

### The payload two cases share

An operation can leave a channel with two candidate types where at most one will inhabit it.
`and_then` is that case: the continuation returns a box of the same family, and that box names an
error type of its own, so the result's error channel has both the input's error and the returned
box's error to hold. `or_else` is the mirror on the value channel.

`common_payload<P1, P2>` names the type that holds either. It is a property of the two payloads,
not of the channel they sit on, so each operation asks for it on whichever of its channels has two
candidates.

Two laws hold by construction and no algebra writes them: a payload joined with itself is itself,
and a payload joined with `never`, the case a one-sided box does not have, is itself. A chain whose
steps all fail the same way therefore needs no specialization, and that is every chain over
`std::optional` or over one `std::expected<T, E>`.

The library ships no specialization of its own. Payloads nobody adapted have no common payload,
and an operation over them keeps refusing rather than picking a side:

```
  'specialize common_payload for these two payloads to name the type that holds either'
```

What an algebra does have to satisfy is checked. The two orders must answer alike, and the type
named must be constructible from each payload, since the operation has to put whichever error it
holds into it:

```cpp
struct request_error {
    request_error(io_error);
    request_error(parse_error);
};

template<> struct beman::monadics::common_payload<io_error, parse_error> { using type = request_error; };
template<> struct beman::monadics::common_payload<parse_error, io_error> { using type = request_error; };

std::expected<int, io_error> e = ...;
e | and_then([](int v) { return std::expected<int, parse_error>{v}; });
// std::expected<int, request_error>
```

### Example: `filter`

Keeping a value only when a predicate holds needs both cases from *one* handler, and a carrier
stands for a single case. So that handler names the result and builds through the box's traits,
while `carry_error` passes the failure through:

```cpp
namespace bms = beman::monadics;

template<typename Pred>
class filter_t {
public:
    constexpr explicit filter_t(Pred pred) : pred_(std::move(pred)) {}

    template<bms::box Box>
        requires(!bms::has_error_channel<Box> || bms::on_error<"filter: the box must not carry an error">)
             && (bms::invocable_with_value<Pred, const Box&> || bms::on_error<"filter: pred must accept the value">)
    [[nodiscard]] friend constexpr auto operator|(Box&& box, filter_t&& op) {
        using R = std::remove_cvref_t<Box>;
        using Traits = bms::get_box_traits<Box>;

        return bms::either(
            [&](auto&& v) -> R {
                if (op.pred_(v))
                    return Traits::make(std::forward<decltype(v)>(v));
                return Traits::make_error(bms::unit{});
            },
            bms::carry_error,
            std::forward<Box>(box));
    }

private:
    Pred pred_;
};

inline constexpr bms::callable_adaptor<filter_t> filter{};

static_assert((std::optional{4} | filter([](int v) { return v % 2 == 0; })) == std::optional{4});
static_assert((std::optional{3} | filter([](int v) { return v % 2 == 0; })) == std::nullopt);
```

The `|| on_error<"...">` clauses are how the library reports a constraint failure by name rather
than as "no matching operator". See [Concept Constraints and Error
Messages](#concept-constraints-and-error-messages).

### Example: `value_or`

```cpp
#include <beman/monadics/monadics.hpp>

template <beman::monadics::box Box>
constexpr auto value_or(Box&& box, typename beman::monadics::get_box_traits<Box>::value_type fallback) {
    using Traits = beman::monadics::get_box_traits<Box>;
    if (Traits::has_value(box))
        return Traits::value(std::forward<Box>(box));
    return fallback;
}
```

Works for `std::optional`, `std::expected`, `std::shared_ptr`, and any user-adapted type —
zero changes to the library or to the box types.

### Not an example: `flatten`

A nested box needs no new operation. `and_then` returns whatever box its callable
produces, so a callable that hands the inner box straight back is flatten:

```cpp
std::optional<std::optional<int>> nested{std::optional{42}};
auto flat = nested | beman::monadics::and_then([](auto inner) { return inner; });
```

`flat` is `optional<int>{42}`. Writing it this way also inherits `and_then`'s
constraint that the returned box carry the same error type, so
`expected<expected<int, char>, int>` is rejected instead of flattened onto one of
its two error types.

The callable has to return by value. `std::identity` forwards its argument, and
the operations refuse a callable whose result is a reference into the box.

### Example: pipe-compatible operations via `operator|`

New operations can support the `|` pipe syntax by defining a closure type that holds the
argument and exposes `operator|` as a hidden friend — the same pattern used by the library's
own operations. `callable_adaptor<T>` is the factory object that constructs a `T<Fn>` from
a user-provided callable:

```cpp
#include <beman/monadics/monadics.hpp>
namespace bms = beman::monadics;

template<typename Fn>
class value_or_t {
public:
    constexpr explicit value_or_t(Fn fn) : fn_(std::move(fn)) {}

    template<bms::box Box>
    friend constexpr auto operator|(Box&& box, value_or_t&& op) {
        using Traits = bms::get_box_traits<Box>;
        if (Traits::has_value(box))
            return Traits::value(std::forward<Box>(box));
        return std::move(op.fn_)();
    }

private:
    Fn fn_;
};

inline constexpr bms::callable_adaptor<value_or_t> value_or{};
```

```cpp
#include <optional>

constexpr auto v = std::optional<int>{} | value_or([] { return 42; }); // 42
static_assert(v == 42);
```

`callable_adaptor<value_or_t>` is the callable object users invoke (e.g. `value_or(fn)`).
It constructs a `value_or_t<decay_t<Fn>>` storing the argument. The hidden friend `operator|`
is found via ADL on the closure type — both `Box` and `Fn` are in scope simultaneously, so
constraints can reference both without any indirection.

This pattern requires no changes to `beman.monadics` and no changes to `std::optional`.

## Value-Category Preservation

All four operations preserve the value category of the box. Passing an rvalue box forwards the
contained value as an rvalue to the callback, enabling move semantics through the chain:

```cpp
std::optional<std::string>{"hello"}
    | and_then([](std::string&& s) {       // s is rvalue
          return std::optional{std::move(s)};
      });
```

## Concept Constraints and Error Messages

The `operator|` for each operation is constrained by a named concept
(`and_thenable_impl`, `or_elseable_impl`, etc.) rather than an inline `requires` expression.
Named concepts surface human-readable `on_error<"msg">` diagnostics when a constraint fails,
whereas inline `requires` expressions produce only "expression not well-formed".

If `and_then` receives a callable that returns a box with a different error type, the compiler
reports:

```
constraint 'and_thenable_return<..., ...>' not satisfied:
  'Should return the Box with same error_type'
```

> **Note:** MSVC will print ASCII characters.

## The `or_else` / `and_then` Asymmetry

`and_then` must preserve the **error type**: the callback replaces the value but the error
channel stays the same.

`or_else` must preserve the **value type**: the callback replaces the error but the value
channel stays the same.

This mirrors `std::expected`:

- `and_then(f)` — `f: T -> expected<U, E>` (E is fixed)
- `or_else(f)` — `f: E -> expected<T, F>` (T is fixed)

## Error Channel Detection

Not all boxes have a real error channel. `std::optional` has no error type — absence is
represented structurally. The library detects this via `has_error_channel`, which asks
whether the box's `error_type` is anything other than `unit`:

```cpp
template<typename Box>
concept has_error_channel = box<Box> && !std::same_as<typename get_box_traits<Box>::error_type, unit>;
```

`error(box)` is unary for every box, yielding `unit` where nothing is stored, so this is a
comparison of types rather than a probe of how many arguments `error` accepts. That matters
beyond tidiness: an `error` with two possible arities forced every consumer of it to branch on
which one it had, and those branches spread through the deduction probes, the invocation
helpers and the propagation helpers. One arity removes all of them.

For `optional<T>`, `error_type` is `unit`, so `has_error_channel<optional<T>>` is false and
`transform_error` is unavailable.

## Compile-time Evaluation

All four operations are `constexpr`. Any chain where the box type's `box_traits` methods
and all provided callables are also `constexpr` evaluates entirely at compile time:

```cpp
#include <beman/monadics/monadics.hpp>
#include <optional>
namespace bms = beman::monadics;

template <typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    using error_type = unit;
};

constexpr auto result =
    std::optional{6}
    | bms::and_then([](int v) { return std::optional{v * 7}; })
    | bms::transform([](int v) { return v - 0; });

static_assert(result == std::optional{42});
```

This applies equally to `std::expected` and any custom box type whose `box_traits`
members are `constexpr`. Standard types that prevent compile-time evaluation (e.g.
`std::shared_ptr`, I/O, non-`constexpr` functions) naturally stay runtime-only — the
library imposes no restriction either way.

## Non-Goals

- **Runtime overhead**: all operations are `constexpr`; no virtual dispatch, no allocation,
  no type erasure. See [Compile-time Evaluation](#compile-time-evaluation).
- **Heterogeneous chaining**: `and_then` does not allow changing the box template (only the
  value type). Mixing `optional` and `expected` in a single chain is not supported.
- **Lazy evaluation**: operations execute eagerly. Lazy/deferred evaluation is out of scope.
