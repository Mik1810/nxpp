import {
  DiGraphInt,
  DiGraphStr,
  GraphInt,
  GraphStr,
  MultiDiGraphInt,
  MultiDiGraphStr,
  MultiGraphInt,
  MultiGraphStr,
} from "../index.js";

import type {
  AllPairsShortestPathSourceEntry,
  AttributeValue,
  DiGraph,
  EdgeEndpoints,
  Graph,
  MultiDiGraph,
  MultiGraph,
  ShortestPathDistanceEntry,
  SingleSourceShortestPathResult,
  TraversalEdge,
  TraversalPredecessorEntry,
  TraversalSuccessorEntry,
  TraversalTree,
} from "../types.js";

function exerciseGraphNumber(graph: Graph<number>): void {
  graph.addNode(1);
  graph.addEdge(1, 2, 3);
  graph.hasNode(1);
  graph.hasEdge(1, 2);
  const nodes: number[] = graph.nodes();
  const neighbors: number[] = graph.neighbors(1);
  const weight: number = graph.getEdgeWeight(1, 2);
  graph.setEdgeWeight(1, 2, weight);
  const nodeAttr: AttributeValue = graph.getNodeAttr(1, "label");
  const maybeNodeAttr: AttributeValue | null = graph.tryGetNodeAttr(1, "label");
  const edgeAttr: AttributeValue = graph.getEdgeAttr(1, 2, "capacity");
  const maybeEdgeAttr: AttributeValue | null = graph.tryGetEdgeAttr(1, 2, "capacity");
  const edgeNumericAttr: number = graph.getEdgeNumericAttr(1, 2, "capacity");
  const bfsEdges: TraversalEdge<number>[] = graph.bfsEdges(1);
  const bfsTree: TraversalTree<number> = graph.bfsTree(1);
  const bfsSuccessors: TraversalSuccessorEntry<number>[] = graph.bfsSuccessors(1);
  const dfsEdges: TraversalEdge<number>[] = graph.dfsEdges(1);
  const dfsTree: TraversalTree<number> = graph.dfsTree(1);
  const dfsPredecessors: TraversalPredecessorEntry<number>[] = graph.dfsPredecessors(1);
  const dfsSuccessors: TraversalSuccessorEntry<number>[] = graph.dfsSuccessors(1);
  const shortestPath: number[] = graph.shortestPath(1, 2);
  const shortestPathWeighted: number[] = graph.shortestPathWeighted(1, 2, "weight");
  const shortestPathLength: number = graph.shortestPathLength(1, 2);
  const shortestPathLengthWeighted: number = graph.shortestPathLengthWeighted(1, 2, "weight");
  const dijkstraPath: number[] = graph.dijkstraPath(1, 2);
  const dijkstraPathWeighted: number[] = graph.dijkstraPathWeighted(1, 2, "weight");
  const dijkstraShortestPaths: SingleSourceShortestPathResult<number> = graph.dijkstraShortestPaths(1);
  const dijkstraPathLengths: ShortestPathDistanceEntry<number>[] = graph.dijkstraPathLengths(1);
  const dijkstraPathLength: number = graph.dijkstraPathLength(1, 2);
  const dijkstraPathLengthWeighted: number = graph.dijkstraPathLengthWeighted(1, 2, "weight");
  const bellmanFordPath: number[] = graph.bellmanFordPath(1, 2);
  const bellmanFordPathWeighted: number[] = graph.bellmanFordPathWeighted(1, 2, "weight");
  const bellmanFordShortestPaths: SingleSourceShortestPathResult<number> = graph.bellmanFordShortestPaths(1);
  const bellmanFordPathLength: number = graph.bellmanFordPathLength(1, 2);
  const bellmanFordPathLengthWeighted: number = graph.bellmanFordPathLengthWeighted(1, 2, "weight");
  const dagShortestPaths: SingleSourceShortestPathResult<number> = graph.dagShortestPaths(1);
  const floydWarshallAllPairsShortestPaths: number[][] = graph.floydWarshallAllPairsShortestPaths();
  const floydWarshallAllPairsShortestPathsMap: AllPairsShortestPathSourceEntry<number>[] =
    graph.floydWarshallAllPairsShortestPathsMap();
  graph.setNodeAttr(1, "label", "source");
  graph.setEdgeAttr(1, 2, "capacity", 3);
  graph.hasNodeAttr(1, "label");
  graph.hasEdgeAttr(1, 2, "capacity");
  graph.removeEdge(1, 2);
  graph.removeNode(1);
  graph.clear();
  void nodes;
  void neighbors;
  void nodeAttr;
  void maybeNodeAttr;
  void edgeAttr;
  void maybeEdgeAttr;
  void edgeNumericAttr;
  void bfsEdges;
  void bfsTree;
  void bfsSuccessors;
  void dfsEdges;
  void dfsTree;
  void dfsPredecessors;
  void dfsSuccessors;
  void shortestPath;
  void shortestPathWeighted;
  void shortestPathLength;
  void shortestPathLengthWeighted;
  void dijkstraPath;
  void dijkstraPathWeighted;
  void dijkstraShortestPaths;
  void dijkstraPathLengths;
  void dijkstraPathLength;
  void dijkstraPathLengthWeighted;
  void bellmanFordPath;
  void bellmanFordPathWeighted;
  void bellmanFordShortestPaths;
  void bellmanFordPathLength;
  void bellmanFordPathLengthWeighted;
  void dagShortestPaths;
  void floydWarshallAllPairsShortestPaths;
  void floydWarshallAllPairsShortestPathsMap;
}

