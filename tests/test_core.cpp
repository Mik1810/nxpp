#include <cmath>
#include <filesystem>
#include <functional>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
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

std::size_t count_occurrences(const std::string& text, const std::string& needle) {
    std::size_t count = 0;
    std::size_t pos = 0;
    while ((pos = text.find(needle, pos)) != std::string::npos) {
        ++count;
        pos += needle.size();
    }
    return count;
}

void test_string_attributes_and_normalization() {
    nxpp::DiGraph graph;

    graph.add_edge("Milan", "Rome", 5.0, {{"company", "Trenitalia"}, {"capacity", 8}});
    graph.node("Milan")["region"] = "Lombardy";

    expect(graph.get_edge_attr<std::string>("Milan", "Rome", "company") == "Trenitalia",
           "edge string attribute mismatch");
    expect(graph.get_node_attr<std::string>("Milan", "region") == "Lombardy",
           "node string attribute mismatch");
    expect(graph.get_edge_numeric_attr("Milan", "Rome", "capacity") == 8.0,
           "edge numeric attribute mismatch");

    auto maybe_company = graph.try_get_edge_attr<std::string>("Milan", "Rome", "company");
    expect(maybe_company.has_value(), "try_get_edge_attr should return a value");
    expect(*maybe_company == "Trenitalia", "try_get_edge_attr returned the wrong string");
}

void test_dijkstra_result_wrapper() {
    nxpp::DiGraph graph;
    graph.add_edge("Milan", "Rome", 5.0);
    graph.add_edge("Rome", "Naples", 2.5);
    graph.add_edge("Milan", "Florence", 2.0);
    graph.add_edge("Florence", "Naples", 4.0);
    graph.add_edge("Naples", "Bari", 3.0);

    const auto result = graph.dijkstra_shortest_paths("Milan");

    expect(std::abs(result.distance.at("Naples") - 6.0) < 1e-9,
           "wrong Dijkstra distance for Naples");
    expect(result.predecessor.at("Bari") == "Naples",
           "wrong predecessor for Bari");

    const std::vector<std::string> expected_path = {"Milan", "Florence", "Naples"};
    expect(result.path_to("Naples") == expected_path,
           "wrong on-demand path for Naples");
}

void test_num_edges_counts_current_edges() {
    nxpp::DiGraph graph;

    expect(graph.num_edges() == 0, "new graph should report zero edges");

    graph.add_edge("Milan", "Rome", 5.0);
    graph.add_edge("Rome", "Naples", 2.5);

    expect(graph.num_edges() == 2, "num_edges should count inserted directed edges");

    graph.remove_edge("Milan", "Rome");

    expect(graph.num_edges() == 1, "num_edges should reflect removed edges");
}

void test_floyd_warshall_matrix_and_map_match() {
    nxpp::DiGraph graph;
    graph.add_edge("B", "C", 2.0);
    graph.add_edge("A", "B", 1.0);
    graph.add_node("D");

    const auto matrix = graph.floyd_warshall_all_pairs_shortest_paths();
    const auto map = graph.floyd_warshall_all_pairs_shortest_paths_map();

    expect(matrix.size() == 4, "Floyd-Warshall matrix should include every node");
    expect(matrix[0][2] == 3.0, "Floyd-Warshall matrix should use stable sorted node order");
    expect(map.at("A").at("C") == 3.0, "Floyd-Warshall map should match matrix distances");
    expect(map.at("A").at("D") == std::numeric_limits<double>::max(),
           "Floyd-Warshall map should keep unreachable pairs at numeric infinity");
}

