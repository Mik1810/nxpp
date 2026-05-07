#include <functional>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef NXPP_HEADER_UNDER_TEST
#define NXPP_HEADER_UNDER_TEST "include/nxpp/flow.hpp"
#endif

#include NXPP_HEADER_UNDER_TEST

#include "test_helpers.hpp"

using namespace nxpp::test;

nxpp::UnweightedDiGraphInt make_max_flow_graph() {
    nxpp::UnweightedDiGraphInt graph;

    graph.add_edge(0, 1, {"capacity", 1});
    graph.add_edge(0, 3, {"capacity", 2});
    graph.add_edge(1, 2, {"capacity", 3});
    graph.add_edge(1, 4, {"capacity", 4});
    graph.add_edge(2, 5, {"capacity", 2});
    graph.add_edge(3, 2, {"capacity", 2});
    graph.add_edge(4, 5, {"capacity", 2});

    return graph;
}

nxpp::WeightedDiGraphInt make_min_cost_flow_graph() {
    nxpp::WeightedDiGraphInt graph;

    graph.add_edge(0, 1, 2, {"capacity", 1});
    graph.add_edge(0, 3, 3, {"capacity", 2});
    graph.add_edge(1, 2, 5, {"capacity", 3});
    graph.add_edge(1, 4, 1, {"capacity", 4});
    graph.add_edge(2, 5, 3, {"capacity", 2});
    graph.add_edge(3, 1, 1, {"capacity", 1});
    graph.add_edge(3, 2, 6, {"capacity", 2});
    graph.add_edge(4, 5, 1, {"capacity", 2});

    return graph;
}

nxpp::UnweightedMultiDiGraphInt make_multigraph_flow_graph() {
    nxpp::UnweightedMultiDiGraphInt graph;

    const auto e01a = graph.add_edge_with_id(0, 1);
    const auto e01b = graph.add_edge_with_id(0, 1);
    const auto e12a = graph.add_edge_with_id(1, 2);
    const auto e12b = graph.add_edge_with_id(1, 2);

    graph.set_edge_attr(e01a, "capacity", 1L);
    graph.set_edge_attr(e01b, "capacity", 2L);
    graph.set_edge_attr(e12a, "capacity", 1L);
    graph.set_edge_attr(e12b, "capacity", 2L);

    return graph;
}

nxpp::WeightedMultiDiGraphInt make_multigraph_min_cost_flow_graph() {
    nxpp::WeightedMultiDiGraphInt graph;

    const auto e01a = graph.add_edge_with_id(0, 1, 1);
    const auto e01b = graph.add_edge_with_id(0, 1, 5);
    const auto e12a = graph.add_edge_with_id(1, 2, 1);
    const auto e12b = graph.add_edge_with_id(1, 2, 5);

    graph.set_edge_attr(e01a, "capacity", 1L);
    graph.set_edge_attr(e01b, "capacity", 1L);
    graph.set_edge_attr(e12a, "capacity", 1L);
    graph.set_edge_attr(e12b, "capacity", 1L);

    return graph;
}

void test_maximum_flow_matches_snippet_case() {
    auto graph = make_max_flow_graph();

    const auto result = graph.maximum_flow(0, 5);

    expect(result.value == 3, "maximum_flow should match the reference snippet value");
    expect(result.flow.at({1, 4}) == 1, "flow across edge (1, 4) should match the snippet");
    expect(result.flow.at({4, 5}) == 1, "flow across edge (4, 5) should be one unit");
}

void test_minimum_cut_matches_flow_value_and_partition() {
    auto graph = make_max_flow_graph();

    const auto result = graph.minimum_cut(0, 5);

    expect(result.value == 3, "minimum_cut value should match the max-flow value");
    expect(result.reachable.size() == 1 && result.reachable.front() == 0,
           "only the source should stay reachable in the residual graph");
    expect(result.cut_edges.size() == 2, "the cut should expose two outgoing cut edges");
}

