import {
    assert,
    assertMethods,
    assertThrows,
    expectedMultiMethods,
    expectedSimpleMethods,
    nxpp,
} from "./helpers.mjs";

const graphInt = new nxpp.GraphInt();
assertMethods(graphInt, expectedSimpleMethods, "GraphInt");
assert.equal(graphInt.hasNodeAttr(1, "label"), false, "GraphInt hasNodeAttr() must return false for missing nodes");
assert.equal(graphInt.tryGetNodeAttr(1, "label"), null, "GraphInt tryGetNodeAttr() must return null for missing nodes");
graphInt.setNodeAttr(1, "label", "source");
graphInt.setNodeAttr(1, "enabled", true);
assert.equal(graphInt.hasNodeAttr(1, "label"), true, "GraphInt hasNodeAttr() must report stored node attrs");
assert.equal(graphInt.getNodeAttr(1, "label"), "source", "GraphInt getNodeAttr() must return stored string attrs");
assert.equal(graphInt.getNodeAttr(1, "enabled"), true, "GraphInt getNodeAttr() must return stored boolean attrs");
assert.equal(graphInt.tryGetNodeAttr(1, "missing"), null, "GraphInt tryGetNodeAttr() must return null for missing keys");
assertThrows(() => graphInt.getNodeAttr(1, "missing"), "GraphInt getNodeAttr() must throw for missing keys");
graphInt.setEdgeAttr(1, 2, "capacity", 7);
assert.equal(graphInt.hasEdgeAttr(1, 2, "capacity"), true, "GraphInt hasEdgeAttr() must report stored edge attrs");
assert.equal(graphInt.getEdgeAttr(1, 2, "capacity"), 7, "GraphInt getEdgeAttr() must return stored numeric attrs");
assert.equal(graphInt.getEdgeNumericAttr(1, 2, "capacity"), 7, "GraphInt getEdgeNumericAttr() must read numeric custom attrs");
assert.equal(graphInt.getEdgeWeight(1, 2), 1, "GraphInt setEdgeAttr() must preserve implicit default weight on created edges");
assert.equal(graphInt.getEdgeNumericAttr(1, 2, "weight"), 1, "GraphInt getEdgeNumericAttr() must support the built-in weight key");
graphInt.setEdgeAttr(1, 2, "note", "ok");
assert.equal(graphInt.getEdgeAttr(1, 2, "note"), "ok", "GraphInt getEdgeAttr() must preserve string attrs");
assert.equal(graphInt.tryGetEdgeAttr(2, 3, "capacity"), null, "GraphInt tryGetEdgeAttr() must return null for missing edges");
assertThrows(() => graphInt.getEdgeAttr(2, 3, "capacity"), "GraphInt getEdgeAttr() must throw for missing edges");

const diStr = new nxpp.DiGraphStr();
assertMethods(diStr, expectedSimpleMethods, "DiGraphStr");
diStr.setNodeAttr("A", "role", "start");
assert.equal(diStr.getNodeAttr("A", "role"), "start", "DiGraphStr getNodeAttr() must return stored attrs");
diStr.setEdgeAttr("A", "B", "company", "rail");
assert.equal(diStr.hasEdge("A", "B"), true, "DiGraphStr setEdgeAttr() must create missing directed edges");
assert.equal(diStr.getEdgeAttr("A", "B", "company"), "rail", "DiGraphStr getEdgeAttr() must return stored string attrs");
assert.equal(diStr.tryGetEdgeAttr("B", "A", "company"), null, "DiGraphStr endpoint attr lookup must preserve edge direction");

const multiGraphInt = new nxpp.MultiGraphInt();
assertMethods(multiGraphInt, expectedMultiMethods, "MultiGraphInt");
multiGraphInt.setNodeAttr(1, "kind", "hub");
assert.equal(multiGraphInt.getNodeAttr(1, "kind"), "hub", "MultiGraphInt getNodeAttr() must return stored attrs");
multiGraphInt.addEdge(1, 2, 5);
multiGraphInt.addEdge(1, 2, 8);
const multiGraphEdgeId = Array.from(new Set(Array.from(multiGraphInt.edgeIdsBetween(1, 2))))[0];
multiGraphInt.setEdgeAttrById(multiGraphEdgeId, "capacity", 11);
multiGraphInt.setEdgeAttrById(multiGraphEdgeId, "active", false);
assert.equal(multiGraphInt.hasEdgeAttrById(multiGraphEdgeId, "capacity"), true, "MultiGraphInt hasEdgeAttrById() must report stored attrs");
assert.equal(multiGraphInt.getEdgeAttrById(multiGraphEdgeId, "capacity"), 11, "MultiGraphInt getEdgeAttrById() must return stored attrs");
assert.equal(multiGraphInt.getEdgeAttrById(multiGraphEdgeId, "active"), false, "MultiGraphInt getEdgeAttrById() must return stored booleans");
assert.equal(multiGraphInt.getEdgeNumericAttrById(multiGraphEdgeId, "capacity"), 11, "MultiGraphInt getEdgeNumericAttrById() must read numeric attrs");
assert.equal(multiGraphInt.tryGetEdgeAttrById(99999, "capacity"), null, "MultiGraphInt tryGetEdgeAttrById() must return null for missing edge IDs");
assertThrows(() => multiGraphInt.getEdgeAttrById(99999, "capacity"), "MultiGraphInt getEdgeAttrById() must throw for missing edge IDs");

const multiDiGraphStr = new nxpp.MultiDiGraphStr();
assertMethods(multiDiGraphStr, expectedMultiMethods, "MultiDiGraphStr");
multiDiGraphStr.addEdge("S", "T", 2);
multiDiGraphStr.addEdge("S", "T", 4);
const multiDiGraphIds = Array.from(multiDiGraphStr.edgeIdsBetween("S", "T")).sort((a, b) => a - b);
multiDiGraphStr.setEdgeAttr("S", "T", "label", "aggregate");
assert.equal(multiDiGraphStr.hasEdgeAttr("S", "T", "label"), true, "MultiDiGraphStr hasEdgeAttr() must support endpoint convenience lookup");
multiDiGraphStr.setEdgeAttrById(multiDiGraphIds[1], "label", "second");
assert.equal(multiDiGraphStr.getEdgeAttrById(multiDiGraphIds[1], "label"), "second", "MultiDiGraphStr getEdgeAttrById() must support precise writes");
assert.equal(multiDiGraphStr.getEdgeNumericAttrById(multiDiGraphIds[1], "weight"), 4, "MultiDiGraphStr getEdgeNumericAttrById() must support the built-in weight key");
