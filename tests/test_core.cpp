#include <cmath>
#include <algorithm>
#include <filesystem>
#include <functional>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#ifndef NXPP_HEADER_UNDER_TEST
#define NXPP_HEADER_UNDER_TEST "include/nxpp.hpp"
#endif

#include NXPP_HEADER_UNDER_TEST

#include "test_helpers.hpp"

using namespace nxpp::test;

static_assert(noexcept(std::declval<const nxpp::DiGraph&>().num_vertices()));
static_assert(noexcept(std::declval<const nxpp::DiGraph&>().num_edges()));
static_assert(nxpp::ValidNodeID<int>);
static_assert(nxpp::NumericNodeID<int>);
static_assert(std::is_same_v<nxpp::storage::Vec, nxpp::Graph<>::OutEdgeListSelector>);
static_assert(std::is_same_v<
    decltype(std::declval<const nxpp::WeightedDiGraphInt&>().edges()),
    decltype(std::declval<const nxpp::UnweightedDiGraphInt&>().edges())
>);
static_assert(std::is_same_v<
    decltype(std::declval<const nxpp::WeightedDiGraphInt&>().weighted_edges()),
    std::vector<std::tuple<int, int, int>>
>);

void test_storage_selector_aliases() {
    nxpp::Graph<int, int, true, false, true, nxpp::storage::List, nxpp::storage::List> graph;

    graph.add_edge(1, 2, 3);

    expect(graph.num_vertices() == 2, "storage alias graph should store vertices");
    expect(graph.num_edges() == 1, "storage alias graph should store edges");
}