void test_push_relabel_and_cycle_canceling_match_reference_cost() {
    auto graph = make_min_cost_flow_graph();

    const long flow = graph.push_relabel_maximum_flow(0, 5);
    const long cost = graph.cycle_canceling();

    expect(flow == 3, "push_relabel_maximum_flow should match the reference max-flow value");
    expect(cost == 22, "cycle_canceling should match the reference min-cost value");
}

void test_cycle_canceling_requires_cached_flow_state() {
    auto graph = make_min_cost_flow_graph();

    expect_runtime_error_message(
        [&] { (void)graph.cycle_canceling(); },
        "Min-cost-flow state unavailable: run push_relabel_maximum_flow(...) first.",
        "cycle_canceling should require a prior push_relabel_maximum_flow call");
}

void test_cycle_canceling_invalidated_by_graph_mutation() {
    auto graph = make_min_cost_flow_graph();

    (void)graph.push_relabel_maximum_flow(0, 5);
    const auto edge_id = graph.edge_ids(0, 1).front();
    graph.set_edge_attr(edge_id, "capacity", 2L);

    expect_runtime_error_message(
        [&] { (void)graph.cycle_canceling(); },
        "Min-cost-flow state invalidated by graph mutation: rerun push_relabel_maximum_flow(...) before cycle_canceling().",
        "cycle_canceling should reject stale staged state after graph mutation");
}

void test_staged_min_cost_flow_states_are_isolated_between_graph_instances() {
    auto first = make_min_cost_flow_graph();
    auto second = make_min_cost_flow_graph();

    (void)first.push_relabel_maximum_flow(0, 5);
    (void)second.push_relabel_maximum_flow(0, 5);

    expect(first.cycle_canceling() == 22, "first graph should keep its staged flow state");
    expect(second.cycle_canceling() == 22, "second graph should keep a separate staged flow state");
}

void test_copy_after_staged_flow_has_no_cached_flow_state() {
    auto graph = make_min_cost_flow_graph();

    (void)graph.push_relabel_maximum_flow(0, 5);
    auto copied = graph;

    expect_runtime_error_message(
        [&] { (void)copied.cycle_canceling(); },
        "Min-cost-flow state unavailable: run push_relabel_maximum_flow(...) first.",
        "copied graph should not inherit staged min-cost-flow state");
    expect(graph.cycle_canceling() == 22, "copying should not clear the source graph's staged flow state");
}

void test_move_after_staged_flow_leaves_source_empty_and_uncached() {
    auto graph = make_min_cost_flow_graph();

    (void)graph.push_relabel_maximum_flow(0, 5);
    nxpp::WeightedDiGraphInt moved(std::move(graph));

    expect(graph.num_vertices() == 0 && graph.num_edges() == 0,
           "moved-from graph should be valid and empty after staged flow move");
    expect_runtime_error_message(
        [&] { (void)graph.cycle_canceling(); },
        "Min-cost-flow state unavailable: run push_relabel_maximum_flow(...) first.",
        "moved-from graph should not retain staged min-cost-flow state");
    expect_runtime_error_message(
        [&] { (void)moved.cycle_canceling(); },
        "Min-cost-flow state unavailable: run push_relabel_maximum_flow(...) first.",
        "moved graph should not inherit stale staged min-cost-flow state");

    expect(moved.push_relabel_maximum_flow(0, 5) == 3,
           "moved graph should be able to stage flow again");
    expect(moved.cycle_canceling() == 22, "moved graph should compute min-cost flow after restaging");
}

void test_successive_shortest_path_matches_reference_flow_and_cost() {
    auto graph = make_min_cost_flow_graph();

    const auto result = graph.successive_shortest_path_nonnegative_weights(0, 5);

    expect(result.flow == 3, "successive_shortest_path_nonnegative_weights should find flow 3");
    expect(result.cost == 22, "successive_shortest_path_nonnegative_weights should find cost 22");
    expect(result.edge_flows.at({4, 5}) == 2, "edge (4, 5) should carry two units in the min-cost solution");
}

