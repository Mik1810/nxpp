# Project Roadmap: nxpp (NetworkX for C++)

## Objective
Creare una libreria C++ che offra una sintassi il più possibile identica e intuitiva a quella della libreria Python `networkx`, mantenendo però alte prestazioni e riducendo al minimo il *bloat*. Il progetto sfrutterà internamente strutture dati e algoritmi efficienti (come quelli della Boost Graph Library - BGL), esponendo però un'API pulita e user-friendly.

---

## 🏗 Fase 1: Strutture Dati Core
- [ ] Implementazione della classe `Graph` (Grafo non orientato).
- [ ] Implementazione della classe `DiGraph` (Grafo orientato).
- [ ] Implementazione delle classi `MultiGraph` e `MultiDiGraph`.
- [ ] Supporto per identificatori di nodi generici (es. interi, stringhe) tramite template o tipi flessibili.
- [ ] Metodi fondamentali identici a NetworkX: `add_node`, `add_nodes_from`, `add_edge`, `add_edges_from`, `remove_node`, `remove_edge`, `clear`.
- [ ] Supporto per attributi personalizzati su nodi e archi.

## 🐍 Fase 2: Wrappers API e Sintassi Python-like
- [ ] Overload degli operatori per replicare l'accesso a dizionario (es. `G[u][v]`).
- [ ] Viste e iteratori C++11/C++20 per simulare `G.nodes()`, `G.edges()`, `G.neighbors(n)`.
- [ ] Gestione degli errori intuitiva (simulazione di eccezioni come `NetworkXError`, `NodeNotFound`).

## ⚙️ Fase 3: Algoritmi Fondamentali (Porting & Integrazione BGL)
Integrazione e wrapping degli algoritmi della BGL per matcha le signature di NetworkX:
- [x] **Traversals**: `bfs_edges`, `bfs_tree`, `dfs_edges`, `dfs_tree`.
- [x] **Shortest Paths**: `shortest_path`, `dijkstra_path`, `bellman_ford_path`.
- [x] **Componenti**: `connected_components`, `strongly_connected_components`.
- [ ] **Generatori**: Generatori base come `complete_graph`, `path_graph`, `erdos_renyi_graph`.

## 🧪 Fase 4: Suite di Test Comparativa (C++ vs Python)
Questo è un traguardo fondamentale per garantire la correttezza della libreria.
- [x] **Generazione Dataset**: Creazione di script per generare grafi di input standardizzati (in formato edgelist, JSON o GraphML).
- [x] **Test Runner Python**: Script che esegue algoritmi specifici usano l'originale `networkx` e salva i risultati (nodi, archi, path, metriche) in file temporanei/JSON.
- [x] **Test Runner C++**: Suite di test su `nxpp` (es. tramite GTest/Catch2) che elabora gli stessi input.
- [x] **Validatore di Matching**: Script di comparazione automatica che verifica che gli output in C++ corrispondano *esattamente* agli output generati dallo script Python.

## 🚀 Fase 5: Algoritmi Avanzati e Flussi
Integrare le funzionalità più complesse basate sugli snippet BGL forniti:
- [ ] **Network Flow**: `maximum_flow` (es. Edmonds-Karp/Push-Relabel), `minimum_cut`.
- [ ] **Centrality**: `degree_centrality`, `betweenness_centrality`, `pagerank`.
- [ ] Benchmarking completo per comparare le performance (tempo di esecuzione, utilizzo memoria, tempo di compilazione) tra `nxpp` e `networkx`.

## 🔮 Orizzonti Futuri e Distribuzione
- [ ] Refactoring per rendere la libreria "Header-only" (studiando il trade-off sui tempi di compilazione).
- [ ] (Opzionale) Sviluppo di binding Python (es. tramite pybind11) in modo che la libreria possa essere usata in Python come backend drop-in ad altissime performance per NetworkX.
