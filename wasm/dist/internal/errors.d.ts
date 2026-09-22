export declare const disposedGraphMessage = "WASM graph operation failed: graph has been disposed.";
export interface WasmExceptionRuntime {
    getExceptionMessage(error: unknown): [string, string];
    decrementExceptionRefcount(error: unknown): void;
}
export declare function normalizeWasmGraphError(error: unknown, runtime?: WasmExceptionRuntime): Error;
export declare function wrapRawGraph<T extends object>(raw: T, runtime: WasmExceptionRuntime): T;
