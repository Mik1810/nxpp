import { toArray } from "../internal/wrap.js";

import type {
  CentralityScoreEntry,
  NodeId,
} from "../types.js";

export function toCentralityScores<T extends NodeId>(
  raw: Iterable<CentralityScoreEntry<T>> | ArrayLike<CentralityScoreEntry<T>>,
): CentralityScoreEntry<T>[] {
  return toArray(raw);
}

export type { CentralityScoreEntry } from "../types.js";
