# nxpp (npm wasm package)

Experimental Node.js wasm package for nxpp.

## Usage

```js
import nxpp from "@mik1810/nxpp-wasm";

const g = new nxpp.DiGraph();
g.addEdge(1, 2, 1);
g.addEdge(2, 3, 2);
g.addEdge(1, 3, 5);

const r = g.dijkstraShortestPaths(1);
console.log(r.distanceTo(3));
```

## Local publish dry-run

```bash
cd wasm
npm pack
```
