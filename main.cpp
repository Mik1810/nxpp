#include <iostream>
#include <string>

#include "include/nxpp.hpp"

using namespace nxpp;
using namespace std;

void test_dijkstra_scc() {
    // 1. Inizializza un grafo diretto con chiavi testuali
    auto G = DiGraph();

    // 2. Popolamento
    G.add_edge("Milano", "Roma", 5.0);
    G.add_edge("Roma", "Napoli", 2.5);
    G.add_edge("Venezia", "Milano", 2.0);
    G.add_edge("Napoli", "Roma", 3.0); 

    // 3. Stampa dei Nodi
    print("Nodi:");
    for (const auto& n : G.nodes()) {
        print("-", n);
    }
    print("\n");

    // Stampa di un peso salvato
    print("Peso Milano -> Roma:", G.get_edge_weight("Milano", "Roma"), "\n");

    // 4. Dijkstra Shortest Path
    print("Dijkstra (Venezia -> Napoli):");
    auto path = dijkstra_path(G, string("Venezia"), string("Napoli"));
    for (const auto& n : path) {
        print("->", n);
    }
    print("\n");

    // 5. Componenti Fortemente Connesse (SCC)
    print("Componenti Fortemente Connesse (SCC):");
    auto sccs = strongly_connected_components(G);
    for (size_t i = 0; i < sccs.size(); ++i) {
        print("SCC", i + 1, ":");
        for (const auto& n : sccs[i]) {
            print("  *", n);
        }
    }
    print("\n");
}

void test_integer_nodes() {
    // 1. Inizializza un grafo NON orientato con chiavi Intere
    Graph<int> G;

    // 2. Aggiunge più archi in blocco
    G.add_edges_from({
        {1, 2}, {2, 3}, {3, 4},
        {5, 6}, {6, 7} // Un'isola disconnessa
    });

    print("Nodi (Interi):");
    for (const auto& n : G.nodes()) {
        print("-", n);
    }
    print("\n");

    // 3. Esegue una visita BFS in ampiezza dal nodo 1
    print("Albero BFS (partendo dal nodo 1):");
    auto edges = bfs_edges(G, 1);
    for (const auto& e : edges) {
        print(" (", e.first, "->", e.second, ")");
    }
    print("\n\n");

    // 4. Trova le Componenti Connesse generiche (CC)
    print("Componenti Connesse (Undirected):");
    auto ccs = connected_components(G);
    for (size_t i = 0; i < ccs.size(); ++i) {
        print("CC", i + 1, ":");
        for (const auto& n : ccs[i]) {
            print("  *", n);
        }
    }
    print("\n");
}

int main() {
    test_dijkstra_scc();
    test_integer_nodes();
    return 0;
}