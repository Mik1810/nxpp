import { runtime } from "../load.js";
import {
  assertAttributeValue,
  assertEdgeId,
  assertFiniteNumber,
  assertIntNodeId,
  assertStringNodeId,
} from "../internal/assert.js";
import { disposedGraphMessage, wrapRawGraph } from "../internal/errors.js";
import { toArray, toEdgeEndpoints } from "../internal/wrap.js";

import type {
  AllPairsShortestPathSourceEntry,
  AttributeValue,
  MultiDiGraph,
  MultiGraph,
  NodeId,
  ShortestPathDistanceEntry,
  SpanningTreeEdge,
  SingleSourceShortestPathResult,
  TraversalEdge,
  TraversalPredecessorEntry,
  TraversalSuccessorEntry,
  TraversalTree,
} from "../types.js";
import type { RawMultiGraph } from "../internal/wasm_types.js";
import {
  toAllPairsShortestPathMap,
  toAllPairsShortestPathMatrix,
  toSingleSourceShortestPathResult,
} from "../algorithms/shortest_paths.js";

const disposeSymbol = (Symbol as unknown as { dispose?: symbol }).dispose;

class BaseMultiGraph<T extends NodeId> {
  private rawObject: RawMultiGraph<T> | null;
  private readonly assertNode: (value: unknown, label: string) => asserts value is T;

  constructor(
    factory: () => RawMultiGraph<T>,
    assertNode: (value: unknown, label: string) => asserts value is T,
  ) {
    this.rawObject = wrapRawGraph(factory());
    this.assertNode = assertNode;
    if (disposeSymbol !== undefined) {
      Object.defineProperty(this, disposeSymbol, {
        value: () => this.dispose(),
        configurable: true,
      });
    }
  }

  protected get raw(): RawMultiGraph<T> {
    if (this.rawObject === null) {
      throw new Error(disposedGraphMessage);
    }
    return this.rawObject;
  }

  private operationFailed(message: string): never {
    throw new Error(`WASM graph operation failed: ${message}`);
  }

  private requireNodeExists(id: T): void {
    if (!this.raw.hasNode(id)) {
      this.operationFailed("Node lookup failed: node not found.");
    }
  }

  private requireEdgeExists(source: T, target: T): void {
    this.requireNodeExists(source);
    this.requireNodeExists(target);
    if (!this.raw.hasEdge(source, target)) {
      this.operationFailed("Edge lookup failed: edge not found.");
    }
  }

  private requireEdgeIdExists(edgeId: number): void {
    if (!this.raw.hasEdgeId(edgeId)) {
      this.operationFailed("Edge lookup failed: edge not found.");
    }
  }

  private requireWeightKey(weightKey: string): void {
    if (weightKey !== "weight") {
      this.operationFailed("Weight lookup failed: only the built-in edge weight property named 'weight' is supported.");
    }
  }

  private runPathLookup<R>(fn: () => R): R {
    try {
      return fn();
    } catch (error) {
      if (error instanceof Error && error.message === "WASM graph operation failed: unknown runtime error.") {
        this.operationFailed("Shortest-path lookup failed: target node is unreachable.");
      }
      throw error;
    }
  }

  addNode(id: T): void {
    this.assertNode(id, "id");
    this.raw.addNode(id);
  }

  addEdge(source: T, target: T, weight: number): void {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    assertFiniteNumber(weight, "weight");
    this.raw.addEdge(source, target, weight);
  }

  hasNode(id: T): boolean {
    this.assertNode(id, "id");
    return this.raw.hasNode(id);
  }

  hasEdge(source: T, target: T): boolean {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    return this.raw.hasEdge(source, target);
  }

  nodes(): T[] {
    return toArray(this.raw.nodes());
  }

  neighbors(id: T): T[] {
    this.assertNode(id, "id");
    this.requireNodeExists(id);
    return toArray(this.raw.neighbors(id));
  }

  removeNode(id: T): void {
    this.assertNode(id, "id");
    this.requireNodeExists(id);
    this.raw.removeNode(id);
  }

  removeEdge(source: T, target: T): void {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    this.requireEdgeExists(source, target);
    this.raw.removeEdge(source, target);
  }

