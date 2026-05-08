import {
    assert,
    assertMethods,
    assertThrowsMessageIncludes,
    expectedMultiMethods,
    expectedSimpleMethods,
    nxpp,
} from "./helpers.mjs";

function endpointFlowMap(entries) {
    return new Map(Array.from(entries, (entry) => [`${entry.source}->${entry.target}`, entry.flow]));
}

function edgeIdFlowMap(entries) {
    return new Map(Array.from(entries, (entry) => [entry.edgeId, entry.flow]));
}

function setCapacity(graph, source, target, capacity) {
    graph.setEdgeAttr(source, target, "capacity", capacity);
}

const flowGraph = new nxpp.DiGraphInt();
assertMethods(flowGraph, expectedSimpleMethods, "DiGraphInt");
flowGraph.addEdge(0, 1, 1);
flowGraph.addEdge(0, 3, 1);
flowGraph.addEdge(1, 2, 1);
flowGraph.addEdge(1, 4, 1);
flowGraph.addEdge(2, 5, 1);
flowGraph.addEdge(3, 2, 1);
flowGraph.addEdge(4, 5, 1);
setCapacity(flowGraph, 0, 1, 1);
setCapacity(flowGraph, 0, 3, 2);
setCapacity(flowGraph, 1, 2, 3);
setCapacity(flowGraph, 1, 4, 4);
setCapacity(flowGraph, 2, 5, 2);
setCapacity(flowGraph, 3, 2, 2);
setCapacity(flowGraph, 4, 5, 2);

const maximumFlow = flowGraph.maximumFlow(0, 5);
assert.equal(maximumFlow.value, 3, "DiGraphInt maximumFlow() must return the reference flow value");
assert.equal(
    endpointFlowMap(maximumFlow.edgeFlows).get("0->1") + endpointFlowMap(maximumFlow.edgeFlows).get("0->3"),
    3,
    "DiGraphInt maximumFlow() edgeFlows must expose endpoint flow assignments",
);
assert.equal(maximumFlow.edgeFlowsById.length, 7, "DiGraphInt maximumFlow() must expose edge-id flow assignments");

const minimumCut = flowGraph.minimumCut(0, 5);
assert.equal(minimumCut.value, 3, "DiGraphInt minimumCut() must match max-flow value");
assert.deepEqual(
    Array.from(minimumCut.reachable).sort((a, b) => a - b),
    [0],
    "DiGraphInt minimumCut() must expose reachable partition",
);
assert.equal(minimumCut.cutEdges.length, 2, "DiGraphInt minimumCut() must expose cut endpoint entries");
assert.equal(minimumCut.cutEdgeIds.length, 2, "DiGraphInt minimumCut() must expose cut edge IDs");

const minCostGraph = new nxpp.DiGraphInt();
minCostGraph.addEdge(0, 1, 2);
minCostGraph.addEdge(0, 3, 3);
minCostGraph.addEdge(1, 2, 5);
minCostGraph.addEdge(1, 4, 1);
minCostGraph.addEdge(2, 5, 3);
minCostGraph.addEdge(3, 1, 1);
minCostGraph.addEdge(3, 2, 6);
minCostGraph.addEdge(4, 5, 1);
setCapacity(minCostGraph, 0, 1, 1);
setCapacity(minCostGraph, 0, 3, 2);
setCapacity(minCostGraph, 1, 2, 3);
setCapacity(minCostGraph, 1, 4, 4);
setCapacity(minCostGraph, 2, 5, 2);
setCapacity(minCostGraph, 3, 1, 1);
setCapacity(minCostGraph, 3, 2, 2);
setCapacity(minCostGraph, 4, 5, 2);

const minCost = minCostGraph.maxFlowMinCost(0, 5);
assert.equal(minCost.flow, 3, "DiGraphInt maxFlowMinCost() must return the reference flow");
assert.equal(minCost.cost, 22, "DiGraphInt maxFlowMinCost() must return the reference cost");
assert.equal(edgeIdFlowMap(minCost.edgeFlowsById).size, 8, "DiGraphInt maxFlowMinCost() must expose edge-id flows");

const sspMinCost = minCostGraph.maxFlowMinCostSuccessiveShortestPath(0, 5);
assert.equal(sspMinCost.flow, 3, "DiGraphInt SSP min-cost flow must return the reference flow");
assert.equal(sspMinCost.cost, 22, "DiGraphInt SSP min-cost flow must return the reference cost");
assertThrowsMessageIncludes(
    () => minCostGraph.cycleCanceling(),
    "Min-cost-flow state unavailable",
    "cycleCanceling() must reject missing staged state",
);

const stagedGraph = new nxpp.DiGraphInt();
stagedGraph.addEdge(0, 1, 2);
stagedGraph.addEdge(1, 2, 1);
setCapacity(stagedGraph, 0, 1, 2);
setCapacity(stagedGraph, 1, 2, 2);
assert.equal(stagedGraph.pushRelabelMaximumFlow(0, 2), 2, "pushRelabelMaximumFlow() must stage max-flow state");
assert.equal(stagedGraph.cycleCanceling(), 6, "cycleCanceling() must consume staged state");
assert.equal(stagedGraph.pushRelabelMaximumFlow(0, 2), 2, "pushRelabelMaximumFlow() must be rerunnable");
stagedGraph.addNode(99);
assertThrowsMessageIncludes(
    () => stagedGraph.cycleCanceling(),
    "Min-cost-flow state invalidated by graph mutation",
    "cycleCanceling() must reject invalidated staged state",
);

const multiGraph = new nxpp.MultiDiGraphInt();
assertMethods(multiGraph, expectedMultiMethods, "MultiDiGraphInt");
multiGraph.addEdge(0, 1, 1);
multiGraph.addEdge(0, 1, 1);
multiGraph.addEdge(1, 2, 1);
multiGraph.addEdge(1, 2, 1);
const sourceEdgeIds = Array.from(multiGraph.edgeIdsBetween(0, 1)).sort((a, b) => a - b);
const sinkEdgeIds = Array.from(multiGraph.edgeIdsBetween(1, 2)).sort((a, b) => a - b);
multiGraph.setEdgeAttrById(sourceEdgeIds[0], "capacity", 1);
multiGraph.setEdgeAttrById(sourceEdgeIds[1], "capacity", 2);
multiGraph.setEdgeAttrById(sinkEdgeIds[0], "capacity", 1);
multiGraph.setEdgeAttrById(sinkEdgeIds[1], "capacity", 2);
const multiFlow = multiGraph.maximumFlow(0, 2);
const multiEdgeFlows = edgeIdFlowMap(multiFlow.edgeFlowsById);
assert.equal(multiFlow.value, 3, "MultiDiGraphInt maximumFlow() must aggregate parallel capacity");
assert.equal(multiEdgeFlows.get(sourceEdgeIds[0]), 1, "MultiDiGraphInt maximumFlow() must preserve first source edge flow");
assert.equal(multiEdgeFlows.get(sourceEdgeIds[1]), 2, "MultiDiGraphInt maximumFlow() must preserve second source edge flow");
