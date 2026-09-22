import type { CentralityScoreEntry, NodeId } from "../types.js";
export declare function toCentralityScores<T extends NodeId>(raw: readonly CentralityScoreEntry<T>[]): CentralityScoreEntry<T>[];
export type { CentralityScoreEntry } from "../types.js";
