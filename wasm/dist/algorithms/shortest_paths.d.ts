import type { AllPairsShortestPathSourceEntry, NodeId, SingleSourceShortestPathResult } from "../types.js";
import type { RawSingleSourceShortestPathResult } from "../internal/wasm_types.js";
export declare function toSingleSourceShortestPathResult<T extends NodeId>(raw: RawSingleSourceShortestPathResult<T>): SingleSourceShortestPathResult<T>;
export declare function toAllPairsShortestPathMatrix(raw: Iterable<Iterable<number> | ArrayLike<number>> | ArrayLike<Iterable<number> | ArrayLike<number>>): number[][];
export declare function toAllPairsShortestPathMap<T extends NodeId>(raw: Iterable<AllPairsShortestPathSourceEntry<T>> | ArrayLike<AllPairsShortestPathSourceEntry<T>>): AllPairsShortestPathSourceEntry<T>[];
export type { AllPairsShortestPathDistanceEntry, AllPairsShortestPathSourceEntry, ShortestPathDistanceEntry, ShortestPathPredecessorEntry, SingleSourceShortestPathResult, } from "../types.js";
