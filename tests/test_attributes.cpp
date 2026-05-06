#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

#ifndef NXPP_HEADER_UNDER_TEST
#define NXPP_HEADER_UNDER_TEST "include/nxpp/attributes.hpp"
#endif

#include NXPP_HEADER_UNDER_TEST

#include "test_helpers.hpp"

using namespace nxpp::test;

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

void test_numeric_edge_attrs_support_common_arithmetic_types() {
    nxpp::DiGraph graph;
    graph["Milan"]["Rome"] = 5.0;
    graph["Milan"]["Rome"]["signed"] = -3;
    graph["Milan"]["Rome"]["unsigned_int"] = 7u;
    graph["Milan"]["Rome"]["unsigned_long"] = 11ul;
    graph["Milan"]["Rome"]["unsigned_long_long"] = 13ull;
    graph["Milan"]["Rome"]["size"] = static_cast<std::size_t>(17);
    graph["Milan"]["Rome"]["floating"] = 2.5f;
    graph["Milan"]["Rome"]["wide_floating"] = static_cast<long double>(3.5);

    expect(graph.get_edge_numeric_attr("Milan", "Rome", "signed") == -3.0,
           "numeric edge lookup should support signed integers");
    expect(graph.get_edge_numeric_attr("Milan", "Rome", "unsigned_int") == 7.0,
           "numeric edge lookup should support unsigned int");
    expect(graph.get_edge_numeric_attr("Milan", "Rome", "unsigned_long") == 11.0,
           "numeric edge lookup should support unsigned long");
    expect(graph.get_edge_numeric_attr("Milan", "Rome", "unsigned_long_long") == 13.0,
           "numeric edge lookup should support unsigned long long");
    expect(graph.get_edge_numeric_attr("Milan", "Rome", "size") == 17.0,
           "numeric edge lookup should support std::size_t");
    expect(graph.get_edge_numeric_attr("Milan", "Rome", "floating") == 2.5,
           "numeric edge lookup should support floats");
    expect(graph.get_edge_numeric_attr("Milan", "Rome", "wide_floating") == 3.5,
           "numeric edge lookup should support long double");
}

int main() {
    return run_tests({
        {"missing node attr throws", test_missing_node_attr_throws},
        {"missing edge attr throws", test_missing_edge_attr_throws},
        {"attribute type mismatch throws", test_type_mismatch_throws},
        {"try_get returns empty for missing or mismatch", test_try_get_returns_empty_for_missing_or_mismatch},
        {"non-numeric edge attr throws in numeric lookup", test_non_numeric_edge_attr_throws_in_numeric_lookup},
        {"numeric edge attrs support common arithmetic types", test_numeric_edge_attrs_support_common_arithmetic_types},
    });
}
