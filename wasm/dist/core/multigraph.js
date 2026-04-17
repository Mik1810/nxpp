import { runtime } from "../load.js";
import { assertEdgeId, assertFiniteNumber, assertIntNodeId, assertStringNodeId, } from "../internal/assert.js";
import { toArray, toEdgeEndpoints } from "../internal/wrap.js";
class BaseMultiGraph {
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
    hasEdgeId(edgeId) {
        assertEdgeId(edgeId);
        return this.raw.hasEdgeId(edgeId);
    }
    edgeIds() {
        return toArray(this.raw.edgeIds());
    }
    edgeIdsBetween(source, target) {
        this.assertNode(source, "source");
        this.assertNode(target, "target");
        return toArray(this.raw.edgeIdsBetween(source, target));
    }
    getEdgeEndpoints(edgeId) {
        assertEdgeId(edgeId);
        return toEdgeEndpoints(this.raw.getEdgeEndpoints(edgeId));
    }
    getEdgeWeightById(edgeId) {
        assertEdgeId(edgeId);
        return this.raw.getEdgeWeightById(edgeId);
    }
    setEdgeWeightById(edgeId, weight) {
        assertEdgeId(edgeId);
        assertFiniteNumber(weight, "weight");
        this.raw.setEdgeWeightById(edgeId, weight);
    }
    removeEdgeById(edgeId) {
        assertEdgeId(edgeId);
        this.raw.removeEdgeById(edgeId);
    }
    clear() {
        this.raw.clear();
    }
}
export class MultiGraphInt extends BaseMultiGraph {
    constructor() {
        super(() => new runtime.MultiGraphInt(), assertIntNodeId);
    }
}
export class MultiGraphStr extends BaseMultiGraph {
    constructor() {
        super(() => new runtime.MultiGraphStr(), assertStringNodeId);
    }
}
export class MultiDiGraphInt extends BaseMultiGraph {
    constructor() {
        super(() => new runtime.MultiDiGraphInt(), assertIntNodeId);
    }
}
export class MultiDiGraphStr extends BaseMultiGraph {
    constructor() {
        super(() => new runtime.MultiDiGraphStr(), assertStringNodeId);
    }
}
