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
    expect(result.paths.at("Naples") == expected_path,
           "wrong materialized path for Naples");
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

    passed += run_test("string attributes and normalization", test_string_attributes_and_normalization) ? 1 : 0;
    passed += run_test("dijkstra result wrapper", test_dijkstra_result_wrapper) ? 1 : 0;
    passed += run_test("multigraph edge_id path", test_multigraph_edge_id_path) ? 1 : 0;
    passed += run_test("multigraph remove_edge cleanup", test_multigraph_remove_edge_cleanup) ? 1 : 0;
    passed += run_test("proxy assignment normalizes C-strings", test_proxy_assignment_normalizes_c_strings) ? 1 : 0;

    return passed == total ? 0 : 1;
}
