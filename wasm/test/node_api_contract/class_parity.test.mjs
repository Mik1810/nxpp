import {
    assert,
    assertMethods,
    expectedMultiMethods,
    expectedSimpleMethods,
    nxpp,
} from "./helpers.mjs";

const edgeIdMethods = expectedMultiMethods.filter((methodName) => !expectedSimpleMethods.includes(methodName));

const graphClasses = [
    { name: "GraphInt", constructor: nxpp.GraphInt, source: 1, target: 2, directed: false, multi: false },
    { name: "GraphStr", constructor: nxpp.GraphStr, source: "s", target: "t", directed: false, multi: false },
    { name: "DiGraphInt", constructor: nxpp.DiGraphInt, source: 1, target: 2, directed: true, multi: false },
    { name: "DiGraphStr", constructor: nxpp.DiGraphStr, source: "s", target: "t", directed: true, multi: false },
    {
        name: "MultiGraphInt",
        constructor: nxpp.MultiGraphInt,
        source: 1,
        target: 2,
        directed: false,
        multi: true,
    },
    {
        name: "MultiGraphStr",
        constructor: nxpp.MultiGraphStr,
        source: "s",
        target: "t",
        directed: false,
        multi: true,
    },
    {
        name: "MultiDiGraphInt",
        constructor: nxpp.MultiDiGraphInt,
        source: 1,
        target: 2,
        directed: true,
        multi: true,
    },
    {
        name: "MultiDiGraphStr",
        constructor: nxpp.MultiDiGraphStr,
        source: "s",
        target: "t",
        directed: true,
        multi: true,
    },
];

for (const graphClass of graphClasses) {
    const graph = new graphClass.constructor();
    const expectedMethods = graphClass.multi ? expectedMultiMethods : expectedSimpleMethods;
    assertMethods(graph, expectedMethods, graphClass.name);

    const componentMethod = graphClass.directed ? "stronglyConnectedComponents" : "connectedComponents";
    const excludedComponentMethod = graphClass.directed ? "connectedComponents" : "stronglyConnectedComponents";
    assert.equal(typeof graph[componentMethod], "function", `${graphClass.name} must expose ${componentMethod}()`);
    assert.equal(
        typeof graph[excludedComponentMethod],
        "undefined",
        `${graphClass.name} must not expose ${excludedComponentMethod}()`,
    );

    for (const methodName of edgeIdMethods) {
        assert.equal(
            typeof graph[methodName],
            graphClass.multi ? "function" : "undefined",
            `${graphClass.name} edge-ID capability mismatch for ${methodName}()`,
        );
    }

    graph.addEdge(graphClass.source, graphClass.target, 1);
    assert.equal(
        graph.hasEdge(graphClass.source, graphClass.target),
        true,
        `${graphClass.name} must keep edge orientation`,
    );
    assert.equal(
        graph.hasEdge(graphClass.target, graphClass.source),
        !graphClass.directed,
        `${graphClass.name} directedness contract changed`,
    );

    if (graphClass.multi) {
        assert.equal(
            Array.from(graph.edgeIdsBetween(graphClass.source, graphClass.target)).length,
            1,
            `${graphClass.name} must expose the inserted edge ID`,
        );
    }

    const subgraph = graph.subgraph([graphClass.source, graphClass.target]);
    assert.equal(
        subgraph instanceof graphClass.constructor,
        true,
        `${graphClass.name} subgraph() must preserve the concrete runtime class`,
    );
    assertMethods(subgraph, expectedMethods, `${graphClass.name} subgraph`);
    assert.equal(
        subgraph.hasEdge(graphClass.source, graphClass.target),
        true,
        `${graphClass.name} subgraph() must preserve induced edges`,
    );

    subgraph.dispose();
    graph.dispose();
}
