import nxpp from "@mik1810/nxpp-wasm";
import assert from "node:assert/strict";


assert.equal(typeof nxpp.DiGraph, "function", "DiGraph export is missing");
assert.equal(typeof nxpp.DijkstraResult, "function", "DijkstraResult export is missing");
assert.equal(typeof nxpp.EdgeEndpoints, "function", "EdgeEndpoints export is missing");

const graph = new nxpp.DiGraph();

const expectedMethods = [
    "addNode",
    "addEdge",
    "hasNode",
    "hasEdge",
    "hasEdgeId",
    "nodes",
    "edgeIds",
    "edgeIdsBetween",
    "neighbors",
    "removeNode",
    "removeEdge",
    "getEdgeWeight",
    "getEdgeEndpoints",
    "getEdgeWeightById",
    "setEdgeWeightById",
    "dijkstraShortestPaths",
    "clear",
];

for (const methodName of expectedMethods) {
    assert.equal(
        typeof graph[methodName],
        "function",
        `Expected method ${methodName} to exist on DiGraph`
    );
}

graph.addNode(1);
graph.addNode(2);
graph.addNode(3);
graph.addEdge(1, 2, 1);
graph.addEdge(2, 3, 2);

assert.equal(graph.hasNode(1), true, "hasNode must return true for existing node");
assert.equal(graph.hasNode(999), false, "hasNode must return false for missing node");
assert.equal(graph.hasEdge(1, 2), true, "hasEdge must return true for existing edge");
assert.equal(graph.hasEdge(1, 3), false, "hasEdge must return false for missing edge");

const nodes = Array.from(graph.nodes()).sort((a, b) => a - b);
assert.deepEqual(nodes, [1, 2, 3], "nodes() must return numeric node IDs");

const neighbors1 = Array.from(graph.neighbors(1)).sort((a, b) => a - b);
assert.deepEqual(neighbors1, [2], "neighbors() must return outgoing neighbors");

assert.equal(graph.getEdgeWeight(1, 2), 1, "getEdgeWeight() produced unexpected value");

const edgeIds = Array.from(graph.edgeIds()).sort((a, b) => a - b);
assert.equal(edgeIds.length, 2, "edgeIds() must list both current edges");
assert.equal(graph.hasEdgeId(edgeIds[0]), true, "hasEdgeId() must return true for existing edge");
assert.equal(graph.hasEdgeId(999999), false, "hasEdgeId() must return false for missing edge");

const edgeIdsBetween12 = Array.from(graph.edgeIdsBetween(1, 2));
assert.equal(edgeIdsBetween12.length, 1, "edgeIdsBetween() must return matching edge ids");

const endpoints12 = graph.getEdgeEndpoints(edgeIdsBetween12[0]);
assert.equal(typeof endpoints12.source, "function", "EdgeEndpoints.source() must exist");
assert.equal(typeof endpoints12.target, "function", "EdgeEndpoints.target() must exist");
assert.equal(endpoints12.source(), 1, "getEdgeEndpoints() produced unexpected source");
assert.equal(endpoints12.target(), 2, "getEdgeEndpoints() produced unexpected target");

assert.equal(typeof graph.dijkstraDistance, "undefined", "dijkstraDistance alias must not be exposed");
assert.equal(typeof graph.dijkstraPath, "undefined", "dijkstraPath alias must not be exposed");

const shortestPaths = graph.dijkstraShortestPaths(1);
assert.equal(typeof shortestPaths.hasPathTo, "function", "hasPathTo() must exist on DijkstraResult");
assert.equal(typeof shortestPaths.distanceTo, "function", "distanceTo() must exist on DijkstraResult");
assert.equal(typeof shortestPaths.pathTo, "function", "pathTo() must exist on DijkstraResult");
assert.equal(typeof shortestPaths.reachableNodes, "function", "reachableNodes() must exist on DijkstraResult");

assert.equal(shortestPaths.hasPathTo(3), true, "hasPathTo() must return true for reachable target");
assert.equal(shortestPaths.distanceTo(3), 3, "distanceTo() produced unexpected value");
assert.deepEqual(Array.from(shortestPaths.pathTo(3)), [1, 2, 3], "pathTo() produced unexpected path");

const reachable = Array.from(shortestPaths.reachableNodes()).sort((a, b) => a - b);
assert.deepEqual(reachable, [1, 2, 3], "reachableNodes() must list known nodes");

assert.equal(graph.getEdgeWeightById(edgeIdsBetween12[0]), 1, "getEdgeWeightById() produced unexpected value");
graph.setEdgeWeightById(edgeIdsBetween12[0], 7);
assert.equal(graph.getEdgeWeightById(edgeIdsBetween12[0]), 7, "setEdgeWeightById() must update weight");
assert.equal(graph.getEdgeWeight(1, 2), 7, "endpoint-based getEdgeWeight() must see updated precise weight");

graph.removeEdge(2, 3);
assert.equal(graph.hasEdge(2, 3), false, "removeEdge() must remove the selected edge");

