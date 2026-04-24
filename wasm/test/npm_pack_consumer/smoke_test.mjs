import nxpp from "@mik1810/nxpp-wasm";

const g = new nxpp.DiGraphInt();
g.addEdge(1, 2, 3);
g.addEdge(2, 3, 4);

const path = g.dijkstraPath(1, 3);
if (JSON.stringify(path) !== JSON.stringify([1, 2, 3])) {
  throw new Error(`Unexpected path: ${JSON.stringify(path)}`);
}

const weight = g.getEdgeWeight(1, 2);
if (weight !== 3) {
  throw new Error(`Unexpected edge weight: ${weight}`);
}

g.dispose();

console.log("[WASM-NPM-PACK] consumer-smoke-ok");
