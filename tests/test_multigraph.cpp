#include <algorithm>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#ifndef NXPP_HEADER_UNDER_TEST
#define NXPP_HEADER_UNDER_TEST "include/nxpp/multigraph.hpp"
#endif

#include NXPP_HEADER_UNDER_TEST

#include "test_helpers.hpp"

using namespace nxpp::test;

void test_parallel_edges_get_distinct_ids() {
    nxpp::MultiDiGraph graph;

    const auto fast = graph.add_edge_with_id("Milan", "Rome", 5.0);
    const auto night = graph.add_edge_with_id("Milan", "Rome", 8.0);
    const auto cargo = graph.add_edge_with_id("Milan", "Rome", 12.0);

    expect(fast != night && fast != cargo && night != cargo,
           "parallel edges should all receive distinct ids");

    const auto ids = graph.edge_ids("Milan", "Rome");
    expect(ids.size() == 3, "edge_ids(u, v) should return every parallel edge");
}

void test_undirected_edge_ids_are_not_duplicated() {
    nxpp::GraphInt simple_graph;
    const auto simple_id = simple_graph.add_edge_with_id(1, 2, 1);

    const auto simple_ids = simple_graph.edge_ids(1, 2);
    expect(simple_ids.size() == 1, "undirected edge_ids(u, v) should return one id for one edge");
    expect(simple_ids.front() == simple_id, "undirected edge_ids(u, v) should return the stored edge id");

    const auto reversed_simple_ids = simple_graph.edge_ids(2, 1);
    expect(reversed_simple_ids.size() == 1, "undirected edge_ids(v, u) should return one id for one edge");
    expect(reversed_simple_ids.front() == simple_id, "undirected edge_ids(v, u) should find the same edge id");

    nxpp::MultiGraphInt multigraph;
    const auto first = multigraph.add_edge_with_id(1, 2, 1);
    const auto second = multigraph.add_edge_with_id(2, 1, 2);

    const auto ids = multigraph.edge_ids(1, 2);
    expect(ids.size() == 2, "undirected multigraph edge_ids(u, v) should not duplicate parallel ids");
    expect(std::find(ids.begin(), ids.end(), first) != ids.end(),
           "undirected multigraph edge_ids(u, v) should include the first edge id");
    expect(std::find(ids.begin(), ids.end(), second) != ids.end(),
           "undirected multigraph edge_ids(u, v) should include the reversed insertion edge id");
}

void test_parallel_edges_keep_distinct_attributes() {
    nxpp::MultiDiGraph graph;

    const auto fast = graph.add_edge_with_id("Milan", "Rome", 5.0);
    const auto night = graph.add_edge_with_id("Milan", "Rome", 8.0);

    graph.set_edge_attr(fast, "service", "fast");
    graph.set_edge_attr(night, "service", "night");

    expect(graph.get_edge_attr<std::string>(fast, "service") == "fast",
           "first parallel edge should keep its own attributes");
    expect(graph.get_edge_attr<std::string>(night, "service") == "night",
           "second parallel edge should keep its own attributes");
}

void test_remove_edge_by_id_is_precise() {
    nxpp::MultiDiGraph graph;

    const auto fast = graph.add_edge_with_id("Milan", "Rome", 5.0);
    const auto night = graph.add_edge_with_id("Milan", "Rome", 8.0);

    graph.set_edge_attr(fast, "service", "fast");
    graph.set_edge_attr(night, "service", "night");
    graph.remove_edge(fast);

    expect(!graph.has_edge_id(fast), "removed edge_id should no longer exist");
    expect(graph.has_edge_id(night), "other parallel edges should stay alive");
    expect(graph.has_edge("Milan", "Rome"),
           "has_edge(u, v) should remain true while one parallel edge survives");
    expect(graph.edge_ids("Milan", "Rome").size() == 1,
           "exactly one parallel edge should remain after remove_edge(edge_id)");
    expect(graph.get_edge_attr<std::string>(night, "service") == "night",
           "remaining parallel edge should keep its attributes");
    expect(!graph.has_edge_attr(fast, "service"),
           "removed edge_id should lose tracked attributes");
}

void test_remove_missing_edge_id_throws_without_mutation() {
    nxpp::MultiDiGraph graph;
    const auto edge_id = graph.add_edge_with_id("Milan", "Rome", 5.0);

    expect_runtime_error_message(
        [&] { graph.remove_edge(edge_id + 100); },
        "Edge lookup failed: edge not found.",
        "remove_edge(edge_id) should reject missing edge IDs");
    expect(graph.has_edge_id(edge_id), "failed remove_edge(edge_id) should not remove existing edges");
    expect(graph.has_edge("Milan", "Rome"), "failed remove_edge(edge_id) should not remove endpoint edges");
}

