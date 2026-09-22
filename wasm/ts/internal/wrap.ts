import type { EdgeEndpoints, NodeId } from "../types.js";
import type { RawEdgeEndpoints } from "./wasm_types.js";

export function toArray<T>(value: readonly T[]): T[] {
  return [...value];
}

export function toEdgeEndpoints<T extends NodeId>(value: RawEdgeEndpoints<T>): EdgeEndpoints<T> {
  return {
    source: () => value.source,
    target: () => value.target,
  };
}