function exerciseGraphString(graph: Graph<string>): void {
  graph.addNode("a");
  graph.addEdge("a", "b", 3);
  graph.hasNode("a");
  graph.hasEdge("a", "b");
  const nodes: string[] = graph.nodes();
  const neighbors: string[] = graph.neighbors("a");
  const weight: number = graph.getEdgeWeight("a", "b");
  graph.setEdgeWeight("a", "b", weight);
  const nodeAttr: AttributeValue = graph.getNodeAttr("a", "label");
  const maybeNodeAttr: AttributeValue | null = graph.tryGetNodeAttr("a", "label");
  const edgeAttr: AttributeValue = graph.getEdgeAttr("a", "b", "capacity");
  const maybeEdgeAttr: AttributeValue | null = graph.tryGetEdgeAttr("a", "b", "capacity");
  const edgeNumericAttr: number = graph.getEdgeNumericAttr("a", "b", "capacity");
  const bfsEdges: TraversalEdge<string>[] = graph.bfsEdges("a");
  const bfsTree: TraversalTree<string> = graph.bfsTree("a");
  const bfsSuccessors: TraversalSuccessorEntry<string>[] = graph.bfsSuccessors("a");
  const dfsEdges: TraversalEdge<string>[] = graph.dfsEdges("a");
  const dfsTree: TraversalTree<string> = graph.dfsTree("a");
  const dfsPredecessors: TraversalPredecessorEntry<string>[] = graph.dfsPredecessors("a");
  const dfsSuccessors: TraversalSuccessorEntry<string>[] = graph.dfsSuccessors("a");
  const shortestPath: string[] = graph.shortestPath("a", "b");
  const shortestPathWeighted: string[] = graph.shortestPathWeighted("a", "b", "weight");
  const shortestPathLength: number = graph.shortestPathLength("a", "b");
  const shortestPathLengthWeighted: number = graph.shortestPathLengthWeighted("a", "b", "weight");
  const dijkstraPath: string[] = graph.dijkstraPath("a", "b");
  const dijkstraPathWeighted: string[] = graph.dijkstraPathWeighted("a", "b", "weight");
  const dijkstraShortestPaths: SingleSourceShortestPathResult<string> = graph.dijkstraShortestPaths("a");
  const dijkstraPathLengths: ShortestPathDistanceEntry<string>[] = graph.dijkstraPathLengths("a");
  const dijkstraPathLength: number = graph.dijkstraPathLength("a", "b");
  const dijkstraPathLengthWeighted: number = graph.dijkstraPathLengthWeighted("a", "b", "weight");
  const bellmanFordPath: string[] = graph.bellmanFordPath("a", "b");
  const bellmanFordPathWeighted: string[] = graph.bellmanFordPathWeighted("a", "b", "weight");
  const bellmanFordShortestPaths: SingleSourceShortestPathResult<string> = graph.bellmanFordShortestPaths("a");
  const bellmanFordPathLength: number = graph.bellmanFordPathLength("a", "b");
  const bellmanFordPathLengthWeighted: number = graph.bellmanFordPathLengthWeighted("a", "b", "weight");
  const dagShortestPaths: SingleSourceShortestPathResult<string> = graph.dagShortestPaths("a");
  const floydWarshallAllPairsShortestPaths: number[][] = graph.floydWarshallAllPairsShortestPaths();
  const floydWarshallAllPairsShortestPathsMap: AllPairsShortestPathSourceEntry<string>[] =
    graph.floydWarshallAllPairsShortestPathsMap();
  graph.setNodeAttr("a", "label", "source");
  graph.setEdgeAttr("a", "b", "capacity", 3);
  graph.hasNodeAttr("a", "label");
  graph.hasEdgeAttr("a", "b", "capacity");
  graph.removeEdge("a", "b");
  graph.removeNode("a");
  graph.clear();
  void nodes;
  void neighbors;
  void nodeAttr;
  void maybeNodeAttr;
  void edgeAttr;
  void maybeEdgeAttr;
  void edgeNumericAttr;
  void bfsEdges;
  void bfsTree;
  void bfsSuccessors;
  void dfsEdges;
  void dfsTree;
  void dfsPredecessors;
  void dfsSuccessors;
  void shortestPath;
  void shortestPathWeighted;
  void shortestPathLength;
  void shortestPathLengthWeighted;
  void dijkstraPath;
  void dijkstraPathWeighted;
  void dijkstraShortestPaths;
  void dijkstraPathLengths;
  void dijkstraPathLength;
  void dijkstraPathLengthWeighted;
  void bellmanFordPath;
  void bellmanFordPathWeighted;
  void bellmanFordShortestPaths;
  void bellmanFordPathLength;
  void bellmanFordPathLengthWeighted;
  void dagShortestPaths;
  void floydWarshallAllPairsShortestPaths;
  void floydWarshallAllPairsShortestPathsMap;
}

