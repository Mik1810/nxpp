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

void test_destruction() {
    auto G = DiGraph();

    G.add_edges_from({
        {"A", "B"}, {"A", "C"}, {"B", "D"}, {"C", "D"}, {"D", "E"}
    });

    print("Neighbors di A:");
    for (const auto& n : G.neighbors("A")) {
        print("-", n);
    }
    print("\n");

    print("Grafo iniziale:");
    for (const auto& n : G.nodes()) print(n);
    print("\n");

    G.remove_node("B");
    print("Dopo aver rimosso B:");
    for (const auto& n : G.nodes()) print(n);
    print("\n");

    G.remove_edge("C", "D");

    print("Dopo aver rimosso C->D, l'albero BFS da A esplora:");
    auto edges = bfs_edges(G, string("A"));
    for (const auto& e : edges) {
        print(" (", e.first, "->", e.second, ")");
    }
    print("\n\n");

    G.clear();
    print("Dopo clear(), quanti nodi rimangono?", G.nodes().size(), "\n");
}

void test_multigraph() {
    print("=== NXPP (C++) - MultiGraph Test ===\n");
    auto MG = MultiDiGraph();
    MG.add_edge("Milano", "Roma", 5.0);
    MG.add_edge("Milano", "Roma", 2.5);

    auto G = DiGraph();
    G.add_edge("Milano", "Roma", 5.0);
    G.add_edge("Milano", "Roma", 2.5);

    print("Archi in MultiDiGraph:");
    for (const auto& edge : MG.edges()) {
        print(" (", std::get<0>(edge), "->", std::get<1>(edge), ") peso:", std::get<2>(edge));
    }
    print("\n");

    print("Archi in DiGraph (Standard):");
    for (const auto& edge : G.edges()) {
        print(" (", std::get<0>(edge), "->", std::get<1>(edge), ") peso:", std::get<2>(edge));
    }
    print("\n");
}

int main() {
    test_dijkstra_scc();
    test_integer_nodes();
    test_destruction();
    test_multigraph();
    return 0;
}