import { createNxppWithLoader } from "./context.js";
export const nodeRuntimeLoader = Object.freeze({
    async load(options) {
        const module = await import("../../build/nxpp_node.mjs");
        const createNxppModule = module.default;
        return createNxppModule(options);
    },
});
export async function createNxpp(options = {}) {
    return createNxppWithLoader(nodeRuntimeLoader, options);
}
