import type { RawRuntimeModule } from "./internal/wasm_types.js";

const REQUIRED_RUNTIME_CLASSES = [
  "GraphInt",
  "GraphStr",
  "DiGraphInt",
  "DiGraphStr",
  "MultiGraphInt",
  "MultiGraphStr",
  "MultiDiGraphInt",
  "MultiDiGraphStr",
] as const;

function assertRuntimeShape(runtime: unknown): asserts runtime is RawRuntimeModule {
  if (!runtime || typeof runtime !== "object") {
    throw new Error("nxpp wasm runtime did not load a valid module object.");
  }

  for (const className of REQUIRED_RUNTIME_CLASSES) {
    const ctor = (runtime as Record<string, unknown>)[className];
    if (typeof ctor !== "function") {
      throw new Error(`nxpp wasm runtime is missing required class: ${className}`);
    }
  }
}

let runtimePromise: Promise<RawRuntimeModule> | undefined;

async function loadCreateModule(): Promise<(options?: Record<string, unknown>) => Promise<unknown>> {
  const module = await import("../build/nxpp_node.mjs");
  return module.default as (options?: Record<string, unknown>) => Promise<unknown>;
}

export async function createNxpp(options: Record<string, unknown> = {}): Promise<RawRuntimeModule> {
  const createNxppModule = await loadCreateModule();
  const runtime = await createNxppModule(options);
  assertRuntimeShape(runtime);
  return runtime;
}

export async function loadNxppRuntime(options: Record<string, unknown> = {}): Promise<RawRuntimeModule> {
  if (!runtimePromise) {
    runtimePromise = createNxpp(options);
  }
  return runtimePromise;
}

export const runtime = await loadNxppRuntime();
