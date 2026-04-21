import type { AllPairsShortestPathSourceEntry, AttributeValue, NodeId, ShortestPathDistanceEntry, ShortestPathPredecessorEntry, TraversalEdge, TraversalPredecessorEntry, TraversalSuccessorEntry, TraversalTree } from "../types.js";
export interface RawSingleSourceShortestPathResult<T extends NodeId> {
    distance: Iterable<ShortestPathDistanceEntry<T>> | ArrayLike<ShortestPathDistanceEntry<T>>;
    predecessor: Iterable<ShortestPathPredecessorEntry<T>> | ArrayLike<ShortestPathPredecessorEntry<T>>;
}
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
    hasNodeAttr(id: T, key: string): boolean;
    getNodeAttr(id: T, key: string): AttributeValue;
    tryGetNodeAttr(id: T, key: string): AttributeValue | null;
    setNodeAttr(id: T, key: string, value: AttributeValue): void;
    hasEdgeAttr(source: T, target: T, key: string): boolean;
    getEdgeAttr(source: T, target: T, key: string): AttributeValue;
    tryGetEdgeAttr(source: T, target: T, key: string): AttributeValue | null;
    setEdgeAttr(source: T, target: T, key: string, value: AttributeValue): void;
    getEdgeNumericAttr(source: T, target: T, key: string): number;
    bfsEdges(start: T): Iterable<TraversalEdge<T>> | ArrayLike<TraversalEdge<T>>;
    bfsTree(start: T): TraversalTree<T>;
    bfsSuccessors(start: T): Iterable<TraversalSuccessorEntry<T>> | ArrayLike<TraversalSuccessorEntry<T>>;
    dfsEdges(start: T): Iterable<TraversalEdge<T>> | ArrayLike<TraversalEdge<T>>;
    dfsTree(start: T): TraversalTree<T>;
    dfsPredecessors(start: T): Iterable<TraversalPredecessorEntry<T>> | ArrayLike<TraversalPredecessorEntry<T>>;
    dfsSuccessors(start: T): Iterable<TraversalSuccessorEntry<T>> | ArrayLike<TraversalSuccessorEntry<T>>;
    shortestPath(source: T, target: T): Iterable<T> | ArrayLike<T>;
    shortestPathWeighted(source: T, target: T, weightKey: string): Iterable<T> | ArrayLike<T>;
    shortestPathLength(source: T, target: T): number;
    shortestPathLengthWeighted(source: T, target: T, weightKey: string): number;
    dijkstraPath(source: T, target: T): Iterable<T> | ArrayLike<T>;
    dijkstraPathWeighted(source: T, target: T, weightKey: string): Iterable<T> | ArrayLike<T>;
    dijkstraShortestPaths(source: T): RawSingleSourceShortestPathResult<T>;
    dijkstraPathLengths(source: T): Iterable<ShortestPathDistanceEntry<T>> | ArrayLike<ShortestPathDistanceEntry<T>>;
    dijkstraPathLength(source: T, target: T): number;
    dijkstraPathLengthWeighted(source: T, target: T, weightKey: string): number;
    bellmanFordPath(source: T, target: T): Iterable<T> | ArrayLike<T>;
    bellmanFordPathWeighted(source: T, target: T, weightKey: string): Iterable<T> | ArrayLike<T>;
    bellmanFordShortestPaths(source: T): RawSingleSourceShortestPathResult<T>;
    bellmanFordPathLength(source: T, target: T): number;
    bellmanFordPathLengthWeighted(source: T, target: T, weightKey: string): number;
    dagShortestPaths(source: T): RawSingleSourceShortestPathResult<T>;
    floydWarshallAllPairsShortestPaths(): Iterable<Iterable<number> | ArrayLike<number>> | ArrayLike<Iterable<number> | ArrayLike<number>>;
    floydWarshallAllPairsShortestPathsMap(): Iterable<AllPairsShortestPathSourceEntry<T>> | ArrayLike<AllPairsShortestPathSourceEntry<T>>;
    clear(): void;
}
export interface RawMultiGraph<T extends NodeId> extends RawSimpleGraph<T> {
    hasEdgeId(edgeId: number): boolean;
    edgeIds(): Iterable<number> | ArrayLike<number>;
    edgeIdsBetween(source: T, target: T): Iterable<number> | ArrayLike<number>;
    getEdgeEndpoints(edgeId: number): RawEdgeEndpoints<T>;
    getEdgeWeightById(edgeId: number): number;
    setEdgeWeightById(edgeId: number, weight: number): void;
    hasEdgeAttrById(edgeId: number, key: string): boolean;
    getEdgeAttrById(edgeId: number, key: string): AttributeValue;
    tryGetEdgeAttrById(edgeId: number, key: string): AttributeValue | null;
    setEdgeAttrById(edgeId: number, key: string, value: AttributeValue): void;
    getEdgeNumericAttrById(edgeId: number, key: string): number;
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
