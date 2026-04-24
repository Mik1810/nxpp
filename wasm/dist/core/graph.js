import { runtime } from "../load.js";
import { assertAttributeValue, assertFiniteNumber, assertIntNodeId, assertStringNodeId, } from "../internal/assert.js";
import { disposedGraphMessage, wrapRawGraph } from "../internal/errors.js";
import { toArray } from "../internal/wrap.js";
import { toAllPairsShortestPathMap, toAllPairsShortestPathMatrix, toSingleSourceShortestPathResult, } from "../algorithms/shortest_paths.js";
const disposeSymbol = Symbol.dispose;
class BaseSimpleGraph {
    rawObject;
    assertNode;
    constructor(factory, assertNode) {
        this.rawObject = wrapRawGraph(factory());
        this.assertNode = assertNode;
        if (disposeSymbol !== undefined) {
            Object.defineProperty(this, disposeSymbol, {
                value: () => this.dispose(),
                configurable: true,
            });
        }
    }
    get raw() {
        if (this.rawObject === null) {
            throw new Error(disposedGraphMessage);
        }
        return this.rawObject;
    }
    operationFailed(message) {
        throw new Error(`WASM graph operation failed: ${message}`);
    }
    requireNodeExists(id) {
        if (!this.raw.hasNode(id)) {
            this.operationFailed("Node lookup failed: node not found.");
        }
    }
    requireEdgeExists(source, target) {
        this.requireNodeExists(source);
        this.requireNodeExists(target);
        if (!this.raw.hasEdge(source, target)) {
            this.operationFailed("Edge lookup failed: edge not found.");
        }
    }
    requireWeightKey(weightKey) {
        if (weightKey !== "weight") {
            this.operationFailed("Weight lookup failed: only the built-in edge weight property named 'weight' is supported.");
        }
    }
    runPathLookup(fn) {
        try {
            return fn();
        }
        catch (error) {
            if (error instanceof Error && error.message === "WASM graph operation failed: unknown runtime error.") {
                this.operationFailed("Shortest-path lookup failed: target node is unreachable.");
            }
            throw error;
        }
    }
    addNode(id) {
        this.assertNode(id, "id");
        this.raw.addNode(id);
    }
    addEdge(source, target, weight) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        assertFiniteNumber(weight, "weight");
        this.raw.addEdge(source, target, weight);
    }
    hasNode(id) {
        this.assertNode(id, "id");
        return this.raw.hasNode(id);
    }
    hasEdge(source, target) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        return this.raw.hasEdge(source, target);
    }
    nodes() {
        return toArray(this.raw.nodes());
    }
    neighbors(id) {
        this.assertNode(id, "id");
        this.requireNodeExists(id);
        return toArray(this.raw.neighbors(id));
    }
    removeNode(id) {
        this.assertNode(id, "id");
        this.requireNodeExists(id);
        this.raw.removeNode(id);
    }
    removeEdge(source, target) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireEdgeExists(source, target);
        this.raw.removeEdge(source, target);
    }
    getEdgeWeight(source, target) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireEdgeExists(source, target);
        return this.raw.getEdgeWeight(source, target);
    }
    setEdgeWeight(source, target, weight) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        assertFiniteNumber(weight, "weight");
        this.requireEdgeExists(source, target);
        this.raw.setEdgeWeight(source, target, weight);
    }
    hasNodeAttr(id, key) {
        this.assertNode(id, "id");
        return this.raw.hasNodeAttr(id, key);
    }
    getNodeAttr(id, key) {
        this.assertNode(id, "id");
        this.requireNodeExists(id);
        if (!this.raw.hasNodeAttr(id, key)) {
            this.operationFailed("Node attribute lookup failed: key not found.");
        }
        return this.raw.getNodeAttr(id, key);
    }
    tryGetNodeAttr(id, key) {
        this.assertNode(id, "id");
        return this.raw.tryGetNodeAttr(id, key);
    }
    setNodeAttr(id, key, value) {
        this.assertNode(id, "id");
        assertAttributeValue(value, "value");
        this.raw.setNodeAttr(id, key, value);
    }
    hasEdgeAttr(source, target, key) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        return this.raw.hasEdgeAttr(source, target, key);
    }
    getEdgeAttr(source, target, key) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireEdgeExists(source, target);
        if (!this.raw.hasEdgeAttr(source, target, key)) {
            this.operationFailed("Edge attribute lookup failed: key not found.");
        }
        return this.raw.getEdgeAttr(source, target, key);
    }
    tryGetEdgeAttr(source, target, key) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        return this.raw.tryGetEdgeAttr(source, target, key);
    }
    setEdgeAttr(source, target, key, value) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        assertAttributeValue(value, "value");
        this.raw.setEdgeAttr(source, target, key, value);
    }
    getEdgeNumericAttr(source, target, key) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireEdgeExists(source, target);
        return this.raw.getEdgeNumericAttr(source, target, key);
    }
    bfsEdges(start) {
        this.assertNode(start, "start");
        this.requireNodeExists(start);
        return toArray(this.raw.bfsEdges(start));
    }
    bfsTree(start) {
        this.assertNode(start, "start");
        this.requireNodeExists(start);
        return this.raw.bfsTree(start);
    }
    bfsSuccessors(start) {
        this.assertNode(start, "start");
        this.requireNodeExists(start);
        return toArray(this.raw.bfsSuccessors(start));
    }
    dfsEdges(start) {
        this.assertNode(start, "start");
        this.requireNodeExists(start);
        return toArray(this.raw.dfsEdges(start));
    }
    dfsTree(start) {
        this.assertNode(start, "start");
        this.requireNodeExists(start);
        return this.raw.dfsTree(start);
    }
    dfsPredecessors(start) {
        this.assertNode(start, "start");
        this.requireNodeExists(start);
        return toArray(this.raw.dfsPredecessors(start));
    }
    dfsSuccessors(start) {
        this.assertNode(start, "start");
        this.requireNodeExists(start);
        return toArray(this.raw.dfsSuccessors(start));
    }
    shortestPath(source, target) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireNodeExists(source);
        this.requireNodeExists(target);
        return this.runPathLookup(() => toArray(this.raw.shortestPath(source, target)));
    }
    shortestPathWeighted(source, target, weightKey = "weight") {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireWeightKey(weightKey);
        this.requireNodeExists(source);
        this.requireNodeExists(target);
        return this.runPathLookup(() => toArray(this.raw.shortestPathWeighted(source, target, weightKey)));
    }
    shortestPathLength(source, target) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireNodeExists(source);
        this.requireNodeExists(target);
        return this.runPathLookup(() => this.raw.shortestPathLength(source, target));
    }
    shortestPathLengthWeighted(source, target, weightKey = "weight") {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireWeightKey(weightKey);
        this.requireNodeExists(source);
        this.requireNodeExists(target);
        return this.runPathLookup(() => this.raw.shortestPathLengthWeighted(source, target, weightKey));
    }
    dijkstraPath(source, target) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireNodeExists(source);
        this.requireNodeExists(target);
        return this.runPathLookup(() => toArray(this.raw.dijkstraPath(source, target)));
    }
    dijkstraPathWeighted(source, target, weightKey = "weight") {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireWeightKey(weightKey);
        this.requireNodeExists(source);
        this.requireNodeExists(target);
        return this.runPathLookup(() => toArray(this.raw.dijkstraPathWeighted(source, target, weightKey)));
    }
    dijkstraShortestPaths(source) {
        this.assertNode(source, "source");
        this.requireNodeExists(source);
        return toSingleSourceShortestPathResult(this.raw.dijkstraShortestPaths(source));
    }
    dijkstraPathLengths(source) {
        this.assertNode(source, "source");
        this.requireNodeExists(source);
        return toArray(this.raw.dijkstraPathLengths(source));
    }
    dijkstraPathLength(source, target) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireNodeExists(source);
        this.requireNodeExists(target);
        return this.runPathLookup(() => this.raw.dijkstraPathLength(source, target));
    }
    dijkstraPathLengthWeighted(source, target, weightKey = "weight") {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireWeightKey(weightKey);
        this.requireNodeExists(source);
        this.requireNodeExists(target);
        return this.runPathLookup(() => this.raw.dijkstraPathLengthWeighted(source, target, weightKey));
    }
    bellmanFordPath(source, target) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireNodeExists(source);
        this.requireNodeExists(target);
        return this.runPathLookup(() => toArray(this.raw.bellmanFordPath(source, target)));
    }
    bellmanFordPathWeighted(source, target, weightKey = "weight") {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireWeightKey(weightKey);
        this.requireNodeExists(source);
        this.requireNodeExists(target);
        return this.runPathLookup(() => toArray(this.raw.bellmanFordPathWeighted(source, target, weightKey)));
    }
    bellmanFordShortestPaths(source) {
        this.assertNode(source, "source");
        this.requireNodeExists(source);
        return toSingleSourceShortestPathResult(this.raw.bellmanFordShortestPaths(source));
    }
    bellmanFordPathLength(source, target) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireNodeExists(source);
        this.requireNodeExists(target);
        return this.runPathLookup(() => this.raw.bellmanFordPathLength(source, target));
    }
    bellmanFordPathLengthWeighted(source, target, weightKey = "weight") {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireWeightKey(weightKey);
        this.requireNodeExists(source);
        this.requireNodeExists(target);
        return this.runPathLookup(() => this.raw.bellmanFordPathLengthWeighted(source, target, weightKey));
    }
    dagShortestPaths(source) {
        this.assertNode(source, "source");
        this.requireNodeExists(source);
        return toSingleSourceShortestPathResult(this.raw.dagShortestPaths(source));
    }
    floydWarshallAllPairsShortestPaths() {
        return toAllPairsShortestPathMatrix(this.raw.floydWarshallAllPairsShortestPaths());
    }
    floydWarshallAllPairsShortestPathsMap() {
        return toAllPairsShortestPathMap(this.raw.floydWarshallAllPairsShortestPathsMap());
    }
    kruskalMinimumSpanningTree() {
        return toArray(this.raw.kruskalMinimumSpanningTree());
    }
    primMinimumSpanningTree(root) {
        this.assertNode(root, "root");
        this.requireNodeExists(root);
        return toArray(this.raw.primMinimumSpanningTree(root));
    }
    clear() {
        this.raw.clear();
    }
    dispose() {
        if (this.rawObject === null) {
            return;
        }
        this.rawObject.delete();
        this.rawObject = null;
    }
}
export class GraphInt extends BaseSimpleGraph {
    constructor() {
        super(() => new runtime.GraphInt(), assertIntNodeId);
    }
}
export class GraphStr extends BaseSimpleGraph {
    constructor() {
        super(() => new runtime.GraphStr(), assertStringNodeId);
    }
}
export class DiGraphInt extends BaseSimpleGraph {
    constructor() {
        super(() => new runtime.DiGraphInt(), assertIntNodeId);
    }
}
export class DiGraphStr extends BaseSimpleGraph {
    constructor() {
        super(() => new runtime.DiGraphStr(), assertStringNodeId);
    }
}
