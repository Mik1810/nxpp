import { createSimpleGraphClasses } from "../../core/graph.js";
import { runtime } from "../load.js";

const graphClasses = createSimpleGraphClasses(runtime);

export const GraphInt = graphClasses.GraphInt;
export type GraphInt = InstanceType<typeof GraphInt>;

export const GraphStr = graphClasses.GraphStr;
export type GraphStr = InstanceType<typeof GraphStr>;

export const DiGraphInt = graphClasses.DiGraphInt;
export type DiGraphInt = InstanceType<typeof DiGraphInt>;

export const DiGraphStr = graphClasses.DiGraphStr;
export type DiGraphStr = InstanceType<typeof DiGraphStr>;
