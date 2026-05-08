import type { CentralityScoreEntry, NodeId } from "../types.js";
export declare function toCentralityScores<T extends NodeId>(raw: Iterable<CentralityScoreEntry<T>> | ArrayLike<CentralityScoreEntry<T>>): CentralityScoreEntry<T>[];
export type { CentralityScoreEntry } from "../types.js";
