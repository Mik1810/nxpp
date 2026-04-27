#include <iostream>
#include <string>
#include <vector>

#include "include/nxpp.hpp"

using namespace std;

void showcase_string_node_ids_and_attributes() {
    cout << "\nString Node IDs And Attributes\n";

    // This is the first thing nxpp buys us: we can start from human-readable IDs
    // and attach typed attributes without building descriptor plumbing first.
    nxpp::DiGraph graph;
    graph.add_edge("Milan", "Rome", 5.0, {{"capacity", 8}, {"company", "Trenitalia"}});
    graph.add_edge("Rome", "Naples", 2.5);
    graph.add_edge("Milan", "Florence", 2.0);
    graph.add_edge("Florence", "Naples", 4.0);

    graph.node("Rome")["population"] = 2800000;
    graph.node("Milan")["region"] = "Lombardy";

    cout << "Nodes:";
    for (const auto& node : graph.nodes()) {
        cout << " " << node;
    }
    cout << '\n';
    cout << "Edges:\n";
    for (const auto& [u, v, weight] : graph.edges()) {
        cout << "  " << u << " -> " << v << " (weight=" << weight << ")\n";
    }
    cout << "Rome population: " << graph.get_node_attr<int>("Rome", "population") << '\n';
    cout << "Milan -> Rome operator: "
         << graph.get_edge_attr<string>("Milan", "Rome", "company") << '\n';
}

void showcase_materialized_shortest_path_results() {
    cout << "\nMaterialized Shortest Path Results\n";

    // Distances and predecessors are materialized up front, while full paths
    // are reconstructed on demand from the stored predecessor map.
    nxpp::DiGraph graph;
    graph.add_edge("Milan", "Rome", 5.0);
    graph.add_edge("Rome", "Naples", 2.5);
    graph.add_edge("Milan", "Florence", 2.0);
    graph.add_edge("Florence", "Naples", 4.0);
    graph.add_edge("Naples", "Bari", 3.0);

    auto result = graph.dijkstra_shortest_paths("Milan");

    cout << "Distance Milan -> Naples: " << result.distance.at("Naples") << '\n';
    cout << "Path Milan -> Naples:";
    if (result.has_path_to("Naples")) {
        for (const auto& node : result.path_to("Naples")) {
            cout << " " << node;
        }
    }
    cout << '\n';
    cout << "Predecessor of Bari: " << result.predecessor.at("Bari") << '\n';
}

void showcase_traversal_and_generators() {
    cout << "\nTraversal And Generators\n";

    // For small generated graphs, nxpp lets the example stay close to the idea
    // we are demonstrating instead of the graph-construction ceremony.
    auto path = nxpp::path_graph(5);
    auto tree_edges = path.bfs_edges(0);
    auto centrality = path.degree_centrality();

    cout << "BFS tree edges from 0:\n";
    for (const auto& [u, v] : tree_edges) {
        cout << "  " << u << " -> " << v << '\n';
    }

    cout << "Degree centrality on path_graph(5):\n";
    for (int node = 0; node < 5; ++node) {
        cout << "  node " << node << ": " << centrality.at(node) << '\n';
    }
}

void showcase_flow_and_cut_helpers() {
    cout << "\nFlow And Cut Helpers\n";

    // The flow helpers expose a compact result object, so the demo can stay
    // focused on the cut/value pair instead of the residual-network setup.
    nxpp::DiGraphInt flow_graph;
    flow_graph.add_edge(0, 1, 1.0, {{"capacity", 3}});
    flow_graph.add_edge(0, 2, 1.0, {{"capacity", 2}});
    flow_graph.add_edge(1, 3, 1.0, {{"capacity", 2}});
    flow_graph.add_edge(2, 3, 1.0, {{"capacity", 2}});
    flow_graph.add_edge(1, 2, 1.0, {{"capacity", 1}});

    auto flow = flow_graph.maximum_flow(0, 3);
    auto cut = flow_graph.minimum_cut(0, 3);

    cout << "Maximum flow value: " << flow.value << '\n';
    cout << "Minimum cut value: " << cut.value << '\n';
    cout << "Cut edges:\n";
    for (const auto& [u, v] : cut.cut_edges) {
        cout << "  " << u << " -> " << v << '\n';
    }
}

void showcase_precise_multigraph_edges() {
    cout << "\nPrecise Multigraph Edge Identity\n";

    // The recent multigraph work is easiest to see here: edge_id gives us a
    // stable way to talk about one exact parallel edge.
    nxpp::MultiDiGraph graph;

    auto fast_train = graph.add_edge_with_id("Milan", "Rome", 5.0);
    auto night_train = graph.add_edge_with_id("Milan", "Rome", 7.5);

    graph.set_edge_attr(fast_train, "service", "fast");
    graph.set_edge_attr(night_train, "service", "night");
    graph.set_edge_weight(night_train, 8.0);

    auto ids = graph.edge_ids("Milan", "Rome");
    cout << "Parallel edges Milan -> Rome: " << ids.size() << '\n';
    for (auto edge_id : ids) {
        auto [u, v] = graph.get_edge_endpoints(edge_id);
        cout << "  edge_id=" << edge_id << " " << u << " -> " << v
             << " weight=" << graph.get_edge_weight(edge_id)
             << " service=" << graph.get_edge_attr<string>(edge_id, "service")
             << '\n';
    }

    graph.remove_edge(fast_train);
    cout << "After removing one edge_id, remaining Milan -> Rome edges: "
         << graph.edge_ids("Milan", "Rome").size() << '\n';
}

int main() {
    showcase_string_node_ids_and_attributes();
    showcase_materialized_shortest_path_results();
    showcase_traversal_and_generators();
    showcase_flow_and_cut_helpers();
    showcase_precise_multigraph_edges();
    return 0;
}
