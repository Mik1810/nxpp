import {
    assert,
    assertMethods,
    assertThrows,
    expectedMultiMethods,
    expectedSimpleMethods,
    nxpp,
} from "./helpers.mjs";

function scoreMap(entries) {
    return new Map(Array.from(entries, (entry) => [entry.node, entry.score]));
}

function assertNear(actual, expected, tolerance, message) {
    assert.equal(
        Math.abs(actual - expected) <= tolerance,
        true,
        `${message}: expected ${expected}, got ${actual}`,
    );
}

const graphInt = new nxpp.GraphInt();
assertMethods(graphInt, expectedSimpleMethods, "GraphInt");
graphInt.addEdge(0, 1, 1);
graphInt.addEdge(1, 2, 1);
graphInt.addEdge(2, 3, 1);
const degree = scoreMap(graphInt.degreeCentrality());
assertNear(degree.get(0), 1 / 3, 1e-9, "GraphInt degreeCentrality() must score leaf nodes");
assertNear(degree.get(1), 2 / 3, 1e-9, "GraphInt degreeCentrality() must score interior nodes");
const betweenness = scoreMap(graphInt.betweennessCentrality());
assertNear(betweenness.get(0), 0, 1e-9, "GraphInt betweennessCentrality() must score path endpoints as zero");
assert.equal(
    betweenness.get(1) > 0 && betweenness.get(2) > 0,
    true,
    "GraphInt betweennessCentrality() must score path interior nodes above zero",
);

const diGraphStr = new nxpp.DiGraphStr();
assertMethods(diGraphStr, expectedSimpleMethods, "DiGraphStr");
diGraphStr.addEdge("A", "B", 1);
diGraphStr.addEdge("B", "C", 1);
diGraphStr.addEdge("C", "B", 1);
diGraphStr.addEdge("C", "D", 1);
const rank = scoreMap(diGraphStr.pagerank());
const totalRank = Array.from(rank.values()).reduce((sum, value) => sum + value, 0);
assertNear(totalRank, 1, 1e-9, "DiGraphStr pagerank() scores must be normalized");
assert.equal(rank.get("B") > rank.get("A"), true, "DiGraphStr pagerank() must rank referenced nodes above source-only nodes");
assertThrows(() => diGraphStr.pagerank(Number.NaN), "pagerank() must reject non-finite tolerance");
assertThrows(() => diGraphStr.pagerank(1e-6, -1), "pagerank() must reject negative maxIterations");

const multiDiGraphInt = new nxpp.MultiDiGraphInt();
assertMethods(multiDiGraphInt, expectedMultiMethods, "MultiDiGraphInt");
multiDiGraphInt.addEdge(1, 2, 1);
multiDiGraphInt.addEdge(2, 1, 1);
const multiDegree = scoreMap(multiDiGraphInt.degreeCentrality());
assertNear(multiDegree.get(1), 2, 1e-9, "MultiDiGraphInt degreeCentrality() must account for in and out degree");