void test_remove_edge_by_endpoints_removes_all_parallel_edges() {
    nxpp::MultiDiGraph graph;

    const auto fast = graph.add_edge_with_id("Milan", "Rome", 5.0);
    const auto night = graph.add_edge_with_id("Milan", "Rome", 8.0);
    const auto cargo = graph.add_edge_with_id("Milan", "Rome", 12.0);

    graph.set_edge_attr(fast, "service", "fast");
    graph.set_edge_attr(night, "service", "night");
    graph.set_edge_attr(cargo, "service", "cargo");
    graph.remove_edge("Milan", "Rome");

    expect(!graph.has_edge("Milan", "Rome"),
           "remove_edge(u, v) should erase every parallel edge");
    expect(graph.edge_ids("Milan", "Rome").empty(),
           "edge_ids(u, v) should be empty after removing all parallel edges");
    expect(!graph.has_edge_id(fast) && !graph.has_edge_id(night) && !graph.has_edge_id(cargo),
           "all parallel edge ids should be removed");
    expect(!graph.has_edge_attr(fast, "service") &&
               !graph.has_edge_attr(night, "service") &&
               !graph.has_edge_attr(cargo, "service"),
           "all parallel edge attributes should be cleaned up");
}

void test_undirected_remove_edge_by_endpoints_removes_reversed_parallel_edges() {
    nxpp::MultiGraph graph;

    const auto fast = graph.add_edge_with_id("Milan", "Rome", 5.0);
    const auto night = graph.add_edge_with_id("Rome", "Milan", 8.0);
    const auto cargo = graph.add_edge_with_id("Milan", "Rome", 12.0);

    graph.set_edge_attr(fast, "service", "fast");
    graph.set_edge_attr(night, "service", "night");
    graph.set_edge_attr(cargo, "service", "cargo");
    graph.remove_edge("Milan", "Rome");

    expect(!graph.has_edge("Milan", "Rome"), "undirected remove_edge(u, v) should remove forward lookup");
    expect(!graph.has_edge("Rome", "Milan"), "undirected remove_edge(u, v) should remove reverse lookup");
    expect(graph.edge_ids("Milan", "Rome").empty(),
           "undirected edge_ids(u, v) should be empty after removing all parallel edges");
    expect(!graph.has_edge_id(fast) && !graph.has_edge_id(night) && !graph.has_edge_id(cargo),
           "undirected endpoint removal should remove every parallel edge id");
    expect(!graph.has_edge_attr(fast, "service") &&
               !graph.has_edge_attr(night, "service") &&
               !graph.has_edge_attr(cargo, "service"),
           "undirected endpoint removal should clean every parallel edge attribute");
}

void test_edge_endpoints_stay_correct_after_partial_removal() {
    nxpp::MultiDiGraph graph;

    const auto first = graph.add_edge_with_id("A", "B", 1.0);
    const auto second = graph.add_edge_with_id("A", "B", 2.0);

    graph.remove_edge(first);

    const auto endpoints = graph.get_edge_endpoints(second);
    expect(endpoints.first == "A" && endpoints.second == "B",
           "remaining parallel edge should still report the correct endpoints");
}

void test_num_edges_counts_parallel_edges() {
    nxpp::MultiGraphInt graph;

    const auto first = graph.add_edge_with_id(1, 2, 1);
    graph.add_edge_with_id(1, 2, 2);

    expect(graph.num_edges() == 2, "num_edges should count parallel edges separately");

    graph.remove_edge(first);

    expect(graph.num_edges() == 1, "num_edges should update after removing one parallel edge");
}

void test_remove_node_clears_incident_edge_ids() {
    nxpp::MultiDiGraph graph;

    const auto incoming = graph.add_edge_with_id("A", "B", 1.0);
    const auto outgoing = graph.add_edge_with_id("B", "C", 2.0);
    const auto unrelated = graph.add_edge_with_id("C", "D", 3.0);

    graph.remove_node("B");

    expect(!graph.has_edge_id(incoming), "remove_node should clear incoming edge ids");
    expect(!graph.has_edge_id(outgoing), "remove_node should clear outgoing edge ids");
    expect(graph.has_edge_id(unrelated), "remove_node should preserve unrelated edge ids");

    const auto endpoints = graph.get_edge_endpoints(unrelated);
    expect(endpoints.first == "C" && endpoints.second == "D",
           "unrelated edge id should keep valid endpoints after remove_node");
}

void test_remove_nodes_from_clears_incident_edge_ids() {
    nxpp::MultiDiGraph graph;

    const auto removed_source = graph.add_edge_with_id("A", "B", 1.0);
    const auto removed_target = graph.add_edge_with_id("C", "D", 2.0);
    const auto unrelated = graph.add_edge_with_id("E", "F", 3.0);

    graph.remove_nodes_from(std::vector<std::string>{"A", "D"});

    expect(!graph.has_edge_id(removed_source), "remove_nodes_from should clear edge ids incident to removed sources");
    expect(!graph.has_edge_id(removed_target), "remove_nodes_from should clear edge ids incident to removed targets");
    expect(graph.has_edge_id(unrelated), "remove_nodes_from should preserve unrelated edge ids");
    expect(graph.get_edge_weight(unrelated) == 3.0,
           "unrelated edge id should keep valid weight lookup after remove_nodes_from");
}

