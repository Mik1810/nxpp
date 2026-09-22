import assert from "node:assert/strict";

import createRawModule from "../../runtime/node.mjs";

const runtime = await createRawModule();

function assertUnownedValue(value, message) {
    assert.notEqual(value, null, message);
    assert.equal(typeof value, "object", message);
    assert.equal(typeof value.delete, "undefined", `${message}: value must not expose delete()`);
}

const multigraph = new runtime.MultiDiGraphInt();
multigraph.addEdge(1, 2, 1);
const edgeIds = multigraph.edgeIdsBetween(1, 2);
const endpoints = multigraph.getEdgeEndpoints(edgeIds[0]);
assert.deepEqual(endpoints, { source: 1, target: 2 }, "raw edge endpoints must be a plain DTO");
assertUnownedValue(endpoints, "raw edge endpoints must not require disposal");
assertUnownedValue(edgeIds, "raw edge-id arrays must not require disposal");
multigraph.delete();

const graph = new runtime.GraphInt();
graph.addEdge(1, 2, 1);
graph.addNode(3);

const traversal = graph.bfsEdges(1);
const components = graph.connectedComponents();
const shortestPaths = graph.dijkstraShortestPaths(1);
const spanningTree = graph.kruskalMinimumSpanningTree();
const centrality = graph.degreeCentrality();

assertUnownedValue(traversal, "raw traversal arrays must not require disposal");
assertUnownedValue(components, "raw component arrays must not require disposal");
assertUnownedValue(shortestPaths, "raw shortest-path DTOs must not require disposal");
assertUnownedValue(shortestPaths.distance, "raw shortest-path distance arrays must not require disposal");
assertUnownedValue(spanningTree, "raw spanning-tree arrays must not require disposal");
assertUnownedValue(centrality, "raw centrality arrays must not require disposal");
assert.deepEqual(graph.bfsEdges(3), [], "raw traversal must preserve empty result arrays");
graph.delete();

const flowGraph = new runtime.DiGraphInt();
flowGraph.addEdge(0, 1, 1);
flowGraph.setEdgeAttr(0, 1, "capacity", 3);
const maximumFlow = flowGraph.maximumFlow(0, 1, "capacity");
assertUnownedValue(maximumFlow, "raw flow DTOs must not require disposal");
assertUnownedValue(maximumFlow.edgeFlows, "raw flow arrays must not require disposal");
flowGraph.delete();
