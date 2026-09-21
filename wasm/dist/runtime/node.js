import { createNxppRuntime } from "./context.js";
async function loadCreateModule() {
    const module = await import("../../build/nxpp_node.mjs");
    return module.default;
}
function initializationError(error) {
    if (error instanceof Error && error.message.startsWith("nxpp wasm runtime initialization failed:")) {
        return error;
    }
    const detail = error instanceof Error ? error.message : String(error);
    return new Error(`nxpp wasm runtime initialization failed: ${detail}`, { cause: error });
}
export async function createNxpp(options = {}) {
    try {
        const createNxppModule = await loadCreateModule();
        return createNxppRuntime(await createNxppModule(options));
    }
    catch (error) {
        throw initializationError(error);
    }
}
