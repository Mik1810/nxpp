import assert from "node:assert/strict";
import path from "node:path";
import { fileURLToPath, pathToFileURL } from "node:url";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const rootDir = path.resolve(__dirname, "..", "..");

const modulePath =
    process.env.NXPP_WASM_NODE_MODULE ??
    path.join(rootDir, "wasm", "build", "nxpp_node.mjs");

const { default: createNxppModule } = await import(pathToFileURL(modulePath).href);
const nxppModule = await createNxppModule();

assert.equal(typeof nxppModule.DiGraph, "function", "DiGraph export is missing");
assert.equal(typeof nxppModule.DijkstraResult, "function", "DijkstraResult export is missing");

const graph = new nxppModule.DiGraph();

const expectedMethods = [
    "addNode",
    "addEdge",
    "hasNode",
    "hasEdge",
    "nodes",
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

console.log("[WASM-NODE] contract-tests-ok");
