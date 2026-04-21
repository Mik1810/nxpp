import { assert, nxpp } from "./helpers.mjs";

assert.equal(typeof nxpp.DiGraphInt, "function", "DiGraphInt export is missing");
assert.equal(typeof nxpp.DiGraphStr, "function", "DiGraphStr export is missing");
assert.equal(typeof nxpp.GraphInt, "function", "GraphInt export is missing");
assert.equal(typeof nxpp.GraphStr, "function", "GraphStr export is missing");
assert.equal(typeof nxpp.MultiGraphInt, "function", "MultiGraphInt export is missing");
assert.equal(typeof nxpp.MultiGraphStr, "function", "MultiGraphStr export is missing");
assert.equal(typeof nxpp.MultiDiGraphInt, "function", "MultiDiGraphInt export is missing");
assert.equal(typeof nxpp.MultiDiGraphStr, "function", "MultiDiGraphStr export is missing");

assert.equal(typeof nxpp.DiGraph, "undefined", "Legacy DiGraph export must not be exposed");
assert.equal(typeof nxpp.MultiDiGraph, "undefined", "Legacy MultiDiGraph export must not be exposed");
assert.equal(typeof nxpp.DijkstraResult, "undefined", "DijkstraResult must not be part of graph-core exports");
