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
graph.addEdge(1, 2, 4);
graph.addEdge(1, 3, 1);
graph.addEdge(2, 3, 2);
graph.addEdge(2, 4, 3);
graph.addEdge(3, 4, 5);

assert.deepEqual(
    graph.kruskalMinimumSpanningTree(),
    [
        { source: 1, target: 3 },
        { source: 2, target: 3 },
        { source: 2, target: 4 },
    ],
    "GraphInt kruskalMinimumSpanningTree() must expose the weighted MST edge list",
);
assert.deepEqual(
    graph.primMinimumSpanningTree(1),
    [
        { source: 3, target: 2 },
        { source: 1, target: 3 },
        { source: 2, target: 4 },
    ],
    "GraphInt primMinimumSpanningTree(root) must expose parent-map edges without the root self-parent",
);
assertThrows(
    () => graph.primMinimumSpanningTree(99),
    "primMinimumSpanningTree() must reject a missing root node",
);

const stringGraph = new nxpp.GraphStr();
assertMethods(stringGraph, expectedSimpleMethods, "GraphStr");
stringGraph.addEdge("a", "b", 4);
stringGraph.addEdge("a", "c", 1);
stringGraph.addEdge("b", "c", 2);

assert.deepEqual(
    stringGraph.kruskalMinimumSpanningTree(),
    [
        { source: "a", target: "c" },
        { source: "b", target: "c" },
    ],
    "GraphStr kruskalMinimumSpanningTree() must preserve string node IDs",
);
assert.deepEqual(
    stringGraph.primMinimumSpanningTree("a"),
    [
        { source: "c", target: "b" },
        { source: "a", target: "c" },
    ],
    "GraphStr primMinimumSpanningTree(root) must preserve string node IDs",
);

const multigraph = new nxpp.MultiGraphInt();
assertMethods(multigraph, expectedMultiMethods, "MultiGraphInt");
multigraph.addEdge(1, 2, 10);
multigraph.addEdge(1, 2, 1);
multigraph.addEdge(2, 3, 2);
multigraph.addEdge(1, 3, 5);

assert.deepEqual(
    multigraph.kruskalMinimumSpanningTree(),
    [
        { source: 1, target: 2 },
        { source: 2, target: 3 },
    ],
    "MultiGraphInt kruskalMinimumSpanningTree() must choose the best parallel edge weight",
);

const digraph = new nxpp.DiGraphInt();
assertMethods(digraph, expectedSimpleMethods, "DiGraphInt");
