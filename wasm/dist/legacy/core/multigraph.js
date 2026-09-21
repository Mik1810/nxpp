import { createMultiGraphClasses } from "../../core/multigraph.js";
import { runtime } from "../load.js";
const graphClasses = createMultiGraphClasses(runtime);
export const MultiGraphInt = graphClasses.MultiGraphInt;
export const MultiGraphStr = graphClasses.MultiGraphStr;
export const MultiDiGraphInt = graphClasses.MultiDiGraphInt;
export const MultiDiGraphStr = graphClasses.MultiDiGraphStr;
