import { runtime } from "../load.js";
import { assertAttributeValue, assertEdgeId, assertFiniteNumber, assertIntNodeId, assertStringNodeId, } from "../../internal/assert.js";
import { toArray, toEdgeEndpoints } from "../../internal/wrap.js";
import { BaseGraph } from "./graph.js";
class BaseMultiGraph extends BaseGraph {
    constructor(factory, assertNode) {
        super(factory, assertNode);
    }
    requireEdgeIdExists(edgeId) {
        if (!this.raw.hasEdgeId(edgeId)) {
            this.operationFailed("Edge lookup failed: edge not found.");
        }
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
        this.requireEdgeIdExists(edgeId);
        return toEdgeEndpoints(this.raw.getEdgeEndpoints(edgeId));
    }
    getEdgeWeightById(edgeId) {
        assertEdgeId(edgeId);
        this.requireEdgeIdExists(edgeId);
        return this.raw.getEdgeWeightById(edgeId);
    }
    setEdgeWeightById(edgeId, weight) {
        assertEdgeId(edgeId);
        assertFiniteNumber(weight, "weight");
        this.requireEdgeIdExists(edgeId);
        this.raw.setEdgeWeightById(edgeId, weight);
        this.markGraphMutation();
    }
    hasEdgeAttrById(edgeId, key) {
        assertEdgeId(edgeId);
        return this.raw.hasEdgeAttrById(edgeId, key);
    }
    getEdgeAttrById(edgeId, key) {
        assertEdgeId(edgeId);
        this.requireEdgeIdExists(edgeId);
        if (!this.raw.hasEdgeAttrById(edgeId, key)) {
            this.operationFailed("Edge attribute lookup failed: key not found.");
        }
        return this.raw.getEdgeAttrById(edgeId, key);
    }
    tryGetEdgeAttrById(edgeId, key) {
        assertEdgeId(edgeId);
        return this.raw.tryGetEdgeAttrById(edgeId, key);
    }
    setEdgeAttrById(edgeId, key, value) {
        assertEdgeId(edgeId);
        assertAttributeValue(value, "value");
        this.requireEdgeIdExists(edgeId);
        this.raw.setEdgeAttrById(edgeId, key, value);
        this.markGraphMutation();
    }
    getEdgeNumericAttrById(edgeId, key) {
        assertEdgeId(edgeId);
        this.requireEdgeIdExists(edgeId);
        return this.raw.getEdgeNumericAttrById(edgeId, key);
    }
    removeEdgeById(edgeId) {
        assertEdgeId(edgeId);
        this.requireEdgeIdExists(edgeId);
        this.raw.removeEdgeById(edgeId);
        this.markGraphMutation();
    }
}
export class MultiGraphInt extends BaseMultiGraph {
    constructor(raw) {
        super(raw ?? (() => new runtime.MultiGraphInt()), assertIntNodeId);
    }
    createFromRaw(raw) {
        return new MultiGraphInt(raw);
    }
    connectedComponents() {
        return this.readConnectedComponents();
    }
}
export class MultiGraphStr extends BaseMultiGraph {
    constructor(raw) {
        super(raw ?? (() => new runtime.MultiGraphStr()), assertStringNodeId);
    }
    createFromRaw(raw) {
        return new MultiGraphStr(raw);
    }
    connectedComponents() {
        return this.readConnectedComponents();
    }
}
export class MultiDiGraphInt extends BaseMultiGraph {
    constructor(raw) {
        super(raw ?? (() => new runtime.MultiDiGraphInt()), assertIntNodeId);
    }
    createFromRaw(raw) {
        return new MultiDiGraphInt(raw);
    }
    stronglyConnectedComponents() {
        return this.readStronglyConnectedComponents();
    }
}
export class MultiDiGraphStr extends BaseMultiGraph {
    constructor(raw) {
        super(raw ?? (() => new runtime.MultiDiGraphStr()), assertStringNodeId);
    }
    createFromRaw(raw) {
        return new MultiDiGraphStr(raw);
    }
    stronglyConnectedComponents() {
        return this.readStronglyConnectedComponents();
    }
}
