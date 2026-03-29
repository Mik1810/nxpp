#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

#include "include/nxpp/attributes.hpp"

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

void test_missing_node_attr_throws() {
    nxpp::DiGraph graph;
    graph.add_node("Rome");

    expect_throws(
        [&] { (void)graph.get_node_attr<std::string>("Rome", "nickname"); },
        "missing node attribute should throw");
}

void test_missing_edge_attr_throws() {
    nxpp::DiGraph graph;
    graph.add_edge("Milan", "Rome", 5.0);

    expect_throws(
        [&] { (void)graph.get_edge_attr<std::string>("Milan", "Rome", "company"); },
        "missing edge attribute should throw");
}

void test_type_mismatch_throws() {
    nxpp::DiGraph graph;
    graph.node("Rome")["nickname"] = "Caput Mundi";
    graph["Milan"]["Rome"] = 5.0;
    graph["Milan"]["Rome"]["capacity"] = 8;

    expect_throws(
        [&] { (void)graph.get_node_attr<int>("Rome", "nickname"); },
        "node attribute type mismatch should throw");
    expect_throws(
        [&] { (void)graph.get_edge_attr<std::string>("Milan", "Rome", "capacity"); },
        "edge attribute type mismatch should throw");
}

void test_try_get_returns_empty_for_missing_or_mismatch() {
    nxpp::DiGraph graph;
    graph.node("Rome")["nickname"] = "Caput Mundi";
    graph["Milan"]["Rome"] = 5.0;
    graph["Milan"]["Rome"]["capacity"] = 8;

    expect(!graph.try_get_node_attr<std::string>("Rome", "region").has_value(),
           "try_get_node_attr should return empty for missing key");
    expect(!graph.try_get_node_attr<int>("Rome", "nickname").has_value(),
           "try_get_node_attr should return empty for type mismatch");
    expect(!graph.try_get_edge_attr<std::string>("Milan", "Rome", "company").has_value(),
           "try_get_edge_attr should return empty for missing key");
    expect(!graph.try_get_edge_attr<std::string>("Milan", "Rome", "capacity").has_value(),
           "try_get_edge_attr should return empty for type mismatch");
}

void test_non_numeric_edge_attr_throws_in_numeric_lookup() {
    nxpp::DiGraph graph;
    graph["Milan"]["Rome"] = 5.0;
    graph["Milan"]["Rome"]["company"] = "Trenitalia";

    expect_throws(
        [&] { (void)graph.get_edge_numeric_attr("Milan", "Rome", "company"); },
        "numeric edge lookup should throw on string attribute");
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

    passed += run_test("missing node attr throws", test_missing_node_attr_throws) ? 1 : 0;
    passed += run_test("missing edge attr throws", test_missing_edge_attr_throws) ? 1 : 0;
    passed += run_test("attribute type mismatch throws", test_type_mismatch_throws) ? 1 : 0;
    passed += run_test("try_get returns empty for missing or mismatch", test_try_get_returns_empty_for_missing_or_mismatch) ? 1 : 0;
    passed += run_test("non-numeric edge attr throws in numeric lookup", test_non_numeric_edge_attr_throws_in_numeric_lookup) ? 1 : 0;

    return passed == total ? 0 : 1;
}
