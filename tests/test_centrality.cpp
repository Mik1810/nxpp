#include <cmath>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

#ifndef NXPP_HEADER_UNDER_TEST
#define NXPP_HEADER_UNDER_TEST "include/nxpp/centrality.hpp"
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

void expect_near(double actual, double expected, double tolerance, const std::string& message) {
    if (std::abs(actual - expected) > tolerance) {
        throw std::runtime_error(
            message + ": expected " + std::to_string(expected) + ", got " + std::to_string(actual)
        );
    }
}

void test_degree_centrality_path_graph() {
    nxpp::GraphInt graph;
    graph.add_edge(0, 1, 1);
    graph.add_edge(1, 2, 1);
    graph.add_edge(2, 3, 1);

    const auto centrality = graph.degree_centrality();

    expect_near(centrality.at(0), 1.0 / 3.0, 1e-9, "leaf node should have degree centrality 1/3");
    expect_near(centrality.at(1), 2.0 / 3.0, 1e-9, "interior node should have degree centrality 2/3");
    expect_near(centrality.at(2), 2.0 / 3.0, 1e-9, "second interior node should have degree centrality 2/3");
    expect_near(centrality.at(3), 1.0 / 3.0, 1e-9, "other leaf node should have degree centrality 1/3");
}

void test_pagerank_returns_normalized_ranking() {
    nxpp::DiGraph graph;
    graph.add_edge("A", "B", 1.0);
    graph.add_edge("B", "C", 1.0);
    graph.add_edge("C", "B", 1.0);
    graph.add_edge("C", "D", 1.0);

    const auto rank = graph.pagerank();

    const double total = rank.at("A") + rank.at("B") + rank.at("C") + rank.at("D");
    expect_near(total, 1.0, 1e-9, "pagerank scores should stay normalized");
    expect(rank.at("B") > rank.at("A"), "referenced node should rank above source-only node");
    expect(rank.at("C") > rank.at("D"), "internally linked node should rank above sink-only leaf");
}

void test_betweenness_centrality_path_graph() {
    nxpp::Graph<> graph;
    graph.add_edge("A", "B", 1.0);
    graph.add_edge("B", "C", 1.0);
    graph.add_edge("C", "D", 1.0);

    const auto bc = graph.betweenness_centrality();

    expect_near(bc.at("A"), 0.0, 1e-9, "leaf node A should have betweenness 0");
    expect_near(bc.at("D"), 0.0, 1e-9, "leaf node D should have betweenness 0");
    expect(bc.at("B") > 0.0, "interior node B should have positive betweenness");
    expect(bc.at("C") > 0.0, "interior node C should have positive betweenness");
    expect_near(bc.at("B"), bc.at("C"), 1e-9, "symmetric interior nodes should have equal betweenness");
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
    constexpr int total = 3;

    passed += run_test("degree_centrality path graph", test_degree_centrality_path_graph) ? 1 : 0;
    passed += run_test("pagerank returns normalized ranking", test_pagerank_returns_normalized_ranking) ? 1 : 0;
    passed += run_test("betweenness_centrality path graph", test_betweenness_centrality_path_graph) ? 1 : 0;

    return passed == total ? 0 : 1;
}
