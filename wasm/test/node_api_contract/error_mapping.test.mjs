import {
    assertThrowsMessage,
    assertThrowsMessageIncludes,
    nxpp,
} from "./helpers.mjs";

const prefix = "WASM graph operation failed:";

const graph = new nxpp.GraphInt();
assertThrowsMessage(
    () => graph.neighbors(99),
    `${prefix} Node lookup failed: node not found.`,
    "missing-node runtime errors must be normalized at the facade boundary",
);

graph.addNode(1);
assertThrowsMessage(
    () => graph.getEdgeWeight(1, 2),
    `${prefix} Node lookup failed: node not found.`,
    "missing-endpoint edge lookup errors must be normalized",
);

graph.addNode(2);
assertThrowsMessage(
    () => graph.getEdgeWeight(1, 2),
    `${prefix} Edge lookup failed: edge not found.`,
    "missing-edge runtime errors must be normalized",
);

const disconnected = new nxpp.GraphInt();
disconnected.addNode(1);
disconnected.addNode(2);
assertThrowsMessage(
    () => disconnected.shortestPath(1, 2),
    `${prefix} Shortest-path lookup failed: target node is unreachable.`,
    "disconnected shortest-path errors must be normalized",
);

const attrs = new nxpp.GraphInt();
attrs.addNode(1);
assertThrowsMessage(
    () => attrs.getNodeAttr(1, "missing"),
    `${prefix} Node attribute lookup failed: key not found.`,
    "missing-attribute runtime errors must be normalized",
);

const multigraph = new nxpp.MultiGraphInt();
assertThrowsMessageIncludes(
    () => multigraph.getEdgeEndpoints(99999),
    `${prefix} Edge lookup failed: edge not found.`,
    "missing edge-id runtime errors must be normalized",
);
