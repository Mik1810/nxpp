#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../include/nxpp.hpp"

// Utility per scrivere json array di tuple (archi)
void write_json(const std::string& path, const std::vector<std::pair<int, int>>& edges) {
    std::ofstream out(path);
    out << "[\n";
    for (size_t i = 0; i < edges.size(); ++i) {
        out << "  [" << edges[i].first << ", " << edges[i].second << "]";
        if (i < edges.size() - 1) out << ",";
        out << "\n";
    }
    out << "]\n";
}

// Utility per scrivere json array (path)
void write_json(const std::string& path, const std::vector<int>& path_nodes) {
    std::ofstream out(path);
    out << "[\n";
    for (size_t i = 0; i < path_nodes.size(); ++i) {
        out << "  " << path_nodes[i];
        if (i < path_nodes.size() - 1) out << ",";
        out << "\n";
    }
    out << "]\n";
}

// Utility per scrivere array di array (componenti)
void write_json(const std::string& path, const std::vector<std::vector<int>>& cc) {
    std::ofstream out(path);
    out << "[\n";
    for (size_t i = 0; i < cc.size(); ++i) {
        out << "  [";
        for (size_t j = 0; j < cc[i].size(); ++j) {
            out << cc[i][j];
            if (j < cc[i].size() - 1) out << ", ";
        }
        out << "]";
        if (i < cc.size() - 1) out << ",";
        out << "\n";
    }
    out << "]\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <edgelist_path>\n";
        return 1;
    }

    std::string edgelist_path = argv[1];
    std::ifstream infile(edgelist_path);
    if (!infile.is_open()) {
        std::cerr << "Error opening " << edgelist_path << "\n";
        return 1;
    }

    int u, v;
    int max_node = -1;
    std::vector<std::pair<int, int>> raw_edges;
    while (infile >> u >> v) {
        raw_edges.push_back({u, v});
        if (u > max_node) max_node = u;
        if (v > max_node) max_node = v;
    }
    
    // SimpleGraph unweighted
    SimpleGraph g;
    std::vector<SimpleGraph::Node> nodes;
    for (int i = 0; i <= max_node; ++i) {
        nodes.push_back(static_cast<SimpleGraph::Node>(i));
    }
    g.add_nodes_from(nodes);
    
    std::vector<std::pair<SimpleGraph::Node, SimpleGraph::Node>> graph_edges;
    for(auto& p : raw_edges) {
        graph_edges.push_back({static_cast<SimpleGraph::Node>(p.first), static_cast<SimpleGraph::Node>(p.second)});
    }
    g.add_edges_from(graph_edges);

    // 1. BFS da 0
    auto bfs_res = nxpp::bfs_edges(g, 0);
    write_json("bfs_actual.json", bfs_res);

    // 2. Dijkstra da 0 a 45
    // Per testare usiamo SimpleWeightedUndirectedGraph
    SimpleWeightedUndirectedGraph wg;
    std::vector<SimpleWeightedUndirectedGraph::Node> wnodes;
    for (int i = 0; i <= max_node; ++i) {
        wnodes.push_back(static_cast<SimpleWeightedUndirectedGraph::Node>(i));
    }
    wg.add_nodes_from(wnodes);
    
    std::vector<std::tuple<SimpleWeightedUndirectedGraph::Node, SimpleWeightedUndirectedGraph::Node, double>> wedges;
    for(auto p : raw_edges) {
        wedges.push_back({static_cast<SimpleWeightedUndirectedGraph::Node>(p.first), static_cast<SimpleWeightedUndirectedGraph::Node>(p.second), 1.0}); // peso 1
    }
    wg.add_edges_from(wedges);

    try {
        auto dijkstra_res = nxpp::dijkstra_path(wg, 0, 45);
        write_json("dijkstra_actual.json", dijkstra_res);
    } catch(const std::exception& e) {
        std::cerr << "Dijkstra error: " << e.what() << "\n";
    }

    // 3. Connected Components
    auto cc_res = nxpp::connected_components(g);
    write_json("cc_actual.json", cc_res);

    std::cout << "nxpp_test_runner completed.\n";
    return 0;
}