void test_edges_are_endpoint_pairs_for_all_graphs() {
    nxpp::WeightedDiGraphInt weighted;
    weighted.add_edge(1, 2, 7);

    nxpp::UnweightedDiGraphInt unweighted;
    unweighted.add_edge(1, 2);

    expect(weighted.edges() == unweighted.edges(),
           "edges() should expose endpoint pairs for weighted and unweighted graphs");
    expect(weighted.edge_pairs() == weighted.edges(),
           "edge_pairs() should remain an alias for endpoint-pair edges");

    const auto weighted_edges = weighted.weighted_edges();
    expect(weighted_edges.size() == 1, "weighted_edges() should expose weighted edges");
    expect(std::get<0>(weighted_edges.front()) == 1 &&
               std::get<1>(weighted_edges.front()) == 2 &&
               std::get<2>(weighted_edges.front()) == 7,
           "weighted_edges() should preserve endpoints and weight");
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

void test_topological_sort_orders_dag_and_rejects_cycle() {
    nxpp::DiGraphInt graph;
    graph.add_edge(1, 2, 1.0);
    graph.add_edge(1, 3, 1.0);
    graph.add_edge(2, 4, 1.0);
    graph.add_edge(3, 4, 1.0);

    const auto order = graph.topological_sort();
    const auto before = [&](int left, int right) {
        return std::find(order.begin(), order.end(), left) < std::find(order.begin(), order.end(), right);
    };

    expect(order.size() == 4, "topological_sort should return every DAG node");
    expect(before(1, 2), "topological_sort should place source before successor 2");
    expect(before(1, 3), "topological_sort should place source before successor 3");
    expect(before(2, 4), "topological_sort should place node 2 before sink");
    expect(before(3, 4), "topological_sort should place node 3 before sink");

    graph.add_edge(4, 1, 1.0);

    bool threw = false;
    try {
        (void)graph.topological_sort();
    } catch (const std::exception&) {
        threw = true;
    }
    expect(threw, "topological_sort should reject directed cycles");
}

void test_kruskal_minimum_spanning_tree_selects_light_edges() {
    nxpp::GraphInt graph;
    graph.add_edge(1, 2, 1);
    graph.add_edge(2, 3, 2);
    graph.add_edge(1, 3, 10);
    graph.add_edge(3, 4, 3);
    graph.add_edge(2, 4, 8);

    const auto mst = graph.kruskal_minimum_spanning_tree();
    const auto has_tree_edge = [&](int a, int b) {
        return std::find(mst.begin(), mst.end(), std::pair<int, int>{a, b}) != mst.end()
            || std::find(mst.begin(), mst.end(), std::pair<int, int>{b, a}) != mst.end();
    };

    expect(mst.size() == 3, "Kruskal MST should return n - 1 edges for a connected graph");
    expect(has_tree_edge(1, 2), "Kruskal MST should keep the light 1-2 edge");
    expect(has_tree_edge(2, 3), "Kruskal MST should keep the light 2-3 edge");
    expect(has_tree_edge(3, 4), "Kruskal MST should keep the light 3-4 edge");
    expect(!has_tree_edge(1, 3), "Kruskal MST should skip the heavy cycle edge");
    expect(!has_tree_edge(2, 4), "Kruskal MST should skip the heavier alternative edge");
}

void test_dag_shortest_paths_small_weighted_graph() {
    nxpp::WeightedDiGraphStr graph;
    graph.add_edge("A", "B", 2.0);
    graph.add_edge("A", "C", 5.0);
    graph.add_edge("B", "C", 1.0);
    graph.add_edge("C", "D", 3.0);
    graph.add_node("E");

    const auto result = graph.dag_shortest_paths("A");

    expect(result.distance.at("A") == 0.0, "DAG shortest paths should set source distance to zero");
    expect(result.distance.at("C") == 3.0, "DAG shortest paths should relax through B");
    expect(result.distance.at("D") == 6.0, "DAG shortest paths should reach D through the light path");
    expect(result.distance.at("E") == std::numeric_limits<double>::max(),
           "DAG shortest paths should keep unreachable nodes at numeric infinity");
    expect(result.path_to("D") == std::vector<std::string>({"A", "B", "C", "D"}),
           "DAG shortest paths should reconstruct the light path");
}

void test_batch_node_and_edge_insertion() {
    nxpp::Graph<> graph;
    graph.add_nodes_from({"A", "B", "A"});
    graph.add_edges_from(std::vector<std::pair<std::string, std::string>>{{"A", "B"}, {"B", "C"}});

    expect(graph.num_vertices() == 3, "add_nodes_from and add_edges_from should create unique nodes");
    expect(graph.has_edge("A", "B"), "add_edges_from should add the first unweighted edge");
    expect(graph.has_edge("B", "C"), "add_edges_from should add the second unweighted edge");

    nxpp::WeightedDiGraphStr weighted;
    weighted.add_edges_from(std::vector<std::tuple<std::string, std::string, double>>{
        {"A", "B", 2.5},
        {"B", "C", 4.0},
    });

    expect(weighted.num_vertices() == 3, "weighted add_edges_from should create endpoint nodes");
    expect(std::abs(weighted.get_edge_weight("A", "B") - 2.5) < 1e-9,
           "weighted add_edges_from should store tuple weights");
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

void test_viz_dot_layout_option() {
    nxpp::DiGraph graph;
    graph.add_edge("A", "B");

    nxpp::viz::DotOptions options;
    options.layout = nxpp::viz::DotLayout::Neato;
    const std::string dot = nxpp::viz::to_dot(graph, options);

    expect(dot.find("  layout=neato;\n") != std::string::npos,
           "viz DOT should emit a selected Graphviz layout when requested");
}

void test_viz_dot_user_attrs_option() {
    nxpp::DiGraph graph;
    graph.add_edge("A", "B", 3.0, {{"tooltip", "fast train"}, {"capacity", 8}});
    graph.node("A")["color"] = "light blue";

    const std::string default_dot = nxpp::viz::to_dot(graph);
    expect(default_dot.find("tooltip=") == std::string::npos,
           "viz DOT should hide user edge attributes by default");
    expect(default_dot.find("color=") == std::string::npos,
           "viz DOT should hide user node attributes by default");

    nxpp::viz::DotOptions options;
    options.show_user_attrs = true;
    options.graph_attrs["rankdir"] = "LR";
    options.graph_attrs["bgcolor"] = "light gray";
    const std::string dot = nxpp::viz::to_dot(graph, options);

    expect(dot.find("  bgcolor=\"light gray\";\n") != std::string::npos,
           "viz DOT should quote graph attributes when needed");
    expect(dot.find("  rankdir=LR;\n") != std::string::npos,
           "viz DOT should emit plain graph attributes without quotes");
    expect(dot.find("\"A\" [label=\"A\" color=\"light blue\"]") != std::string::npos,
           "viz DOT should emit quoted user node attributes when requested");
    expect(dot.find("\"A\" -> \"B\" [weight=3 label=3 capacity=8 tooltip=\"fast train\"]") != std::string::npos,
           "viz DOT should emit built-in and user edge attributes when requested");
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

void test_viz_write_dot_open_failure_throws() {
    nxpp::DiGraphInt graph;
    graph.add_edge(1, 2, 4);

    const auto missing_dir = std::filesystem::temp_directory_path() / "nxpp_missing_dot_dir";
    const auto output_path = missing_dir / "graph.dot";
    std::filesystem::remove_all(missing_dir);

    expect_runtime_error_message(
        [&] { nxpp::viz::write_dot(graph, output_path); },
        "DOT export failed: could not open output file.",
        "write_dot should report file open failures");
}

void test_prim_mst_root_self_entry() {
    nxpp::GraphInt graph;
    graph.add_edge(1, 2, 3);
    graph.add_edge(2, 3, 4);
    graph.add_edge(1, 3, 10);

    const auto parents = graph.prim_minimum_spanning_tree(1);

    expect(parents.at(1) == 1, "Prim MST parent map should keep the root self-entry");
    expect(parents.at(2) == 1, "Prim MST should connect node 2 through the root");
    expect(parents.at(3) == 2, "Prim MST should connect node 3 through node 2");
}

void test_lookup_map_operator_missing_key_throws() {
    nxpp::lookup_map<std::string, int> legacy;
    legacy["present"] = 7;
    const auto& const_legacy = legacy;

    expect(const_legacy["present"] == 7, "lookup_map const operator[] should read existing keys");

    bool lookup_threw = false;
    try {
        (void)const_legacy["missing"];
    } catch (const std::out_of_range&) {
        lookup_threw = true;
    }
    expect(lookup_threw, "lookup_map const operator[] should throw for missing keys");

    nxpp::indexed_lookup_map<std::string, int> indexed;
    indexed.push_back("present", 11);
    expect(indexed["present"] == 11, "indexed_lookup_map operator[] should read existing keys");

    bool indexed_threw = false;
    try {
        (void)indexed["missing"];
    } catch (const std::out_of_range&) {
        indexed_threw = true;
    }
    expect(indexed_threw, "indexed_lookup_map operator[] should throw for missing keys");
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

void test_graph_copy_and_move_have_independent_state() {
    nxpp::DiGraph graph;
    graph.add_edge("A", "B", 2.0, {{"label", "original"}});
    graph.node("A")["color"] = "red";

    nxpp::DiGraph copied = graph;
    copied.add_edge("B", "C", 3.0);
    copied.node("A")["color"] = "blue";
    copied.set_edge_attr(copied.edge_ids("A", "B").front(), "label", "copy");

    expect(!graph.has_node("C"), "copy mutation should not add nodes to the source graph");
    expect(graph.get_node_attr<std::string>("A", "color") == "red",
           "copy mutation should not change source node attributes");
    expect(graph.get_edge_attr<std::string>("A", "B", "label") == "original",
           "copy mutation should not change source edge attributes");

    nxpp::DiGraph moved(std::move(copied));
    expect(moved.has_edge("A", "B") && moved.has_edge("B", "C"),
           "moved graph should keep copied edge state");
    expect(moved.get_node_attr<std::string>("A", "color") == "blue",
           "moved graph should keep copied node attributes");
    expect(moved.get_edge_attr<std::string>("A", "B", "label") == "copy",
           "moved graph should keep copied edge attributes");
    expect(copied.num_vertices() == 0 && copied.num_edges() == 0,
           "moved-from graph should be valid and empty");
    copied.add_node("after-move");
    expect(copied.has_node("after-move"), "moved-from graph should remain usable");
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

void test_subgraph_copies_undirected_unweighted_graph() {
    nxpp::Graph<> graph;
    graph.add_edge("A", "B");
    graph.add_edge("B", "C");
    graph.add_edge("C", "D");

    auto subgraph = graph.subgraph(std::vector<std::string>{"A", "B", "C"});

    expect(subgraph.num_vertices() == 3, "undirected unweighted subgraph should copy selected nodes");
    expect(subgraph.edges().size() == 2, "undirected unweighted subgraph should copy only induced edges");
    expect(subgraph.has_edge("A", "B"), "undirected unweighted subgraph should keep A-B");
    expect(subgraph.has_edge("B", "A"), "undirected unweighted subgraph should preserve reverse lookup");
    expect(subgraph.has_edge("B", "C"), "undirected unweighted subgraph should keep B-C");
    expect(!subgraph.has_node("D"), "undirected unweighted subgraph should omit unselected nodes");
    expect(!subgraph.has_edge("C", "D"), "undirected unweighted subgraph should omit outgoing boundary edges");
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

int main() {
    return run_tests({
        {"storage selector aliases", test_storage_selector_aliases},
        {"edges are endpoint pairs for all graphs", test_edges_are_endpoint_pairs_for_all_graphs},
        {"string attributes and normalization", test_string_attributes_and_normalization},
        {"dijkstra result wrapper", test_dijkstra_result_wrapper},
        {"num_edges counts current edges", test_num_edges_counts_current_edges},
        {"Floyd-Warshall matrix and map match", test_floyd_warshall_matrix_and_map_match},
        {"topological_sort orders DAG and rejects cycle", test_topological_sort_orders_dag_and_rejects_cycle},
        {"Kruskal MST selects light edges", test_kruskal_minimum_spanning_tree_selects_light_edges},
        {"DAG shortest paths small weighted graph", test_dag_shortest_paths_small_weighted_graph},
        {"batch node and edge insertion", test_batch_node_and_edge_insertion},
        {"multigraph edge_id path", test_multigraph_edge_id_path},
        {"multigraph remove_edge cleanup", test_multigraph_remove_edge_cleanup},
        {"viz DOT weighted directed export", test_viz_dot_weighted_directed_export},
        {"viz DOT unweighted undirected export", test_viz_dot_unweighted_undirected_export},
        {"viz DOT multigraph edge IDs", test_viz_dot_multigraph_edge_ids},
        {"viz DOT quoted string weights", test_viz_dot_quotes_string_weights},
        {"viz DOT quoted plus-signed numbers", test_viz_dot_quotes_plus_signed_numbers},
        {"viz DOT layout option", test_viz_dot_layout_option},
        {"viz DOT user attrs option", test_viz_dot_user_attrs_option},
        {"viz write_dot file", test_viz_write_dot_file},
        {"viz write_dot open failure throws", test_viz_write_dot_open_failure_throws},
        {"Prim MST root self-entry", test_prim_mst_root_self_entry},
        {"lookup_map operator missing key throws", test_lookup_map_operator_missing_key_throws},
        {"proxy assignment normalizes C-strings", test_proxy_assignment_normalizes_c_strings},
        {"graph copy and move have independent state", test_graph_copy_and_move_have_independent_state},
        {"subgraph copies induced weighted graph", test_subgraph_copies_induced_weighted_graph},
        {"subgraph initializer list and missing node", test_subgraph_initializer_list_and_missing_node},
        {"subgraph copies undirected unweighted graph", test_subgraph_copies_undirected_unweighted_graph},
        {"subgraph preserves multigraph parallel edges", test_subgraph_preserves_multigraph_parallel_edges},
    });
}
