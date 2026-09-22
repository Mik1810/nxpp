import type {
  AllPairsShortestPathSourceEntry,
  AttributeValue,
  CentralityScoreEntry,
  MaximumFlowResult,
  MinCostMaxFlowResult,
  MinimumCutResult,
  NodeId,
  ShortestPathDistanceEntry,
  ShortestPathPredecessorEntry,
  SpanningTreeEdge,
  TraversalEdge,
  TraversalPredecessorEntry,
  TraversalSuccessorEntry,
  TraversalTree,
} from "../types.js";

export interface RawSingleSourceShortestPathResult<T extends NodeId> {
  distance: ShortestPathDistanceEntry<T>[];
  predecessor: ShortestPathPredecessorEntry<T>[];
}

export interface RawConnectedComponentsGraph<T extends NodeId> {
  connectedComponents(): T[][];
}

export interface RawStronglyConnectedComponentsGraph<T extends NodeId> {
  stronglyConnectedComponents(): T[][];
}

export interface RawEdgeEndpoints<T extends NodeId> {
  source: T;
  target: T;
}

export interface RawDisposable {
  delete(): void;
}

export interface RawSimpleGraph<T extends NodeId> extends RawDisposable {
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
  subgraph(nodes: T[]): RawSimpleGraph<T>;
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
  shortestPathWeighted(source: T, target: T, weightKey: string): T[];
  shortestPathLength(source: T, target: T): number;
  shortestPathLengthWeighted(source: T, target: T, weightKey: string): number;
  dijkstraPath(source: T, target: T): T[];
  dijkstraPathWeighted(source: T, target: T, weightKey: string): T[];
  dijkstraShortestPaths(source: T): RawSingleSourceShortestPathResult<T>;
  dijkstraPathLengths(source: T): ShortestPathDistanceEntry<T>[];
  dijkstraPathLength(source: T, target: T): number;
  dijkstraPathLengthWeighted(source: T, target: T, weightKey: string): number;
  bellmanFordPath(source: T, target: T): T[];
  bellmanFordPathWeighted(source: T, target: T, weightKey: string): T[];
  bellmanFordShortestPaths(source: T): RawSingleSourceShortestPathResult<T>;
  bellmanFordPathLength(source: T, target: T): number;
  bellmanFordPathLengthWeighted(source: T, target: T, weightKey: string): number;
  dagShortestPaths(source: T): RawSingleSourceShortestPathResult<T>;
  floydWarshallAllPairsShortestPaths(): number[][];
  floydWarshallAllPairsShortestPathsMap(): AllPairsShortestPathSourceEntry<T>[];
  kruskalMinimumSpanningTree(): SpanningTreeEdge<T>[];
  primMinimumSpanningTree(root: T): SpanningTreeEdge<T>[];
  degreeCentrality(): CentralityScoreEntry<T>[];
  pagerank(tolerance: number, maxIterations: number): CentralityScoreEntry<T>[];
  betweennessCentrality(): CentralityScoreEntry<T>[];
  maximumFlow(source: T, target: T, capacityKey: string): MaximumFlowResult<T>;
  minimumCut(source: T, target: T, capacityKey: string): MinimumCutResult<T>;
  maxFlowMinCost(source: T, target: T, capacityKey: string, weightKey: string): MinCostMaxFlowResult<T>;
  maxFlowMinCostSuccessiveShortestPath(source: T, target: T, capacityKey: string, weightKey: string): MinCostMaxFlowResult<T>;
  pushRelabelMaximumFlow(source: T, target: T, capacityKey: string, weightKey: string): number;
  cycleCanceling(weightKey: string): number;
  clear(): void;
}

export interface RawMultiGraph<T extends NodeId> extends RawSimpleGraph<T> {
  subgraph(nodes: T[]): RawMultiGraph<T>;
  hasEdgeId(edgeId: number): boolean;
  edgeIds(): number[];
  edgeIdsBetween(source: T, target: T): number[];
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

export interface RawUndirectedGraph<T extends NodeId> extends RawSimpleGraph<T>, RawConnectedComponentsGraph<T> {
  subgraph(nodes: T[]): RawUndirectedGraph<T>;
}

export interface RawDirectedGraph<T extends NodeId> extends RawSimpleGraph<T>, RawStronglyConnectedComponentsGraph<T> {
  subgraph(nodes: T[]): RawDirectedGraph<T>;
}

export interface RawUndirectedMultiGraph<T extends NodeId> extends RawMultiGraph<T>, RawConnectedComponentsGraph<T> {
  subgraph(nodes: T[]): RawUndirectedMultiGraph<T>;
}

export interface RawDirectedMultiGraph<T extends NodeId> extends RawMultiGraph<T>, RawStronglyConnectedComponentsGraph<T> {
  subgraph(nodes: T[]): RawDirectedMultiGraph<T>;
}

export type Constructor<T> = new () => T;

export interface RawRuntimeModule {
  getExceptionMessage(error: unknown): [string, string];
  decrementExceptionRefcount(error: unknown): void;
  GraphInt: Constructor<RawUndirectedGraph<number>>;
  GraphStr: Constructor<RawUndirectedGraph<string>>;
  DiGraphInt: Constructor<RawDirectedGraph<number>>;
  DiGraphStr: Constructor<RawDirectedGraph<string>>;
  MultiGraphInt: Constructor<RawUndirectedMultiGraph<number>>;
  MultiGraphStr: Constructor<RawUndirectedMultiGraph<string>>;
  MultiDiGraphInt: Constructor<RawDirectedMultiGraph<number>>;
  MultiDiGraphStr: Constructor<RawDirectedMultiGraph<string>>;
}
