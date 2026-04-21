import { toArray } from "../internal/wrap.js";

import type {
  AllPairsShortestPathDistanceEntry,
  AllPairsShortestPathSourceEntry,
  NodeId,
  ShortestPathDistanceEntry,
  ShortestPathPredecessorEntry,
  SingleSourceShortestPathResult,
} from "../types.js";
import type { RawSingleSourceShortestPathResult } from "../internal/wasm_types.js";

class SingleSourceShortestPathResultImpl<T extends NodeId> implements SingleSourceShortestPathResult<T> {
  readonly distance: ShortestPathDistanceEntry<T>[];
  readonly predecessor: ShortestPathPredecessorEntry<T>[];

  private readonly distanceMap: Map<T, number>;
  private readonly predecessorMap: Map<T, T>;

  constructor(raw: RawSingleSourceShortestPathResult<T>) {
    this.distance = toArray(raw.distance);
    this.predecessor = toArray(raw.predecessor);
    this.distanceMap = new Map(this.distance.map((entry) => [entry.node, entry.distance]));
    this.predecessorMap = new Map(this.predecessor.map((entry) => [entry.node, entry.predecessor]));
  }

  hasPathTo(target: T): boolean {
    const distance = this.distanceMap.get(target);
    return distance !== undefined && Number.isFinite(distance);
  }

  pathTo(target: T): T[] {
    const distance = this.distanceMap.get(target);
    if (distance === undefined) {
      throw new Error("Path reconstruction failed: target node not found in result.");
    }
    if (!Number.isFinite(distance)) {
      throw new Error("Path reconstruction failed: target node is unreachable.");
    }

    const path: T[] = [];
    let current = target;
    const maxHops = this.predecessorMap.size + 1;

    for (let hops = 0; hops <= maxHops; hops += 1) {
      path.push(current);
      const predecessor = this.predecessorMap.get(current);
      if (predecessor === undefined) {
        throw new Error("Path reconstruction failed: predecessor map is incomplete.");
      }
      if (predecessor === current) {
        path.reverse();
        return path;
      }
      current = predecessor;
    }

    throw new Error("Path reconstruction failed: predecessor cycle detected.");
  }
}

export function toSingleSourceShortestPathResult<T extends NodeId>(
  raw: RawSingleSourceShortestPathResult<T>,
): SingleSourceShortestPathResult<T> {
  return new SingleSourceShortestPathResultImpl(raw);
}

export function toAllPairsShortestPathMatrix(
  raw: Iterable<Iterable<number> | ArrayLike<number>> | ArrayLike<Iterable<number> | ArrayLike<number>>,
): number[][] {
  return toArray(raw).map((row) => toArray(row));
}

export function toAllPairsShortestPathMap<T extends NodeId>(
  raw: Iterable<AllPairsShortestPathSourceEntry<T>> | ArrayLike<AllPairsShortestPathSourceEntry<T>>,
): AllPairsShortestPathSourceEntry<T>[] {
  return toArray(raw).map((entry) => ({
    source: entry.source,
    distances: toArray(entry.distances),
  }));
}

export type {
  AllPairsShortestPathDistanceEntry,
  AllPairsShortestPathSourceEntry,
  ShortestPathDistanceEntry,
  ShortestPathPredecessorEntry,
  SingleSourceShortestPathResult,
} from "../types.js";
