import nxpp from "@mik1810/nxpp-wasm";
import assert from "node:assert/strict";

assert.equal(typeof nxpp.DiGraphInt, "function", "DiGraphInt export is missing");
assert.equal(typeof nxpp.DiGraphStr, "function", "DiGraphStr export is missing");
assert.equal(typeof nxpp.GraphInt, "function", "GraphInt export is missing");
assert.equal(typeof nxpp.GraphStr, "function", "GraphStr export is missing");
assert.equal(typeof nxpp.MultiGraphInt, "function", "MultiGraphInt export is missing");
assert.equal(typeof nxpp.MultiGraphStr, "function", "MultiGraphStr export is missing");
assert.equal(typeof nxpp.MultiDiGraphInt, "function", "MultiDiGraphInt export is missing");
assert.equal(typeof nxpp.MultiDiGraphStr, "function", "MultiDiGraphStr export is missing");

assert.equal(typeof nxpp.DiGraph, "undefined", "Legacy DiGraph export must not be exposed");
assert.equal(typeof nxpp.MultiDiGraph, "undefined", "Legacy MultiDiGraph export must not be exposed");
assert.equal(typeof nxpp.DijkstraResult, "undefined", "DijkstraResult must not be part of graph-core exports");

const expectedSimpleMethods = [
    "addNode",
    "addEdge",
    "hasNode",
    "hasEdge",
    "nodes",
    "neighbors",
    "removeNode",
    "removeEdge",
    "getEdgeWeight",
    "setEdgeWeight",
    "clear",
];

const expectedMultiMethods = [
    ...expectedSimpleMethods,
    "hasEdgeId",
    "edgeIds",
    "edgeIdsBetween",
    "getEdgeEndpoints",
    "getEdgeWeightById",
    "setEdgeWeightById",
    "removeEdgeById",
];

function assertMethods(target, methods, graphName) {
    for (const methodName of methods) {
        assert.equal(
            typeof target[methodName],
            "function",
            `Expected method ${methodName} to exist on ${graphName}`,
        );
    }
}

const graphInt = new nxpp.GraphInt();
assertMethods(graphInt, expectedSimpleMethods, "GraphInt");
graphInt.addEdge(1, 2, 3);
assert.equal(graphInt.hasEdge(1, 2), true, "GraphInt hasEdge() must support undirected edges");
assert.equal(graphInt.hasEdge(2, 1), true, "GraphInt hasEdge() must support reverse endpoint lookup");

const graphStr = new nxpp.GraphStr();
assertMethods(graphStr, expectedSimpleMethods, "GraphStr");
graphStr.addEdge("U", "V", 2);
assert.equal(graphStr.hasEdge("U", "V"), true, "GraphStr hasEdge() must support undirected edges");

const multiGraphInt = new nxpp.MultiGraphInt();
assertMethods(multiGraphInt, expectedMultiMethods, "MultiGraphInt");
multiGraphInt.addEdge(1, 2, 1);
multiGraphInt.addEdge(1, 2, 2);
assert.equal(Array.from(new Set(Array.from(multiGraphInt.edgeIdsBetween(1, 2)))).length, 2, "MultiGraphInt edgeIdsBetween() must report both parallel edge IDs");

const multiGraphStr = new nxpp.MultiGraphStr();
assertMethods(multiGraphStr, expectedMultiMethods, "MultiGraphStr");
multiGraphStr.addEdge("X", "Y", 1);
multiGraphStr.addEdge("X", "Y", 4);
assert.equal(Array.from(new Set(Array.from(multiGraphStr.edgeIdsBetween("X", "Y")))).length, 2, "MultiGraphStr edgeIdsBetween() must report both parallel edge IDs");

const diInt = new nxpp.DiGraphInt();
assertMethods(diInt, expectedSimpleMethods, "DiGraphInt");

diInt.addNode(1);
diInt.addNode(2);
diInt.addNode(3);
diInt.addEdge(1, 2, 4);
diInt.addEdge(2, 3, 2);

assert.equal(diInt.hasNode(1), true, "DiGraphInt hasNode() must return true for an existing node");
assert.equal(diInt.hasNode(999), false, "DiGraphInt hasNode() must return false for a missing node");
assert.equal(diInt.hasEdge(1, 2), true, "DiGraphInt hasEdge() must return true for an existing edge");
assert.equal(diInt.hasEdge(1, 3), false, "DiGraphInt hasEdge() must return false for a missing edge");
assert.deepEqual(Array.from(diInt.nodes()).sort((a, b) => a - b), [1, 2, 3], "DiGraphInt nodes() must preserve numeric IDs");
assert.deepEqual(Array.from(diInt.neighbors(1)).sort((a, b) => a - b), [2], "DiGraphInt neighbors() must return outgoing adjacency");
assert.equal(diInt.getEdgeWeight(1, 2), 4, "DiGraphInt getEdgeWeight() returned an unexpected value");
diInt.setEdgeWeight(1, 2, 7);
assert.equal(diInt.getEdgeWeight(1, 2), 7, "DiGraphInt setEdgeWeight() must update endpoint-selected weight");

diInt.removeEdge(2, 3);
assert.equal(diInt.hasEdge(2, 3), false, "DiGraphInt removeEdge() must remove selected edge");
diInt.removeNode(2);
assert.equal(diInt.hasNode(2), false, "DiGraphInt removeNode() must remove selected node");

