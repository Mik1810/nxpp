export function assertFiniteNumber(value, label) {
    if (typeof value !== "number" || !Number.isFinite(value)) {
        throw new TypeError(`${label} must be a finite number.`);
    }
}
export function assertAttributeValue(value, label) {
    if (typeof value === "string" || typeof value === "boolean") {
        return;
    }
    if (typeof value === "number" && Number.isFinite(value)) {
        return;
    }
    throw new TypeError(`${label} must be a string, finite number, or boolean.`);
}
export function assertIntNodeId(value, label) {
    if (!Number.isInteger(value)) {
        throw new TypeError(`${label} must be an integer-valued number for *Int graphs.`);
    }
}
export function assertStringNodeId(value, label) {
    if (typeof value !== "string") {
        throw new TypeError(`${label} must be a string for *Str graphs.`);
    }
}
export function assertEdgeId(value, label = "edgeId") {
    if (!Number.isInteger(value) || value < 0) {
        throw new TypeError(`${label} must be a non-negative integer.`);
    }
}
