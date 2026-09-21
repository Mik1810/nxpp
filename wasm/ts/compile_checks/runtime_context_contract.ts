import { createNxpp, createNxppRuntime } from "../runtime/index.js";

import type { NxppRuntime } from "../runtime/index.js";
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
void exerciseRuntimeContext;
