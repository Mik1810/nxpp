import { runtime } from "../load.js";
import {
  assertFiniteNumber,
  assertIntNodeId,
  assertStringNodeId,
} from "../internal/assert.js";
import { toArray } from "../internal/wrap.js";

import type { DiGraph, Graph, NodeId } from "../types.js";
import type { RawSimpleGraph } from "../internal/wasm_types.js";

class BaseSimpleGraph<T extends NodeId> {
  protected readonly raw: RawSimpleGraph<T>;
  private readonly assertNode: (value: unknown, label: string) => asserts value is T;

  constructor(
    factory: () => RawSimpleGraph<T>,
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

  clear(): void {
    this.raw.clear();
  }
}

export class GraphInt extends BaseSimpleGraph<number> implements Graph<number> {
  constructor() {
    super(() => new runtime.GraphInt(), assertIntNodeId);
  }
}

export class GraphStr extends BaseSimpleGraph<string> implements Graph<string> {
  constructor() {
    super(() => new runtime.GraphStr(), assertStringNodeId);
  }
}

export class DiGraphInt extends BaseSimpleGraph<number> implements DiGraph<number> {
  constructor() {
    super(() => new runtime.DiGraphInt(), assertIntNodeId);
  }
}

export class DiGraphStr extends BaseSimpleGraph<string> implements DiGraph<string> {
  constructor() {
    super(() => new runtime.DiGraphStr(), assertStringNodeId);
  }
}
