import { createNxppWithLoader } from "./context.js";

import type { NxppModuleOptions, NxppRuntime, NxppRuntimeLoader } from "./context.js";

type CreateNxppModule = (options?: NxppModuleOptions) => Promise<unknown>;

export const nodeRuntimeLoader: NxppRuntimeLoader = Object.freeze({
  async load(options: NxppModuleOptions): Promise<unknown> {
    const module = await import("../../build/nxpp_node.mjs");
    const createNxppModule = module.default as CreateNxppModule;
    return createNxppModule(options);
  },
});

export async function createNxpp(options: NxppModuleOptions = {}): Promise<NxppRuntime> {
  return createNxppWithLoader(nodeRuntimeLoader, options);
}
