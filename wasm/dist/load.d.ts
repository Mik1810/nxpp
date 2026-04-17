import type { RawRuntimeModule } from "./internal/wasm_types.js";
export declare function createNxpp(options?: Record<string, unknown>): Promise<RawRuntimeModule>;
export declare function loadNxppRuntime(options?: Record<string, unknown>): Promise<RawRuntimeModule>;
export declare const runtime: RawRuntimeModule;
