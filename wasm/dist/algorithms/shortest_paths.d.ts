import type { AllPairsShortestPathSourceEntry, NodeId, SingleSourceShortestPathResult } from "../types.js";
import type { RawSingleSourceShortestPathResult } from "../internal/wasm_types.js";
export declare function toSingleSourceShortestPathResult<T extends NodeId>(raw: RawSingleSourceShortestPathResult<T>): SingleSourceShortestPathResult<T>;
export declare function toAllPairsShortestPathMatrix(raw: readonly (readonly number[])[]): number[][];
export declare function toAllPairsShortestPathMap<T extends NodeId>(raw: readonly AllPairsShortestPathSourceEntry<T>[]): AllPairsShortestPathSourceEntry<T>[];
export type { AllPairsShortestPathDistanceEntry, AllPairsShortestPathSourceEntry, ShortestPathDistanceEntry, ShortestPathPredecessorEntry, SingleSourceShortestPathResult, } from "../types.js";
