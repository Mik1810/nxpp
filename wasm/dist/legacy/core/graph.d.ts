export declare const GraphInt: new (raw?: import("../../internal/wasm_types.js").RawSimpleGraph<number>) => import("../../core/graph.js").GraphIntFacade;
export type GraphInt = InstanceType<typeof GraphInt>;
export declare const GraphStr: new (raw?: import("../../internal/wasm_types.js").RawSimpleGraph<string>) => import("../../core/graph.js").GraphStrFacade;
export type GraphStr = InstanceType<typeof GraphStr>;
export declare const DiGraphInt: new (raw?: import("../../internal/wasm_types.js").RawSimpleGraph<number>) => import("../../core/graph.js").DiGraphIntFacade;
export type DiGraphInt = InstanceType<typeof DiGraphInt>;
export declare const DiGraphStr: new (raw?: import("../../internal/wasm_types.js").RawSimpleGraph<string>) => import("../../core/graph.js").DiGraphStrFacade;
export type DiGraphStr = InstanceType<typeof DiGraphStr>;
