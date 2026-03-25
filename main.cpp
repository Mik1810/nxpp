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
    for (auto& scc : sccs) std::sort(scc.begin(), scc.end());
    std::sort(sccs.begin(), sccs.end(), [](const auto& a, const auto& b) {
        return a[0] < b[0];
    });

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
    for (auto& cc : ccs) std::sort(cc.begin(), cc.end());
    std::sort(ccs.begin(), ccs.end(), [](const auto& a, const auto& b) {
        return a[0] < b[0];
    });

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
    print("NXPP (C++) - MultiGraph Test\n");
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

void test_attributes() {
    print("NXPP (C++) - Custom Attributes Test\n");
    auto G = DiGraph();
    
    // Test Node Attributes
    G.node("Roma")["population"] = 2800000;
    G.node("Milano")["is_capital"] = false;

    // Test Edge Attributes
    G["Roma"]["Milano"] = 5.0; // Peso predefinito
    G["Roma"]["Milano"]["company"] = std::string("Trenitalia");
    G["Roma"]["Milano"]["distance"] = 580;

    int pop = G.get_node_attr<int>("Roma", "population");
    std::string company = G.get_edge_attr<std::string>("Roma", "Milano", "company");
    int dist = G.get_edge_attr<int>("Roma", "Milano", "distance");

    print("Popolazione di Roma:", pop);
    print("Compagnia Treno Roma->Milano:", company);
    print("Distanza:", dist, "km\n");

    auto missing_speed = G.try_get_edge_attr<int>("Roma", "Milano", "speed");
    print("Attributo speed presente?", missing_speed.has_value());
}

void test_generators() {
    print("NXPP (C++) - Generators Test\n");
    
    auto G = nxpp::complete_graph(5);
    print("Complete Graph (5) - Nodi:", G.nodes().size(), "Archi:", G.edges().size());
    
    auto P = nxpp::path_graph(4);
    print("Path Graph (4) - Archi:");
    for (const auto& e : P.edges()) {
        print(" (", std::get<0>(e), "->", std::get<1>(e), ")");
    }
    print("\n");
}

void test_degree_centrality() {
    print("NXPP (C++) - Degree Centrality Test\n");

    Graph<int> G;
    G.add_edges_from({
        {0, 1}, {0, 2}, {1, 2}, {2, 3}
    });

    auto centrality = nxpp::degree_centrality(G);
    auto nodes = G.nodes();
    std::sort(nodes.begin(), nodes.end());

    for (const auto& node : nodes) {
        print("Node", node, "centrality:", centrality.at(node));
    }
    print("\n");
}

void test_directed_neighbors_api() {
    print("NXPP (C++) - Directed Neighbor API Test\n");

    auto G = DiGraph();
    G.add_edges_from({
        {"A", "B"}, {"C", "B"}, {"B", "D"}
    });

    print("neighbors(B):");
    for (const auto& node : G.neighbors("B")) {
        print("-", node);
    }

    print("successors(B):");
    for (const auto& node : G.successors("B")) {
        print("-", node);
    }

    print("predecessors(B):");
    for (const auto& node : G.predecessors("B")) {
        print("-", node);
    }
    print("\n");
}

void test_missing_algorithm_wrappers() {
    print("NXPP (C++) - Wrapper Coverage Test\n");

    Graph<int> G;
    G.add_edges_from({
        {0, 1}, {1, 2}, {2, 3}
    });

    auto bfsT = nxpp::bfs_tree(G, 0);
    auto dfsT = nxpp::dfs_tree(G, 0);
    auto sp = nxpp::shortest_path(G, 0, 3);
    auto spl = nxpp::shortest_path_length(G, 0, 3);
    auto weighted_sp = nxpp::shortest_path(G, 0, 3, "weight");
    auto weighted_spl = nxpp::shortest_path_length(G, 0, 3, "weight");
    auto bfp = nxpp::bellman_ford_path(G, 0, 3, "weight");
    auto bfpl = nxpp::bellman_ford_path_length(G, 0, 3, "weight");

    print("BFS tree edges:", bfsT.edges().size());
    print("DFS tree edges:", dfsT.edges().size());
    print("Shortest path length:", sp.size());
    print("Shortest path distance:", spl);
    print("Weighted shortest path length:", weighted_sp.size());
    print("Weighted shortest path distance:", weighted_spl);
    print("Bellman-Ford path length:", bfp.size());
    print("Bellman-Ford path distance:", bfpl, "\n");
}

int main() {
    test_dijkstra_scc();
    test_integer_nodes();
    test_destruction();
    test_multigraph();
    test_attributes();
    test_generators();
    test_degree_centrality();
    test_directed_neighbors_api();
    test_missing_algorithm_wrappers();
    return 0;
}