void test_edge_id_index_survives_copy_move_and_subgraph() {
    nxpp::MultiDiGraph graph;

    const auto first = graph.add_edge_with_id("A", "B", 1.0);
    const auto second = graph.add_edge_with_id("B", "C", 2.0);
    graph.set_edge_attr(second, "label", "kept");

    nxpp::MultiDiGraph copied(graph);
    expect(copied.has_edge_id(first), "copied graph should preserve first edge id lookup");
    expect(copied.get_edge_endpoints(second).second == "C",
           "copied graph should preserve edge endpoint lookup");

    nxpp::MultiDiGraph assigned;
    assigned = copied;
    expect(assigned.get_edge_weight(first) == 1.0,
           "copy-assigned graph should preserve edge weight lookup");

    nxpp::MultiDiGraph moved(std::move(assigned));
    expect(moved.has_edge_id(second), "moved graph should preserve edge id lookup");
    expect(moved.get_edge_attr<std::string>(second, "label") == "kept",
           "moved graph should preserve edge attributes by edge id");

    auto subgraph = moved.subgraph(std::vector<std::string>{"B", "C"});
    const auto subgraph_ids = subgraph.edge_ids("B", "C");
    expect(subgraph_ids.size() == 1, "subgraph should contain the selected internal edge");
    expect(subgraph.has_edge_id(subgraph_ids.front()),
           "subgraph should index its newly assigned edge id");
    expect(subgraph.get_edge_weight(subgraph_ids.front()) == 2.0,
           "subgraph edge id should support weight lookup");
}

void test_multigraph_attr_bearing_endpoint_adds_throw() {
    nxpp::MultiDiGraph weighted_graph;

    bool threw = false;
    try {
        weighted_graph.add_edge("A", "B", 3.0, {{"capacity", 5L}});
    } catch (const std::runtime_error& ex) {
        threw = std::string(ex.what()).find("add_edge_with_id") != std::string::npos;
    }
    expect(threw, "weighted multigraph add_edge(u, v, w, attrs) should throw and suggest edge_id path");

    threw = false;
    try {
        weighted_graph.add_edge("A", "B", {"label", std::string("fast")});
    } catch (const std::runtime_error& ex) {
        threw = std::string(ex.what()).find("add_edge_with_id") != std::string::npos;
    }
    expect(threw, "weighted multigraph add_edge(u, v, attr) should throw and suggest edge_id path");

    nxpp::MultiGraph unweighted_graph;

    threw = false;
    try {
        unweighted_graph.add_edge("X", "Y", {{"capacity", 7L}});
    } catch (const std::runtime_error& ex) {
        threw = std::string(ex.what()).find("add_edge_with_id") != std::string::npos;
    }
    expect(threw, "unweighted multigraph add_edge(u, v, attrs) should throw and suggest edge_id path");
}

void test_numeric_edge_attrs_support_unsigned_edge_ids() {
    nxpp::MultiDiGraph graph;
    const auto edge_id = graph.add_edge_with_id("A", "B", 1.0);
    graph.set_edge_attr(edge_id, "capacity", static_cast<std::size_t>(21));

    expect(graph.get_edge_numeric_attr(edge_id, "capacity") == 21.0,
           "edge-id numeric lookup should support unsigned attribute values");
}

int main() {
    return run_tests({
        {"parallel edges get distinct ids", test_parallel_edges_get_distinct_ids},
        {"undirected edge_ids are not duplicated", test_undirected_edge_ids_are_not_duplicated},
        {"parallel edges keep distinct attributes", test_parallel_edges_keep_distinct_attributes},
        {"remove_edge(edge_id) is precise", test_remove_edge_by_id_is_precise},
        {"remove missing edge_id throws without mutation", test_remove_missing_edge_id_throws_without_mutation},
        {"remove_edge(u, v) removes all parallel edges", test_remove_edge_by_endpoints_removes_all_parallel_edges},
        {"undirected remove_edge(u, v) removes reversed parallel edges", test_undirected_remove_edge_by_endpoints_removes_reversed_parallel_edges},
        {"edge endpoints stay correct after partial removal", test_edge_endpoints_stay_correct_after_partial_removal},
        {"num_edges counts parallel edges", test_num_edges_counts_parallel_edges},
        {"remove_node clears incident edge ids", test_remove_node_clears_incident_edge_ids},
        {"remove_nodes_from clears incident edge ids", test_remove_nodes_from_clears_incident_edge_ids},
        {"edge id index survives copy move and subgraph", test_edge_id_index_survives_copy_move_and_subgraph},
        {"multigraph attr-bearing endpoint adds throw", test_multigraph_attr_bearing_endpoint_adds_throw},
        {"numeric edge attrs support unsigned edge IDs", test_numeric_edge_attrs_support_unsigned_edge_ids},
    });
}
