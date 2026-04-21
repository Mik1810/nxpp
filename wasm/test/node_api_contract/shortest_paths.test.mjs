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
digraph.addEdge(1, 2, 5);
digraph.addEdge(1, 3, 1);
digraph.addEdge(3, 2, 1);
digraph.addEdge(2, 4, 1);
digraph.addEdge(3, 4, 10);

assert.deepEqual(
    digraph.shortestPath(1, 4),
    [1, 2, 4],
    "DiGraphInt shortestPath() must use the unweighted path semantics",
);
assert.equal(
    digraph.shortestPathLength(1, 4),
    2,
    "DiGraphInt shortestPathLength() must report the unweighted hop count",
);
assert.deepEqual(
    digraph.shortestPathWeighted(1, 4),
    [1, 3, 2, 4],
    "DiGraphInt shortestPathWeighted() must use the built-in weight channel",
);
assert.equal(
    digraph.shortestPathLengthWeighted(1, 4),
    3,
    "DiGraphInt shortestPathLengthWeighted() must report the weighted distance",
);
assert.deepEqual(
    digraph.dijkstraPath(1, 4),
    [1, 3, 2, 4],
    "DiGraphInt dijkstraPath() must match the weighted shortest path",
);
assert.equal(
    digraph.dijkstraPathLength(1, 4),
    3,
    "DiGraphInt dijkstraPathLength() must report the weighted distance",
);

const dijkstraResult = digraph.dijkstraShortestPaths(1);
assert.equal(dijkstraResult.hasPathTo(4), true, "dijkstraShortestPaths() must mark reachable nodes");
assert.deepEqual(
    dijkstraResult.pathTo(4),
    [1, 3, 2, 4],
    "dijkstraShortestPaths().pathTo() must reconstruct the weighted shortest path",
);
assert.deepEqual(
    dijkstraResult.distance,
    [
        { node: 1, distance: 0 },
        { node: 2, distance: 2 },
        { node: 3, distance: 1 },
        { node: 4, distance: 3 },
    ],
    "dijkstraShortestPaths().distance must expose the single-source distance table",
);
assert.deepEqual(
    dijkstraResult.predecessor,
    [
        { node: 1, predecessor: 1 },
        { node: 2, predecessor: 3 },
        { node: 3, predecessor: 1 },
        { node: 4, predecessor: 2 },
    ],
    "dijkstraShortestPaths().predecessor must expose the predecessor table",
);
assert.deepEqual(
    digraph.dijkstraPathLengths(1),
    dijkstraResult.distance,
    "dijkstraPathLengths() must mirror the distance view of dijkstraShortestPaths()",
);
assertThrows(
    () => digraph.shortestPathWeighted(1, 4, "capacity"),
    "weighted shortest-path wrappers must reject unsupported weight keys",
);
assertThrows(
    () => digraph.dijkstraPathWeighted(1, 4, "capacity"),
    "dijkstra weighted wrappers must reject unsupported weight keys",
);
assertThrows(
    () => digraph.dijkstraPathLengthWeighted(1, 4, "capacity"),
    "weighted dijkstra length wrappers must reject unsupported weight keys",
);

const weightedStrings = new nxpp.DiGraphStr();
assertMethods(weightedStrings, expectedSimpleMethods, "DiGraphStr");
weightedStrings.addEdge("A", "B", 4);
weightedStrings.addEdge("A", "C", 5);
weightedStrings.addEdge("B", "C", -2);
weightedStrings.addEdge("C", "D", 3);

assert.deepEqual(
    weightedStrings.bellmanFordPath("A", "D"),
    ["A", "B", "C", "D"],
    "DiGraphStr bellmanFordPath() must support negative edge weights",
);
assert.equal(
    weightedStrings.bellmanFordPathLength("A", "D"),
    5,
    "DiGraphStr bellmanFordPathLength() must report the Bellman-Ford distance",
);

const bellmanFordResult = weightedStrings.bellmanFordShortestPaths("A");
assert.equal(
    bellmanFordResult.hasPathTo("D"),
    true,
    "bellmanFordShortestPaths() must mark reachable string targets",
);
assert.deepEqual(
    bellmanFordResult.pathTo("D"),
    ["A", "B", "C", "D"],
    "bellmanFordShortestPaths().pathTo() must reconstruct the Bellman-Ford path",
);

const dag = new nxpp.MultiDiGraphInt();
assertMethods(dag, expectedMultiMethods, "MultiDiGraphInt");
dag.addEdge(1, 2, 1);
dag.addEdge(1, 3, 2);
dag.addEdge(2, 4, 3);
dag.addEdge(3, 4, 1);

assert.deepEqual(
    dag.dagShortestPaths(1).pathTo(4),
    [1, 3, 4],
    "MultiDiGraphInt dagShortestPaths() must reconstruct the DAG shortest path",
);

const disconnected = new nxpp.GraphInt();
assertMethods(disconnected, expectedSimpleMethods, "GraphInt");
disconnected.addNode(1);
disconnected.addNode(2);
assertThrows(() => disconnected.shortestPath(1, 2), "shortestPath() must throw when no path exists");
assertThrows(() => disconnected.dijkstraPath(1, 2), "dijkstraPath() must throw when no path exists");

const allPairs = new nxpp.GraphInt();
assertMethods(allPairs, expectedSimpleMethods, "GraphInt");
allPairs.addEdge(1, 2, 4);
allPairs.addEdge(2, 3, 1);
allPairs.addEdge(1, 3, 10);

assert.deepEqual(
    allPairs.floydWarshallAllPairsShortestPaths(),
    [
        [0, 4, 5],
        [4, 0, 1],
        [5, 1, 0],
    ],
    "GraphInt floydWarshallAllPairsShortestPaths() must return the dense weighted matrix in stable node order",
);
assert.deepEqual(
    allPairs.floydWarshallAllPairsShortestPathsMap(),
    [
        {
            source: 1,
            distances: [
                { target: 1, distance: 0 },
                { target: 2, distance: 4 },
                { target: 3, distance: 5 },
            ],
        },
        {
            source: 2,
            distances: [
                { target: 1, distance: 4 },
                { target: 2, distance: 0 },
                { target: 3, distance: 1 },
            ],
        },
        {
            source: 3,
            distances: [
                { target: 1, distance: 5 },
                { target: 2, distance: 1 },
                { target: 3, distance: 0 },
            ],
        },
    ],
    "GraphInt floydWarshallAllPairsShortestPathsMap() must return the nested weighted DTO view",
);

const multiAllPairs = new nxpp.MultiGraphInt();
assertMethods(multiAllPairs, expectedMultiMethods, "MultiGraphInt");
multiAllPairs.addEdge(1, 2, 5);
multiAllPairs.addEdge(1, 2, 2);

assert.deepEqual(
    multiAllPairs.floydWarshallAllPairsShortestPaths(),
    [
        [0, 2],
        [2, 0],
    ],
    "MultiGraphInt floydWarshallAllPairsShortestPaths() must use the best parallel edge weight",
);
