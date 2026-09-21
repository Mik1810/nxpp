import type { NxppRuntime } from "./context.js";
export type NxppModuleOptions = Record<string, unknown>;
export declare function createNxpp(options?: NxppModuleOptions): Promise<NxppRuntime>;
