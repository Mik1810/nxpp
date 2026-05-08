import type { NodeId } from "../types.js";
export declare function toComponentGroups<T extends NodeId>(raw: Iterable<Iterable<T> | ArrayLike<T>> | ArrayLike<Iterable<T> | ArrayLike<T>>): T[][];
export type { ConnectedComponents, StronglyConnectedComponents, } from "../types.js";
