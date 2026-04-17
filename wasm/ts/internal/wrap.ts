import type { EdgeEndpoints, NodeId } from "../types.js";
import type { RawEdgeEndpoints } from "./wasm_types.js";

export function toArray<T>(value: Iterable<T> | ArrayLike<T>): T[] {
  return Array.from(value as Iterable<T>);
}

export function toEdgeEndpoints<T extends NodeId>(value: RawEdgeEndpoints<T>): EdgeEndpoints<T> {
  return {
    source: () => value.source(),
    target: () => value.target(),
  };
}
