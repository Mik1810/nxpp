import {
    assert,
    assertMethods,
    expectedMultiMethods,
    nxpp,
    toSortedNumbers,
    toSortedStrings,
} from "./helpers.mjs";

const multiGraphInt = new nxpp.MultiGraphInt();
assertMethods(multiGraphInt, expectedMultiMethods, "MultiGraphInt");
multiGraphInt.addNode(7);
multiGraphInt.addEdge(1, 2, 1);
multiGraphInt.addEdge(1, 2, 2);
const multiGraphIntIds = toSortedNumbers(new Set(Array.from(multiGraphInt.edgeIdsBetween(1, 2))));
assert.equal(multiGraphInt.hasNode(7), true, "MultiGraphInt hasNode() must report inserted nodes");
assert.equal(multiGraphInt.hasEdge(2, 1), true, "MultiGraphInt hasEdge() must support reverse endpoint lookup");
assert.equal(multiGraphIntIds.length, 2, "MultiGraphInt edgeIdsBetween() must report both parallel edge IDs");
assert.deepEqual(toSortedNumbers(multiGraphInt.edgeIds()), multiGraphIntIds, "MultiGraphInt edgeIds() must list inserted edge IDs");
assert.equal(multiGraphInt.hasEdgeId(multiGraphIntIds[0]), true, "MultiGraphInt hasEdgeId() must report existing IDs");
assert.equal(multiGraphInt.getEdgeWeight(1, 2), 1, "MultiGraphInt getEdgeWeight() must return the first matching edge weight");
multiGraphInt.setEdgeWeight(1, 2, 5);
assert.equal(multiGraphInt.getEdgeWeight(1, 2), 5, "MultiGraphInt setEdgeWeight() must update the endpoint-selected edge");
multiGraphInt.clear();
assert.deepEqual(Array.from(multiGraphInt.nodes()), [], "MultiGraphInt clear() must remove all nodes");
assert.deepEqual(Array.from(multiGraphInt.edgeIds()), [], "MultiGraphInt clear() must remove all edge IDs");

const multiGraphStr = new nxpp.MultiGraphStr();
assertMethods(multiGraphStr, expectedMultiMethods, "MultiGraphStr");
multiGraphStr.addNode("Z");
multiGraphStr.addEdge("X", "Y", 1);
multiGraphStr.addEdge("X", "Y", 4);
const multiGraphStrIds = toSortedNumbers(new Set(Array.from(multiGraphStr.edgeIdsBetween("X", "Y"))));
assert.equal(multiGraphStr.hasNode("Z"), true, "MultiGraphStr hasNode() must report inserted nodes");
assert.equal(multiGraphStr.hasEdge("Y", "X"), true, "MultiGraphStr hasEdge() must support reverse endpoint lookup");
assert.equal(multiGraphStrIds.length, 2, "MultiGraphStr edgeIdsBetween() must report both parallel edge IDs");
assert.deepEqual(toSortedStrings(multiGraphStr.nodes()), ["X", "Y", "Z"], "MultiGraphStr nodes() must preserve string IDs");

const multiInt = new nxpp.MultiDiGraphInt();
assertMethods(multiInt, expectedMultiMethods, "MultiDiGraphInt");
multiInt.addEdge(1, 2, 5);
multiInt.addEdge(1, 2, 8);
const multiIntIds = toSortedNumbers(multiInt.edgeIdsBetween(1, 2));
assert.equal(multiIntIds.length, 2, "MultiDiGraphInt edgeIdsBetween() must return all parallel edge IDs");
assert.equal(multiInt.hasEdgeId(multiIntIds[0]), true, "MultiDiGraphInt hasEdgeId() must report existing IDs");
const intEndpoints = multiInt.getEdgeEndpoints(multiIntIds[0]);
assert.equal(typeof intEndpoints.source, "function", "MultiDiGraphInt getEdgeEndpoints() must return endpoint wrapper");
assert.equal(typeof intEndpoints.target, "function", "MultiDiGraphInt getEdgeEndpoints() must return endpoint wrapper");
assert.equal(intEndpoints.source(), 1, "MultiDiGraphInt edge endpoint source must match inserted edge");
assert.equal(intEndpoints.target(), 2, "MultiDiGraphInt edge endpoint target must match inserted edge");
const originalWeightById = multiInt.getEdgeWeightById(multiIntIds[1]);
multiInt.setEdgeWeightById(multiIntIds[1], originalWeightById + 10);
assert.equal(
    multiInt.getEdgeWeightById(multiIntIds[1]),
    originalWeightById + 10,
    "MultiDiGraphInt setEdgeWeightById() must update only selected edge",
);
multiInt.removeEdgeById(multiIntIds[0]);
assert.equal(multiInt.hasEdgeId(multiIntIds[0]), false, "MultiDiGraphInt removeEdgeById() must remove selected edge");
assert.equal(multiInt.hasEdgeId(multiIntIds[1]), true, "MultiDiGraphInt removeEdgeById() must preserve other parallel edges");

const multiStr = new nxpp.MultiDiGraphStr();
assertMethods(multiStr, expectedMultiMethods, "MultiDiGraphStr");
multiStr.addEdge("S", "T", 2.0);
multiStr.addEdge("S", "T", 4.0);
const multiStrIds = toSortedNumbers(multiStr.edgeIdsBetween("S", "T"));
assert.equal(multiStrIds.length, 2, "MultiDiGraphStr edgeIdsBetween() must return all parallel edge IDs");
assert.equal(multiStr.hasEdgeId(multiStrIds[0]), true, "MultiDiGraphStr hasEdgeId() must report existing IDs");
const strEndpoints = multiStr.getEdgeEndpoints(multiStrIds[0]);
assert.equal(strEndpoints.source(), "S", "MultiDiGraphStr edge endpoint source must match inserted edge");
assert.equal(strEndpoints.target(), "T", "MultiDiGraphStr edge endpoint target must match inserted edge");
multiStr.setEdgeWeightById(multiStrIds[0], 9.5);
assert.equal(multiStr.getEdgeWeightById(multiStrIds[0]), 9.5, "MultiDiGraphStr setEdgeWeightById() must update selected edge");
