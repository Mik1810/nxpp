#include <cmath>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef NXPP_HEADER_UNDER_TEST
#include NXPP_HEADER_UNDER_TEST
#else
#include "include/nxpp/attributes.hpp"
#include "include/nxpp/generators.hpp"
#include "include/nxpp/graph.hpp"
#include "include/nxpp/traversal.hpp"
#include "include/nxpp/shortest_paths.hpp"
#include "include/nxpp/components.hpp"
#include "include/nxpp/centrality.hpp"
#endif

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

template <typename Fn>
void expect_runtime_error_message(Fn&& fn, const std::string& expected_message, const std::string& failure_message) {
    try {
        fn();
    } catch (const std::runtime_error& ex) {
        if (std::string(ex.what()) == expected_message) {
            return;
        }
        throw std::runtime_error(
            failure_message + ": expected \"" + expected_message + "\", got \"" + ex.what() + "\""
        );
    }

    throw std::runtime_error(failure_message + ": no std::runtime_error thrown");
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

    expect_runtime_error_message(
        [&] { (void)graph.neighbors("Milan"); },
        "Node lookup failed: node not found.",
        "neighbors() on a missing node should report the standardized lookup error");
    expect_runtime_error_message(
        [&] { (void)graph.bfs_edges("Milan"); },
        "Traversal failed: start node not found.",
        "bfs_edges() on a missing start node should report the standardized traversal error");
    expect_runtime_error_message(
        [&] { (void)graph.shortest_path("Rome", "Milan"); },
        "Shortest-path lookup failed: source or target node not found.",
        "shortest_path() with a missing target should report the standardized shortest-path lookup error");
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
    expect(!result.has_path_to("C"),
           "unreachable node should not report an available path");
    expect_runtime_error_message(
        [&] { (void)result.path_to("C"); },
        "Path reconstruction failed: target node is unreachable.",
        "unreachable node path reconstruction should report the standardized path reconstruction error");

    expect_runtime_error_message(
        [&] { (void)graph.shortest_path("A", "C", "weight"); },
        "Shortest-path lookup failed: target node is unreachable.",
        "weighted shortest_path() should report the standardized unreachable-target error");
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

struct OrderedOnlyNodeId {
    std::string value;

    bool operator<(const OrderedOnlyNodeId& other) const {
        return value < other.value;
    }

    bool operator==(const OrderedOnlyNodeId& other) const {
        return value == other.value;
    }
};

OrderedOnlyNodeId make_node_id(const char* value) {
    return OrderedOnlyNodeId{value};
}

void test_ordered_only_node_ids_work_without_hash_support() {
    nxpp::Graph<OrderedOnlyNodeId, int, false, false, true> graph;
    const auto a = make_node_id("A");
    const auto b = make_node_id("B");
    const auto c = make_node_id("C");

    graph.add_edge(a, b, 4);
    graph.add_edge(b, c, 5);
    graph.node(a)["population"] = 7;

    expect(graph.has_node(a), "ordered-only source node should be present");
    expect(graph.has_edge(a, b), "ordered-only edge should be present");
    expect(graph.get_node_attr<int>(a, "population") == 7,
           "ordered-only node attribute lookup should succeed");

    const auto shortest = graph.dijkstra_shortest_paths(a);
    expect(shortest.distance.at(c) == 9,
           "ordered-only node IDs should work through shortest-path result maps");
    expect(shortest.path_to(c).size() == 3,
           "ordered-only node IDs should support on-demand path reconstruction");

    const auto components = graph.connected_components();
    expect(components.at(a) == components.at(c),
           "ordered-only node IDs should work through indexed component results");

    const auto centrality = graph.degree_centrality();
    expect(std::abs(centrality.at(b) - 1.0) < 1e-9,
           "ordered-only node IDs should work through indexed centrality results");

    nxpp::Graph<OrderedOnlyNodeId, int, true, false, true> directed_graph;
    directed_graph.add_edge(a, b, 1);
    directed_graph.add_edge(b, a, 1);
    directed_graph.add_edge(b, c, 1);
    directed_graph.add_edge(c, b, 1);

    const auto roots = directed_graph.strong_components();
    expect(roots.at(a) == roots.at(c),
           "ordered-only node IDs should work through indexed SCC root results");

    nxpp::Graph<OrderedOnlyNodeId, int, true, false, true> traversal_graph;
    traversal_graph.add_edge(a, b, 1);
    traversal_graph.add_edge(b, c, 1);

    const auto bfs_successors = traversal_graph.bfs_successors(a);
    expect(bfs_successors.at(a) == std::vector<OrderedOnlyNodeId>({b}),
           "ordered-only node IDs should work through indexed BFS successor results");
    expect(bfs_successors.at(b) == std::vector<OrderedOnlyNodeId>({c}),
           "indexed BFS successor results should keep linear traversal structure");

    const auto dfs_predecessors = traversal_graph.dfs_predecessors(a);
    expect(dfs_predecessors.at(b) == a && dfs_predecessors.at(c) == b,
           "ordered-only node IDs should work through indexed DFS predecessor results");

    const auto dfs_successors = traversal_graph.dfs_successors(a);
    expect(dfs_successors.at(a) == std::vector<OrderedOnlyNodeId>({b}),
           "ordered-only node IDs should work through indexed DFS successor results");
    expect(dfs_successors.at(b) == std::vector<OrderedOnlyNodeId>({c}),
           "indexed DFS successor results should keep linear traversal structure");
}

void test_integer_generators_still_work() {
    const auto complete = nxpp::complete_graph<nxpp::GraphInt>(4);
    expect(complete.num_vertices() == 4, "complete_graph should create the requested number of nodes");
    expect(complete.has_edge(0, 1), "complete_graph should connect distinct node IDs");

    const auto path = nxpp::path_graph<nxpp::GraphInt>(4);
    expect(path.num_vertices() == 4, "path_graph should create the requested number of nodes");
    expect(path.has_edge(0, 1) && path.has_edge(1, 2) && path.has_edge(2, 3),
           "path_graph should link consecutive integer node IDs");

    const auto er = nxpp::erdos_renyi_graph<nxpp::GraphInt>(6, 0.5, 7);
    expect(er.num_vertices() <= 6, "erdos_renyi_graph should stay within the requested integer ID range");
}

void test_betweenness_centrality_basic() {
    // Linear chain: A-B-C-D. B and C lie on all shortest paths between the endpoints,
    // so they should have strictly higher betweenness than the leaf nodes A and D.
    nxpp::Graph<> graph;
    graph.add_edge("A", "B", 1.0);
    graph.add_edge("B", "C", 1.0);
    graph.add_edge("C", "D", 1.0);

    const auto bc = graph.betweenness_centrality();

    expect(bc.contains("A"), "betweenness_centrality should include node A");
    expect(bc.contains("B"), "betweenness_centrality should include node B");
    expect(bc.contains("C"), "betweenness_centrality should include node C");
    expect(bc.contains("D"), "betweenness_centrality should include node D");

    expect(std::abs(bc.at("A")) < 1e-9,
           "leaf node A should have betweenness 0");
    expect(std::abs(bc.at("D")) < 1e-9,
           "leaf node D should have betweenness 0");
    expect(bc.at("B") > 0.0,
           "interior node B should have positive betweenness");
    expect(bc.at("C") > 0.0,
           "interior node C should have positive betweenness");

    // Empty graph: result should be empty.
    nxpp::Graph<> empty;
    const auto bc_empty = empty.betweenness_centrality();
    expect(bc_empty.begin() == bc_empty.end(),
           "empty graph should return empty betweenness map");

    // Single node: betweenness is 0.
    nxpp::Graph<> singleton;
    singleton.add_node("X");
    const auto bc_single = singleton.betweenness_centrality();
    expect(bc_single.contains("X"), "singleton result should contain the node");
    expect(std::abs(bc_single.at("X")) < 1e-9,
           "singleton node should have betweenness 0");
}

void test_implicit_creation_policy() {
    // NodeAttrProxy::operator= creates the node if absent.
    nxpp::Graph<> g1;
    g1.node("X")["k"] = 1;
    expect(g1.has_node("X"), "NodeAttrProxy assignment should create the node if absent");

    // EdgeProxy::operator= creates the edge (and endpoints) if absent.
    nxpp::WeightedGraphStr g2;
    g2["A"]["B"] = 5.0;
    expect(g2.has_edge("A", "B"), "EdgeProxy assignment should create the edge if absent");

    // EdgeAttrProxy::operator= creates the edge if absent.
    nxpp::Graph<> g3;
    g3["P"]["Q"]["label"] = 1;
    expect(g3.has_edge("P", "Q"), "EdgeAttrProxy assignment should create the edge if absent");

    // Read-only accessors never create nodes.
    nxpp::Graph<> g4;
    expect(!g4.has_node("Z"), "has_node on absent node should return false without creating it");
    expect(!g4.has_edge("Z", "W"), "has_edge on absent edge should return false without creating it");
    expect(!g4.has_node("Z"), "has_node should still return false after has_edge check");

    // neighbors() on a missing node throws, not creates.
    expect_throws([&] { (void)g4.neighbors("Z"); },
                  "neighbors() on absent node should throw, not create the node");
}

void test_pagerank_returns_normalized_ranking() {
    nxpp::DiGraph graph;
    graph.add_edge("A", "B", 1.0);
    graph.add_edge("B", "C", 1.0);
    graph.add_edge("C", "B", 1.0);
    graph.add_edge("C", "D", 1.0);

    const auto rank = graph.pagerank();

    expect(rank.contains("A"), "pagerank should include node A");
    expect(rank.contains("B"), "pagerank should include node B");
    expect(rank.contains("C"), "pagerank should include node C");
    expect(rank.contains("D"), "pagerank should include node D");

    const double total = rank.at("A") + rank.at("B") + rank.at("C") + rank.at("D");
    expect(std::abs(total - 1.0) < 1e-9,
           "pagerank scores should stay normalized");
    expect(rank.at("B") > rank.at("A"),
           "pagerank should rank the more referenced node above a source-only node");
    expect(rank.at("C") > rank.at("D"),
           "pagerank should rank an internally linked node above a sink-only leaf");
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
    constexpr int total = 10;

    passed += run_test("empty graph reports empty collections", test_empty_graph_reports_empty_collections) ? 1 : 0;
    passed += run_test("singleton graph has no neighbors or traversal edges", test_singleton_graph_has_no_neighbors_or_traversal_edges) ? 1 : 0;
    passed += run_test("missing node operations throw", test_missing_node_operations_throw) ? 1 : 0;
    passed += run_test("disconnected shortest paths preserve unreachable state", test_disconnected_shortest_paths_preserve_unreachable_state) ? 1 : 0;
    passed += run_test("disconnected component groups split graph correctly", test_disconnected_component_groups_split_graph_correctly) ? 1 : 0;
    passed += run_test("ordered-only node IDs work without hash support", test_ordered_only_node_ids_work_without_hash_support) ? 1 : 0;
    passed += run_test("integer generators still work", test_integer_generators_still_work) ? 1 : 0;
    passed += run_test("implicit creation policy", test_implicit_creation_policy) ? 1 : 0;
    passed += run_test("pagerank returns normalized ranking", test_pagerank_returns_normalized_ranking) ? 1 : 0;
    passed += run_test("betweenness_centrality basic", test_betweenness_centrality_basic) ? 1 : 0;

    return passed == total ? 0 : 1;
}
