#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "include/nxpp.hpp"

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

void test_empty_graph_reports_empty_collections() {
    nxpp::Graph<> graph;

    expect(graph.nodes().empty(), "empty graph should report no nodes");
    expect(graph.edges().empty(), "empty graph should report no edges");
    const auto components = graph.connected_components();
    expect(components.begin() == components.end(),
           "empty graph should report no connected-component mapping");
    expect(graph.connected_component_groups().empty(),
           "empty graph should report no connected-component groups");
}

void test_singleton_graph_has_no_neighbors_or_traversal_edges() {
    nxpp::DiGraph graph;
    graph.add_node("solo");

    expect(graph.nodes().size() == 1, "singleton graph should contain one node");
    expect(graph.neighbors("solo").empty(), "singleton graph should have no neighbors");
    expect(graph.successors("solo").empty(), "singleton graph should have no successors");
    expect(graph.predecessors("solo").empty(), "singleton graph should have no predecessors");
    expect(graph.bfs_edges("solo").empty(), "singleton graph should have no BFS tree edges");
    expect(graph.dfs_edges("solo").empty(), "singleton graph should have no DFS tree edges");
}

void test_missing_node_operations_throw() {
    nxpp::DiGraph graph;
    graph.add_node("Rome");

    expect_throws(
        [&] { (void)graph.neighbors("Milan"); },
        "neighbors() on a missing node should throw");
    expect_throws(
        [&] { (void)graph.bfs_edges("Milan"); },
        "bfs_edges() on a missing start node should throw");
    expect_throws(
        [&] { (void)graph.shortest_path("Rome", "Milan"); },
        "shortest_path() with a missing target should throw");
}

void test_disconnected_shortest_paths_preserve_unreachable_state() {
    nxpp::WeightedGraphStr graph;
    graph.add_edge("A", "B", 2.0);
    graph.add_node("C");

    const auto result = graph.dijkstra_shortest_paths("A");

    expect(result.distance.at("A") == 0.0, "source distance should be zero");
    expect(result.distance.at("B") == 2.0, "reachable node should have finite distance");
    expect(result.distance.at("C") == std::numeric_limits<double>::max(),
           "unreachable node should keep max distance");
    expect(result.paths.find("C") == result.paths.end(),
           "unreachable node should not get a materialized path");

    expect_throws(
        [&] { (void)graph.shortest_path("A", "C", "weight"); },
        "weighted shortest_path() should throw for unreachable targets");
}

void test_disconnected_component_groups_split_graph_correctly() {
    nxpp::Graph<> graph;
    graph.add_edge("A", "B");
    graph.add_edge("C", "D");

    const auto components = graph.connected_component_groups();
    const auto component_map = graph.connected_components();

    expect(components.size() == 2, "two disconnected pairs should yield two components");
    expect(std::distance(component_map.begin(), component_map.end()) == 4,
           "component map should include every node");
    expect(component_map.at("A") == component_map.at("B"),
           "connected nodes should share a component id");
    expect(component_map.at("C") == component_map.at("D"),
           "connected nodes should share a component id");
    expect(component_map.at("A") != component_map.at("C"),
           "disconnected pairs should land in different components");
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

    passed += run_test("empty graph reports empty collections", test_empty_graph_reports_empty_collections) ? 1 : 0;
    passed += run_test("singleton graph has no neighbors or traversal edges", test_singleton_graph_has_no_neighbors_or_traversal_edges) ? 1 : 0;
    passed += run_test("missing node operations throw", test_missing_node_operations_throw) ? 1 : 0;
    passed += run_test("disconnected shortest paths preserve unreachable state", test_disconnected_shortest_paths_preserve_unreachable_state) ? 1 : 0;
    passed += run_test("disconnected component groups split graph correctly", test_disconnected_component_groups_split_graph_correctly) ? 1 : 0;

    return passed == total ? 0 : 1;
}
