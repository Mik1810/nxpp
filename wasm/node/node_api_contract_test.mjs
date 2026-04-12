import assert from "node:assert/strict";
import path from "node:path";
import { fileURLToPath, pathToFileURL } from "node:url";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const rootDir = path.resolve(__dirname, "..", "..");

const modulePath =
    process.env.NXPP_WASM_NODE_MODULE ??
    path.join(rootDir, ".tmp", "wasm-node", "nxpp_node.mjs");

const { default: createNxppModule } = await import(pathToFileURL(modulePath).href);
const nxppModule = await createNxppModule();

assert.equal(typeof nxppModule.DiGraph, "function", "DiGraph export is missing");

const graph = new nxppModule.DiGraph();

const expectedMethods = [
    "addNode",
    "addEdge",
    "hasNode",
    "hasEdge",
    "nodes",
    "dijkstraDistance",
    "dijkstraPath",
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

const distance = graph.dijkstraDistance(1, 3);
assert.equal(typeof distance, "number", "dijkstraDistance() must return a number");
assert.equal(distance, 3, "dijkstraDistance() produced unexpected value");

const pathResult = Array.from(graph.dijkstraPath(1, 3));
assert.deepEqual(pathResult, [1, 2, 3], "dijkstraPath() produced unexpected path");

graph.clear();
assert.deepEqual(Array.from(graph.nodes()), [], "clear() must remove all nodes");

let threw = false;
let thrownValue;
try {
    graph.dijkstraDistance(1, 3);
} catch (error) {
    threw = true;
    thrownValue = error;
}

assert.equal(threw, true, "Invalid dijkstraDistance() query must throw");
assert.notEqual(thrownValue, undefined, "Invalid shortest path query must throw a value");

console.log("[WASM-NODE] contract-tests-ok");
