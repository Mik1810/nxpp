# Attribute System Design Evaluation

This document evaluates the current `std::any`-based attribute storage model and
compares it pragmatically against two plausible future alternatives.

It is the companion evaluation for [Issue #40](https://github.com/Mik1810/nxpp/issues/40).
For the current API surface and usage guidance, see
[`API_REFERENCE.md`](API_REFERENCE.md#attribute-api-reference).

---

## 1. Current model: `std::any`

### How it works

Attributes are stored in two private maps:

```
node_properties: std::map<NodeID, std::map<std::string, std::any>>
edge_properties: std::map<std::size_t, std::map<std::string, std::any>>
```

Each attribute value is a `std::any` — a type-erased container that holds one
value of any copyable type. Retrieval uses `std::any_cast<T>`, which throws
`std::bad_any_cast` on a type mismatch.

### Pros

| | |
|---|---|
| **Fully heterogeneous** | Any copyable C++ type can be stored — `int`, `double`, `std::string`, custom structs, `std::vector<double>`. |
| **NetworkX-compatible ergonomics** | Dynamic attribute assignment (`G.node(u)["k"] = v`) maps directly to Python's dict-on-node model. |
| **No schema declaration required** | Users can add attributes on the fly without registering them upfront. |
| **Low implementation surface** | The storage layer is a thin `std::map<std::string, std::any>` — straightforward to maintain. |

### Cons

| | |
|---|---|
| **Runtime type checks only** | The compiler cannot detect a mismatch between the stored type and the requested cast type. |
| **Silent wrong-type storage** | Storing `int` and later casting to `double` fails at runtime with no earlier warning. |
| **No introspection** | There is no type-safe way to enumerate "what types are stored across all attributes". `std::any` only exposes `type()` for runtime comparison. |
| **Cannot store non-copyable types** | `std::any` requires `CopyConstructible`; move-only types cannot be stored. |
| **Opaque to tooling** | Static analysis and IDE completion cannot reason about which type a specific attribute key holds. |

### When `std::any` works well

- Rapid prototyping and exploratory graph analysis
- Ports of Python/NetworkX workflows where attribute types are implicit
- Educational use cases where API simplicity matters more than type safety
- Heterogeneous data (e.g., mixing `int` population counts with `std::string` labels on the same node type)

### Where it falls short

- Production pipelines where wrong-type-stored errors surface only at runtime, far from the write site
- Serialization paths that need to enumerate all stored types
- Integration with typed result structures (e.g., a typed DataFrame layer on top of graph attributes)

---

## 2. Alternative A: bounded `std::variant`

### How it works

Replace `std::any` with a bounded variant covering the most common attribute types:

```cpp
using AttrValue = std::variant<bool, int, long, double, std::string>;
using AttrMap   = std::map<std::string, AttrValue>;
```

Storage shape stays the same; only the leaf value type changes.

Reads use `std::get<T>` or `std::visit`. The checked accessor API
(`get_node_attr<T>`, `try_get_node_attr<T>`) would translate to
`std::get_if<T>` or `std::holds_alternative<T>`.

### Pros

| | |
|---|---|
| **Bounded type set** | The compiler and tooling know exactly which types are possible in an attribute value. |
| **`std::visit` introspection** | Code can safely enumerate or transform all attributes without runtime-cast guesswork. |
| **Slightly better cast safety** | `std::get<T>` throws `std::bad_variant_access` on mismatch, same as today, but `std::holds_alternative<T>` allows cheaper pre-checks. |
| **Same API shape** | `get_node_attr<T>`, `try_get_node_attr<T>`, `has_node_attr` remain valid with minor internal rewiring. |
| **Same proxy ergonomics** | `G.node(u)["k"] = 42;` still works — the proxy `operator=` resolves the variant alternative from the argument type. |

### Cons

| | |
|---|---|
| **Cannot store custom types** | Any user-defined struct (e.g., `MyMetadata`) cannot be stored as an attribute. |
| **Type set is frozen at definition** | Extending the variant to add a new type is a recompilation of every translation unit that includes the header. |
| **int/long/double ambiguity** | Users must be careful about which alternative is active; `42` stores as `int`, not `long`, which still requires attention at the call site. |
| **Proxy operator= type resolution** | The proxy template deduction must resolve which variant alternative to activate; edge cases with narrowing conversions need explicit handling. |
| **Migration breakage** | Any existing user code storing non-variant types (custom structs, vectors) breaks immediately. |

### Impact on existing API layers

| Layer | Impact |
|---|---|
| Proxy syntax | Minimal — `operator=(const T&)` activates the matching alternative or fails to compile for unsupported types |
| Checked accessors | Minimal — `any_cast<T>` → `std::get<T>` or `std::get_if<T>` |
| `edge_id` precision | None — the precision path stays unchanged |
| Multigraph support | None — storage shape is identical |

---

## 3. Alternative B: typed attribute schema

### How it works

Each graph instance carries a compile-time (or registration-time) schema that
maps attribute key names to concrete types. Users declare which attributes
exist and what type each holds before populating the graph:

```cpp
// Hypothetical API — not implemented
auto G = nxpp::Graph<MyNodeID>()
    .with_node_attr<int>("population")
    .with_node_attr<std::string>("label")
    .with_edge_attr<double>("capacity");

G.set_node_attr("Rome", "population", 3'000'000); // type-checked at compile time
```

Reads become fully type-safe member calls:

```cpp
int pop = G.get_node_attr("Rome", "population"); // no template parameter needed
```

### Pros

| | |
|---|---|
| **Full compile-time type safety** | Attribute type mismatches are caught by the compiler. |
| **No runtime cast failures** | `std::bad_any_cast` and `std::bad_variant_access` are eliminated for schema-declared keys. |
| **Tooling-friendly** | IDEs and static analysis can reason about attribute types without reading code flow. |
| **Explicit surface** | The attribute schema is part of the type or construction call, making graph semantics self-documenting. |

### Cons

| | |
|---|---|
| **Major breaking change** | The entire attribute API must be redesigned. All existing code breaks. |
| **Incompatible with NetworkX dynamic style** | Python NetworkX has no schema; a typed schema makes the C++ bridge semantically different. |
| **High implementation cost** | The schema must be reflected in the type system or a registration mechanism, and integrated across all algorithm wrappers, proxy layers, serialization, and result wrappers. |
| **Verbose for simple cases** | A graph with two or three ad-hoc attributes requires a schema declaration before a single line of graph population. |
| **Runtime-schema variant** | If the schema is not compile-time (e.g., registered at construction), the type-safety benefit is reduced to a form of validated `std::any` — similar complexity, less flexibility. |

### Impact on existing API layers

| Layer | Impact |
|---|---|
| Proxy syntax | Entire proxy layer must be redesigned around the schema |
| Checked accessors | Fully replaced by schema-generated typed accessors |
| `edge_id` precision | Compatible but requires schema-aware edge attribute path |
| BGL integration | All algorithm wrappers that read/write attributes need to be schema-aware |

---

## 4. Comparison

| Criterion | `std::any` (current) | Bounded `std::variant` | Typed schema |
|---|:---:|:---:|:---:|
| Heterogeneous value types | Full | Bounded set | Schema-declared |
| Compile-time type safety | None | Partial (bounded) | Full |
| Custom object support | Yes | No | Yes (if compile-time) |
| `std::visit` / type enumeration | No | Yes | Yes |
| Proxy syntax preserved | Yes | Yes (minor changes) | No (full redesign) |
| `edge_id` precision preserved | Yes | Yes | Compatible |
| API breakage on migration | None | High | Very high |
| Implementation cost | None | Moderate | Very high |
| Ergonomics for simple use cases | High | High | Low |
| Ergonomics for production pipelines | Medium | Medium–High | High (after migration) |

---

## 5. Current decision and rationale

**Stay with `std::any`.**

The project is a C++20/NetworkX bridge designed for educational, prototyping, and
exploratory graph analysis use cases. In that context:

- Heterogeneity and zero-schema ergonomics outweigh the safety cost of runtime type checks.
- The `try_get_*_attr<T>` checked accessors already provide a safe read path that makes type expectations explicit at the call site.
- No concrete production use case has emerged where `std::any` is a blocking limitation.
- Switching to `std::variant` would immediately break any user storing custom types as attributes, for a benefit (bounded type set) that does not match the project's current audience.
- Switching to a typed schema would require a major version bump, a redesign of the proxy layer, and full replacement of the attribute accessor surface — premature given the project's scope.

This decision should be revisited when a concrete need arises — for example, if a
serialization layer or a Python-binding integration requires introspectable attribute
types that `std::any` cannot provide cleanly.

---

## 6. Conservative migration path (if a future switch is chosen)

This section documents a realistic step-by-step path from `std::any` to bounded
`std::variant`, should a future major version decide to make that change. It is
not a current plan.

**Phase 1 — Parallel alias (non-breaking)**

Introduce an `AttrVariant` alias alongside `std::any`:

```cpp
using AttrVariant = std::variant<bool, int, long, double, std::string>;
```

Add opt-in overloads of `add_edge` and `NodeAttrProxy::operator=` that accept
`AttrVariant` values. The default storage path remains `std::any`.

Risk at this phase: none for existing users.

**Phase 2 — Deprecation (minor breaking)**

Mark the `std::any`-based write paths (`add_edge` attribute overloads,
`NodeAttrProxy::operator=` for out-of-set types) as `[[deprecated]]`.

Update the proxy `operator=` to reject types outside `AttrVariant` with a
`static_assert` or a clear compile error.

Risk at this phase: users storing custom structs or non-variant types receive
compile warnings or errors and must migrate to explicit serialization.

**Phase 3 — Removal (major version bump)**

Remove `std::any` storage entirely. Replace `AttrMap` definition to use
`AttrVariant`. Update all checked accessor implementations to use
`std::get_if<T>` instead of `std::any_cast<T>`.

Risk at this phase: hard break for any code that survived Phase 2 without migration.

**Timeline recommendation**: do not start Phase 1 without a concrete motivating
use case. The compatibility cost is real and the benefit of a bounded type set
only materializes if the project's user base needs type-safe attribute introspection.
