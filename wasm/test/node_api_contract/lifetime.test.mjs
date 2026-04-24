import {
    assert,
    assertMethods,
    assertThrows,
    expectedMultiMethods,
    expectedSimpleMethods,
    nxpp,
} from "./helpers.mjs";

const graph = new nxpp.GraphInt();
assertMethods(graph, expectedSimpleMethods, "GraphInt");
graph.addEdge(1, 2, 1);
graph.dispose();
graph.dispose();

assertThrows(
    () => graph.nodes(),
    "GraphInt operations after dispose() must throw a clear error",
);

const multigraph = new nxpp.MultiGraphStr();
assertMethods(multigraph, expectedMultiMethods, "MultiGraphStr");
multigraph.addEdge("a", "b", 1);
multigraph.dispose();
multigraph.dispose();

assertThrows(
    () => multigraph.edgeIds(),
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
    assertThrows(
        () => disposable.nodes(),
        "Symbol.dispose must release the underlying graph",
    );
}
