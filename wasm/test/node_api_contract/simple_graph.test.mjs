import {
    assert,
    assertMethods,
    assertThrows,
    expectedSimpleMethods,
    nxpp,
    toSortedNumbers,
    toSortedStrings,
} from "./helpers.mjs";

const graphInt = new nxpp.GraphInt();
assertMethods(graphInt, expectedSimpleMethods, "GraphInt");
graphInt.addNode(3);
graphInt.addEdge(1, 2, 3);
assert.equal(graphInt.hasNode(3), true, "GraphInt hasNode() must report inserted nodes");
assert.equal(graphInt.hasEdge(1, 2), true, "GraphInt hasEdge() must support undirected edges");
assert.equal(graphInt.hasEdge(2, 1), true, "GraphInt hasEdge() must support reverse endpoint lookup");
assert.deepEqual(toSortedNumbers(graphInt.nodes()), [1, 2, 3], "GraphInt nodes() must preserve numeric IDs");
assert.deepEqual(toSortedNumbers(graphInt.neighbors(1)), [2], "GraphInt neighbors() must preserve undirected adjacency");
assert.equal(graphInt.getEdgeWeight(1, 2), 3, "GraphInt getEdgeWeight() returned an unexpected value");
graphInt.setEdgeWeight(1, 2, 6);
assert.equal(graphInt.getEdgeWeight(1, 2), 6, "GraphInt setEdgeWeight() must update the selected edge");
graphInt.removeEdge(1, 2);
assert.equal(graphInt.hasEdge(1, 2), false, "GraphInt removeEdge() must remove selected edge");
graphInt.clear();
assert.deepEqual(Array.from(graphInt.nodes()), [], "GraphInt clear() must remove all nodes");

const graphStr = new nxpp.GraphStr();
assertMethods(graphStr, expectedSimpleMethods, "GraphStr");
graphStr.addNode("W");
graphStr.addEdge("U", "V", 2);
assert.equal(graphStr.hasNode("W"), true, "GraphStr hasNode() must report inserted nodes");
assert.equal(graphStr.hasEdge("U", "V"), true, "GraphStr hasEdge() must support undirected edges");
assert.equal(graphStr.hasEdge("V", "U"), true, "GraphStr hasEdge() must support reverse endpoint lookup");
assert.deepEqual(toSortedStrings(graphStr.nodes()), ["U", "V", "W"], "GraphStr nodes() must preserve string IDs");
assert.deepEqual(toSortedStrings(graphStr.neighbors("U")), ["V"], "GraphStr neighbors() must preserve undirected adjacency");
graphStr.clear();
assert.deepEqual(Array.from(graphStr.nodes()), [], "GraphStr clear() must remove all nodes");

const diInt = new nxpp.DiGraphInt();
assertMethods(diInt, expectedSimpleMethods, "DiGraphInt");
diInt.addNode(1);
diInt.addNode(2);
diInt.addNode(3);
diInt.addEdge(1, 2, 4);
diInt.addEdge(2, 3, 2);
assert.equal(diInt.hasNode(1), true, "DiGraphInt hasNode() must return true for an existing node");
assert.equal(diInt.hasNode(999), false, "DiGraphInt hasNode() must return false for a missing node");
assert.equal(diInt.hasEdge(1, 2), true, "DiGraphInt hasEdge() must return true for an existing edge");
assert.equal(diInt.hasEdge(1, 3), false, "DiGraphInt hasEdge() must return false for a missing edge");
assert.deepEqual(toSortedNumbers(diInt.nodes()), [1, 2, 3], "DiGraphInt nodes() must preserve numeric IDs");
assert.deepEqual(toSortedNumbers(diInt.neighbors(1)), [2], "DiGraphInt neighbors() must return outgoing adjacency");
assert.equal(diInt.getEdgeWeight(1, 2), 4, "DiGraphInt getEdgeWeight() returned an unexpected value");
diInt.setEdgeWeight(1, 2, 7);
assert.equal(diInt.getEdgeWeight(1, 2), 7, "DiGraphInt setEdgeWeight() must update endpoint-selected weight");
diInt.removeEdge(2, 3);
assert.equal(diInt.hasEdge(2, 3), false, "DiGraphInt removeEdge() must remove selected edge");
diInt.removeNode(2);
assert.equal(diInt.hasNode(2), false, "DiGraphInt removeNode() must remove selected node");
assertThrows(() => diInt.setEdgeWeight(2, 3, 9), "DiGraphInt setEdgeWeight() must reject missing edges");
diInt.clear();
assert.deepEqual(Array.from(diInt.nodes()), [], "DiGraphInt clear() must remove all nodes");

const diStr = new nxpp.DiGraphStr();
assertMethods(diStr, expectedSimpleMethods, "DiGraphStr");
diStr.addNode("A");
diStr.addNode("B");
diStr.addNode("C");
diStr.addEdge("A", "B", 1.5);
diStr.addEdge("B", "C", 2.5);
assert.equal(diStr.hasNode("A"), true, "DiGraphStr hasNode() must return true for existing nodes");
assert.equal(diStr.hasEdge("A", "B"), true, "DiGraphStr hasEdge() must return true for existing edges");
assert.equal(diStr.hasEdge("B", "A"), false, "DiGraphStr hasEdge() must preserve directed orientation");
assert.deepEqual(toSortedStrings(diStr.nodes()), ["A", "B", "C"], "DiGraphStr nodes() must preserve string IDs");
assert.deepEqual(toSortedStrings(diStr.neighbors("A")), ["B"], "DiGraphStr neighbors() must preserve string adjacency");
assert.equal(diStr.getEdgeWeight("A", "B"), 1.5, "DiGraphStr getEdgeWeight() returned an unexpected value");
diStr.setEdgeWeight("A", "B", 3.5);
assert.equal(diStr.getEdgeWeight("A", "B"), 3.5, "DiGraphStr setEdgeWeight() must update endpoint-selected weight");
diStr.clear();
assert.deepEqual(Array.from(diStr.nodes()), [], "DiGraphStr clear() must remove all nodes");
