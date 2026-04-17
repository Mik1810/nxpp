import type { MultiDiGraph, MultiGraph, NodeId } from "../types.js";
import type { RawMultiGraph } from "../internal/wasm_types.js";
declare class BaseMultiGraph<T extends NodeId> {
    protected readonly raw: RawMultiGraph<T>;
    private readonly assertNode;
    constructor(factory: () => RawMultiGraph<T>, assertNode: (value: unknown, label: string) => asserts value is T);
    addNode(id: T): void;
    addEdge(source: T, target: T, weight: number): void;
    hasNode(id: T): boolean;
    hasEdge(source: T, target: T): boolean;
    nodes(): T[];
    neighbors(id: T): T[];
    removeNode(id: T): void;
    removeEdge(source: T, target: T): void;
    getEdgeWeight(source: T, target: T): number;
    setEdgeWeight(source: T, target: T, weight: number): void;
    hasEdgeId(edgeId: number): boolean;
    edgeIds(): number[];
    edgeIdsBetween(source: T, target: T): number[];
    getEdgeEndpoints(edgeId: number): import("../types.js").EdgeEndpoints<T>;
    getEdgeWeightById(edgeId: number): number;
    setEdgeWeightById(edgeId: number, weight: number): void;
    removeEdgeById(edgeId: number): void;
    clear(): void;
}
export declare class MultiGraphInt extends BaseMultiGraph<number> implements MultiGraph<number> {
    constructor();
}
export declare class MultiGraphStr extends BaseMultiGraph<string> implements MultiGraph<string> {
    constructor();
}
export declare class MultiDiGraphInt extends BaseMultiGraph<number> implements MultiDiGraph<number> {
    constructor();
}
export declare class MultiDiGraphStr extends BaseMultiGraph<string> implements MultiDiGraph<string> {
    constructor();
}
export {};
