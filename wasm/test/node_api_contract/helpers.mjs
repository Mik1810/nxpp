import assert from "node:assert/strict";
import nxpp from "@mik1810/nxpp-wasm";

export const expectedSimpleMethods = [
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
    "hasNodeAttr",
    "getNodeAttr",
    "tryGetNodeAttr",
    "setNodeAttr",
    "hasEdgeAttr",
    "getEdgeAttr",
    "tryGetEdgeAttr",
    "setEdgeAttr",
    "getEdgeNumericAttr",
    "bfsEdges",
    "bfsTree",
    "bfsSuccessors",
    "dfsEdges",
    "dfsTree",
    "dfsPredecessors",
    "dfsSuccessors",
    "shortestPath",
    "shortestPathWeighted",
    "shortestPathLength",
    "shortestPathLengthWeighted",
    "dijkstraPath",
    "dijkstraPathWeighted",
    "dijkstraShortestPaths",
    "dijkstraPathLengths",
    "dijkstraPathLength",
    "dijkstraPathLengthWeighted",
    "bellmanFordPath",
    "bellmanFordPathWeighted",
    "bellmanFordShortestPaths",
    "bellmanFordPathLength",
    "bellmanFordPathLengthWeighted",
    "dagShortestPaths",
    "floydWarshallAllPairsShortestPaths",
    "floydWarshallAllPairsShortestPathsMap",
    "kruskalMinimumSpanningTree",
    "primMinimumSpanningTree",
    "clear",
    "dispose",
];

export const expectedMultiMethods = [
    ...expectedSimpleMethods,
    "hasEdgeId",
    "edgeIds",
    "edgeIdsBetween",
    "getEdgeEndpoints",
    "getEdgeWeightById",
    "setEdgeWeightById",
    "hasEdgeAttrById",
    "getEdgeAttrById",
    "tryGetEdgeAttrById",
    "setEdgeAttrById",
    "getEdgeNumericAttrById",
    "removeEdgeById",
];

export function assertMethods(target, methods, graphName) {
    for (const methodName of methods) {
        assert.equal(
            typeof target[methodName],
            "function",
            `Expected method ${methodName} to exist on ${graphName}`,
        );
    }
}

export function assertThrows(fn, message) {
    let threw = false;
    try {
        fn();
    } catch {
        threw = true;
    }
    assert.equal(threw, true, message);
}

export function assertThrowsMessage(fn, expectedMessage, message) {
    let thrown;
    try {
        fn();
    } catch (error) {
        thrown = error;
    }

    assert.notEqual(thrown, undefined, message);
    assert.equal(thrown.message, expectedMessage, message);
}

export function assertThrowsMessageIncludes(fn, expectedText, message) {
    let thrown;
    try {
        fn();
    } catch (error) {
        thrown = error;
    }

    assert.notEqual(thrown, undefined, message);
    assert.equal(
        thrown.message.includes(expectedText),
        true,
        `${message}: expected "${thrown.message}" to include "${expectedText}"`,
    );
}

export function toSortedNumbers(values) {
    return Array.from(values).sort((a, b) => a - b);
}

export function toSortedStrings(values) {
    return Array.from(values).sort();
}

export { assert, nxpp };
