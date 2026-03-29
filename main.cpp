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
    auto path = G.dijkstra_path(string("Venezia"), string("Napoli"));
    for (const auto& n : path) {
        print("->", n);
    }
    print("\n");

    // 5. Componenti Fortemente Connesse (SCC)
    print("Componenti Fortemente Connesse (SCC):");
    auto sccs = G.strongly_connected_components();
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
    auto edges = G.bfs_edges(1);
    for (const auto& e : edges) {
        print(" (", e.first, "->", e.second, ")");
    }
    print("\n\n");

    // 4. Trova le Componenti Connesse generiche (CC)
    print("Componenti Connesse (Undirected):");
    auto ccs = G.connected_component_groups();
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
    auto edges = G.bfs_edges(string("A"));
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

    auto centrality = G.degree_centrality();
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

    auto bfsT = G.bfs_tree(0);
    auto dfsT = G.dfs_tree(0);
    auto sp = G.shortest_path(0, 3);
    auto spl = G.shortest_path_length(0, 3);
    auto weighted_sp = G.shortest_path(0, 3, "weight");
    auto weighted_spl = G.shortest_path_length(0, 3, "weight");
    auto bfp = G.bellman_ford_path(0, 3, "weight");
    auto bfpl = G.bellman_ford_path_length(0, 3, "weight");

    print("BFS tree edges:", bfsT.edges().size());
    print("DFS tree edges:", dfsT.edges().size());
    print("Shortest path node count:", sp.size());
    print("Shortest path distance:", spl);
    print("Weighted shortest path node count:", weighted_sp.size());
    print("Weighted shortest path distance:", weighted_spl);
    print("Bellman-Ford path node count:", bfp.size());
    print("Bellman-Ford path distance:", bfpl, "\n");
}

void test_snippet_backed_essentials() {
    print("NXPP (C++) - Snippet-Backed Essentials Test\n");

    DiGraphInt dag;
    dag.add_edges_from({
        {5, 2}, {5, 0}, {4, 0}, {4, 1}, {2, 3}, {3, 1}
    });
    auto topo = dag.topological_sort();
    print("Topological sort size:", topo.size());

    Graph<int> wg;
    wg.add_edges_from({
        {0, 1, 2.0}, {0, 3, 6.0}, {1, 2, 3.0},
        {1, 3, 8.0}, {1, 4, 5.0}, {2, 4, 7.0}, {3, 4, 9.0}
    });
    auto mst = wg.kruskal_minimum_spanning_tree();
    auto prim = wg.prim_minimum_spanning_tree(0);
    print("Kruskal MST edges:", mst.size());
    print("Prim parent entries:", prim.size());

    DiGraphInt flow_g;
    flow_g.add_edge(0, 1, 1.0);
    flow_g.add_edge(0, 3, 2.0);
    flow_g.add_edge(1, 2, 3.0);
    flow_g.add_edge(1, 4, 4.0);
    flow_g.add_edge(2, 5, 2.0);
    flow_g.add_edge(3, 2, 2.0);
    flow_g.add_edge(4, 5, 2.0);
    flow_g[0][1]["capacity"] = 1L;
    flow_g[0][3]["capacity"] = 2L;
    flow_g[1][2]["capacity"] = 3L;
    flow_g[1][4]["capacity"] = 4L;
    flow_g[2][5]["capacity"] = 2L;
    flow_g[3][2]["capacity"] = 2L;
    flow_g[4][5]["capacity"] = 2L;
    auto flow = flow_g.maximum_flow(0, 5);
    auto cut = flow_g.minimum_cut(0, 5);
    print("Maximum flow:", flow.value);
    print("Flow on (1,4):", flow.flow[{1, 4}]);
    print("Minimum cut value:", cut.value);
    print("Minimum cut edges:", cut.cut_edges.size());

    DiGraphInt dag_sp;
    dag_sp.add_edges_from({
        {0, 2, 3.0}, {1, 0, 2.0}, {1, 2, 2.0},
        {1, 4, 5.0}, {2, 3, 1.0}, {3, 4, 5.0}
    });
    auto dag_dist = dag_sp.dag_shortest_paths(0);
    auto fw = dag_sp.floyd_warshall_all_pairs_shortest_paths();
    print("DAG shortest distance to 4:", dag_dist.distance[4]);
    print("Floyd-Warshall 0->4:", fw[0][4]);

    bool sat = nxpp::two_sat_satisfiable(2, {{1, 2}, {-1, 2}});
    print("2-SAT satisfiable:", sat, "\n");
}

void test_remaining_snippet_essentials() {
    print("NXPP (C++) - Remaining Snippet Essentials Test\n");

    DiGraphInt flow_g;
    flow_g.add_edge(0, 1, 2.0);
    flow_g.add_edge(0, 3, 3.0);
    flow_g.add_edge(1, 2, 5.0);
    flow_g.add_edge(1, 4, 1.0);
    flow_g.add_edge(2, 5, 3.0);
    flow_g.add_edge(3, 1, 1.0);
    flow_g.add_edge(3, 2, 6.0);
    flow_g.add_edge(4, 5, 1.0);

    flow_g[0][1]["capacity"] = 1L;
    flow_g[0][3]["capacity"] = 2L;
    flow_g[1][2]["capacity"] = 3L;
    flow_g[1][4]["capacity"] = 4L;
    flow_g[2][5]["capacity"] = 2L;
    flow_g[3][1]["capacity"] = 1L;
    flow_g[3][2]["capacity"] = 2L;
    flow_g[4][5]["capacity"] = 2L;

    auto mcmf_cc = flow_g.max_flow_min_cost_cycle_canceling(0, 5);
    auto mcmf_ssp = flow_g.successive_shortest_path_nonnegative_weights(0, 5);
    print("Cycle-canceling flow:", mcmf_cc.flow, "cost:", mcmf_cc.cost);
    print("SSP flow:", mcmf_ssp.flow, "cost:", mcmf_ssp.cost);

    DiGraphInt sccg;
    sccg.add_edges_from({
        {0, 2}, {1, 0}, {1, 4}, {2, 1}, {2, 3}, {3, 2}, {3, 4}
    });
    auto roots = sccg.strongly_connected_component_roots();
    print("SCC representative for 0:", roots.at(0));
    print("SCC representative for 4:", roots.at(4), "\n");
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
    test_snippet_backed_essentials();
    test_remaining_snippet_essentials();
    return 0;
}
