import type { AllPairsShortestPathSourceEntry, AttributeValue, MultiDiGraph, MultiGraph, NodeId, ShortestPathDistanceEntry, SpanningTreeEdge, SingleSourceShortestPathResult, TraversalEdge, TraversalPredecessorEntry, TraversalSuccessorEntry, TraversalTree } from "../types.js";
import type { RawMultiGraph } from "../internal/wasm_types.js";
declare class BaseMultiGraph<T extends NodeId> {
    private rawObject;
    private readonly assertNode;
    constructor(factory: () => RawMultiGraph<T>, assertNode: (value: unknown, label: string) => asserts value is T);
    protected get raw(): RawMultiGraph<T>;
    private operationFailed;
    private requireNodeExists;
    private requireEdgeExists;
    private requireEdgeIdExists;
    private requireWeightKey;
    private runPathLookup;
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
    clear(): void;
    dispose(): void;
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
