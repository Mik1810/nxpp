import assert from "node:assert/strict";
import nxpp from "@mik1810/nxpp-wasm";
import {
    graphClassNames,
    intersectMethodNames,
    readRawContract,
} from "../../scripts/raw_contract.mjs";

const rawContract = readRawContract();
const multiGraphClassNames = graphClassNames.filter((className) => className.startsWith("Multi"));
const allGeneratedMethodNames = new Set(
    [...rawContract.classes.values()].flatMap((methods) => methods.map((method) => method.name)),
);

export const expectedSimpleMethods = [
    ...intersectMethodNames(rawContract, graphClassNames),
    "dispose",
];

export const expectedMultiMethods = [
    ...intersectMethodNames(rawContract, multiGraphClassNames),
    "dispose",
];

export function expectedFacadeMethods(className) {
    const methods = rawContract.classes.get(className);
    assert.notEqual(methods, undefined, `Missing generated raw contract for ${className}`);
    return [...methods.map((method) => method.name), "dispose"];
}

export function assertMethods(target, methods, graphName) {
    for (const methodName of methods) {
        assert.equal(
            typeof target[methodName],
            "function",
            `Expected method ${methodName} to exist on ${graphName}`,
        );
    }
}

export function assertGeneratedCapabilities(target, className) {
    const expected = new Set(expectedFacadeMethods(className));
    for (const methodName of allGeneratedMethodNames) {
        assert.equal(
            typeof target[methodName],
            expected.has(methodName) ? "function" : "undefined",
            `${className} capability mismatch for ${methodName}()`,
        );
    }
}

export function assertThrows(fn, message) {
    let threw = false;
    try {
        fn();
    } catch {
        threw = true;
    }
    assert.equal(threw, true, message);
}

export function assertThrowsMessage(fn, expectedMessage, message) {
    let thrown;
    try {
        fn();
    } catch (error) {
        thrown = error;
    }

    assert.notEqual(thrown, undefined, message);
    assert.equal(thrown.message, expectedMessage, message);
}

export function assertThrowsMessageIncludes(fn, expectedText, message) {
    let thrown;
    try {
        fn();
    } catch (error) {
        thrown = error;
    }

    assert.notEqual(thrown, undefined, message);
    assert.equal(
        thrown.message.includes(expectedText),
        true,
        `${message}: expected "${thrown.message}" to include "${expectedText}"`,
    );
}

export function assertUnownedValue(value, message) {
    assert.notEqual(value, null, message);
    assert.equal(typeof value, "object", message);
    assert.equal(typeof value.delete, "undefined", `${message}: value must not expose delete()`);
}

export function toSortedNumbers(values) {
    return Array.from(values).sort((a, b) => a - b);
}

export function toSortedStrings(values) {
    return Array.from(values).sort();
}

export { assert, nxpp };