void test_multigraph_edge_id_path() {
    nxpp::MultiDiGraph graph;

    const auto fast = graph.add_edge_with_id("Milan", "Rome", 5.0);
    const auto night = graph.add_edge_with_id("Milan", "Rome", 7.5);

    graph.set_edge_attr(fast, "service", "fast");
    graph.set_edge_attr(night, "service", "night");
    graph.set_edge_weight(night, 8.0);

    expect(fast != night, "parallel edges should receive different ids");
    expect(graph.has_edge_id(fast), "first edge_id should exist");
    expect(graph.has_edge_id(night), "second edge_id should exist");
    expect(graph.get_edge_attr<std::string>(fast, "service") == "fast",
           "wrong service for first multigraph edge");
    expect(graph.get_edge_attr<std::string>(night, "service") == "night",
           "wrong service for second multigraph edge");
    expect(std::abs(graph.get_edge_weight(night) - 8.0) < 1e-9,
           "wrong weight for second multigraph edge");

    const auto endpoints = graph.get_edge_endpoints(fast);
    expect(endpoints.first == "Milan" && endpoints.second == "Rome",
           "wrong endpoints for edge_id lookup");

    const auto ids = graph.edge_ids("Milan", "Rome");
    expect(ids.size() == 2, "edge_ids(u, v) should expose both parallel edges");
}

void test_multigraph_remove_edge_cleanup() {
    nxpp::MultiDiGraph graph;

    const auto e1 = graph.add_edge_with_id("A", "B", 1.0);
    const auto e2 = graph.add_edge_with_id("A", "B", 2.0);
    graph.set_edge_attr(e1, "label", "first");
    graph.set_edge_attr(e2, "label", "second");

    graph.remove_edge("A", "B");

    expect(!graph.has_edge("A", "B"), "all parallel edges should be removed");
    expect(graph.edge_ids("A", "B").empty(), "edge_ids(u, v) should be empty after removal");
    expect(!graph.has_edge_id(e1), "first edge_id should be removed");
    expect(!graph.has_edge_id(e2), "second edge_id should be removed");
    expect(!graph.has_edge_attr(e1, "label"), "first edge attributes should be cleaned up");
    expect(!graph.has_edge_attr(e2, "label"), "second edge attributes should be cleaned up");
}

void test_viz_dot_weighted_directed_export() {
    nxpp::DiGraph graph;
    graph.add_edge("A\"1", "B", 3.5);

    const std::string dot = nxpp::viz::to_dot(graph);

    expect(dot.find("digraph G") != std::string::npos, "viz DOT should emit a directed graph");
    expect(dot.find("\"A\\\"1\"") != std::string::npos, "viz DOT should escape quoted node IDs");
    expect(dot.find("\"A\\\"1\" -> \"B\" [weight=3.5 label=3.5]") != std::string::npos,
           "viz DOT should emit visible labels from built-in weights");
}

void test_viz_dot_unweighted_undirected_export() {
    nxpp::UnweightedGraphStr graph;
    graph.add_node("isolated");
    graph.add_edge("A", "B");

    const std::string dot = nxpp::viz::to_dot(graph);

    expect(dot.find("graph G") != std::string::npos, "viz DOT should emit an undirected graph");
    expect(dot.find("\"A\" -- \"B\"") != std::string::npos, "viz DOT should use undirected edge syntax");
    expect(dot.find("weight=") == std::string::npos, "unweighted viz DOT should not emit weights");
    expect(dot.find("\"isolated\" [label=\"isolated\"]") != std::string::npos,
           "viz DOT should preserve isolated nodes");
}

void test_viz_dot_multigraph_edge_ids() {
    nxpp::MultiDiGraph graph;
    const auto first = graph.add_edge_with_id("A", "B", 1.0);
    const auto second = graph.add_edge_with_id("A", "B", 2.0);

    nxpp::viz::DotOptions options;
    options.show_edge_ids = true;
    const std::string dot = nxpp::viz::to_dot(graph, options);

    expect(count_occurrences(dot, "\"A\" -> \"B\"") == 2,
           "viz DOT should preserve parallel edges");
    expect(dot.find("edge_id=" + std::to_string(first)) != std::string::npos,
           "viz DOT should expose the first edge ID when requested");
    expect(dot.find("edge_id=" + std::to_string(second)) != std::string::npos,
           "viz DOT should expose the second edge ID when requested");
}

void test_viz_dot_quotes_string_weights() {
    nxpp::Graph<std::string, std::string, true> graph;
    graph.add_edge("A", "B", "fast train");

    const std::string dot = nxpp::viz::to_dot(graph);

    expect(dot.find("\"A\" -> \"B\" [weight=\"fast train\" label=\"fast train\"]") != std::string::npos,
           "viz DOT should quote string weights that are not plain DOT identifiers");
}

