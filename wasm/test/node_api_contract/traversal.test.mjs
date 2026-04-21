import {
    assert,
    assertMethods,
    assertThrows,
    expectedMultiMethods,
    expectedSimpleMethods,
    nxpp,
} from "./helpers.mjs";

const digraph = new nxpp.DiGraphInt();
assertMethods(digraph, expectedSimpleMethods, "DiGraphInt");
digraph.addEdge(1, 2, 1);
digraph.addEdge(1, 3, 1);
digraph.addEdge(2, 4, 1);
digraph.addEdge(3, 5, 1);

assert.deepEqual(
    digraph.bfsEdges(1),
    [
        { source: 1, target: 2 },
        { source: 1, target: 3 },
        { source: 2, target: 4 },
        { source: 3, target: 5 },
    ],
    "DiGraphInt bfsEdges() must return ordered BFS tree edges",
);

assert.deepEqual(
    digraph.bfsTree(1),
    {
        nodes: [1, 2, 3, 4, 5],
        edges: [
            { source: 1, target: 2 },
            { source: 1, target: 3 },
            { source: 2, target: 4 },
            { source: 3, target: 5 },
        ],
    },
    "DiGraphInt bfsTree() must materialize the BFS tree as nodes plus edges",
);

assert.deepEqual(
    digraph.bfsSuccessors(1),
    [
        { node: 1, successors: [2, 3] },
        { node: 2, successors: [4] },
        { node: 3, successors: [5] },
    ],
    "DiGraphInt bfsSuccessors() must return sparse BFS successor entries",
);

const chain = new nxpp.DiGraphStr();
assertMethods(chain, expectedSimpleMethods, "DiGraphStr");
chain.addEdge("A", "B", 1);
chain.addEdge("B", "C", 1);
chain.addEdge("C", "D", 1);

assert.deepEqual(
    chain.dfsEdges("A"),
    [
        { source: "A", target: "B" },
        { source: "B", target: "C" },
        { source: "C", target: "D" },
    ],
    "DiGraphStr dfsEdges() must return ordered DFS tree edges",
);

assert.deepEqual(
    chain.dfsTree("A"),
    {
        nodes: ["A", "B", "C", "D"],
        edges: [
            { source: "A", target: "B" },
            { source: "B", target: "C" },
            { source: "C", target: "D" },
        ],
    },
    "DiGraphStr dfsTree() must materialize the DFS tree as nodes plus edges",
);

assert.deepEqual(
    chain.dfsPredecessors("A"),
    [
        { node: "B", predecessor: "A" },
        { node: "C", predecessor: "B" },
        { node: "D", predecessor: "C" },
    ],
    "DiGraphStr dfsPredecessors() must return sparse predecessor entries",
);

assert.deepEqual(
    chain.dfsSuccessors("A"),
    [
        { node: "A", successors: ["B"] },
        { node: "B", successors: ["C"] },
        { node: "C", successors: ["D"] },
    ],
    "DiGraphStr dfsSuccessors() must return sparse successor entries",
);

const undirected = new nxpp.MultiGraphInt();
assertMethods(undirected, expectedMultiMethods, "MultiGraphInt");
undirected.addEdge(1, 2, 1);
undirected.addEdge(2, 3, 1);

assert.deepEqual(
    undirected.bfsEdges(1),
    [
        { source: 1, target: 2 },
        { source: 2, target: 3 },
    ],
    "MultiGraphInt bfsEdges() must work on multigraph backends",
);

const singleton = new nxpp.GraphStr();
assertMethods(singleton, expectedSimpleMethods, "GraphStr");
singleton.addNode("solo");
assert.deepEqual(singleton.bfsEdges("solo"), [], "GraphStr bfsEdges() must be empty on singleton graphs");
assert.deepEqual(singleton.dfsEdges("solo"), [], "GraphStr dfsEdges() must be empty on singleton graphs");
assert.deepEqual(
    singleton.bfsTree("solo"),
    { nodes: ["solo"], edges: [] },
    "GraphStr bfsTree() must preserve singleton roots",
);

assertThrows(() => singleton.bfsEdges("missing"), "bfsEdges() must throw on missing start nodes");
assertThrows(() => singleton.dfsEdges("missing"), "dfsEdges() must throw on missing start nodes");
