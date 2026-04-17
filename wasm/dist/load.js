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
function assertRuntimeShape(runtime) {
    if (!runtime || typeof runtime !== "object") {
        throw new Error("nxpp wasm runtime did not load a valid module object.");
    }
    for (const className of REQUIRED_RUNTIME_CLASSES) {
        const ctor = runtime[className];
        if (typeof ctor !== "function") {
            throw new Error(`nxpp wasm runtime is missing required class: ${className}`);
        }
    }
}
let runtimePromise;
async function loadCreateModule() {
    const module = await import("../build/nxpp_node.mjs");
    return module.default;
}
export async function createNxpp(options = {}) {
    const createNxppModule = await loadCreateModule();
    const runtime = await createNxppModule(options);
    assertRuntimeShape(runtime);
    return runtime;
}
export async function loadNxppRuntime(options = {}) {
    if (!runtimePromise) {
        runtimePromise = createNxpp(options);
    }
    return runtimePromise;
}
export const runtime = await loadNxppRuntime();
