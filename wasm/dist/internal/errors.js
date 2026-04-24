export const disposedGraphMessage = "WASM graph operation failed: graph has been disposed.";
export function normalizeWasmGraphError(error) {
    if (error instanceof Error) {
        if (error.message.startsWith("WASM graph operation failed:")) {
            return error;
        }
        return new Error(`WASM graph operation failed: ${error.message}`);
    }
    if (typeof error === "string") {
        return new Error(`WASM graph operation failed: ${error}`);
    }
    if (typeof error === "object" && error !== null && "message" in error) {
        const message = error.message;
        if (typeof message === "string" && message.length > 0) {
            return new Error(`WASM graph operation failed: ${message}`);
        }
    }
    if (typeof error === "object" && error !== null) {
        const text = String(error);
        if (text !== "[object Object]") {
            return new Error(`WASM graph operation failed: ${text}`);
        }
    }
    return new Error("WASM graph operation failed: unknown runtime error.");
}
export function wrapRawGraph(raw) {
    return new Proxy(raw, {
        get(target, property, receiver) {
            const value = Reflect.get(target, property, receiver);
            if (typeof value !== "function") {
                return value;
            }
            return (...args) => {
                try {
                    return value.apply(target, args);
                }
                catch (error) {
                    throw normalizeWasmGraphError(error);
                }
            };
        },
    });
}
