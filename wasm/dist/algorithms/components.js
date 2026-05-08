import { toArray } from "../internal/wrap.js";
export function toComponentGroups(raw) {
    return toArray(raw).map((component) => toArray(component));
}
