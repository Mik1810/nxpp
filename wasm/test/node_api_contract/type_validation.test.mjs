import { assertThrows, nxpp } from "./helpers.mjs";

const graphInt = new nxpp.GraphInt();
assertThrows(() => graphInt.addNode("x"), "GraphInt must reject non-integer node IDs");
assertThrows(() => graphInt.addEdge(1, 2, Number.POSITIVE_INFINITY), "GraphInt must reject non-finite weights");
assertThrows(() => graphInt.setNodeAttr(1, "meta", {}), "GraphInt must reject unsupported attribute value types");
assertThrows(() => graphInt.setNodeAttr(1, "meta", null), "GraphInt must reject null attribute values");

const graphStr = new nxpp.GraphStr();
assertThrows(() => graphStr.addNode(1), "GraphStr must reject non-string node IDs");
assertThrows(() => graphStr.setEdgeAttr("A", "B", "meta", undefined), "GraphStr must reject undefined attribute values");

const diInt = new nxpp.DiGraphInt();
assertThrows(() => diInt.addNode("x"), "DiGraphInt must reject non-integer node IDs");
assertThrows(() => diInt.setEdgeWeight(1, 2, Number.NaN), "DiGraphInt must reject non-finite weights in setEdgeWeight()");

const diStr = new nxpp.DiGraphStr();
assertThrows(() => diStr.addNode(1), "DiGraphStr must reject non-string node IDs");

const multiGraphInt = new nxpp.MultiGraphInt();
assertThrows(() => multiGraphInt.hasEdgeId(-1), "MultiGraphInt must reject negative edge IDs");
assertThrows(() => multiGraphInt.setEdgeWeightById(0.5, 1), "MultiGraphInt must reject non-integer edge IDs");
assertThrows(() => multiGraphInt.setEdgeAttrById(0, "meta", Number.NaN), "MultiGraphInt must reject non-finite numeric attrs by edge ID");

const multiGraphStr = new nxpp.MultiGraphStr();
assertThrows(() => multiGraphStr.addNode(42), "MultiGraphStr must reject non-string node IDs");

const multiInt = new nxpp.MultiDiGraphInt();
assertThrows(() => multiInt.addNode("x"), "MultiDiGraphInt must reject non-integer node IDs");
assertThrows(() => multiInt.setEdgeWeightById(0, Number.POSITIVE_INFINITY), "MultiDiGraphInt must reject non-finite weights by edge ID");

const multiStr = new nxpp.MultiDiGraphStr();
assertThrows(() => multiStr.addNode(42), "MultiDiGraphStr must reject non-string node IDs");
