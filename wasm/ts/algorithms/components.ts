import { toArray } from "../internal/wrap.js";

import type { NodeId } from "../types.js";

export function toComponentGroups<T extends NodeId>(
  raw: readonly (readonly T[])[],
): T[][] {
  return toArray(raw).map((component) => toArray(component));
}

export type {
  ConnectedComponents,
  StronglyConnectedComponents,
} from "../types.js";
