# nxpp — GitHub Issues Roadmap

## Core Governance and API Stability

### SemVer Compliance Audit
**Priority:** High  
**Type:** Governance / Release Engineering

#### Problem
`1.4.0` introduced a breaking API change (`edges()` semantics) while using a minor release version.

#### Goals
- Audit all releases since `1.0.0`
- Identify accidental breaking changes
- Define strict SemVer enforcement policy

#### Tasks
- [ ] Review CHANGELOG against public API surface
- [ ] Define explicit "breaking change" criteria
- [ ] Add release checklist entry for API compatibility review
- [ ] Document migration expectations for future major releases

#### Expected Outcome
Future releases follow strict SemVer guarantees.


---

### Define Official Boost Support Matrix
**Priority:** High  
**Type:** Build / Compatibility

#### Problem
Boost minimum version policy is not fully synchronized across:
- `CONTRIBUTING.md`
- `conanfile.py`
- CI workflows
- `CMakeLists.txt`

#### Tasks
- [ ] Define official minimum supported Boost version
- [ ] Add minimum-version CI job
- [ ] Add latest-version CI job
- [ ] Update `find_package(Boost ...)`
- [ ] Update documentation

#### Suggested Policy
```text
Minimum supported Boost: 1.86.0
Tested versions:
- 1.86.0
- latest stable
```


---

# WASM Rewrite Roadmap

## WASM Architecture Rewrite
**Priority:** High  
**Type:** WASM / TypeScript

#### Problem
Current WASM bindings evolved incrementally and no longer cleanly reflect the architecture of the C++ core.

#### Goals
- Treat WASM as a deliberate JS/TS facade
- Avoid mirroring C++ syntax directly
- Separate runtime concerns from core graph semantics

#### Tasks
- [ ] Define stable JS/TS API conventions
- [ ] Separate Embind layer from TS facade
- [ ] Minimize duplicated validation logic
- [ ] Define error mapping policy
- [ ] Define attribute serialization policy
- [ ] Define multigraph edge-ID policy
- [ ] Add architectural diagrams

#### Deliverables
- `wasm/API_POLICY.md`
- `wasm/ARCHITECTURE.md`
- New binding structure


---

## WASM Phase 1 — Core Graph API
**Priority:** High  
**Type:** WASM

#### Scope
- GraphInt
- GraphStr
- DiGraphInt
- DiGraphStr

#### Features
- [ ] addNode
- [ ] addEdge
- [ ] removeNode
- [ ] removeEdge
- [ ] hasNode
- [ ] hasEdge
- [ ] nodes
- [ ] neighbors
- [ ] edge weights

#### Notes
Focus on correctness and predictable JS semantics before algorithm coverage.


---

## WASM Phase 2 — Attributes
**Priority:** High  
**Type:** WASM

#### Scope
Attribute support for:
- nodes
- edges

#### Tasks
- [ ] Define supported JS attribute types
- [ ] Implement serialization layer
- [ ] Add type-safe TS declarations
- [ ] Add contract tests
- [ ] Define unsupported type behavior

#### Suggested Initial Supported Types
- string
- number
- boolean
- null


---

## WASM Phase 3 — Multigraph Support
**Priority:** High  
**Type:** WASM

#### Problem
Multigraph semantics require stable edge IDs.

#### Tasks
- [ ] Expose edge IDs as first-class JS objects
- [ ] Define parallel-edge APIs
- [ ] Add multigraph traversal tests
- [ ] Add attribute tests for parallel edges
- [ ] Add deletion consistency tests

#### Notes
Do not emulate ambiguous `(u, v)` edge APIs for multigraphs.


---

## WASM Phase 4 — Traversal Algorithms
**Priority:** Medium  
**Type:** WASM / Algorithms

#### Scope
- BFS
- DFS

#### Tasks
- [ ] Expose traversal APIs
- [ ] Add iterator-style interfaces where possible
- [ ] Add traversal order tests
- [ ] Add disconnected graph tests
- [ ] Add large-graph tests


---

## WASM Phase 5 — Shortest Paths
**Priority:** Medium  
**Type:** WASM / Algorithms

#### Scope
- Dijkstra
- Bellman-Ford
- Floyd-Warshall

#### Tasks
- [ ] Define result object model
- [ ] Implement path reconstruction
- [ ] Add unreachable-node tests
- [ ] Add negative-cycle tests
- [ ] Clarify weight-key policy


---

## WASM Phase 6 — Components and SCC
**Priority:** Medium  
**Type:** WASM / Algorithms

#### Tasks
- [ ] connected_components
- [ ] strongly_connected_components
- [ ] result wrappers
- [ ] contract tests


