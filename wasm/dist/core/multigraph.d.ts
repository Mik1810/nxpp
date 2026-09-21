import type { ConnectedComponents, MultiDiGraph, MultiGraph, StronglyConnectedComponents } from "../types.js";
import type { RawMultiGraph, RawRuntimeModule } from "../internal/wasm_types.js";
export interface MultiGraphIntFacade extends MultiGraph<number>, ConnectedComponents<number> {
    subgraph(nodes: number[]): MultiGraphIntFacade;
}
export interface MultiGraphStrFacade extends MultiGraph<string>, ConnectedComponents<string> {
    subgraph(nodes: string[]): MultiGraphStrFacade;
}
export interface MultiDiGraphIntFacade extends MultiDiGraph<number>, StronglyConnectedComponents<number> {
    subgraph(nodes: number[]): MultiDiGraphIntFacade;
}
export interface MultiDiGraphStrFacade extends MultiDiGraph<string>, StronglyConnectedComponents<string> {
    subgraph(nodes: string[]): MultiDiGraphStrFacade;
}
export interface MultiGraphClasses {
    readonly MultiGraphInt: new (raw?: RawMultiGraph<number>) => MultiGraphIntFacade;
    readonly MultiGraphStr: new (raw?: RawMultiGraph<string>) => MultiGraphStrFacade;
    readonly MultiDiGraphInt: new (raw?: RawMultiGraph<number>) => MultiDiGraphIntFacade;
    readonly MultiDiGraphStr: new (raw?: RawMultiGraph<string>) => MultiDiGraphStrFacade;
}
export declare function createMultiGraphClasses(runtime: RawRuntimeModule): MultiGraphClasses;
