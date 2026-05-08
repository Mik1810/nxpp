import { runtime } from "../load.js";
import { assertAttributeValue, assertFiniteNumber, assertIntNodeId, assertStringValue, assertStringNodeId, } from "../internal/assert.js";
import { disposedGraphMessage, wrapRawGraph } from "../internal/errors.js";
import { toArray } from "../internal/wrap.js";
import { toCentralityScores } from "../algorithms/centrality.js";
import { toComponentGroups } from "../algorithms/components.js";
import { toAllPairsShortestPathMap, toAllPairsShortestPathMatrix, toSingleSourceShortestPathResult, } from "../algorithms/shortest_paths.js";
const disposeSymbol = Symbol.dispose;
function connectedComponents(raw) {
    return toComponentGroups(raw.connectedComponents());
}
function stronglyConnectedComponents(raw) {
    return toComponentGroups(raw.stronglyConnectedComponents());
}
class BaseSimpleGraph {
    rawObject;
    assertNode;
    mutationVersion = 0;
    stagedFlowMutationVersion = null;
    constructor(factory, assertNode) {
        this.rawObject = wrapRawGraph(typeof factory === "function" ? factory() : factory);
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
    markGraphMutation() {
        this.mutationVersion += 1;
    }
    markStagedFlow() {
        this.stagedFlowMutationVersion = this.mutationVersion;
    }
    requireStagedFlow() {
        if (this.stagedFlowMutationVersion === null) {
            this.operationFailed("Min-cost-flow state unavailable: run push_relabel_maximum_flow(...) first.");
        }
        if (this.stagedFlowMutationVersion !== this.mutationVersion) {
            this.operationFailed("Min-cost-flow state invalidated by graph mutation: rerun push_relabel_maximum_flow(...) before cycle_canceling().");
        }
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
    requireAttributeKey(key, label) {
        assertStringValue(key, label);
        if (key.length === 0) {
            throw new TypeError(`${label} must not be empty.`);
        }
    }
    requirePagerankMaxIterations(maxIterations) {
        if (!Number.isInteger(maxIterations) || maxIterations < 0) {
            throw new TypeError("maxIterations must be a non-negative integer.");
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
        this.markGraphMutation();
    }
    addEdge(source, target, weight) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        assertFiniteNumber(weight, "weight");
        this.raw.addEdge(source, target, weight);
        this.markGraphMutation();
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
        this.markGraphMutation();
    }
    removeEdge(source, target) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireEdgeExists(source, target);
        this.raw.removeEdge(source, target);
        this.markGraphMutation();
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
        this.markGraphMutation();
    }
    subgraph(nodes) {
        if (!Array.isArray(nodes)) {
            this.operationFailed("subgraph nodes must be an array.");
        }
        for (const [index, node] of nodes.entries()) {
            this.assertNode(node, `nodes[${index}]`);
            this.requireNodeExists(node);
        }
        return this.createFromRaw(this.raw.subgraph(nodes));
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
        this.markGraphMutation();
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
        this.markGraphMutation();
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
    degreeCentrality() {
        return toCentralityScores(this.raw.degreeCentrality());
    }
    pagerank(tolerance = 1e-6, maxIterations = 100) {
        assertFiniteNumber(tolerance, "tolerance");
        this.requirePagerankMaxIterations(maxIterations);
        return toCentralityScores(this.raw.pagerank(tolerance, maxIterations));
    }
    betweennessCentrality() {
        return toCentralityScores(this.raw.betweennessCentrality());
    }
    maximumFlow(source, target, capacityKey = "capacity") {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireAttributeKey(capacityKey, "capacityKey");
        this.requireNodeExists(source);
        this.requireNodeExists(target);
        return this.raw.maximumFlow(source, target, capacityKey);
    }
    minimumCut(source, target, capacityKey = "capacity") {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireAttributeKey(capacityKey, "capacityKey");
        this.requireNodeExists(source);
        this.requireNodeExists(target);
        return this.raw.minimumCut(source, target, capacityKey);
    }
    maxFlowMinCost(source, target, capacityKey = "capacity", weightKey = "weight") {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireAttributeKey(capacityKey, "capacityKey");
        this.requireAttributeKey(weightKey, "weightKey");
        this.requireNodeExists(source);
        this.requireNodeExists(target);
        return this.raw.maxFlowMinCost(source, target, capacityKey, weightKey);
    }
    maxFlowMinCostSuccessiveShortestPath(source, target, capacityKey = "capacity", weightKey = "weight") {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireAttributeKey(capacityKey, "capacityKey");
        this.requireAttributeKey(weightKey, "weightKey");
        this.requireNodeExists(source);
        this.requireNodeExists(target);
        return this.raw.maxFlowMinCostSuccessiveShortestPath(source, target, capacityKey, weightKey);
    }
    pushRelabelMaximumFlow(source, target, capacityKey = "capacity", weightKey = "weight") {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.requireAttributeKey(capacityKey, "capacityKey");
        this.requireAttributeKey(weightKey, "weightKey");
        this.requireNodeExists(source);
        this.requireNodeExists(target);
        const flow = this.raw.pushRelabelMaximumFlow(source, target, capacityKey, weightKey);
        this.markStagedFlow();
        return flow;
    }
    cycleCanceling(weightKey = "weight") {
        this.requireAttributeKey(weightKey, "weightKey");
        this.requireStagedFlow();
        return this.raw.cycleCanceling(weightKey);
    }
    clear() {
        this.raw.clear();
        this.markGraphMutation();
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
    constructor(raw) {
        super(raw ?? (() => new runtime.GraphInt()), assertIntNodeId);
    }
    createFromRaw(raw) {
        return new GraphInt(raw);
    }
    connectedComponents() {
        return connectedComponents(this.raw);
    }
}
export class GraphStr extends BaseSimpleGraph {
    constructor(raw) {
        super(raw ?? (() => new runtime.GraphStr()), assertStringNodeId);
    }
    createFromRaw(raw) {
        return new GraphStr(raw);
    }
    connectedComponents() {
        return connectedComponents(this.raw);
    }
}
export class DiGraphInt extends BaseSimpleGraph {
    constructor(raw) {
        super(raw ?? (() => new runtime.DiGraphInt()), assertIntNodeId);
    }
    createFromRaw(raw) {
        return new DiGraphInt(raw);
    }
    stronglyConnectedComponents() {
        return stronglyConnectedComponents(this.raw);
    }
}
export class DiGraphStr extends BaseSimpleGraph {
    constructor(raw) {
        super(raw ?? (() => new runtime.DiGraphStr()), assertStringNodeId);
    }
    createFromRaw(raw) {
        return new DiGraphStr(raw);
    }
    stronglyConnectedComponents() {
        return stronglyConnectedComponents(this.raw);
    }
}