---

## WASM Phase 7 — Centrality
**Priority:** Medium  
**Type:** WASM / Algorithms

#### Scope
- PageRank
- Degree centrality
- Betweenness centrality

#### Tasks
- [ ] Clarify weighted vs unweighted semantics
- [ ] Add convergence tests
- [ ] Add normalization tests


---

## WASM Phase 8 — Flow Algorithms
**Priority:** Medium  
**Type:** WASM / Algorithms

#### Problem
Flow APIs are significantly more complex than traversal/path APIs.

#### Tasks
- [ ] Define flow result model
- [ ] Define edge-flow serialization
- [ ] Define multigraph semantics
- [ ] Add staged-flow tests
- [ ] Add invalidation tests

#### Notes
This should likely be the final WASM algorithm module.


---

# Testing and CI

## Add Compile-Time Tracking
**Priority:** Medium  
**Type:** CI / Performance

#### Problem
Header-only template growth may significantly increase compile times.

#### Tasks
- [ ] Add compile-time benchmark target
- [ ] Measure umbrella header impact
- [ ] Measure modular include impact
- [ ] Track CI compile durations over time


---

## Add Dedicated Flow Cache Stress Tests
**Priority:** High  
**Type:** Testing

#### Problem
The staged min-cost-flow cache is one of the most technically delicate areas.

#### Tasks
- [ ] Test graph mutation after staged operations
- [ ] Test multiple graph instances
- [ ] Test cache invalidation
- [ ] Test pointer reuse scenarios
- [ ] Test concurrent staged sequences
- [ ] Test cleanup after exceptions


---

## Add API Compatibility Snapshot Tests
**Priority:** Medium  
**Type:** Testing / Governance

#### Goals
Detect accidental API drift.

#### Tasks
- [ ] Snapshot public aliases
- [ ] Snapshot public method signatures
- [ ] Snapshot exported CMake targets
- [ ] Snapshot WASM exports


---

# Documentation

## Create ROADMAP.md
**Priority:** Medium  
**Type:** Documentation

#### Goals
Provide a centralized roadmap for:
- core C++
- WASM
- packaging
- CI
- performance
- browser support

#### Tasks
- [ ] Define milestones
- [ ] Define stabilization targets
- [ ] Define experimental areas


---

## Document Weight Attribute Policy
**Priority:** Medium  
**Type:** Documentation

#### Problem
Weighted shortest paths currently support only `"weight"` as the weight attribute channel.

#### Tasks
- [ ] Add dedicated documentation section
- [ ] Add examples
- [ ] Clarify unsupported custom weight keys
- [ ] Explain rationale


---

## Document Thread-Safety Guarantees
**Priority:** Medium  
**Type:** Documentation

#### Goals
Clarify:
- read-only safety
- mutation safety
- staged flow constraints
- cache behavior

#### Tasks
- [ ] Add dedicated thread-safety document
- [ ] Add examples of unsafe patterns
- [ ] Add examples of safe synchronization


---

# Performance and Benchmarks

## Native vs WASM Benchmark Suite
**Priority:** Low  
**Type:** Performance / WASM

#### Goals
Measure:
- graph creation overhead
- traversal overhead
- shortest-path overhead
- serialization overhead

#### Tasks
- [ ] Define benchmark graphs
- [ ] Define reproducible benchmark harness
- [ ] Add CI benchmark mode
- [ ] Publish benchmark results


---

# API Cleanup

## Deprecated Alias Policy
**Priority:** Medium  
**Type:** API Cleanup

#### Problem
Deprecated compatibility wrappers increase long-term maintenance burden.

#### Tasks
- [ ] Define deprecation lifecycle
- [ ] Define removal timing
- [ ] Audit deprecated APIs
- [ ] Avoid adding new compatibility aliases unnecessarily

#### Suggested Policy
```text
Deprecated APIs remain supported until the next major release.
```


---

# Long-Term Ideas

## Lazy Traversal Views
**Priority:** Low  
**Type:** API / Performance

#### Goals
Introduce lazy traversal ranges/views.

#### Possible APIs
```cpp
auto view = G.bfs_edges_view(start);
```

#### Notes
Should integrate naturally with ranges pipelines where possible.


---

## Browser Runtime Exploration
**Priority:** Low  
**Type:** WASM

#### Problem
Current WASM target is Node.js only.

#### Tasks
- [ ] Audit Emscripten browser constraints
- [ ] Evaluate bundle size
- [ ] Evaluate async module loading
- [ ] Evaluate worker compatibility

#### Notes
Do not prioritize before Node runtime stabilization.
