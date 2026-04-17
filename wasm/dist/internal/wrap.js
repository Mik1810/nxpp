export function toArray(value) {
    return Array.from(value);
}
export function toEdgeEndpoints(value) {
    return {
        source: () => value.source(),
        target: () => value.target(),
    };
}
