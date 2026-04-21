import { toArray } from "../internal/wrap.js";
class SingleSourceShortestPathResultImpl {
    distance;
    predecessor;
    distanceMap;
    predecessorMap;
    constructor(raw) {
        this.distance = toArray(raw.distance);
        this.predecessor = toArray(raw.predecessor);
        this.distanceMap = new Map(this.distance.map((entry) => [entry.node, entry.distance]));
        this.predecessorMap = new Map(this.predecessor.map((entry) => [entry.node, entry.predecessor]));
    }
    hasPathTo(target) {
        const distance = this.distanceMap.get(target);
        return distance !== undefined && Number.isFinite(distance);
    }
    pathTo(target) {
        const distance = this.distanceMap.get(target);
        if (distance === undefined) {
            throw new Error("Path reconstruction failed: target node not found in result.");
        }
        if (!Number.isFinite(distance)) {
            throw new Error("Path reconstruction failed: target node is unreachable.");
        }
        const path = [];
        let current = target;
        const maxHops = this.predecessorMap.size + 1;
        for (let hops = 0; hops <= maxHops; hops += 1) {
            path.push(current);
            const predecessor = this.predecessorMap.get(current);
            if (predecessor === undefined) {
                throw new Error("Path reconstruction failed: predecessor map is incomplete.");
            }
            if (predecessor === current) {
                path.reverse();
                return path;
            }
            current = predecessor;
        }
        throw new Error("Path reconstruction failed: predecessor cycle detected.");
    }
}
export function toSingleSourceShortestPathResult(raw) {
    return new SingleSourceShortestPathResultImpl(raw);
}
export function toAllPairsShortestPathMatrix(raw) {
    return toArray(raw).map((row) => toArray(row));
}
export function toAllPairsShortestPathMap(raw) {
    return toArray(raw).map((entry) => ({
        source: entry.source,
        distances: toArray(entry.distances),
    }));
}
