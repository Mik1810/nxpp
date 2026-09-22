import { createNxpp } from "@mik1810/nxpp-wasm";

import type { DiGraph, NxppRuntime } from "@mik1810/nxpp-wasm";

async function exercisePublishedTypes(): Promise<void> {
  const nxpp: NxppRuntime = await createNxpp();
  const graph: DiGraph<number> = new nxpp.DiGraphInt();
  graph.dispose();
}

void exercisePublishedTypes;