void test_min_cost_aliases_match_specialized_wrappers() {
    auto graph_a = make_min_cost_flow_graph();
    auto graph_b = make_min_cost_flow_graph();

    const auto cycle_result = graph_a.max_flow_min_cost(0, 5);
    const auto ssp_result = graph_b.max_flow_min_cost_successive_shortest_path(0, 5);

    expect(cycle_result.flow == 3 && cycle_result.cost == 22,
           "max_flow_min_cost alias should match the cycle-canceling wrapper");
    expect(ssp_result.flow == 3 && ssp_result.cost == 22,
           "max_flow_min_cost_successive_shortest_path should match the SSP wrapper");
}

void test_fractional_flow_capacity_is_rejected() {
    nxpp::UnweightedDiGraphInt graph;
    graph.add_edge(0, 1, {"capacity", 1.5});

    expect_runtime_error_message(
        [&] { (void)graph.maximum_flow(0, 1); },
        "Flow capacity setup failed: capacity must be a non-negative integral value representable as long.",
        "maximum_flow should reject fractional capacities instead of truncating them");
}

void test_out_of_range_flow_capacity_is_rejected() {
    nxpp::UnweightedDiGraphInt graph;
    graph.add_edge(0, 1, {"capacity", std::numeric_limits<unsigned long long>::max()});

    expect_runtime_error_message(
        [&] { (void)graph.maximum_flow(0, 1); },
        "Flow capacity setup failed: capacity must be a non-negative integral value representable as long.",
        "maximum_flow should reject capacities outside the long range instead of truncating them");
}

void test_multigraph_flow_results_keep_precise_edge_ids() {
    auto graph = make_multigraph_flow_graph();

    const auto source_edges = graph.edge_ids(0, 1);
    const auto sink_edges = graph.edge_ids(1, 2);
    const auto flow = graph.maximum_flow(0, 2);
    const auto cut = graph.minimum_cut(0, 2);

    expect(source_edges.size() == 2 && sink_edges.size() == 2,
           "test graph should expose two parallel edges per stage");
    expect(flow.value == 3, "multigraph maximum_flow should still find the full aggregate flow");
    expect(flow.flow.at({0, 1}) == 3, "endpoint view should aggregate parallel source-edge flow");
    expect(flow.flow.at({1, 2}) == 3, "endpoint view should aggregate parallel sink-edge flow");
    expect(flow.edge_flows_by_id.at(source_edges[0]) == 1, "first source edge should keep its own precise flow");
    expect(flow.edge_flows_by_id.at(source_edges[1]) == 2, "second source edge should keep its own precise flow");
    expect(flow.edge_flows_by_id.at(sink_edges[0]) == 1, "first sink edge should keep its own precise flow");
    expect(flow.edge_flows_by_id.at(sink_edges[1]) == 2, "second sink edge should keep its own precise flow");
    expect(cut.cut_edges.size() == 2, "cut should still expose both parallel cut edges");
    expect(cut.cut_edge_ids.size() == 2, "cut should expose precise edge IDs for both parallel cut edges");
    expect(cut.cut_edges[0] == std::pair<int, int>{0, 1} && cut.cut_edges[1] == std::pair<int, int>{0, 1},
           "endpoint cut view should show duplicate endpoint pairs when parallel cut edges exist");
}

void test_minimum_cut_multigraph_uses_precise_parallel_capacities() {
    nxpp::UnweightedMultiDiGraphInt graph;

    const auto e01a = graph.add_edge_with_id(0, 1);
    const auto e01b = graph.add_edge_with_id(0, 1);
    const auto e12 = graph.add_edge_with_id(1, 2);

    graph.set_edge_attr(e01a, "capacity", 1L);
    graph.set_edge_attr(e01b, "capacity", 10L);
    graph.set_edge_attr(e12, "capacity", 11L);

    const auto cut = graph.minimum_cut(0, 2);

    expect(cut.value == 11, "minimum_cut should aggregate the heterogeneous parallel capacities precisely");
    expect(cut.cut_edges.size() == 2, "minimum_cut should expose both parallel cut edges in the endpoint view");
    expect(cut.cut_edge_ids.size() == 2, "minimum_cut should expose both parallel cut edge IDs");
    expect(cut.cut_edge_ids[0] == e01a && cut.cut_edge_ids[1] == e01b,
           "minimum_cut should preserve the precise edge IDs for heterogeneous parallel cut edges");
}

