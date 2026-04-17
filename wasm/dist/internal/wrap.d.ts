import type { EdgeEndpoints, NodeId } from "../types.js";
import type { RawEdgeEndpoints } from "./wasm_types.js";
export declare function toArray<T>(value: Iterable<T> | ArrayLike<T>): T[];
export declare function toEdgeEndpoints<T extends NodeId>(value: RawEdgeEndpoints<T>): EdgeEndpoints<T>;
