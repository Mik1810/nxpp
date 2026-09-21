import type { AllPairsShortestPathSourceEntry, AttributeValue, CentralityScoreEntry, ConnectedComponents, DiGraph, Graph, MaximumFlowResult, MinCostMaxFlowResult, MinimumCutResult, NodeId, ShortestPathDistanceEntry, SpanningTreeEdge, SingleSourceShortestPathResult, StronglyConnectedComponents, TraversalEdge, TraversalPredecessorEntry, TraversalSuccessorEntry, TraversalTree } from "../types.js";
import type { RawRuntimeModule, RawSimpleGraph } from "../internal/wasm_types.js";
export interface GraphIntFacade extends Graph<number>, ConnectedComponents<number> {
    subgraph(nodes: number[]): GraphIntFacade;
}
export interface GraphStrFacade extends Graph<string>, ConnectedComponents<string> {
    subgraph(nodes: string[]): GraphStrFacade;
}
export interface DiGraphIntFacade extends DiGraph<number>, StronglyConnectedComponents<number> {
    subgraph(nodes: number[]): DiGraphIntFacade;
}
export interface DiGraphStrFacade extends DiGraph<string>, StronglyConnectedComponents<string> {
    subgraph(nodes: string[]): DiGraphStrFacade;
}
export interface SimpleGraphClasses {
    readonly GraphInt: new (raw?: RawSimpleGraph<number>) => GraphIntFacade;
    readonly GraphStr: new (raw?: RawSimpleGraph<string>) => GraphStrFacade;
    readonly DiGraphInt: new (raw?: RawSimpleGraph<number>) => DiGraphIntFacade;
    readonly DiGraphStr: new (raw?: RawSimpleGraph<string>) => DiGraphStrFacade;
}
export declare abstract class BaseGraph<T extends NodeId, RawGraph extends RawSimpleGraph<T>> {
    private rawObject;
    protected readonly assertNode: (value: unknown, label: string) => asserts value is T;
    private mutationVersion;
    private stagedFlowMutationVersion;
    constructor(factory: (() => RawGraph) | RawGraph, assertNode: (value: unknown, label: string) => asserts value is T);
    protected get raw(): RawGraph;
    protected abstract createFromRaw(raw: RawGraph): this;
    protected operationFailed(message: string): never;
    protected markGraphMutation(): void;
    private markStagedFlow;
    private requireStagedFlow;
    private requireNodeExists;
    private requireEdgeExists;
    private requireWeightKey;
    private requireAttributeKey;
    private requirePagerankMaxIterations;
    private runPathLookup;
    protected readConnectedComponents(): T[][];
    protected readStronglyConnectedComponents(): T[][];
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
export declare function createSimpleGraphClasses(runtime: RawRuntimeModule): SimpleGraphClasses;
