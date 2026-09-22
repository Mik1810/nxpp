import type { NxppModuleOptions, NxppRuntime, NxppRuntimeLoader } from "./context.js";
export declare const nodeRuntimeLoader: NxppRuntimeLoader;
export declare function createNxpp(options?: NxppModuleOptions): Promise<NxppRuntime>;
