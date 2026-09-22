import { createNxppWithLoader } from "./context.js";

import type {
  NxppModuleOptions,
  NxppRuntime,
  NxppRuntimeLoader,
} from "./context.js";

export type BrowserModuleFactory = (options?: NxppModuleOptions) => Promise<unknown>;

type LocateFile = (path: string, prefix: string) => string;

function isLocateFile(value: unknown): value is LocateFile {
  return typeof value === "function";
}

export function createBrowserRuntimeLoader(
  createModule: BrowserModuleFactory,
  wasmUrl: string | URL,
): NxppRuntimeLoader {
  const resolvedWasmUrl = String(wasmUrl);

  return Object.freeze({
    async load(options: NxppModuleOptions): Promise<unknown> {
      const fallbackLocateFile = options.locateFile;
      return createModule({
        ...options,
        locateFile(path: string, prefix: string): string {
          if (path.endsWith(".wasm")) {
            return resolvedWasmUrl;
          }
          return isLocateFile(fallbackLocateFile) ? fallbackLocateFile(path, prefix) : `${prefix}${path}`;
        },
      });
    },
  });
}

export function createBrowserNxpp(
  createModule: BrowserModuleFactory,
  wasmUrl: string | URL,
  options: NxppModuleOptions = {},
): Promise<NxppRuntime> {
  return createNxppWithLoader(createBrowserRuntimeLoader(createModule, wasmUrl), options);
}