  getEdgeWeight(source: T, target: T): number {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    this.requireEdgeExists(source, target);
    return this.raw.getEdgeWeight(source, target);
  }

  setEdgeWeight(source: T, target: T, weight: number): void {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    assertFiniteNumber(weight, "weight");
    this.requireEdgeExists(source, target);
    this.raw.setEdgeWeight(source, target, weight);
  }

  hasNodeAttr(id: T, key: string): boolean {
    this.assertNode(id, "id");
    return this.raw.hasNodeAttr(id, key);
  }

  getNodeAttr(id: T, key: string): AttributeValue {
    this.assertNode(id, "id");
    this.requireNodeExists(id);
    if (!this.raw.hasNodeAttr(id, key)) {
      this.operationFailed("Node attribute lookup failed: key not found.");
    }
    return this.raw.getNodeAttr(id, key);
  }

  tryGetNodeAttr(id: T, key: string): AttributeValue | null {
    this.assertNode(id, "id");
    return this.raw.tryGetNodeAttr(id, key);
  }

  setNodeAttr(id: T, key: string, value: AttributeValue): void {
    this.assertNode(id, "id");
    assertAttributeValue(value, "value");
    this.raw.setNodeAttr(id, key, value);
  }

  hasEdgeAttr(source: T, target: T, key: string): boolean {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    return this.raw.hasEdgeAttr(source, target, key);
  }

  getEdgeAttr(source: T, target: T, key: string): AttributeValue {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    this.requireEdgeExists(source, target);
    if (!this.raw.hasEdgeAttr(source, target, key)) {
      this.operationFailed("Edge attribute lookup failed: key not found.");
    }
    return this.raw.getEdgeAttr(source, target, key);
  }

  tryGetEdgeAttr(source: T, target: T, key: string): AttributeValue | null {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    return this.raw.tryGetEdgeAttr(source, target, key);
  }

  setEdgeAttr(source: T, target: T, key: string, value: AttributeValue): void {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    assertAttributeValue(value, "value");
    this.raw.setEdgeAttr(source, target, key, value);
  }

  getEdgeNumericAttr(source: T, target: T, key: string): number {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    this.requireEdgeExists(source, target);
    return this.raw.getEdgeNumericAttr(source, target, key);
  }

  bfsEdges(start: T): TraversalEdge<T>[] {
    this.assertNode(start, "start");
    this.requireNodeExists(start);
    return toArray(this.raw.bfsEdges(start));
  }

  bfsTree(start: T): TraversalTree<T> {
    this.assertNode(start, "start");
    this.requireNodeExists(start);
    return this.raw.bfsTree(start);
  }

  bfsSuccessors(start: T): TraversalSuccessorEntry<T>[] {
    this.assertNode(start, "start");
    this.requireNodeExists(start);
    return toArray(this.raw.bfsSuccessors(start));
  }

  dfsEdges(start: T): TraversalEdge<T>[] {
    this.assertNode(start, "start");
    this.requireNodeExists(start);
    return toArray(this.raw.dfsEdges(start));
  }

  dfsTree(start: T): TraversalTree<T> {
    this.assertNode(start, "start");
    this.requireNodeExists(start);
    return this.raw.dfsTree(start);
  }

  dfsPredecessors(start: T): TraversalPredecessorEntry<T>[] {
    this.assertNode(start, "start");
    this.requireNodeExists(start);
    return toArray(this.raw.dfsPredecessors(start));
  }

  dfsSuccessors(start: T): TraversalSuccessorEntry<T>[] {
    this.assertNode(start, "start");
    this.requireNodeExists(start);
    return toArray(this.raw.dfsSuccessors(start));
  }

  shortestPath(source: T, target: T): T[] {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    this.requireNodeExists(source);
    this.requireNodeExists(target);
    return this.runPathLookup(() => toArray(this.raw.shortestPath(source, target)));
  }

  shortestPathWeighted(source: T, target: T, weightKey = "weight"): T[] {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    this.requireWeightKey(weightKey);
    this.requireNodeExists(source);
    this.requireNodeExists(target);
    return this.runPathLookup(() => toArray(this.raw.shortestPathWeighted(source, target, weightKey)));
  }

