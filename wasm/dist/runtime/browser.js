import { createNxppWithLoader } from "./context.js";
function isLocateFile(value) {
    return typeof value === "function";
}
export function createBrowserRuntimeLoader(createModule, wasmUrl) {
    const resolvedWasmUrl = String(wasmUrl);
    return Object.freeze({
        async load(options) {
            const fallbackLocateFile = options.locateFile;
            return createModule({
                ...options,
                locateFile(path, prefix) {
                    if (path.endsWith(".wasm")) {
                        return resolvedWasmUrl;
                    }
                    return isLocateFile(fallbackLocateFile) ? fallbackLocateFile(path, prefix) : `${prefix}${path}`;
                },
            });
        },
    });
}
export function createBrowserNxpp(createModule, wasmUrl, options = {}) {
    return createNxppWithLoader(createBrowserRuntimeLoader(createModule, wasmUrl), options);
}
