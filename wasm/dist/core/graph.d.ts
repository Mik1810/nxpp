import type { AllPairsShortestPathSourceEntry, AttributeValue, DiGraph, Graph, NodeId, ShortestPathDistanceEntry, SpanningTreeEdge, SingleSourceShortestPathResult, TraversalEdge, TraversalPredecessorEntry, TraversalSuccessorEntry, TraversalTree } from "../types.js";
import type { RawSimpleGraph } from "../internal/wasm_types.js";
declare class BaseSimpleGraph<T extends NodeId> {
    private rawObject;
    private readonly assertNode;
    constructor(factory: () => RawSimpleGraph<T>, assertNode: (value: unknown, label: string) => asserts value is T);
    protected get raw(): RawSimpleGraph<T>;
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
    hasNodeAttr(id: T, key: string): boolean;
    getNodeAttr(id: T, key: string): AttributeValue;
    tryGetNodeAttr(id: T, key: string): AttributeValue | null;
    setNodeAttr(id: T, key: string, value: AttributeValue): void;
    hasEdgeAttr(source: T, target: T, key: string): boolean;
    getEdgeAttr(source: T, target: T, key: string): AttributeValue;
    tryGetEdgeAttr(source: T, target: T, key: string): AttributeValue | null;
    setEdgeAttr(source: T, target: T, key: string, value: AttributeValue): void;
    getEdgeNumericAttr(source: T, target: T, key: string): number;
    bfsEdges(start: T): TraversalEdge<T>[];
    bfsTree(start: T): TraversalTree<T>;
    bfsSuccessors(start: T): TraversalSuccessorEntry<T>[];
    dfsEdges(start: T): TraversalEdge<T>[];
    dfsTree(start: T): TraversalTree<T>;
    dfsPredecessors(start: T): TraversalPredecessorEntry<T>[];
    dfsSuccessors(start: T): TraversalSuccessorEntry<T>[];
    shortestPath(source: T, target: T): T[];
    shortestPathWeighted(source: T, target: T, weightKey?: string): T[];
    shortestPathLength(source: T, target: T): number;
    shortestPathLengthWeighted(source: T, target: T, weightKey?: string): number;
    dijkstraPath(source: T, target: T): T[];
    dijkstraPathWeighted(source: T, target: T, weightKey?: string): T[];
    dijkstraShortestPaths(source: T): SingleSourceShortestPathResult<T>;
    dijkstraPathLengths(source: T): ShortestPathDistanceEntry<T>[];
    dijkstraPathLength(source: T, target: T): number;
    dijkstraPathLengthWeighted(source: T, target: T, weightKey?: string): number;
    bellmanFordPath(source: T, target: T): T[];
    bellmanFordPathWeighted(source: T, target: T, weightKey?: string): T[];
    bellmanFordShortestPaths(source: T): SingleSourceShortestPathResult<T>;
    bellmanFordPathLength(source: T, target: T): number;
    bellmanFordPathLengthWeighted(source: T, target: T, weightKey?: string): number;
    dagShortestPaths(source: T): SingleSourceShortestPathResult<T>;
    floydWarshallAllPairsShortestPaths(): number[][];
    floydWarshallAllPairsShortestPathsMap(): AllPairsShortestPathSourceEntry<T>[];
    kruskalMinimumSpanningTree(): SpanningTreeEdge<T>[];
    primMinimumSpanningTree(root: T): SpanningTreeEdge<T>[];
    clear(): void;
    dispose(): void;
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
