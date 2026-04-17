import type { DiGraph, Graph, NodeId } from "../types.js";
import type { RawSimpleGraph } from "../internal/wasm_types.js";
declare class BaseSimpleGraph<T extends NodeId> {
    protected readonly raw: RawSimpleGraph<T>;
    private readonly assertNode;
    constructor(factory: () => RawSimpleGraph<T>, assertNode: (value: unknown, label: string) => asserts value is T);
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
    clear(): void;
}
export declare class GraphInt extends BaseSimpleGraph<number> implements Graph<number> {
    constructor();
}
export declare class GraphStr extends BaseSimpleGraph<string> implements Graph<string> {
    constructor();
}
export declare class DiGraphInt extends BaseSimpleGraph<number> implements DiGraph<number> {
    constructor();
}
export declare class DiGraphStr extends BaseSimpleGraph<string> implements DiGraph<string> {
    constructor();
}
export {};
