#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef NXPP_HEADER_UNDER_TEST
#define NXPP_HEADER_UNDER_TEST "include/nxpp/multigraph.hpp"
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

void test_edge_endpoints_stay_correct_after_partial_removal() {
    nxpp::MultiDiGraph graph;

    const auto first = graph.add_edge_with_id("A", "B", 1.0);
    const auto second = graph.add_edge_with_id("A", "B", 2.0);

    graph.remove_edge(first);

    const auto endpoints = graph.get_edge_endpoints(second);
    expect(endpoints.first == "A" && endpoints.second == "B",
           "remaining parallel edge should still report the correct endpoints");
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

    passed += run_test("parallel edges get distinct ids", test_parallel_edges_get_distinct_ids) ? 1 : 0;
    passed += run_test("parallel edges keep distinct attributes", test_parallel_edges_keep_distinct_attributes) ? 1 : 0;
    passed += run_test("remove_edge(edge_id) is precise", test_remove_edge_by_id_is_precise) ? 1 : 0;
    passed += run_test("remove_edge(u, v) removes all parallel edges", test_remove_edge_by_endpoints_removes_all_parallel_edges) ? 1 : 0;
    passed += run_test("edge endpoints stay correct after partial removal", test_edge_endpoints_stay_correct_after_partial_removal) ? 1 : 0;

    return passed == total ? 0 : 1;
}
