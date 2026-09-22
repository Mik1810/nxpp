import assert from "node:assert/strict";

import {
    createNxpp,
    createNxppRuntime,
    createNxppWithLoader,
} from "../../dist/runtime/index.js";
import { createBrowserRuntimeLoader } from "../../dist/runtime/browser.js";

assert.throws(
    () => createNxppRuntime({}),
    /nxpp wasm runtime initialization failed: missing required class GraphInt\./,
);

const loaderOptions = { print() {} };
let receivedOptions;
await assert.rejects(
    createNxppWithLoader({
        async load(options) {
            receivedOptions = options;
            throw new Error("loader fixture failed");
        },
    }, loaderOptions),
    (error) => {
        assert.match(error.message, /^nxpp wasm runtime initialization failed: loader fixture failed$/);
        assert.equal(error.cause?.message, "loader fixture failed");
        return true;
    },
);
assert.equal(receivedOptions, loaderOptions, "the runtime-neutral boundary must forward loader options");

let browserModuleOptions;
const browserLoader = createBrowserRuntimeLoader(
    async (options) => {
        browserModuleOptions = options;
        return { marker: "raw-module" };
    },
    new URL("https://example.test/assets/nxpp_browser.wasm"),
);
assert.deepEqual(
    await browserLoader.load({
        print() {},
        locateFile(path, prefix) {
            return `${prefix}fallback/${path}`;
        },
    }),
    { marker: "raw-module" },
);
assert.equal(
    browserModuleOptions.locateFile("nxpp_browser.wasm", "/ignored/"),
    "https://example.test/assets/nxpp_browser.wasm",
    "the browser adapter must own wasm asset resolution",
);
assert.equal(
    browserModuleOptions.locateFile("support.data", "/runtime/"),
    "/runtime/fallback/support.data",
    "the browser adapter must preserve non-wasm asset lookup",
);
assert.equal(typeof browserModuleOptions.print, "function");

await assert.rejects(
    createNxppWithLoader({ async load() { return {}; } }),
    /nxpp wasm runtime initialization failed: missing required class GraphInt\./,
);

const [first, second] = await Promise.all([
    createNxpp({ print() {} }),
    createNxpp(),
]);

assert.notEqual(first.GraphInt, second.GraphInt, "contexts must own distinct facade classes");

const graphCases = [
    [first.GraphInt, 1, 2],
    [first.GraphStr, "a", "b"],
    [first.DiGraphInt, 1, 2],
    [first.DiGraphStr, "a", "b"],
    [first.MultiGraphInt, 1, 2],
    [first.MultiGraphStr, "a", "b"],
    [first.MultiDiGraphInt, 1, 2],
    [first.MultiDiGraphStr, "a", "b"],
];

for (const [GraphClass, source, target] of graphCases) {
    const graph = new GraphClass();
    graph.addEdge(source, target, 1);
    assert.equal(graph.hasEdge(source, target), true);
    graph.dispose();
}

const firstGraph = new first.DiGraphInt();
const secondGraph = new second.DiGraphInt();
firstGraph.addEdge(1, 2, 1);
secondGraph.addEdge(10, 20, 1);
firstGraph.dispose();

assert.deepEqual(secondGraph.nodes(), [10, 20], "disposing one context must not affect another");

const secondSubgraph = secondGraph.subgraph([10, 20]);
assert.equal(secondSubgraph instanceof second.DiGraphInt, true);
assert.equal(secondSubgraph instanceof first.DiGraphInt, false);

secondSubgraph.dispose();
secondGraph.dispose();
