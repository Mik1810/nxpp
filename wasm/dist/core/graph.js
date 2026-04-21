import { runtime } from "../load.js";
import { assertAttributeValue, assertFiniteNumber, assertIntNodeId, assertStringNodeId, } from "../internal/assert.js";
import { toArray } from "../internal/wrap.js";
import { toAllPairsShortestPathMap, toAllPairsShortestPathMatrix, toSingleSourceShortestPathResult, } from "../algorithms/shortest_paths.js";
class BaseSimpleGraph {
    raw;
    assertNode;
    constructor(factory, assertNode) {
        this.raw = factory();
        this.assertNode = assertNode;
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
        return toArray(this.raw.neighbors(id));
    }
    removeNode(id) {
        this.assertNode(id, "id");
        this.raw.removeNode(id);
    }
    removeEdge(source, target) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        this.raw.removeEdge(source, target);
    }
    getEdgeWeight(source, target) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        return this.raw.getEdgeWeight(source, target);
    }
    setEdgeWeight(source, target, weight) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        assertFiniteNumber(weight, "weight");
        this.raw.setEdgeWeight(source, target, weight);
    }
    hasNodeAttr(id, key) {
        this.assertNode(id, "id");
        return this.raw.hasNodeAttr(id, key);
    }
    getNodeAttr(id, key) {
        this.assertNode(id, "id");
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
        return this.raw.getEdgeNumericAttr(source, target, key);
    }
    bfsEdges(start) {
        this.assertNode(start, "start");
        return toArray(this.raw.bfsEdges(start));
    }
    bfsTree(start) {
        this.assertNode(start, "start");
        return this.raw.bfsTree(start);
    }
    bfsSuccessors(start) {
        this.assertNode(start, "start");
        return toArray(this.raw.bfsSuccessors(start));
    }
    dfsEdges(start) {
        this.assertNode(start, "start");
        return toArray(this.raw.dfsEdges(start));
    }
    dfsTree(start) {
        this.assertNode(start, "start");
        return this.raw.dfsTree(start);
    }
    dfsPredecessors(start) {
        this.assertNode(start, "start");
        return toArray(this.raw.dfsPredecessors(start));
    }
    dfsSuccessors(start) {
        this.assertNode(start, "start");
        return toArray(this.raw.dfsSuccessors(start));
    }
    shortestPath(source, target) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        return toArray(this.raw.shortestPath(source, target));
    }
    shortestPathWeighted(source, target, weightKey = "weight") {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        return toArray(this.raw.shortestPathWeighted(source, target, weightKey));
    }
    shortestPathLength(source, target) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        return this.raw.shortestPathLength(source, target);
    }
    shortestPathLengthWeighted(source, target, weightKey = "weight") {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        return this.raw.shortestPathLengthWeighted(source, target, weightKey);
    }
    dijkstraPath(source, target) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        return toArray(this.raw.dijkstraPath(source, target));
    }
    dijkstraPathWeighted(source, target, weightKey = "weight") {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        return toArray(this.raw.dijkstraPathWeighted(source, target, weightKey));
    }
    dijkstraShortestPaths(source) {
        this.assertNode(source, "source");
        return toSingleSourceShortestPathResult(this.raw.dijkstraShortestPaths(source));
    }
    dijkstraPathLengths(source) {
        this.assertNode(source, "source");
        return toArray(this.raw.dijkstraPathLengths(source));
    }
    dijkstraPathLength(source, target) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        return this.raw.dijkstraPathLength(source, target);
    }
    dijkstraPathLengthWeighted(source, target, weightKey = "weight") {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        return this.raw.dijkstraPathLengthWeighted(source, target, weightKey);
    }
    bellmanFordPath(source, target) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        return toArray(this.raw.bellmanFordPath(source, target));
    }
    bellmanFordPathWeighted(source, target, weightKey = "weight") {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        return toArray(this.raw.bellmanFordPathWeighted(source, target, weightKey));
    }
    bellmanFordShortestPaths(source) {
        this.assertNode(source, "source");
        return toSingleSourceShortestPathResult(this.raw.bellmanFordShortestPaths(source));
    }
    bellmanFordPathLength(source, target) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        return this.raw.bellmanFordPathLength(source, target);
    }
    bellmanFordPathLengthWeighted(source, target, weightKey = "weight") {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        return this.raw.bellmanFordPathLengthWeighted(source, target, weightKey);
    }
    dagShortestPaths(source) {
        this.assertNode(source, "source");
        return toSingleSourceShortestPathResult(this.raw.dagShortestPaths(source));
    }
    floydWarshallAllPairsShortestPaths() {
        return toAllPairsShortestPathMatrix(this.raw.floydWarshallAllPairsShortestPaths());
    }
    floydWarshallAllPairsShortestPathsMap() {
        return toAllPairsShortestPathMap(this.raw.floydWarshallAllPairsShortestPathsMap());
    }
    clear() {
        this.raw.clear();
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
