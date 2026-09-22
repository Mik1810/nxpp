export function toArray(value) {
    return [...value];
}
export function toEdgeEndpoints(value) {
    return {
        source: () => value.source,
        target: () => value.target,
    };
}