function exerciseMultiGraphNumber(graph: MultiGraph<number>): void {
  graph.addNode(1);
  graph.addEdge(1, 2, 3);
  graph.hasEdgeId(0);
  const ids: number[] = graph.edgeIds();
  const idsBetween: number[] = graph.edgeIdsBetween(1, 2);
  const endpoints: EdgeEndpoints<number> = graph.getEdgeEndpoints(0);
  const source: number = endpoints.source();
  const target: number = endpoints.target();
  const weightById: number = graph.getEdgeWeightById(0);
  const edgeAttr: AttributeValue = graph.getEdgeAttrById(0, "capacity");
  const maybeEdgeAttr: AttributeValue | null = graph.tryGetEdgeAttrById(0, "capacity");
  const edgeNumericAttr: number = graph.getEdgeNumericAttrById(0, "capacity");
  graph.setEdgeWeightById(0, weightById);
  graph.setEdgeAttrById(0, "capacity", 3);
  graph.hasEdgeAttrById(0, "capacity");
  graph.removeEdgeById(0);
  void ids;
  void idsBetween;
  void source;
  void target;
  void edgeAttr;
  void maybeEdgeAttr;
  void edgeNumericAttr;
}

function exerciseMultiGraphString(graph: MultiGraph<string>): void {
  graph.addNode("a");
  graph.addEdge("a", "b", 3);
  graph.hasEdgeId(0);
  const ids: number[] = graph.edgeIds();
  const idsBetween: number[] = graph.edgeIdsBetween("a", "b");
  const endpoints: EdgeEndpoints<string> = graph.getEdgeEndpoints(0);
  const source: string = endpoints.source();
  const target: string = endpoints.target();
  const weightById: number = graph.getEdgeWeightById(0);
  const edgeAttr: AttributeValue = graph.getEdgeAttrById(0, "capacity");
  const maybeEdgeAttr: AttributeValue | null = graph.tryGetEdgeAttrById(0, "capacity");
  const edgeNumericAttr: number = graph.getEdgeNumericAttrById(0, "capacity");
  graph.setEdgeWeightById(0, weightById);
  graph.setEdgeAttrById(0, "capacity", 3);
  graph.hasEdgeAttrById(0, "capacity");
  graph.removeEdgeById(0);
  void ids;
  void idsBetween;
  void source;
  void target;
  void edgeAttr;
  void maybeEdgeAttr;
  void edgeNumericAttr;
}

const graphNumber: Graph<number> = new GraphInt();
const graphString: Graph<string> = new GraphStr();
const digraphNumber: DiGraph<number> = new DiGraphInt();
const digraphString: DiGraph<string> = new DiGraphStr();
const multigraphNumber: MultiGraph<number> = new MultiGraphInt();
const multigraphString: MultiGraph<string> = new MultiGraphStr();
const multidigraphNumber: MultiDiGraph<number> = new MultiDiGraphInt();
const multidigraphString: MultiDiGraph<string> = new MultiDiGraphStr();

exerciseGraphNumber(graphNumber);
exerciseGraphString(graphString);
exerciseGraphNumber(digraphNumber);
exerciseGraphString(digraphString);
exerciseMultiGraphNumber(multigraphNumber);
exerciseMultiGraphString(multigraphString);
exerciseMultiGraphNumber(multidigraphNumber);
exerciseMultiGraphString(multidigraphString);