void test_viz_dot_quotes_plus_signed_numbers() {
    nxpp::Graph<std::string, std::string, true> graph;
    graph.add_edge("A", "B", "+3");

    const std::string dot = nxpp::viz::to_dot(graph);

    expect(dot.find("\"A\" -> \"B\" [weight=\"+3\" label=\"+3\"]") != std::string::npos,
           "viz DOT should quote plus-signed numeric-looking weights");
}

void test_viz_write_dot_file() {
    nxpp::DiGraphInt graph;
    graph.add_edge(1, 2, 4);

    const auto path = std::filesystem::temp_directory_path() / "nxpp_test_core_viz.dot";
    nxpp::viz::write_dot(graph, path);

    std::string contents;
    {
        std::ifstream in(path);
        contents.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
    }
    std::filesystem::remove(path);

    expect(contents.find("\"1\" -> \"2\" [weight=4 label=4]") != std::string::npos,
           "write_dot should write the same DOT representation to disk");
}

void test_proxy_assignment_normalizes_c_strings() {
    nxpp::DiGraph graph;

    graph["Milan"]["Rome"] = 5.0;
    graph["Milan"]["Rome"]["company"] = "Trenitalia";
    graph.node("Rome")["nickname"] = "Caput Mundi";

    expect(graph.get_edge_attr<std::string>("Milan", "Rome", "company") == "Trenitalia",
           "edge proxy assignment should normalize C-strings");
    expect(graph.get_node_attr<std::string>("Rome", "nickname") == "Caput Mundi",
           "node proxy assignment should normalize C-strings");
}

void test_subgraph_copies_induced_weighted_graph() {
    nxpp::DiGraph graph;
    graph.add_edge("Milan", "Rome", 5.0, {{"service", "fast"}});
    graph.add_edge("Rome", "Florence", 2.0, {{"service", "regional"}});
    graph.add_edge("Florence", "Naples", 4.0);
    graph.node("Milan")["region"] = "Lombardy";
    graph.node("Rome")["region"] = "Lazio";

    const std::vector<std::string> selected = {"Milan", "Rome", "Florence"};
    auto subgraph = graph.subgraph(selected);

    expect(subgraph.num_vertices() == 3, "subgraph should copy selected nodes");
    expect(subgraph.edges().size() == 2, "subgraph should copy only induced edges");
    expect(subgraph.has_node("Milan"), "subgraph should contain Milan");
    expect(!subgraph.has_node("Naples"), "subgraph should omit unselected nodes");
    expect(subgraph.has_edge("Milan", "Rome"), "subgraph should keep internal edges");
    expect(!subgraph.has_edge("Florence", "Naples"), "subgraph should omit edges leaving the selected set");
    expect(std::abs(subgraph.get_edge_weight("Milan", "Rome") - 5.0) < 1e-9,
           "subgraph should preserve built-in edge weights");
    expect(subgraph.get_node_attr<std::string>("Rome", "region") == "Lazio",
           "subgraph should copy node attributes");
    expect(subgraph.get_edge_attr<std::string>("Milan", "Rome", "service") == "fast",
           "subgraph should copy edge attributes");

    subgraph.add_node("Bari");
    subgraph.set_edge_attr(subgraph.edge_ids("Milan", "Rome").front(), "service", "local");

    expect(!graph.has_node("Bari"), "subgraph should be independent from the source graph");
    expect(graph.get_edge_attr<std::string>("Milan", "Rome", "service") == "fast",
           "mutating subgraph edge attributes should not mutate the source graph");

    nxpp::DiGraph moved_subgraph(std::move(subgraph));
    expect(moved_subgraph.has_node("Milan") && moved_subgraph.has_edge("Milan", "Rome"),
           "moved subgraph should keep node and edge property maps bound to itself");
}

