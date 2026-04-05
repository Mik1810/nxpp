#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef NXPP_HEADER_UNDER_TEST
#define NXPP_HEADER_UNDER_TEST "include/nxpp/flow.hpp"
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
    constexpr int total = 6;

    passed += run_test("cycle_canceling requires cached flow state", test_cycle_canceling_requires_cached_flow_state) ? 1 : 0;
    passed += run_test("maximum_flow matches snippet case", test_maximum_flow_matches_snippet_case) ? 1 : 0;
    passed += run_test("minimum_cut matches flow value and partition", test_minimum_cut_matches_flow_value_and_partition) ? 1 : 0;
    passed += run_test("push_relabel and cycle_canceling match reference cost", test_push_relabel_and_cycle_canceling_match_reference_cost) ? 1 : 0;
    passed += run_test("successive_shortest_path matches reference flow and cost", test_successive_shortest_path_matches_reference_flow_and_cost) ? 1 : 0;
    passed += run_test("min-cost aliases match specialized wrappers", test_min_cost_aliases_match_specialized_wrappers) ? 1 : 0;

    return passed == total ? 0 : 1;
}
