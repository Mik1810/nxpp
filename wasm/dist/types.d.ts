export type NodeId = number | string;
export interface EdgeEndpoints<T extends NodeId> {
    source(): T;
    target(): T;
}
export interface Graph<T extends NodeId> {
    addNode(id: T): void;
    addEdge(source: T, target: T, weight: number): void;
    hasNode(id: T): boolean;
    hasEdge(source: T, target: T): boolean;
    nodes(): T[];
    neighbors(id: T): T[];
    removeNode(id: T): void;
    removeEdge(source: T, target: T): void;
    getEdgeWeight(source: T, target: T): number;
    setEdgeWeight(source: T, target: T, weight: number): void;
    clear(): void;
}
export interface DiGraph<T extends NodeId> extends Graph<T> {
}
export interface MultiGraph<T extends NodeId> extends Graph<T> {
    hasEdgeId(edgeId: number): boolean;
    edgeIds(): number[];
    edgeIdsBetween(source: T, target: T): number[];
    getEdgeEndpoints(edgeId: number): EdgeEndpoints<T>;
    getEdgeWeightById(edgeId: number): number;
    setEdgeWeightById(edgeId: number, weight: number): void;
    removeEdgeById(edgeId: number): void;
}
export interface MultiDiGraph<T extends NodeId> extends MultiGraph<T> {
}
