import path from "node:path";
import { fileURLToPath } from "node:url";

import ts from "typescript";

import {
    graphClassNames,
    rawDeclarationPath,
    readRawContract,
} from "./raw_contract.mjs";

const scriptsDirectory = path.dirname(fileURLToPath(import.meta.url));
const rawTypesPath = path.resolve(scriptsDirectory, "../ts/internal/wasm_types.ts");

function fail(message) {
    throw new Error(`[WASM-RAW-CONTRACT] ${message}`);
}

function assertSameNames(actual, expected, label) {
    const actualSorted = [...actual].sort();
    const expectedSorted = [...expected].sort();
    if (actualSorted.length !== expectedSorted.length
        || actualSorted.some((name, index) => name !== expectedSorted[index])) {
        fail(`${label} mismatch: expected [${expectedSorted.join(", ")}], got [${actualSorted.join(", ")}].`);
    }
}

function exportedType(checker, source, exportName) {
    const moduleSymbol = checker.getSymbolAtLocation(source);
    const symbol = moduleSymbol && checker
        .getExportsOfModule(moduleSymbol)
        .find((candidate) => candidate.name === exportName);
    if (!symbol || !symbol.declarations?.[0]) {
        fail(`missing exported type ${exportName} in ${source.fileName}.`);
    }
    const declaration = symbol.declarations[0];
    return checker.getTypeAtLocation(declaration.name ?? declaration);
}

function propertyType(checker, ownerType, propertyName, label) {
    const symbol = checker.getPropertyOfType(ownerType, propertyName);
    if (!symbol || !symbol.declarations?.[0]) {
        fail(`missing ${label} property ${propertyName}.`);
    }
    return checker.getTypeOfSymbolAtLocation(symbol, symbol.declarations[0]);
}

function constructorInstance(checker, moduleType, className, label) {
    const constructorType = propertyType(checker, moduleType, className, label);
    const signatures = constructorType.getConstructSignatures();
    if (signatures.length !== 1) {
        fail(`${label} constructor ${className} must have exactly one construct signature.`);
    }
    return signatures[0].getReturnType();
}

function callableProperties(checker, type) {
    const result = new Map();
    for (const property of checker.getPropertiesOfType(type)) {
        if (!property.declarations?.[0]) {
            continue;
        }
        const propertyValue = checker.getTypeOfSymbolAtLocation(property, property.declarations[0]);
        const signatures = propertyValue.getCallSignatures();
        if (signatures.length > 0) {
            result.set(property.name, signatures[0]);
        }
    }
    return result;
}

function primitiveKind(type) {
    if (type.flags & ts.TypeFlags.NumberLike) {
        return "number";
    }
    if (type.flags & ts.TypeFlags.BooleanLike) {
        return "boolean";
    }
    if (type.flags & ts.TypeFlags.Void) {
        return "void";
    }
    if (type.flags & ts.TypeFlags.Any) {
        return "any";
    }
    return "other";
}

const contract = readRawContract();
assertSameNames(contract.classes.keys(), graphClassNames, "generated graph classes");
assertSameNames(contract.constructors, graphClassNames, "generated graph constructors");

const program = ts.createProgram([rawDeclarationPath, rawTypesPath], {
    target: ts.ScriptTarget.ES2022,
    module: ts.ModuleKind.NodeNext,
    moduleResolution: ts.ModuleResolutionKind.NodeNext,
    strict: true,
    skipLibCheck: true,
    noEmit: true,
});
const diagnostics = ts.getPreEmitDiagnostics(program);
if (diagnostics.length > 0) {
    fail(ts.formatDiagnosticsWithColorAndContext(diagnostics, {
        getCanonicalFileName: (fileName) => fileName,
        getCurrentDirectory: () => process.cwd(),
        getNewLine: () => "\n",
    }));
}

const checker = program.getTypeChecker();
const rawTypesSource = program.getSourceFile(rawTypesPath);
if (!rawTypesSource) {
    fail(`unable to load ${rawTypesPath}.`);
}
const rawModuleType = exportedType(checker, rawTypesSource, "RawRuntimeModule");
assertSameNames(
    checker.getPropertiesOfType(rawModuleType).map((property) => property.name),
    [...contract.runtimeFunctions, ...graphClassNames],
    "RawRuntimeModule exports",
);

for (const className of graphClassNames) {
    const rawInstance = constructorInstance(checker, rawModuleType, className, "RawRuntimeModule");
    const rawMethods = callableProperties(checker, rawInstance);
    const generatedMethods = contract.classes.get(className) ?? [];
    const expectedMethods = ["delete", ...generatedMethods.map((method) => method.name)];

    assertSameNames(rawMethods.keys(), expectedMethods, `${className} methods`);
    for (const method of generatedMethods) {
        const rawSignature = rawMethods.get(method.name);
        const rawArity = rawSignature?.parameters.length;
        if (!rawSignature || rawArity !== method.parameters) {
            fail(`${className}.${method.name} arity mismatch: expected ${method.parameters}, got ${rawArity ?? "missing"}.`);
        }

        for (const [index, generatedType] of method.parameterTypes.entries()) {
            if (!["number", "boolean", "void"].includes(generatedType)) {
                continue;
            }
            const parameter = rawSignature.parameters[index];
            const declaration = parameter.valueDeclaration ?? parameter.declarations?.[0];
            const rawType = declaration && checker.getTypeOfSymbolAtLocation(parameter, declaration);
            if (!rawType || primitiveKind(rawType) !== generatedType) {
                fail(`${className}.${method.name} parameter ${index} must refine generated ${generatedType}.`);
            }
        }

        if (["number", "boolean", "void"].includes(method.returnType)
            && primitiveKind(rawSignature.getReturnType()) !== method.returnType) {
            fail(`${className}.${method.name} must return generated ${method.returnType}.`);
        }
    }
}

console.log("[WASM-RAW-CONTRACT] generated bindings, refined raw types, and class capabilities match");