graph.removeNode(2);
assert.equal(graph.hasNode(2), false, "removeNode() must remove the selected node");
assert.deepEqual(Array.from(graph.nodes()).sort((a, b) => a - b), [1, 3], "removeNode() must update node list");

let neighborsMissingThrew = false;
try {
    graph.neighbors(999);
} catch (error) {
    neighborsMissingThrew = true;
}
assert.equal(neighborsMissingThrew, true, "neighbors() on a missing node must throw");

let removeNodeMissingThrew = false;
try {
    graph.removeNode(999);
} catch (error) {
    removeNodeMissingThrew = true;
}
assert.equal(removeNodeMissingThrew, true, "removeNode() on a missing node must throw");

let removeEdgeMissingThrew = false;
try {
    graph.removeEdge(1, 2);
} catch (error) {
    removeEdgeMissingThrew = true;
}
assert.equal(removeEdgeMissingThrew, true, "removeEdge() on a missing edge must throw");

let getWeightMissingThrew = false;
try {
    graph.getEdgeWeight(1, 2);
} catch (error) {
    getWeightMissingThrew = true;
}
assert.equal(getWeightMissingThrew, true, "getEdgeWeight() on a missing edge must throw");

let getWeightByIdMissingThrew = false;
try {
    graph.getEdgeWeightById(999999);
} catch (error) {
    getWeightByIdMissingThrew = true;
}
assert.equal(getWeightByIdMissingThrew, true, "getEdgeWeightById() on a missing edge must throw");

let setWeightByIdMissingThrew = false;
try {
    graph.setEdgeWeightById(999999, 5);
} catch (error) {
    setWeightByIdMissingThrew = true;
}
assert.equal(setWeightByIdMissingThrew, true, "setEdgeWeightById() on a missing edge must throw");

let getEndpointsMissingThrew = false;
try {
    graph.getEdgeEndpoints(999999);
} catch (error) {
    getEndpointsMissingThrew = true;
}
assert.equal(getEndpointsMissingThrew, true, "getEdgeEndpoints() on a missing edge must throw");

graph.clear();
assert.deepEqual(Array.from(graph.nodes()), [], "clear() must remove all nodes");

let threw = false;
let thrownValue;
try {
    graph.dijkstraShortestPaths(1);
} catch (error) {
    threw = true;
    thrownValue = error;
}

assert.equal(threw, true, "Invalid dijkstraShortestPaths() query must throw");
assert.notEqual(thrownValue, undefined, "Invalid shortest path query must throw a value");

const stringGraph = new nxpp.DiGraph();
stringGraph.addNode("A");
stringGraph.addNode("B");
stringGraph.addNode("C");
stringGraph.addEdge("A", "B", 1.5);
stringGraph.addEdge("B", "C", 2.0);

assert.equal(stringGraph.hasNode("A"), true, "string-backed hasNode() must return true for existing node");
assert.equal(stringGraph.hasEdge("A", "B"), true, "string-backed hasEdge() must return true for existing edge");

const stringNodes = Array.from(stringGraph.nodes()).sort();
assert.deepEqual(stringNodes, ["A", "B", "C"], "nodes() must preserve string node IDs");

const stringNeighbors = Array.from(stringGraph.neighbors("A")).sort();
assert.deepEqual(stringNeighbors, ["B"], "neighbors() must support string node IDs");

assert.equal(stringGraph.getEdgeWeight("A", "B"), 1.5, "getEdgeWeight() must support string-backed graphs");

const stringEdgeIds = Array.from(stringGraph.edgeIds());
assert.equal(stringEdgeIds.length, 2, "edgeIds() must work for string-backed graphs");

const stringShortestPaths = stringGraph.dijkstraShortestPaths("A");
assert.equal(stringShortestPaths.hasPathTo("C"), true, "hasPathTo() must support string-backed graphs");
assert.equal(stringShortestPaths.distanceTo("C"), 3.5, "distanceTo() must support string-backed graphs");
assert.deepEqual(Array.from(stringShortestPaths.pathTo("C")), ["A", "B", "C"], "pathTo() must preserve string node IDs");
assert.deepEqual(Array.from(stringShortestPaths.reachableNodes()).sort(), ["A", "B", "C"], "reachableNodes() must preserve string node IDs");

let mixedIdThrew = false;
try {
    stringGraph.addNode(123);
} catch (error) {
    mixedIdThrew = true;
}
assert.equal(mixedIdThrew, true, "mixing numeric and string node IDs in the same graph must throw");

const numericGraph = new nxpp.DiGraph();
numericGraph.addNode(1);

let mixedEdgeIdThrew = false;
try {
    numericGraph.addEdge(1, "B", 1);
} catch (error) {
    mixedEdgeIdThrew = true;
}
assert.equal(mixedEdgeIdThrew, true, "addEdge() must reject mixed endpoint node ID types");

console.log("[WASM-NODE] contract-tests-ok");
