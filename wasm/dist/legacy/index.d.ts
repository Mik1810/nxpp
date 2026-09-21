export type { AllPairsShortestPathDistanceEntry, AllPairsShortestPathSourceEntry, AttributeValue, CentralityScoreEntry, ConnectedComponents, DiGraph, EdgeEndpoints, FlowEdgeEntry, FlowEdgeIdEntry, Graph, MaximumFlowResult, MinCostMaxFlowResult, MinimumCutResult, MultiDiGraph, MultiGraph, NodeId, ShortestPathDistanceEntry, ShortestPathPredecessorEntry, SpanningTreeEdge, SingleSourceShortestPathResult, StronglyConnectedComponents, TraversalEdge, TraversalPredecessorEntry, TraversalSuccessorEntry, TraversalTree, } from "../types.js";
export { createNxpp, loadNxppRuntime } from "./load.js";
export { DiGraphInt, DiGraphStr, GraphInt, GraphStr, } from "./core/graph.js";
export { MultiDiGraphInt, MultiDiGraphStr, MultiGraphInt, MultiGraphStr, } from "./core/multigraph.js";
import { createNxpp, loadNxppRuntime } from "./load.js";
declare const nxpp: {
    createNxpp: typeof createNxpp;
    loadNxppRuntime: typeof loadNxppRuntime;
    GraphInt: new (raw?: import("../internal/wasm_types.js").RawSimpleGraph<number>) => import("../core/graph.js").GraphIntFacade;
    GraphStr: new (raw?: import("../internal/wasm_types.js").RawSimpleGraph<string>) => import("../core/graph.js").GraphStrFacade;
    DiGraphInt: new (raw?: import("../internal/wasm_types.js").RawSimpleGraph<number>) => import("../core/graph.js").DiGraphIntFacade;
    DiGraphStr: new (raw?: import("../internal/wasm_types.js").RawSimpleGraph<string>) => import("../core/graph.js").DiGraphStrFacade;
    MultiGraphInt: new (raw?: import("../internal/wasm_types.js").RawMultiGraph<number>) => import("../core/multigraph.js").MultiGraphIntFacade;
    MultiGraphStr: new (raw?: import("../internal/wasm_types.js").RawMultiGraph<string>) => import("../core/multigraph.js").MultiGraphStrFacade;
    MultiDiGraphInt: new (raw?: import("../internal/wasm_types.js").RawMultiGraph<number>) => import("../core/multigraph.js").MultiDiGraphIntFacade;
    MultiDiGraphStr: new (raw?: import("../internal/wasm_types.js").RawMultiGraph<string>) => import("../core/multigraph.js").MultiDiGraphStrFacade;
};
export default nxpp;
