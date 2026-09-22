import {
  assertAttributeValue,
  assertEdgeId,
  assertFiniteNumber,
  assertIntNodeId,
  assertStringNodeId,
} from "../internal/assert.js";
import { toArray, toEdgeEndpoints } from "../internal/wrap.js";

import type {
  AttributeValue,
  ConnectedComponents,
  MultiDiGraph,
  MultiGraph,
  NodeId,
  StronglyConnectedComponents,
} from "../types.js";
import type { RawMultiGraph, RawRuntimeModule } from "../internal/wasm_types.js";
import { BaseGraph } from "./graph.js";

export interface MultiGraphIntFacade extends MultiGraph<number>, ConnectedComponents<number> {
  subgraph(nodes: number[]): MultiGraphIntFacade;
}

export interface MultiGraphStrFacade extends MultiGraph<string>, ConnectedComponents<string> {
  subgraph(nodes: string[]): MultiGraphStrFacade;
}

export interface MultiDiGraphIntFacade extends MultiDiGraph<number>, StronglyConnectedComponents<number> {
  subgraph(nodes: number[]): MultiDiGraphIntFacade;
}

export interface MultiDiGraphStrFacade extends MultiDiGraph<string>, StronglyConnectedComponents<string> {
  subgraph(nodes: string[]): MultiDiGraphStrFacade;
}

export interface MultiGraphClasses {
  readonly MultiGraphInt: new (raw?: RawMultiGraph<number>) => MultiGraphIntFacade;
  readonly MultiGraphStr: new (raw?: RawMultiGraph<string>) => MultiGraphStrFacade;
  readonly MultiDiGraphInt: new (raw?: RawMultiGraph<number>) => MultiDiGraphIntFacade;
  readonly MultiDiGraphStr: new (raw?: RawMultiGraph<string>) => MultiDiGraphStrFacade;
}

abstract class BaseMultiGraph<T extends NodeId> extends BaseGraph<T, RawMultiGraph<T>> {
  constructor(
    factory: (() => RawMultiGraph<T>) | RawMultiGraph<T>,
    assertNode: (value: unknown, label: string) => asserts value is T,
    runtime: RawRuntimeModule,
  ) {
    super(factory, assertNode, runtime);
  }

  private requireEdgeIdExists(edgeId: number): void {
    if (!this.raw.hasEdgeId(edgeId)) {
      this.operationFailed("Edge lookup failed: edge not found.");
    }
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
}

export function createMultiGraphClasses(runtime: RawRuntimeModule): MultiGraphClasses {
  class MultiGraphInt extends BaseMultiGraph<number> implements MultiGraph<number>, ConnectedComponents<number> {
    constructor(raw?: RawMultiGraph<number>) {
      super(raw ?? (() => new runtime.MultiGraphInt()), assertIntNodeId, runtime);
    }

    protected createFromRaw(raw: RawMultiGraph<number>): this {
      return new MultiGraphInt(raw) as this;
    }

    connectedComponents(): number[][] {
      return this.readConnectedComponents();
    }
  }

  class MultiGraphStr extends BaseMultiGraph<string> implements MultiGraph<string>, ConnectedComponents<string> {
    constructor(raw?: RawMultiGraph<string>) {
      super(raw ?? (() => new runtime.MultiGraphStr()), assertStringNodeId, runtime);
    }

    protected createFromRaw(raw: RawMultiGraph<string>): this {
      return new MultiGraphStr(raw) as this;
    }

    connectedComponents(): string[][] {
      return this.readConnectedComponents();
    }
  }

  class MultiDiGraphInt extends BaseMultiGraph<number> implements MultiDiGraph<number>, StronglyConnectedComponents<number> {
    constructor(raw?: RawMultiGraph<number>) {
      super(raw ?? (() => new runtime.MultiDiGraphInt()), assertIntNodeId, runtime);
    }

    protected createFromRaw(raw: RawMultiGraph<number>): this {
      return new MultiDiGraphInt(raw) as this;
    }

    stronglyConnectedComponents(): number[][] {
      return this.readStronglyConnectedComponents();
    }
  }

  class MultiDiGraphStr extends BaseMultiGraph<string> implements MultiDiGraph<string>, StronglyConnectedComponents<string> {
    constructor(raw?: RawMultiGraph<string>) {
      super(raw ?? (() => new runtime.MultiDiGraphStr()), assertStringNodeId, runtime);
    }

    protected createFromRaw(raw: RawMultiGraph<string>): this {
      return new MultiDiGraphStr(raw) as this;
    }

    stronglyConnectedComponents(): string[][] {
      return this.readStronglyConnectedComponents();
    }
  }

  return { MultiGraphInt, MultiGraphStr, MultiDiGraphInt, MultiDiGraphStr };
}
