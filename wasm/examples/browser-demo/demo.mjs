import createNxppModule from "./nxpp_browser.mjs";
import { createBrowserNxpp } from "../../dist/runtime/browser.js";

const output = document.querySelector("#output");

function writeResult(text) {
  output.textContent = text;
}

function reconstructPath(predecessorEntries, source, target) {
  const predecessorByNode = new Map(
    predecessorEntries.map((entry) => [entry.node, entry.predecessor]),
  );

  if (!predecessorByNode.has(target)) {
    return null;
  }

  const path = [];
  let current = target;

  while (true) {
    path.push(current);
    if (current === source) {
      path.reverse();
      return path;
    }

    const predecessor = predecessorByNode.get(current);
    if (predecessor === undefined || predecessor === current) {
      return null;
    }

    current = predecessor;
  }
}

try {
  const nxpp = await createBrowserNxpp(
    createNxppModule,
    new URL("./nxpp_browser.wasm", import.meta.url),
  );

  const graph = new nxpp.DiGraphInt();
  try {
    graph.addEdge(1, 2, 2);
    graph.addEdge(1, 3, 6);
    graph.addEdge(2, 3, 1);
    graph.addEdge(3, 4, 1);

    const dijkstra = graph.dijkstraShortestPaths(1);
    const distanceTo4 = dijkstra.distance.find((entry) => entry.node === 4)?.distance;
    const pathTo4 = reconstructPath(dijkstra.predecessor, 1, 4);
    writeResult(
      [
        "Browser WASM module loaded.",
        `neighbors(1): ${JSON.stringify(graph.neighbors(1))}`,
        `dijkstra path 1 -> 4: ${JSON.stringify(pathTo4)}`,
        `dijkstra distance 1 -> 4: ${distanceTo4}`,
      ].join("\n"),
    );
  } finally {
    graph.dispose();
  }
} catch (error) {
  writeResult(`Browser WASM investigation failed:\n${error.message}`);
  throw error;
}
