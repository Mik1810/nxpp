import { assert, nxpp } from "./helpers.mjs";

function normalizeGroups(groups) {
    return Array.from(groups, (group) => Array.from(group).sort()).sort((left, right) => {
        const leftKey = left.join("\u0000");
        const rightKey = right.join("\u0000");
        return leftKey.localeCompare(rightKey);
    });
}

const graphInt = new nxpp.GraphInt();
graphInt.addEdge(1, 2, 1);
graphInt.addNode(3);
graphInt.addEdge(4, 5, 1);
assert.equal(typeof graphInt.connectedComponents, "function", "GraphInt must expose connectedComponents()");
assert.deepEqual(
    normalizeGroups(graphInt.connectedComponents()),
    [[1, 2], [3], [4, 5]],
    "GraphInt connectedComponents() must group undirected components",
);
assert.equal(
    typeof graphInt.stronglyConnectedComponents,
    "undefined",
    "GraphInt must not expose directed-only stronglyConnectedComponents()",
);

const graphStr = new nxpp.GraphStr();
graphStr.addEdge("a", "b", 1);
graphStr.addNode("c");
assert.deepEqual(
    normalizeGroups(graphStr.connectedComponents()),
    [["a", "b"], ["c"]],
    "GraphStr connectedComponents() must preserve string node IDs",
);

const diGraphStr = new nxpp.DiGraphStr();
diGraphStr.addEdge("A", "B", 1);
diGraphStr.addEdge("B", "A", 1);
diGraphStr.addEdge("B", "C", 1);
diGraphStr.addEdge("C", "D", 1);
diGraphStr.addEdge("D", "C", 1);
diGraphStr.addNode("E");
assert.equal(
    typeof diGraphStr.connectedComponents,
    "undefined",
    "DiGraphStr must not expose undirected-only connectedComponents()",
);
assert.equal(
    typeof diGraphStr.stronglyConnectedComponents,
    "function",
    "DiGraphStr must expose stronglyConnectedComponents()",
);
assert.deepEqual(
    normalizeGroups(diGraphStr.stronglyConnectedComponents()),
    [["A", "B"], ["C", "D"], ["E"]],
    "DiGraphStr stronglyConnectedComponents() must group directed SCCs",
);

const multiGraphInt = new nxpp.MultiGraphInt();
multiGraphInt.addEdge(10, 11, 1);
multiGraphInt.addEdge(10, 11, 2);
multiGraphInt.addNode(12);
assert.deepEqual(
    normalizeGroups(multiGraphInt.connectedComponents()),
    [[10, 11], [12]],
    "MultiGraphInt connectedComponents() must work with parallel edges",
);

const multiDiGraphStr = new nxpp.MultiDiGraphStr();
multiDiGraphStr.addEdge("s", "t", 1);
multiDiGraphStr.addEdge("t", "s", 1);
multiDiGraphStr.addEdge("u", "v", 1);
multiDiGraphStr.addEdge("v", "u", 1);
assert.deepEqual(
    normalizeGroups(multiDiGraphStr.stronglyConnectedComponents()),
    [["s", "t"], ["u", "v"]],
    "MultiDiGraphStr stronglyConnectedComponents() must work with parallel-edge-capable directed graphs",
);
