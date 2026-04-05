#include <cmath>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef NXPP_HEADER_UNDER_TEST
#define NXPP_HEADER_UNDER_TEST "include/nxpp.hpp"
#endif

#include NXPP_HEADER_UNDER_TEST

constexpr const char* green = "\033[32m";
constexpr const char* red = "\033[31m";
constexpr const char* reset = "\033[0m";

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

template <typename Fn>
void expect_throws(Fn&& fn, const std::string& message) {
    try {
        fn();
    } catch (const std::runtime_error&) {
        return;
    }

    throw std::runtime_error(message);
}

void test_remove_middle_node_updates_nodes_and_edges() {
    nxpp::DiGraph graph;
    graph.add_edge("A", "B", 1.0);
    graph.add_edge("B", "C", 1.0);
    graph.add_edge("C", "D", 1.0);

    graph.remove_node("B");

    const auto nodes = graph.nodes();
    const auto edges = graph.edges();

    expect(!graph.has_node("B"), "removed node should disappear from the graph");
    expect(nodes.size() == 3, "graph should keep the remaining three nodes");
    expect(edges.size() == 1, "only non-incident edges should remain after remove_node()");
    expect(std::get<0>(edges.front()) == "C" && std::get<1>(edges.front()) == "D",
           "remaining edge set should keep only the non-incident edge");
}

void test_remove_node_cleans_node_and_incident_edge_attributes() {
    nxpp::DiGraph graph;
    graph.add_edge("A", "B", 1.0, {{"label", "ab"}});
    graph.add_edge("B", "C", 2.0, {{"label", "bc"}});
    graph.node("B")["kind"] = "middle";

    graph.remove_node("B");

    expect(!graph.has_node_attr("B", "kind"), "removed node should lose its stored attributes");
    expect(!graph.has_edge_attr("A", "B", "label"),
           "incident edge attributes should be cleaned when a node is removed");
    expect(!graph.has_edge_attr("B", "C", "label"),
           "incident edge attributes should be cleaned when a node is removed");
}

void test_algorithms_still_work_after_descriptor_remap() {
    nxpp::WeightedDiGraph graph;
    graph.add_edge("A", "B", 9.0);
    graph.add_edge("A", "C", 1.0);
    graph.add_edge("C", "D", 2.0);
    graph.add_edge("A", "D", 10.0);
    graph.add_edge("D", "E", 1.0);

    graph.remove_node("B");

    const auto bfs = graph.bfs_edges("A");
    const auto dijkstra = graph.dijkstra_shortest_paths("A");

    expect(bfs.size() == 3, "BFS should still traverse the remapped graph correctly");
    expect(std::abs(dijkstra.distance.at("D") - 3.0) < 1e-9,
           "Dijkstra should still compute correct distances after remove_node()");
    expect(dijkstra.path_to("D") == std::vector<std::string>({"A", "C", "D"}),
           "on-demand paths should still be correct after descriptor remapping");
}

void test_remove_node_updates_component_views() {
    nxpp::Graph<> graph;
    graph.add_edge("A", "B");
    graph.add_edge("C", "D");

    graph.remove_node("B");

    const auto groups = graph.connected_component_groups();
    const auto comp = graph.connected_components();

    expect(groups.size() == 2, "removing B should leave two connected components: A and C-D");
    expect(comp.at("C") == comp.at("D"), "connected nodes should stay in the same component after remap");
    expect(comp.at("A") != comp.at("C"), "disconnected nodes should remain separated after remap");
}

void test_remove_node_in_multigraph_cleans_incident_edge_ids_only() {
    nxpp::MultiDiGraph graph;

    const auto ab1 = graph.add_edge_with_id("A", "B", 1.0);
    const auto ab2 = graph.add_edge_with_id("A", "B", 2.0);
    const auto bc = graph.add_edge_with_id("B", "C", 3.0);
    const auto cd = graph.add_edge_with_id("C", "D", 4.0);

    graph.set_edge_attr(ab1, "label", "ab1");
    graph.set_edge_attr(ab2, "label", "ab2");
    graph.set_edge_attr(bc, "label", "bc");
    graph.set_edge_attr(cd, "label", "cd");

    graph.remove_node("B");

    expect(!graph.has_edge_id(ab1) && !graph.has_edge_id(ab2) && !graph.has_edge_id(bc),
           "incident multigraph edge ids should be removed");
    expect(!graph.has_edge_attr(ab1, "label") && !graph.has_edge_attr(ab2, "label") && !graph.has_edge_attr(bc, "label"),
           "incident multigraph edge attributes should be cleaned");
    expect(graph.has_edge_id(cd), "non-incident multigraph edge ids should survive");
    expect(graph.get_edge_endpoints(cd) == std::pair<std::string, std::string>({"C", "D"}),
           "remaining multigraph edge ids should still resolve to the right endpoints");
}

bool run_test(const std::string& name, const std::function<void()>& fn) {
    try {
        fn();
        std::cout << "[TEST] " << name << " | " << green << "PASS" << reset << "\n";
        return true;
    } catch (const std::exception& ex) {
        std::cout << "[TEST] " << name << " | " << red << "FAIL" << reset
                  << " (" << ex.what() << ")\n";
        return false;
    }
}

int main() {
    int passed = 0;
    constexpr int total = 5;

    passed += run_test("remove middle node updates nodes and edges", test_remove_middle_node_updates_nodes_and_edges) ? 1 : 0;
    passed += run_test("remove_node cleans node and incident edge attributes", test_remove_node_cleans_node_and_incident_edge_attributes) ? 1 : 0;
    passed += run_test("algorithms still work after descriptor remap", test_algorithms_still_work_after_descriptor_remap) ? 1 : 0;
    passed += run_test("remove_node updates component views", test_remove_node_updates_component_views) ? 1 : 0;
    passed += run_test("remove_node in multigraph cleans incident edge ids only", test_remove_node_in_multigraph_cleans_incident_edge_ids_only) ? 1 : 0;

    return passed == total ? 0 : 1;
}
