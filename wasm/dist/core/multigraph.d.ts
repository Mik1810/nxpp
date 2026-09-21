import type { AttributeValue, ConnectedComponents, MultiDiGraph, MultiGraph, NodeId, StronglyConnectedComponents } from "../types.js";
import type { RawMultiGraph } from "../internal/wasm_types.js";
import { BaseGraph } from "./graph.js";
declare abstract class BaseMultiGraph<T extends NodeId> extends BaseGraph<T, RawMultiGraph<T>> {
    constructor(factory: (() => RawMultiGraph<T>) | RawMultiGraph<T>, assertNode: (value: unknown, label: string) => asserts value is T);
    private requireEdgeIdExists;
    hasEdgeId(edgeId: number): boolean;
    edgeIds(): number[];
    edgeIdsBetween(source: T, target: T): number[];
    getEdgeEndpoints(edgeId: number): import("../types.js").EdgeEndpoints<T>;
    getEdgeWeightById(edgeId: number): number;
    setEdgeWeightById(edgeId: number, weight: number): void;
    hasEdgeAttrById(edgeId: number, key: string): boolean;
    getEdgeAttrById(edgeId: number, key: string): AttributeValue;
    tryGetEdgeAttrById(edgeId: number, key: string): AttributeValue | null;
    setEdgeAttrById(edgeId: number, key: string, value: AttributeValue): void;
    getEdgeNumericAttrById(edgeId: number, key: string): number;
    removeEdgeById(edgeId: number): void;
}
export declare class MultiGraphInt extends BaseMultiGraph<number> implements MultiGraph<number>, ConnectedComponents<number> {
    constructor(raw?: RawMultiGraph<number>);
    protected createFromRaw(raw: RawMultiGraph<number>): this;
    connectedComponents(): number[][];
}
export declare class MultiGraphStr extends BaseMultiGraph<string> implements MultiGraph<string>, ConnectedComponents<string> {
    constructor(raw?: RawMultiGraph<string>);
    protected createFromRaw(raw: RawMultiGraph<string>): this;
    connectedComponents(): string[][];
}
export declare class MultiDiGraphInt extends BaseMultiGraph<number> implements MultiDiGraph<number>, StronglyConnectedComponents<number> {
    constructor(raw?: RawMultiGraph<number>);
    protected createFromRaw(raw: RawMultiGraph<number>): this;
    stronglyConnectedComponents(): number[][];
}
export declare class MultiDiGraphStr extends BaseMultiGraph<string> implements MultiDiGraph<string>, StronglyConnectedComponents<string> {
    constructor(raw?: RawMultiGraph<string>);
    protected createFromRaw(raw: RawMultiGraph<string>): this;
    stronglyConnectedComponents(): string[][];
}
export {};
