import { runtime } from "../load.js";
import {
  assertEdgeId,
  assertFiniteNumber,
  assertIntNodeId,
  assertStringNodeId,
} from "../internal/assert.js";
import { toArray, toEdgeEndpoints } from "../internal/wrap.js";

import type { MultiDiGraph, MultiGraph, NodeId } from "../types.js";
import type { RawMultiGraph } from "../internal/wasm_types.js";

class BaseMultiGraph<T extends NodeId> {
  protected readonly raw: RawMultiGraph<T>;
  private readonly assertNode: (value: unknown, label: string) => asserts value is T;

  constructor(
    factory: () => RawMultiGraph<T>,
    assertNode: (value: unknown, label: string) => asserts value is T,
  ) {
    this.raw = factory();
    this.assertNode = assertNode;
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
    return toArray(this.raw.neighbors(id));
  }

  removeNode(id: T): void {
    this.assertNode(id, "id");
    this.raw.removeNode(id);
  }

  removeEdge(source: T, target: T): void {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    this.raw.removeEdge(source, target);
  }

  getEdgeWeight(source: T, target: T): number {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    return this.raw.getEdgeWeight(source, target);
  }

  setEdgeWeight(source: T, target: T, weight: number): void {
    this.assertNode(source, "source");
    this.assertNode(target, "target");
    assertFiniteNumber(weight, "weight");
    this.raw.setEdgeWeight(source, target, weight);
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
    return toEdgeEndpoints(this.raw.getEdgeEndpoints(edgeId));
  }

  getEdgeWeightById(edgeId: number): number {
    assertEdgeId(edgeId);
    return this.raw.getEdgeWeightById(edgeId);
  }

  setEdgeWeightById(edgeId: number, weight: number): void {
    assertEdgeId(edgeId);
    assertFiniteNumber(weight, "weight");
    this.raw.setEdgeWeightById(edgeId, weight);
  }

  removeEdgeById(edgeId: number): void {
    assertEdgeId(edgeId);
    this.raw.removeEdgeById(edgeId);
  }

  clear(): void {
    this.raw.clear();
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
