import nxpp from "@mik1810/nxpp-wasm";

const graph = new nxpp.DiGraphInt();
graph.addEdge(1, 2, 2);
graph.addEdge(1, 3, 6);
graph.addEdge(2, 3, 1);
graph.addEdge(2, 4, 5);
graph.addEdge(3, 4, 1);
graph.addEdge(4, 5, 3);
graph.addEdge(2, 5, 10);

console.log("neighbors(1):", graph.neighbors(1));
console.log("weight(1,3):", graph.getEdgeWeight(1, 3));

const dijkstra = graph.dijkstraShortestPaths(1);
console.log("dijkstra path 1 -> 5:", dijkstra.pathTo(5));
console.log("dijkstra distances from 1:", dijkstra.distance);
