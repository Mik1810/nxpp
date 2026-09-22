import { createNxpp, createNxppRuntime, createNxppWithLoader, nodeRuntimeLoader } from "../runtime/index.js";

import type { NxppRuntime, NxppRuntimeLoader } from "../runtime/index.js";
import type { DiGraph, MultiGraph } from "../types.js";

async function exerciseRuntimeContext(): Promise<void> {
  const nxpp: NxppRuntime = await createNxpp();
  const graph: DiGraph<number> = new nxpp.DiGraphInt();
  const multigraph: MultiGraph<string> = new nxpp.MultiGraphStr();
  const subgraph: DiGraph<number> = graph.subgraph([]);

  subgraph.dispose();
  graph.dispose();
  multigraph.dispose();
}

void createNxppRuntime;
void createNxppWithLoader;
void nodeRuntimeLoader;
const loader: NxppRuntimeLoader = nodeRuntimeLoader;
void loader;
void exerciseRuntimeContext;