let diIntTypeThrew = false;
try {
    diInt.addNode("x");
} catch {
    diIntTypeThrew = true;
}
assert.equal(diIntTypeThrew, true, "DiGraphInt must reject non-integer node IDs");

diInt.clear();
assert.deepEqual(Array.from(diInt.nodes()), [], "DiGraphInt clear() must remove all nodes");

const diStr = new nxpp.DiGraphStr();
assertMethods(diStr, expectedSimpleMethods, "DiGraphStr");

diStr.addNode("A");
diStr.addNode("B");
diStr.addNode("C");
diStr.addEdge("A", "B", 1.5);
diStr.addEdge("B", "C", 2.5);

assert.equal(diStr.hasNode("A"), true, "DiGraphStr hasNode() must return true for existing nodes");
assert.equal(diStr.hasEdge("A", "B"), true, "DiGraphStr hasEdge() must return true for existing edges");
assert.deepEqual(Array.from(diStr.nodes()).sort(), ["A", "B", "C"], "DiGraphStr nodes() must preserve string IDs");
assert.deepEqual(Array.from(diStr.neighbors("A")).sort(), ["B"], "DiGraphStr neighbors() must preserve string adjacency");
assert.equal(diStr.getEdgeWeight("A", "B"), 1.5, "DiGraphStr getEdgeWeight() returned an unexpected value");
diStr.setEdgeWeight("A", "B", 3.5);
assert.equal(diStr.getEdgeWeight("A", "B"), 3.5, "DiGraphStr setEdgeWeight() must update endpoint-selected weight");

let diStrTypeThrew = false;
try {
    diStr.addNode(1);
} catch {
    diStrTypeThrew = true;
}
assert.equal(diStrTypeThrew, true, "DiGraphStr must reject non-string node IDs");

const multiInt = new nxpp.MultiDiGraphInt();
assertMethods(multiInt, expectedMultiMethods, "MultiDiGraphInt");

multiInt.addEdge(1, 2, 5);
multiInt.addEdge(1, 2, 8);

const multiIntIds = Array.from(multiInt.edgeIdsBetween(1, 2)).sort((a, b) => a - b);
assert.equal(multiIntIds.length, 2, "MultiDiGraphInt edgeIdsBetween() must return all parallel edge IDs");
assert.equal(multiInt.hasEdgeId(multiIntIds[0]), true, "MultiDiGraphInt hasEdgeId() must report existing IDs");

const intEndpoints = multiInt.getEdgeEndpoints(multiIntIds[0]);
assert.equal(typeof intEndpoints.source, "function", "MultiDiGraphInt getEdgeEndpoints() must return endpoint wrapper");
assert.equal(typeof intEndpoints.target, "function", "MultiDiGraphInt getEdgeEndpoints() must return endpoint wrapper");
assert.equal(intEndpoints.source(), 1, "MultiDiGraphInt edge endpoint source must match inserted edge");
assert.equal(intEndpoints.target(), 2, "MultiDiGraphInt edge endpoint target must match inserted edge");

const originalWeightById = multiInt.getEdgeWeightById(multiIntIds[1]);
multiInt.setEdgeWeightById(multiIntIds[1], originalWeightById + 10);
assert.equal(
    multiInt.getEdgeWeightById(multiIntIds[1]),
    originalWeightById + 10,
    "MultiDiGraphInt setEdgeWeightById() must update only selected edge",
);

multiInt.removeEdgeById(multiIntIds[0]);
assert.equal(multiInt.hasEdgeId(multiIntIds[0]), false, "MultiDiGraphInt removeEdgeById() must remove selected edge");
assert.equal(multiInt.hasEdgeId(multiIntIds[1]), true, "MultiDiGraphInt removeEdgeById() must preserve other parallel edges");

const multiStr = new nxpp.MultiDiGraphStr();
assertMethods(multiStr, expectedMultiMethods, "MultiDiGraphStr");

multiStr.addEdge("S", "T", 2.0);
multiStr.addEdge("S", "T", 4.0);

const multiStrIds = Array.from(multiStr.edgeIdsBetween("S", "T")).sort((a, b) => a - b);
assert.equal(multiStrIds.length, 2, "MultiDiGraphStr edgeIdsBetween() must return all parallel edge IDs");
assert.equal(multiStr.hasEdgeId(multiStrIds[0]), true, "MultiDiGraphStr hasEdgeId() must report existing IDs");

const strEndpoints = multiStr.getEdgeEndpoints(multiStrIds[0]);
assert.equal(strEndpoints.source(), "S", "MultiDiGraphStr edge endpoint source must match inserted edge");
assert.equal(strEndpoints.target(), "T", "MultiDiGraphStr edge endpoint target must match inserted edge");

multiStr.setEdgeWeightById(multiStrIds[0], 9.5);
assert.equal(multiStr.getEdgeWeightById(multiStrIds[0]), 9.5, "MultiDiGraphStr setEdgeWeightById() must update selected edge");

let multiStrTypeThrew = false;
try {
    multiStr.addNode(42);
} catch {
    multiStrTypeThrew = true;
}
assert.equal(multiStrTypeThrew, true, "MultiDiGraphStr must reject non-string node IDs");

console.log("[WASM-NODE] contract-tests-ok");
