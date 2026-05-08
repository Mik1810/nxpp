import type { AllPairsShortestPathSourceEntry, AttributeValue, CentralityScoreEntry, ConnectedComponents, DiGraph, Graph, MaximumFlowResult, MinCostMaxFlowResult, MinimumCutResult, NodeId, ShortestPathDistanceEntry, SpanningTreeEdge, SingleSourceShortestPathResult, StronglyConnectedComponents, TraversalEdge, TraversalPredecessorEntry, TraversalSuccessorEntry, TraversalTree } from "../types.js";
import type { RawSimpleGraph } from "../internal/wasm_types.js";
declare abstract class BaseSimpleGraph<T extends NodeId> {
    private rawObject;
    private readonly assertNode;
    private mutationVersion;
    private stagedFlowMutationVersion;
    constructor(factory: (() => RawSimpleGraph<T>) | RawSimpleGraph<T>, assertNode: (value: unknown, label: string) => asserts value is T);
    protected get raw(): RawSimpleGraph<T>;
    protected abstract createFromRaw(raw: RawSimpleGraph<T>): this;
    private operationFailed;
    private markGraphMutation;
    private markStagedFlow;
    private requireStagedFlow;
    private requireNodeExists;
    private requireEdgeExists;
    private requireWeightKey;
    private requireAttributeKey;
    private requirePagerankMaxIterations;
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
    subgraph(nodes: T[]): this;
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
    degreeCentrality(): CentralityScoreEntry<T>[];
    pagerank(tolerance?: number, maxIterations?: number): CentralityScoreEntry<T>[];
    betweennessCentrality(): CentralityScoreEntry<T>[];
    maximumFlow(source: T, target: T, capacityKey?: string): MaximumFlowResult<T>;
    minimumCut(source: T, target: T, capacityKey?: string): MinimumCutResult<T>;
    maxFlowMinCost(source: T, target: T, capacityKey?: string, weightKey?: string): MinCostMaxFlowResult<T>;
    maxFlowMinCostSuccessiveShortestPath(source: T, target: T, capacityKey?: string, weightKey?: string): MinCostMaxFlowResult<T>;
    pushRelabelMaximumFlow(source: T, target: T, capacityKey?: string, weightKey?: string): number;
    cycleCanceling(weightKey?: string): number;
    clear(): void;
    dispose(): void;
}
export declare class GraphInt extends BaseSimpleGraph<number> implements Graph<number>, ConnectedComponents<number> {
    constructor(raw?: RawSimpleGraph<number>);
    protected createFromRaw(raw: RawSimpleGraph<number>): this;
    connectedComponents(): number[][];
}
export declare class GraphStr extends BaseSimpleGraph<string> implements Graph<string>, ConnectedComponents<string> {
    constructor(raw?: RawSimpleGraph<string>);
    protected createFromRaw(raw: RawSimpleGraph<string>): this;
    connectedComponents(): string[][];
}
export declare class DiGraphInt extends BaseSimpleGraph<number> implements DiGraph<number>, StronglyConnectedComponents<number> {
    constructor(raw?: RawSimpleGraph<number>);
    protected createFromRaw(raw: RawSimpleGraph<number>): this;
    stronglyConnectedComponents(): number[][];
}
export declare class DiGraphStr extends BaseSimpleGraph<string> implements DiGraph<string>, StronglyConnectedComponents<string> {
    constructor(raw?: RawSimpleGraph<string>);
    protected createFromRaw(raw: RawSimpleGraph<string>): this;
    stronglyConnectedComponents(): string[][];
}
export {};
