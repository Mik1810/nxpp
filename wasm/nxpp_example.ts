import nxpp from "@mik1810/nxpp-wasm";
import type { DiGraph } from "@mik1810/nxpp-wasm";

const graph: DiGraph<number> = new nxpp.DiGraphInt();
graph.addEdge(1, 2, 1);
graph.addEdge(2, 3, 2);
graph.addEdge(1, 3, 5);

console.log("neighbors(1):", graph.neighbors(1));
console.log("weight(1,3):", graph.getEdgeWeight(1, 3));
