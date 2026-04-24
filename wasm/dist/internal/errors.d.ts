export declare const disposedGraphMessage = "WASM graph operation failed: graph has been disposed.";
export declare function normalizeWasmGraphError(error: unknown): Error;
export declare function wrapRawGraph<T extends object>(raw: T): T;
