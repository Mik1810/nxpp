import { createSimpleGraphClasses } from "../core/graph.js";
import { createMultiGraphClasses } from "../core/multigraph.js";
const REQUIRED_RUNTIME_CLASSES = [
    "GraphInt",
    "GraphStr",
    "DiGraphInt",
    "DiGraphStr",
    "MultiGraphInt",
    "MultiGraphStr",
    "MultiDiGraphInt",
    "MultiDiGraphStr",
];
export function requireRawRuntimeModule(runtime) {
    if (!runtime || typeof runtime !== "object") {
        throw new Error("nxpp wasm runtime initialization failed: module factory returned an invalid value.");
    }
    for (const className of REQUIRED_RUNTIME_CLASSES) {
        const ctor = runtime[className];
        if (typeof ctor !== "function") {
            throw new Error(`nxpp wasm runtime initialization failed: missing required class ${className}.`);
        }
    }
    return runtime;
}
export function createNxppRuntime(runtime) {
    const rawRuntime = requireRawRuntimeModule(runtime);
    return Object.freeze({
        ...createSimpleGraphClasses(rawRuntime),
        ...createMultiGraphClasses(rawRuntime),
    });
}
