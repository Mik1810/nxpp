"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var nxpp_wasm_1 = require("@mik1810/nxpp-wasm");
var graph = new nxpp_wasm_1.default.DiGraphInt();
graph.addEdge(1, 2, 1);
graph.addEdge(2, 3, 2);
graph.addEdge(1, 3, 5);
console.log("neighbors(1):", graph.neighbors(1));
console.log("weight(1,3):", graph.getEdgeWeight(1, 3));
