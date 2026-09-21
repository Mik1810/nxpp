import { createSimpleGraphClasses } from "../core/graph.js";
import { createMultiGraphClasses } from "../core/multigraph.js";

import type {
  DiGraphIntFacade,
  DiGraphStrFacade,
  GraphIntFacade,
  GraphStrFacade,
} from "../core/graph.js";
import type {
  MultiDiGraphIntFacade,
  MultiDiGraphStrFacade,
  MultiGraphIntFacade,
  MultiGraphStrFacade,
} from "../core/multigraph.js";
import type { RawRuntimeModule } from "../internal/wasm_types.js";

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

type FacadeConstructor<T> = new () => T;

export interface NxppRuntime {
  readonly GraphInt: FacadeConstructor<GraphIntFacade>;
  readonly GraphStr: FacadeConstructor<GraphStrFacade>;
  readonly DiGraphInt: FacadeConstructor<DiGraphIntFacade>;
  readonly DiGraphStr: FacadeConstructor<DiGraphStrFacade>;
  readonly MultiGraphInt: FacadeConstructor<MultiGraphIntFacade>;
  readonly MultiGraphStr: FacadeConstructor<MultiGraphStrFacade>;
  readonly MultiDiGraphInt: FacadeConstructor<MultiDiGraphIntFacade>;
  readonly MultiDiGraphStr: FacadeConstructor<MultiDiGraphStrFacade>;
}

export function requireRawRuntimeModule(runtime: unknown): RawRuntimeModule {
  if (!runtime || typeof runtime !== "object") {
    throw new Error("nxpp wasm runtime initialization failed: module factory returned an invalid value.");
  }

  for (const className of REQUIRED_RUNTIME_CLASSES) {
    const ctor = (runtime as Record<string, unknown>)[className];
    if (typeof ctor !== "function") {
      throw new Error(`nxpp wasm runtime initialization failed: missing required class ${className}.`);
    }
  }

  return runtime as RawRuntimeModule;
}

export function createNxppRuntime(runtime: unknown): NxppRuntime {
  const rawRuntime = requireRawRuntimeModule(runtime);
  return Object.freeze({
    ...createSimpleGraphClasses(rawRuntime),
    ...createMultiGraphClasses(rawRuntime),
  });
}
