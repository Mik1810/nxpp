import type { DiGraphIntFacade, DiGraphStrFacade, GraphIntFacade, GraphStrFacade } from "../core/graph.js";
import type { MultiDiGraphIntFacade, MultiDiGraphStrFacade, MultiGraphIntFacade, MultiGraphStrFacade } from "../core/multigraph.js";
import type { RawRuntimeModule } from "../internal/wasm_types.js";
export type NxppModuleOptions = Record<string, unknown>;
export interface NxppRuntimeLoader {
    load(options: NxppModuleOptions): Promise<unknown>;
}
type FacadeConstructor<T> = new () => T;
export interface NxppRuntime {
    readonly GraphInt: FacadeConstructor<GraphIntFacade>;
    readonly GraphStr: FacadeConstructor<GraphStrFacade>;
    readonly DiGraphInt: FacadeConstructor<DiGraphIntFacade>;
    readonly DiGraphStr: FacadeConstructor<DiGraphStrFacade>;
    readonly MultiGraphInt: FacadeConstructor<MultiGraphIntFacade>;
    readonly MultiGraphStr: FacadeConstructor<MultiGraphStrFacade>;
    readonly MultiDiGraphInt: FacadeConstructor<MultiDiGraphIntFacade>;
    readonly MultiDiGraphStr: FacadeConstructor<MultiDiGraphStrFacade>;
}
export declare function requireRawRuntimeModule(runtime: unknown): RawRuntimeModule;
export declare function createNxppRuntime(runtime: unknown): NxppRuntime;
export declare function createNxppWithLoader(loader: NxppRuntimeLoader, options?: NxppModuleOptions): Promise<NxppRuntime>;
export {};
