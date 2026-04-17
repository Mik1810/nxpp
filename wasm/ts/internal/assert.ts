export function assertFiniteNumber(value: unknown, label: string): asserts value is number {
  if (typeof value !== "number" || !Number.isFinite(value)) {
    throw new TypeError(`${label} must be a finite number.`);
  }
}

export function assertIntNodeId(value: unknown, label: string): asserts value is number {
  if (!Number.isInteger(value)) {
    throw new TypeError(`${label} must be an integer-valued number for *Int graphs.`);
  }
}

export function assertStringNodeId(value: unknown, label: string): asserts value is string {
  if (typeof value !== "string") {
    throw new TypeError(`${label} must be a string for *Str graphs.`);
  }
}

export function assertEdgeId(value: unknown, label = "edgeId"): asserts value is number {
  if (!Number.isInteger(value) || (value as number) < 0) {
    throw new TypeError(`${label} must be a non-negative integer.`);
  }
}
