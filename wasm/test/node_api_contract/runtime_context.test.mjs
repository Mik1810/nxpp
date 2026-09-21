import assert from "node:assert/strict";

import { createNxpp, createNxppRuntime } from "../../dist/runtime/index.js";

assert.throws(
    () => createNxppRuntime({}),
    /nxpp wasm runtime initialization failed: missing required class GraphInt\./,
);

const [first, second] = await Promise.all([
    createNxpp({ print() {} }),
    createNxpp(),
]);

assert.notEqual(first.GraphInt, second.GraphInt, "contexts must own distinct facade classes");

const graphCases = [
    [first.GraphInt, 1, 2],
    [first.GraphStr, "a", "b"],
    [first.DiGraphInt, 1, 2],
    [first.DiGraphStr, "a", "b"],
    [first.MultiGraphInt, 1, 2],
    [first.MultiGraphStr, "a", "b"],
    [first.MultiDiGraphInt, 1, 2],
    [first.MultiDiGraphStr, "a", "b"],
];

for (const [GraphClass, source, target] of graphCases) {
    const graph = new GraphClass();
    graph.addEdge(source, target, 1);
    assert.equal(graph.hasEdge(source, target), true);
    graph.dispose();
}

const firstGraph = new first.DiGraphInt();
const secondGraph = new second.DiGraphInt();
firstGraph.addEdge(1, 2, 1);
secondGraph.addEdge(10, 20, 1);
firstGraph.dispose();

assert.deepEqual(secondGraph.nodes(), [10, 20], "disposing one context must not affect another");

const secondSubgraph = secondGraph.subgraph([10, 20]);
assert.equal(secondSubgraph instanceof second.DiGraphInt, true);
assert.equal(secondSubgraph instanceof first.DiGraphInt, false);

secondSubgraph.dispose();
secondGraph.dispose();
