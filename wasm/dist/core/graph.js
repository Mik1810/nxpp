import { runtime } from "../load.js";
import { assertFiniteNumber, assertIntNodeId, assertStringNodeId, } from "../internal/assert.js";
import { toArray } from "../internal/wrap.js";
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
