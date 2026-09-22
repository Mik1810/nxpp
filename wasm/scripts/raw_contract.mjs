import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";

import ts from "typescript";

const scriptsDirectory = path.dirname(fileURLToPath(import.meta.url));

export const rawDeclarationPath = path.resolve(
    scriptsDirectory,
    "../generated/nxpp_node.raw.d.ts",
);

export const graphClassNames = Object.freeze([
    "GraphInt",
    "GraphStr",
    "DiGraphInt",
    "DiGraphStr",
    "MultiGraphInt",
    "MultiGraphStr",
    "MultiDiGraphInt",
    "MultiDiGraphStr",
]);

function memberName(member) {
    return member.name && ts.isIdentifier(member.name) ? member.name.text : undefined;
}

function interfaceMethods(node, source) {
    return node.members
        .filter(ts.isMethodSignature)
        .map((member) => ({
            name: memberName(member),
            parameters: member.parameters.length,
            parameterTypes: member.parameters.map((parameter) => parameter.type?.getText(source) ?? "any"),
            returnType: member.type?.getText(source) ?? "any",
        }))
        .filter((method) => method.name !== undefined);
}

export function readRawContract(declarationPath = rawDeclarationPath) {
    const sourceText = fs.readFileSync(declarationPath, "utf8");
    const source = ts.createSourceFile(
        declarationPath,
        sourceText,
        ts.ScriptTarget.Latest,
        true,
        ts.ScriptKind.TS,
    );

    const classes = new Map();
    const constructors = [];
    const runtimeFunctions = [];

    for (const node of source.statements) {
        if (ts.isInterfaceDeclaration(node) && graphClassNames.includes(node.name.text)) {
            classes.set(node.name.text, interfaceMethods(node, source));
            continue;
        }

        if (ts.isInterfaceDeclaration(node) && node.name.text === "EmbindModule") {
            for (const member of node.members) {
                const name = memberName(member);
                if (name !== undefined) {
                    constructors.push(name);
                }
            }
            continue;
        }

        if (ts.isModuleDeclaration(node) && node.name.text === "RuntimeExports" && node.body && ts.isModuleBlock(node.body)) {
            for (const statement of node.body.statements) {
                if (ts.isFunctionDeclaration(statement) && statement.name) {
                    runtimeFunctions.push(statement.name.text);
                }
            }
        }
    }

    return {
        classes,
        constructors,
        runtimeFunctions,
    };
}

export function intersectMethodNames(contract, classNames) {
    const [firstClass, ...remainingClasses] = classNames;
    const firstMethods = contract.classes.get(firstClass) ?? [];
    const remainingSets = remainingClasses.map(
        (className) => new Set((contract.classes.get(className) ?? []).map((method) => method.name)),
    );

    return firstMethods
        .map((method) => method.name)
        .filter((methodName) => remainingSets.every((methods) => methods.has(methodName)));
}