  shortestPathLength(source: T, target: T): number {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    this.requireNodeExists(source);
    this.requireNodeExists(target);
    return this.runPathLookup(() => this.raw.shortestPathLength(source, target));
  }

  shortestPathLengthWeighted(source: T, target: T, weightKey = "weight"): number {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    this.requireWeightKey(weightKey);
    this.requireNodeExists(source);
    this.requireNodeExists(target);
    return this.runPathLookup(() => this.raw.shortestPathLengthWeighted(source, target, weightKey));
  }

  dijkstraPath(source: T, target: T): T[] {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    this.requireNodeExists(source);
    this.requireNodeExists(target);
    return this.runPathLookup(() => toArray(this.raw.dijkstraPath(source, target)));
  }

  dijkstraPathWeighted(source: T, target: T, weightKey = "weight"): T[] {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    this.requireWeightKey(weightKey);
    this.requireNodeExists(source);
    this.requireNodeExists(target);
    return this.runPathLookup(() => toArray(this.raw.dijkstraPathWeighted(source, target, weightKey)));
  }

  dijkstraShortestPaths(source: T): SingleSourceShortestPathResult<T> {
    this.assertNode(source, "source");
    this.requireNodeExists(source);
    return toSingleSourceShortestPathResult(this.raw.dijkstraShortestPaths(source));
  }

  dijkstraPathLengths(source: T): ShortestPathDistanceEntry<T>[] {
    this.assertNode(source, "source");
    this.requireNodeExists(source);
    return toArray(this.raw.dijkstraPathLengths(source));
  }

  dijkstraPathLength(source: T, target: T): number {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    this.requireNodeExists(source);
    this.requireNodeExists(target);
    return this.runPathLookup(() => this.raw.dijkstraPathLength(source, target));
  }

  dijkstraPathLengthWeighted(source: T, target: T, weightKey = "weight"): number {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    this.requireWeightKey(weightKey);
    this.requireNodeExists(source);
    this.requireNodeExists(target);
    return this.runPathLookup(() => this.raw.dijkstraPathLengthWeighted(source, target, weightKey));
  }

  bellmanFordPath(source: T, target: T): T[] {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    this.requireNodeExists(source);
    this.requireNodeExists(target);
    return this.runPathLookup(() => toArray(this.raw.bellmanFordPath(source, target)));
  }

  bellmanFordPathWeighted(source: T, target: T, weightKey = "weight"): T[] {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    this.requireWeightKey(weightKey);
    this.requireNodeExists(source);
    this.requireNodeExists(target);
    return this.runPathLookup(() => toArray(this.raw.bellmanFordPathWeighted(source, target, weightKey)));
  }

  bellmanFordShortestPaths(source: T): SingleSourceShortestPathResult<T> {
    this.assertNode(source, "source");
    this.requireNodeExists(source);
    return toSingleSourceShortestPathResult(this.raw.bellmanFordShortestPaths(source));
  }

  bellmanFordPathLength(source: T, target: T): number {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    this.requireNodeExists(source);
    this.requireNodeExists(target);
    return this.runPathLookup(() => this.raw.bellmanFordPathLength(source, target));
  }

  bellmanFordPathLengthWeighted(source: T, target: T, weightKey = "weight"): number {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    this.requireWeightKey(weightKey);
    this.requireNodeExists(source);
    this.requireNodeExists(target);
    return this.runPathLookup(() => this.raw.bellmanFordPathLengthWeighted(source, target, weightKey));
  }

  dagShortestPaths(source: T): SingleSourceShortestPathResult<T> {
    this.assertNode(source, "source");
    this.requireNodeExists(source);
    return toSingleSourceShortestPathResult(this.raw.dagShortestPaths(source));
  }

  floydWarshallAllPairsShortestPaths(): number[][] {
    return toAllPairsShortestPathMatrix(this.raw.floydWarshallAllPairsShortestPaths());
  }

