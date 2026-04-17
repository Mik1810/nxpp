export type {
  DiGraph,
  EdgeEndpoints,
  Graph,
  MultiDiGraph,
  MultiGraph,
  NodeId,
} from "./types.js";

export { createNxpp, loadNxppRuntime } from "./load.js";

export {
  DiGraphInt,
  DiGraphStr,
  GraphInt,
  GraphStr,
} from "./core/graph.js";

export {
  MultiDiGraphInt,
  MultiDiGraphStr,
  MultiGraphInt,
  MultiGraphStr,
} from "./core/multigraph.js";

import {
  DiGraphInt,
  DiGraphStr,
  GraphInt,
  GraphStr,
} from "./core/graph.js";
import {
  MultiDiGraphInt,
  MultiDiGraphStr,
  MultiGraphInt,
  MultiGraphStr,
} from "./core/multigraph.js";
import { createNxpp, loadNxppRuntime } from "./load.js";

const nxpp = {
  createNxpp,
  loadNxppRuntime,
  GraphInt,
  GraphStr,
  DiGraphInt,
  DiGraphStr,
  MultiGraphInt,
  MultiGraphStr,
  MultiDiGraphInt,
  MultiDiGraphStr,
};

export default nxpp;
