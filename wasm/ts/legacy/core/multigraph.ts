import { createMultiGraphClasses } from "../../core/multigraph.js";
import { runtime } from "../load.js";

const graphClasses = createMultiGraphClasses(runtime);

export const MultiGraphInt = graphClasses.MultiGraphInt;
export type MultiGraphInt = InstanceType<typeof MultiGraphInt>;

export const MultiGraphStr = graphClasses.MultiGraphStr;
export type MultiGraphStr = InstanceType<typeof MultiGraphStr>;

export const MultiDiGraphInt = graphClasses.MultiDiGraphInt;
export type MultiDiGraphInt = InstanceType<typeof MultiDiGraphInt>;

export const MultiDiGraphStr = graphClasses.MultiDiGraphStr;
export type MultiDiGraphStr = InstanceType<typeof MultiDiGraphStr>;