  floydWarshallAllPairsShortestPathsMap(): AllPairsShortestPathSourceEntry<T>[] {
    return toAllPairsShortestPathMap(this.raw.floydWarshallAllPairsShortestPathsMap());
  }

  kruskalMinimumSpanningTree(): SpanningTreeEdge<T>[] {
    return toArray(this.raw.kruskalMinimumSpanningTree());
  }

  primMinimumSpanningTree(root: T): SpanningTreeEdge<T>[] {
    this.assertNode(root, "root");
    this.requireNodeExists(root);
    return toArray(this.raw.primMinimumSpanningTree(root));
  }

  hasEdgeId(edgeId: number): boolean {
    assertEdgeId(edgeId);
    return this.raw.hasEdgeId(edgeId);
  }

  edgeIds(): number[] {
    return toArray(this.raw.edgeIds());
  }

  edgeIdsBetween(source: T, target: T): number[] {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    return toArray(this.raw.edgeIdsBetween(source, target));
  }

  getEdgeEndpoints(edgeId: number) {
    assertEdgeId(edgeId);
    this.requireEdgeIdExists(edgeId);
    return toEdgeEndpoints(this.raw.getEdgeEndpoints(edgeId));
  }

  getEdgeWeightById(edgeId: number): number {
    assertEdgeId(edgeId);
    this.requireEdgeIdExists(edgeId);
    return this.raw.getEdgeWeightById(edgeId);
  }

  setEdgeWeightById(edgeId: number, weight: number): void {
    assertEdgeId(edgeId);
    assertFiniteNumber(weight, "weight");
    this.requireEdgeIdExists(edgeId);
    this.raw.setEdgeWeightById(edgeId, weight);
  }

  hasEdgeAttrById(edgeId: number, key: string): boolean {
    assertEdgeId(edgeId);
    return this.raw.hasEdgeAttrById(edgeId, key);
  }

  getEdgeAttrById(edgeId: number, key: string): AttributeValue {
    assertEdgeId(edgeId);
    this.requireEdgeIdExists(edgeId);
    if (!this.raw.hasEdgeAttrById(edgeId, key)) {
      this.operationFailed("Edge attribute lookup failed: key not found.");
    }
    return this.raw.getEdgeAttrById(edgeId, key);
  }

  tryGetEdgeAttrById(edgeId: number, key: string): AttributeValue | null {
    assertEdgeId(edgeId);
    return this.raw.tryGetEdgeAttrById(edgeId, key);
  }

  setEdgeAttrById(edgeId: number, key: string, value: AttributeValue): void {
    assertEdgeId(edgeId);
    assertAttributeValue(value, "value");
    this.requireEdgeIdExists(edgeId);
    this.raw.setEdgeAttrById(edgeId, key, value);
  }

  getEdgeNumericAttrById(edgeId: number, key: string): number {
    assertEdgeId(edgeId);
    this.requireEdgeIdExists(edgeId);
    return this.raw.getEdgeNumericAttrById(edgeId, key);
  }

  removeEdgeById(edgeId: number): void {
    assertEdgeId(edgeId);
    this.requireEdgeIdExists(edgeId);
    this.raw.removeEdgeById(edgeId);
  }

  clear(): void {
    this.raw.clear();
  }

  dispose(): void {
    if (this.rawObject === null) {
      return;
    }
    this.rawObject.delete();
    this.rawObject = null;
  }
}

export class MultiGraphInt extends BaseMultiGraph<number> implements MultiGraph<number> {
  constructor() {
    super(() => new runtime.MultiGraphInt(), assertIntNodeId);
  }
}

export class MultiGraphStr extends BaseMultiGraph<string> implements MultiGraph<string> {
  constructor() {
    super(() => new runtime.MultiGraphStr(), assertStringNodeId);
  }
}

export class MultiDiGraphInt extends BaseMultiGraph<number> implements MultiDiGraph<number> {
  constructor() {
    super(() => new runtime.MultiDiGraphInt(), assertIntNodeId);
  }
}

export class MultiDiGraphStr extends BaseMultiGraph<string> implements MultiDiGraph<string> {
  constructor() {
    super(() => new runtime.MultiDiGraphStr(), assertStringNodeId);
  }
}
