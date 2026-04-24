import {
    assert,
    assertMethods,
    assertThrows,
    assertThrowsMessage,
    expectedMultiMethods,
    expectedSimpleMethods,
    nxpp,
} from "./helpers.mjs";

const graph = new nxpp.GraphInt();
assertMethods(graph, expectedSimpleMethods, "GraphInt");
graph.addEdge(1, 2, 1);
graph.dispose();
graph.dispose();

assertThrowsMessage(
    () => graph.nodes(),
    "WASM graph operation failed: graph has been disposed.",
    "GraphInt operations after dispose() must throw a clear error",
);

const multigraph = new nxpp.MultiGraphStr();
assertMethods(multigraph, expectedMultiMethods, "MultiGraphStr");
multigraph.addEdge("a", "b", 1);
multigraph.dispose();
multigraph.dispose();

assertThrowsMessage(
    () => multigraph.edgeIds(),
    "WASM graph operation failed: graph has been disposed.",
    "MultiGraphStr operations after dispose() must throw a clear error",
);

if (Symbol.dispose !== undefined) {
    const disposable = new nxpp.DiGraphInt();
    assert.equal(
        typeof disposable[Symbol.dispose],
        "function",
        "graph facade classes must expose Symbol.dispose when the runtime supports it",
    );
    disposable[Symbol.dispose]();
    assertThrowsMessage(
        () => disposable.nodes(),
        "WASM graph operation failed: graph has been disposed.",
        "Symbol.dispose must release the underlying graph",
    );
}
