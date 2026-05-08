#include <any>
#include <cstddef>
#include <map>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#ifndef NXPP_HEADER_UNDER_TEST
#define NXPP_HEADER_UNDER_TEST "include/nxpp.hpp"
#endif

#include NXPP_HEADER_UNDER_TEST

#include "test_helpers.hpp"

using namespace nxpp::test;

namespace {

template <typename Actual, typename Expected>
constexpr bool same = std::is_same_v<Actual, Expected>;

void test_public_graph_alias_snapshot() {
    static_assert(same<nxpp::WeightedGraphInt, nxpp::Graph<int, int>>);
    static_assert(same<nxpp::WeightedDiGraphInt, nxpp::Graph<int, int, true>>);
    static_assert(same<nxpp::WeightedGraphStr, nxpp::Graph<std::string>>);
    static_assert(same<nxpp::WeightedDiGraphStr, nxpp::Graph<std::string, double, true>>);
    static_assert(same<nxpp::WeightedMultiGraphInt, nxpp::Graph<int, int, false, true>>);
    static_assert(same<nxpp::WeightedMultiDiGraphInt, nxpp::Graph<int, int, true, true>>);
    static_assert(same<nxpp::UnweightedGraphInt, nxpp::Graph<int, double, false, false, false>>);
    static_assert(same<nxpp::UnweightedDiGraphInt, nxpp::Graph<int, double, true, false, false>>);

    static_assert(same<nxpp::GraphInt, nxpp::WeightedGraphInt>);
    static_assert(same<nxpp::DiGraphInt, nxpp::WeightedDiGraphInt>);
    static_assert(same<nxpp::GraphStr, nxpp::WeightedGraphStr>);
    static_assert(same<nxpp::DiGraph, nxpp::WeightedDiGraphStr>);
    static_assert(same<nxpp::MultiGraphInt, nxpp::WeightedMultiGraphInt>);
    static_assert(same<nxpp::MultiDiGraphInt, nxpp::WeightedMultiDiGraphInt>);
    static_assert(same<nxpp::MultiGraph, nxpp::WeightedMultiGraphStr>);
    static_assert(same<nxpp::MultiDiGraph, nxpp::WeightedMultiDiGraphStr>);

    static_assert(same<nxpp::storage::Vec, boost::vecS>);
    static_assert(same<nxpp::storage::List, boost::listS>);
    static_assert(same<nxpp::storage::Set, boost::setS>);

    expect(true, "public graph aliases should match the compatibility snapshot");
}

void test_public_graph_method_signature_snapshot() {
    using Graph = nxpp::WeightedDiGraphInt;
    using EdgeAttrs = std::map<std::string, std::any>;

    static_assert(same<Graph::NodeType, int>);
    static_assert(same<Graph::EdgeWeightType, int>);
    static_assert(Graph::is_directed);
    static_assert(Graph::has_builtin_edge_weight);

    static_assert(same<decltype(std::declval<Graph&>().add_node(std::declval<const int&>())), void>);
    static_assert(same<decltype(std::declval<Graph&>().add_edge(std::declval<const int&>(), std::declval<const int&>(), int{})), void>);
    static_assert(same<decltype(std::declval<Graph&>().add_edge(std::declval<const int&>(), std::declval<const int&>(), int{}, std::declval<const EdgeAttrs&>())), void>);
    static_assert(same<decltype(std::declval<Graph&>().remove_node(std::declval<const int&>())), void>);
    static_assert(same<decltype(std::declval<Graph&>().remove_edge(std::declval<const int&>(), std::declval<const int&>())), void>);

    static_assert(same<decltype(std::declval<const Graph&>().nodes()), std::vector<int>>);
    static_assert(same<decltype(std::declval<const Graph&>().edges()), std::vector<std::pair<int, int>>>);
    static_assert(same<decltype(std::declval<const Graph&>().edge_pairs()), std::vector<std::pair<int, int>>>);
    static_assert(same<decltype(std::declval<const Graph&>().weighted_edges()), std::vector<std::tuple<int, int, int>>>);
    static_assert(same<decltype(std::declval<const Graph&>().edge_ids()), std::vector<std::size_t>>);
    static_assert(same<decltype(std::declval<const Graph&>().edge_ids(std::declval<const int&>(), std::declval<const int&>())), std::vector<std::size_t>>);
    static_assert(same<decltype(std::declval<const Graph&>().get_edge_weight(std::declval<const int&>(), std::declval<const int&>())), int>);
    static_assert(same<decltype(std::declval<const Graph&>().get_edge_weight(std::size_t{})), int>);

    expect(true, "public graph method signatures should match the compatibility snapshot");
}

void test_public_algorithm_signature_snapshot() {
    using Graph = nxpp::WeightedDiGraphInt;

    static_assert(same<decltype(std::declval<const Graph&>().maximum_flow(std::declval<const int&>(), std::declval<const int&>())), nxpp::MaximumFlowResult<int>>);
    static_assert(same<decltype(std::declval<const Graph&>().minimum_cut(std::declval<const int&>(), std::declval<const int&>())), nxpp::MinimumCutResult<int>>);
    static_assert(same<decltype(std::declval<const Graph&>().max_flow_min_cost_cycle_canceling(std::declval<const int&>(), std::declval<const int&>())), nxpp::MinCostMaxFlowResult<int>>);
    static_assert(same<decltype(std::declval<const Graph&>().push_relabel_maximum_flow(std::declval<const int&>(), std::declval<const int&>())), long>);
    static_assert(same<decltype(std::declval<const Graph&>().cycle_canceling()), long>);
    static_assert(same<decltype(std::declval<const Graph&>().successive_shortest_path_nonnegative_weights(std::declval<const int&>(), std::declval<const int&>())), nxpp::MinCostMaxFlowResult<int>>);
    static_assert(same<decltype(std::declval<const Graph&>().dijkstra_shortest_paths(std::declval<const int&>())), nxpp::SingleSourceShortestPathResult<int, int>>);

    expect(true, "public algorithm signatures should match the compatibility snapshot");
}

void test_public_result_shape_snapshot() {
    static_assert(same<decltype(nxpp::MaximumFlowResult<int>{}.value), long>);
    static_assert(same<decltype(nxpp::MaximumFlowResult<int>{}.flow), std::map<std::pair<int, int>, long>>);
    static_assert(same<decltype(nxpp::MaximumFlowResult<int>{}.edge_flows_by_id), std::map<std::size_t, long>>);

    static_assert(same<decltype(nxpp::MinimumCutResult<int>{}.value), long>);
    static_assert(same<decltype(nxpp::MinimumCutResult<int>{}.reachable), std::vector<int>>);
    static_assert(same<decltype(nxpp::MinimumCutResult<int>{}.non_reachable), std::vector<int>>);
    static_assert(same<decltype(nxpp::MinimumCutResult<int>{}.cut_edges), std::vector<std::pair<int, int>>>);
    static_assert(same<decltype(nxpp::MinimumCutResult<int>{}.cut_edge_ids), std::vector<std::size_t>>);

    static_assert(same<decltype(nxpp::MinCostMaxFlowResult<int>{}.flow), long>);
    static_assert(same<decltype(nxpp::MinCostMaxFlowResult<int>{}.cost), long>);
    static_assert(same<decltype(nxpp::MinCostMaxFlowResult<int>{}.edge_flows), std::map<std::pair<int, int>, long>>);
    static_assert(same<decltype(nxpp::MinCostMaxFlowResult<int>{}.edge_flows_by_id), std::map<std::size_t, long>>);

    static_assert(same<decltype(nxpp::SingleSourceShortestPathResult<int, int>{}.distance), std::map<int, int>>);
    static_assert(same<decltype(nxpp::SingleSourceShortestPathResult<int, int>{}.predecessor), std::map<int, int>>);
    static_assert(same<decltype(std::declval<const nxpp::SingleSourceShortestPathResult<int, int>&>().has_path_to(std::declval<const int&>())), bool>);
    static_assert(same<decltype(std::declval<const nxpp::SingleSourceShortestPathResult<int, int>&>().path_to(std::declval<const int&>())), std::vector<int>>);

    expect(true, "public result shapes should match the compatibility snapshot");
}

} // namespace

int main() {
    return run_tests({
        {"public graph alias snapshot", test_public_graph_alias_snapshot},
        {"public graph method signature snapshot", test_public_graph_method_signature_snapshot},
        {"public algorithm signature snapshot", test_public_algorithm_signature_snapshot},
        {"public result shape snapshot", test_public_result_shape_snapshot},
    });
}
