import type { NodeId } from "../types.js";
export declare function toComponentGroups<T extends NodeId>(raw: readonly (readonly T[])[]): T[][];
export type { ConnectedComponents, StronglyConnectedComponents, } from "../types.js";
