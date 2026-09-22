import type { NxppModuleOptions, NxppRuntime, NxppRuntimeLoader } from "./context.js";
export type BrowserModuleFactory = (options?: NxppModuleOptions) => Promise<unknown>;
export declare function createBrowserRuntimeLoader(createModule: BrowserModuleFactory, wasmUrl: string | URL): NxppRuntimeLoader;
export declare function createBrowserNxpp(createModule: BrowserModuleFactory, wasmUrl: string | URL, options?: NxppModuleOptions): Promise<NxppRuntime>;
