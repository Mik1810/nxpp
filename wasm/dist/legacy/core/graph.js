import { createSimpleGraphClasses } from "../../core/graph.js";
import { runtime } from "../load.js";
const graphClasses = createSimpleGraphClasses(runtime);
export const GraphInt = graphClasses.GraphInt;
export const GraphStr = graphClasses.GraphStr;
export const DiGraphInt = graphClasses.DiGraphInt;
export const DiGraphStr = graphClasses.DiGraphStr;
