import { createNxpp, createNxppRuntime, createNxppWithLoader, nodeRuntimeLoader } from "../runtime/index.js";
async function exerciseRuntimeContext() {
    const nxpp = await createNxpp();
    const graph = new nxpp.DiGraphInt();
    const multigraph = new nxpp.MultiGraphStr();
    const subgraph = graph.subgraph([]);
    subgraph.dispose();
    graph.dispose();
    multigraph.dispose();
}
void createNxppRuntime;
void createNxppWithLoader;
void nodeRuntimeLoader;
const loader = nodeRuntimeLoader;
void loader;
void exerciseRuntimeContext;
