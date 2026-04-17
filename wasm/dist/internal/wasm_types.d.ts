import type { NodeId } from "../types.js";
export interface RawEdgeEndpoints<T extends NodeId> {
    source(): T;
    target(): T;
}
export interface RawSimpleGraph<T extends NodeId> {
    addNode(id: T): void;
    addEdge(source: T, target: T, weight: number): void;
    hasNode(id: T): boolean;
    hasEdge(source: T, target: T): boolean;
    nodes(): Iterable<T> | ArrayLike<T>;
    neighbors(id: T): Iterable<T> | ArrayLike<T>;
    removeNode(id: T): void;
    removeEdge(source: T, target: T): void;
    getEdgeWeight(source: T, target: T): number;
    setEdgeWeight(source: T, target: T, weight: number): void;
    clear(): void;
}
export interface RawMultiGraph<T extends NodeId> extends RawSimpleGraph<T> {
    hasEdgeId(edgeId: number): boolean;
    edgeIds(): Iterable<number> | ArrayLike<number>;
    edgeIdsBetween(source: T, target: T): Iterable<number> | ArrayLike<number>;
    getEdgeEndpoints(edgeId: number): RawEdgeEndpoints<T>;
    getEdgeWeightById(edgeId: number): number;
    setEdgeWeightById(edgeId: number, weight: number): void;
    removeEdgeById(edgeId: number): void;
}
export type Constructor<T> = new () => T;
export interface RawRuntimeModule {
    GraphInt: Constructor<RawSimpleGraph<number>>;
    GraphStr: Constructor<RawSimpleGraph<string>>;
    DiGraphInt: Constructor<RawSimpleGraph<number>>;
    DiGraphStr: Constructor<RawSimpleGraph<string>>;
    MultiGraphInt: Constructor<RawMultiGraph<number>>;
    MultiGraphStr: Constructor<RawMultiGraph<string>>;
    MultiDiGraphInt: Constructor<RawMultiGraph<number>>;
    MultiDiGraphStr: Constructor<RawMultiGraph<string>>;
}