void test_subgraph_initializer_list_and_missing_node() {
    nxpp::DiGraph graph;
    graph.add_edge("A", "B", 1.0);
    graph.add_edge("B", "C", 2.0);

    auto subgraph = graph.subgraph({"A", "B"});
    expect(subgraph.num_vertices() == 2, "initializer-list subgraph should copy selected nodes");
    expect(subgraph.has_edge("A", "B"), "initializer-list subgraph should keep internal edges");
    expect(!subgraph.has_node("C"), "initializer-list subgraph should omit unselected nodes");

    bool threw = false;
    try {
        (void)graph.subgraph({"A", "missing"});
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    expect(threw, "subgraph should reject missing input nodes");
}

void test_subgraph_preserves_multigraph_parallel_edges() {
    nxpp::MultiDiGraph graph;
    const auto first = graph.add_edge_with_id("A", "B", 1.0);
    const auto second = graph.add_edge_with_id("A", "B", 2.0);
    graph.add_edge("B", "C", 3.0);
    graph.set_edge_attr(first, "label", "first");
    graph.set_edge_attr(second, "label", "second");

    auto subgraph = graph.subgraph(std::vector<std::string>{"A", "B"});
    const auto copied_ids = subgraph.edge_ids("A", "B");

    expect(copied_ids.size() == 2, "subgraph should preserve parallel edges");
    expect(subgraph.edges().size() == 2, "subgraph should omit edges leaving the selected set");
    expect(subgraph.get_edge_attr<std::string>(copied_ids[0], "label") == "first",
           "subgraph should copy first parallel edge attributes");
    expect(subgraph.get_edge_attr<std::string>(copied_ids[1], "label") == "second",
           "subgraph should copy second parallel edge attributes");
    expect(std::abs(subgraph.get_edge_weight(copied_ids[0]) - 1.0) < 1e-9,
           "subgraph should preserve first parallel edge weight");
    expect(std::abs(subgraph.get_edge_weight(copied_ids[1]) - 2.0) < 1e-9,
           "subgraph should preserve second parallel edge weight");

    nxpp::MultiDiGraph moved_subgraph(std::move(subgraph));
    expect(moved_subgraph.edge_ids("A", "B").size() == 2,
           "moved multigraph subgraph should keep edge ID maps bound to itself");
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
    constexpr int total = 16;

    passed += run_test("string attributes and normalization", test_string_attributes_and_normalization) ? 1 : 0;
    passed += run_test("dijkstra result wrapper", test_dijkstra_result_wrapper) ? 1 : 0;
    passed += run_test("num_edges counts current edges", test_num_edges_counts_current_edges) ? 1 : 0;
    passed += run_test("Floyd-Warshall matrix and map match", test_floyd_warshall_matrix_and_map_match) ? 1 : 0;
    passed += run_test("multigraph edge_id path", test_multigraph_edge_id_path) ? 1 : 0;
    passed += run_test("multigraph remove_edge cleanup", test_multigraph_remove_edge_cleanup) ? 1 : 0;
    passed += run_test("viz DOT weighted directed export", test_viz_dot_weighted_directed_export) ? 1 : 0;
    passed += run_test("viz DOT unweighted undirected export", test_viz_dot_unweighted_undirected_export) ? 1 : 0;
    passed += run_test("viz DOT multigraph edge IDs", test_viz_dot_multigraph_edge_ids) ? 1 : 0;
    passed += run_test("viz DOT quoted string weights", test_viz_dot_quotes_string_weights) ? 1 : 0;
    passed += run_test("viz DOT quoted plus-signed numbers", test_viz_dot_quotes_plus_signed_numbers) ? 1 : 0;
    passed += run_test("viz write_dot file", test_viz_write_dot_file) ? 1 : 0;
    passed += run_test("proxy assignment normalizes C-strings", test_proxy_assignment_normalizes_c_strings) ? 1 : 0;
    passed += run_test("subgraph copies induced weighted graph", test_subgraph_copies_induced_weighted_graph) ? 1 : 0;
    passed += run_test("subgraph initializer list and missing node", test_subgraph_initializer_list_and_missing_node) ? 1 : 0;
    passed += run_test("subgraph preserves multigraph parallel edges", test_subgraph_preserves_multigraph_parallel_edges) ? 1 : 0;

    return passed == total ? 0 : 1;
}
