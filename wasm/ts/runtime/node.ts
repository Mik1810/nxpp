import { createNxppRuntime } from "./context.js";

import type { NxppRuntime } from "./context.js";

export type NxppModuleOptions = Record<string, unknown>;

type CreateNxppModule = (options?: NxppModuleOptions) => Promise<unknown>;

async function loadCreateModule(): Promise<CreateNxppModule> {
  const module = await import("../../build/nxpp_node.mjs");
  return module.default as CreateNxppModule;
}

function initializationError(error: unknown): Error {
  if (error instanceof Error && error.message.startsWith("nxpp wasm runtime initialization failed:")) {
    return error;
  }

  const detail = error instanceof Error ? error.message : String(error);
  return new Error(`nxpp wasm runtime initialization failed: ${detail}`, { cause: error });
}

export async function createNxpp(options: NxppModuleOptions = {}): Promise<NxppRuntime> {
  try {
    const createNxppModule = await loadCreateModule();
    return createNxppRuntime(await createNxppModule(options));
  } catch (error) {
    throw initializationError(error);
  }
}