void test_multigraph_min_cost_flow_results_keep_precise_edge_ids() {
    auto cycle_graph = make_multigraph_min_cost_flow_graph();
    auto ssp_graph = make_multigraph_min_cost_flow_graph();

    const auto cycle_source_edges = cycle_graph.edge_ids(0, 1);
    const auto cycle_sink_edges = cycle_graph.edge_ids(1, 2);
    const auto cycle_result = cycle_graph.max_flow_min_cost(0, 2);
    const auto ssp_result = ssp_graph.max_flow_min_cost_successive_shortest_path(0, 2);

    expect(cycle_result.flow == 2 && cycle_result.cost == 12,
           "cycle-canceling wrapper should preserve aggregate min-cost multigraph results");
    expect(ssp_result.flow == 2 && ssp_result.cost == 12,
           "SSP wrapper should preserve aggregate min-cost multigraph results");
    expect(cycle_result.edge_flows.at({0, 1}) == 2 && cycle_result.edge_flows.at({1, 2}) == 2,
           "endpoint min-cost view should aggregate parallel edges");
    expect(cycle_result.edge_flows_by_id.at(cycle_source_edges[0]) == 1,
           "cycle-canceling result should preserve the first source edge separately");
    expect(cycle_result.edge_flows_by_id.at(cycle_source_edges[1]) == 1,
           "cycle-canceling result should preserve the second source edge separately");
    expect(cycle_result.edge_flows_by_id.at(cycle_sink_edges[0]) == 1,
           "cycle-canceling result should preserve the first sink edge separately");
    expect(cycle_result.edge_flows_by_id.at(cycle_sink_edges[1]) == 1,
           "cycle-canceling result should preserve the second sink edge separately");
    expect(ssp_result.edge_flows_by_id.size() == 4,
           "SSP result should also expose a precise edge-id keyed flow view");
}

int main() {
    return run_tests({
        {"cycle_canceling requires cached flow state", test_cycle_canceling_requires_cached_flow_state},
        {"cycle_canceling invalidated by graph mutation", test_cycle_canceling_invalidated_by_graph_mutation},
        {"staged min-cost-flow states are isolated between graph instances", test_staged_min_cost_flow_states_are_isolated_between_graph_instances},
        {"copy after staged flow has no cached flow state", test_copy_after_staged_flow_has_no_cached_flow_state},
        {"move after staged flow leaves source empty and uncached", test_move_after_staged_flow_leaves_source_empty_and_uncached},
        {"maximum_flow matches snippet case", test_maximum_flow_matches_snippet_case},
        {"minimum_cut matches flow value and partition", test_minimum_cut_matches_flow_value_and_partition},
        {"push_relabel and cycle_canceling match reference cost", test_push_relabel_and_cycle_canceling_match_reference_cost},
        {"successive_shortest_path matches reference flow and cost", test_successive_shortest_path_matches_reference_flow_and_cost},
        {"min-cost aliases match specialized wrappers", test_min_cost_aliases_match_specialized_wrappers},
        {"fractional flow capacity is rejected", test_fractional_flow_capacity_is_rejected},
        {"out-of-range flow capacity is rejected", test_out_of_range_flow_capacity_is_rejected},
        {"multigraph flow results keep precise edge ids", test_multigraph_flow_results_keep_precise_edge_ids},
        {"minimum_cut multigraph uses precise parallel capacities", test_minimum_cut_multigraph_uses_precise_parallel_capacities},
        {"multigraph min-cost flow results keep precise edge ids", test_multigraph_min_cost_flow_results_keep_precise_edge_ids},
    });
}
