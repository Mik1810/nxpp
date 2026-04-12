import nxpp from "@mik1810/nxpp-wasm";

const graph = new nxpp.DiGraph();
graph.addEdge(1, 2, 1);
graph.addEdge(2, 3, 2);
graph.addEdge(1, 3, 5);

const result = graph.dijkstraShortestPaths(1);
console.log("distance 1->3:", result.distanceTo(3));
console.log("path 1->3:", Array.from(result.pathTo(3)));
