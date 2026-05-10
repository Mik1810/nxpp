import { performance } from "node:perf_hooks";

import nxpp from "../dist/index.js";

function readIntArg(name, fallback) {
    const index = process.argv.indexOf(name);
    if (index === -1 || index + 1 >= process.argv.length) {
        return fallback;
    }
    const value = Number.parseInt(process.argv[index + 1], 10);
    return Number.isFinite(value) ? value : fallback;
}

const config = {
    nodes: readIntArg("--nodes", 300),
    edges: readIntArg("--edges", 900),
    floydNodes: readIntArg("--floyd-nodes", 35),
    attrOps: readIntArg("--attr-ops", 1000),
    multigraphEdges: readIntArg("--multigraph-edges", 600),
    iterations: readIntArg("--iterations", 5),
};

function consume(value) {
    globalThis.__nxppBenchmarkSink = value;
}

function toArray(value) {
    return Array.from(value);
}

function measureMs(iterations, fn) {
    const start = performance.now();
    for (let i = 0; i < iterations; i += 1) {
        fn();
    }
    return performance.now() - start;
}

function printHeader() {
    console.log("layer,workload,iterations,total_ms,ops_per_second,notes");
}

function printRow(layer, workload, iterations, totalMs, notes) {
    const opsPerSecond = totalMs > 0 ? (iterations * 1000) / totalMs : 0;
    console.log([
        layer,
        workload,
        iterations,
        totalMs.toFixed(3),
        opsPerSecond.toFixed(3),
        notes,
    ].join(","));
}

function makeDiGraph(GraphCtor, nodes = config.nodes, edges = config.edges) {
    const graph = new GraphCtor();
    for (let node = 0; node < nodes; node += 1) {
        graph.addNode(node);
    }
    for (let edge = 0; edge < edges; edge += 1) {
        const source = edge % nodes;
        let target = (edge * 37 + 11) % nodes;
        if (target === source) {
            target = (target + 1) % nodes;
        }
        graph.addEdge(source, target, (edge % 17) + 1);
    }
    return graph;
}

function makeFloydGraph(GraphCtor) {
    const graph = new GraphCtor();
    for (let node = 0; node < config.floydNodes; node += 1) {
        graph.addNode(node);
    }
    for (let node = 0; node + 1 < config.floydNodes; node += 1) {
        graph.addEdge(node, node + 1, 1);
        if (node + 3 < config.floydNodes) {
            graph.addEdge(node, node + 3, 4);
        }
    }
    return graph;
}

function dispose(graph) {
    if (typeof graph.dispose === "function") {
        graph.dispose();
        return;
    }
    if (typeof graph.delete === "function") {
        graph.delete();
    }
}

function runLayer(layer, constructors) {
    const constructionMs = measureMs(config.iterations, () => {
        const graph = makeDiGraph(constructors.DiGraphInt);
        consume(toArray(graph.nodes()).length);
        dispose(graph);
    });
    printRow(layer, "construct_digraph", config.iterations, constructionMs, `nodes=${config.nodes};edges=${config.edges}`);

    const graph = makeDiGraph(constructors.DiGraphInt);
    const bfsMs = measureMs(config.iterations, () => {
        consume(toArray(graph.bfsEdges(0)).length);
    });
    printRow(layer, "bfs_edges", config.iterations, bfsMs, "source=0");

    const dijkstraMs = measureMs(config.iterations, () => {
        const result = graph.dijkstraShortestPaths(0);
        consume(toArray(result.distance).length);
    });
    printRow(layer, "dijkstra_shortest_paths", config.iterations, dijkstraMs, "source=0");
    dispose(graph);

    const floydGraph = makeFloydGraph(constructors.DiGraphInt);
    const floydMs = measureMs(config.iterations, () => {
        consume(toArray(floydGraph.floydWarshallAllPairsShortestPaths()).length);
    });
    printRow(layer, "floyd_warshall_all_pairs", config.iterations, floydMs, `nodes=${config.floydNodes}`);
    dispose(floydGraph);

    const attrGraph = new constructors.DiGraphInt();
    const attrMs = measureMs(config.iterations, () => {
        for (let i = 0; i < config.attrOps; i += 1) {
            const node = i % config.nodes;
            attrGraph.setNodeAttr(node, "label", `node-${node}`);
            consume(attrGraph.getNodeAttr(node, "label"));
        }
    });
    printRow(layer, "attribute_roundtrip", config.iterations, attrMs, `ops_per_iteration=${config.attrOps}`);
    dispose(attrGraph);

    const multigraphMs = measureMs(config.iterations, () => {
        const multigraph = new constructors.MultiDiGraphInt();
        for (let i = 0; i < config.multigraphEdges; i += 1) {
            multigraph.addEdge(i % config.nodes, (i + 1) % config.nodes, 1);
        }
        const ids = toArray(multigraph.edgeIdsBetween(0, 1));
        for (const id of ids) {
            multigraph.setEdgeAttrById(id, "capacity", id + 1);
        }
        consume(ids.length);
        dispose(multigraph);
    });
    printRow(layer, "multigraph_edge_ids", config.iterations, multigraphMs, `edges=${config.multigraphEdges}`);
}

printHeader();
const rawRuntime = await nxpp.createNxpp();
runLayer("raw_wasm", rawRuntime);
runLayer("facade_ts", nxpp);
