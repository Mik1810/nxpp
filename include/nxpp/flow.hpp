#pragma once

#include <boost/graph/edmonds_karp_max_flow.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/find_flow_cost.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>

#include "attributes.hpp"

namespace nxpp {

template <typename NodeID>
struct MaximumFlowResult {
    long value = 0;
    std::map<std::pair<NodeID, NodeID>, long> flow;
};

template <typename NodeID>
struct MinCostMaxFlowResult {
    long flow = 0;
    long cost = 0;
    std::map<std::pair<NodeID, NodeID>, long> edge_flows;
};

namespace detail {

template <typename NodeID>
struct MinCostFlowState {
    using FlowTraits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
    using FlowGraph = boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::directedS,
        boost::no_property,
        boost::property<
            boost::edge_weight_t,
            long,
            boost::property<
                boost::edge_capacity_t,
                long,
                boost::property<
                    boost::edge_residual_capacity_t,
                    long,
                    boost::property<boost::edge_reverse_t, typename FlowTraits::edge_descriptor>
                >
            >
        >
    >;
    using WeightMap = typename boost::property_map<FlowGraph, boost::edge_weight_t>::type;
    using CapacityMap = typename boost::property_map<FlowGraph, boost::edge_capacity_t>::type;
    using ResidualMap = typename boost::property_map<FlowGraph, boost::edge_residual_capacity_t>::type;
    using ReverseMap = typename boost::property_map<FlowGraph, boost::edge_reverse_t>::type;
    using FlowEdgeDesc = typename boost::graph_traits<FlowGraph>::edge_descriptor;

    FlowGraph flow_graph;
    WeightMap weight;
    CapacityMap capacity;
    ResidualMap residual;
    ReverseMap reverse;
    std::unordered_map<NodeID, std::size_t> node_to_index;
    std::map<std::pair<NodeID, NodeID>, FlowEdgeDesc> original_edges;
    NodeID source_id;
    NodeID target_id;
    long value = 0;
    long cost = 0;

    MinCostFlowState()
        : flow_graph(),
          weight(boost::get(boost::edge_weight, flow_graph)),
          capacity(boost::get(boost::edge_capacity, flow_graph)),
          residual(boost::get(boost::edge_residual_capacity, flow_graph)),
          reverse(boost::get(boost::edge_reverse, flow_graph)) {}
};

template <typename GraphWrapper>
auto& min_cost_flow_cache() {
    using NodeID = typename GraphWrapper::NodeType;
    static std::unordered_map<const void*, std::unique_ptr<MinCostFlowState<NodeID>>> cache;
    return cache;
}

} // namespace detail

template <typename NodeID>
struct MinimumCutResult {
    long value = 0;
    std::vector<NodeID> reachable;
    std::vector<NodeID> non_reachable;
    std::vector<std::pair<NodeID, NodeID>> cut_edges;
};

template <typename GraphWrapper>
[[deprecated("Use G.edmonds_karp_maximum_flow(source, target, capacity_attr) instead.")]]
auto edmonds_karp_maximum_flow(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity") {
    return G.edmonds_karp_maximum_flow(source_id, target_id, capacity_attr);
}

template <typename GraphWrapper>
[[deprecated("Use G.push_relabel_maximum_flow_result(source, target, capacity_attr) instead.")]]
auto push_relabel_maximum_flow_result(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity") {
    return G.push_relabel_maximum_flow_result(source_id, target_id, capacity_attr);
}

template <typename GraphWrapper>
[[deprecated("Use G.maximum_flow(source, target, capacity_attr) instead.")]]
auto maximum_flow(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity") {
    return G.maximum_flow(source_id, target_id, capacity_attr);
}

template <typename GraphWrapper>
[[deprecated("Use G.minimum_cut(source, target, capacity_attr) instead.")]]
auto minimum_cut(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity") {
    return G.minimum_cut(source_id, target_id, capacity_attr);
}

template <typename GraphWrapper>
[[deprecated("Use G.max_flow_min_cost_cycle_canceling(source, target, capacity_attr, weight_attr) instead.")]]
auto max_flow_min_cost_cycle_canceling(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") {
    return G.max_flow_min_cost_cycle_canceling(source_id, target_id, capacity_attr, weight_attr);
}

template <typename GraphWrapper>
[[deprecated("Use G.push_relabel_maximum_flow(source, target, capacity_attr, weight_attr) instead.")]]
long push_relabel_maximum_flow(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") {
    return G.push_relabel_maximum_flow(source_id, target_id, capacity_attr, weight_attr);
}

template <typename GraphWrapper>
[[deprecated("Use G.cycle_canceling(weight_attr) instead.")]]
auto cycle_canceling(const GraphWrapper& G, const std::string& weight_attr = "weight") {
    return G.cycle_canceling(weight_attr);
}

template <typename GraphWrapper>
[[deprecated("Use G.successive_shortest_path_nonnegative_weights(source, target, capacity_attr, weight_attr) instead.")]]
auto successive_shortest_path_nonnegative_weights(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") {
    return G.successive_shortest_path_nonnegative_weights(source_id, target_id, capacity_attr, weight_attr);
}

template <typename GraphWrapper>
[[deprecated("Use G.max_flow_min_cost_successive_shortest_path(source, target, capacity_attr, weight_attr) instead.")]]
auto max_flow_min_cost_successive_shortest_path(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") {
    return G.max_flow_min_cost_successive_shortest_path(source_id, target_id, capacity_attr, weight_attr);
}
template <typename GraphWrapper>
[[deprecated("Use G.max_flow_min_cost(source, target, capacity_attr, weight_attr) instead.")]]
auto max_flow_min_cost(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") {
    return G.max_flow_min_cost(source_id, target_id, capacity_attr, weight_attr);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::edmonds_karp_maximum_flow(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr) const {
    if (!has_node(source_id) || !has_node(target_id)) throw std::runtime_error("Source or target node not found in graph.");
    using FlowTraits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
    using FlowGraph = boost::adjacency_list<
        boost::vecS, boost::vecS, boost::directedS, boost::no_property,
        boost::property<boost::edge_capacity_t, long,
            boost::property<boost::edge_residual_capacity_t, long,
                boost::property<boost::edge_reverse_t, typename FlowTraits::edge_descriptor>>>>;
    using CapacityMap = typename boost::property_map<FlowGraph, boost::edge_capacity_t>::type;
    using ResidualMap = typename boost::property_map<FlowGraph, boost::edge_residual_capacity_t>::type;
    using ReverseMap = typename boost::property_map<FlowGraph, boost::edge_reverse_t>::type;
    using FlowEdgeDesc = typename boost::graph_traits<FlowGraph>::edge_descriptor;

    FlowGraph flow_graph;
    CapacityMap capacity = boost::get(boost::edge_capacity, flow_graph);
    ReverseMap reverse = boost::get(boost::edge_reverse, flow_graph);
    const auto all_nodes = nodes();
    std::unordered_map<NodeID, std::size_t> node_to_index;
    for (std::size_t i = 0; i < all_nodes.size(); ++i) { boost::add_vertex(flow_graph); node_to_index[all_nodes[i]] = i; }

    std::map<std::pair<NodeID, NodeID>, FlowEdgeDesc> original_edges;
    for (const auto& [u, v] : edge_pairs()) {
        auto [e, added] = boost::add_edge(node_to_index.at(u), node_to_index.at(v), flow_graph);
        auto [rev, rev_added] = boost::add_edge(node_to_index.at(v), node_to_index.at(u), flow_graph);
        (void)added; (void)rev_added;
        capacity[e] = static_cast<long>(get_edge_numeric_attr(u, v, capacity_attr));
        capacity[rev] = 0;
        reverse[e] = rev;
        reverse[rev] = e;
        original_edges[{u, v}] = e;
    }

    const long flow_value = boost::edmonds_karp_max_flow(flow_graph, node_to_index.at(source_id), node_to_index.at(target_id));
    ResidualMap residual = boost::get(boost::edge_residual_capacity, flow_graph);
    MaximumFlowResult<NodeID> result;
    result.value = flow_value;
    for (const auto& [key, edge_desc] : original_edges) result.flow[key] = capacity[edge_desc] - residual[edge_desc];
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::push_relabel_maximum_flow_result(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr) const {
    if (!has_node(source_id) || !has_node(target_id)) throw std::runtime_error("Source or target node not found in graph.");
    using FlowTraits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
    using FlowGraph = boost::adjacency_list<
        boost::vecS, boost::vecS, boost::directedS, boost::no_property,
        boost::property<boost::edge_capacity_t, long,
            boost::property<boost::edge_residual_capacity_t, long,
                boost::property<boost::edge_reverse_t, typename FlowTraits::edge_descriptor>>>>;
    using CapacityMap = typename boost::property_map<FlowGraph, boost::edge_capacity_t>::type;
    using ResidualMap = typename boost::property_map<FlowGraph, boost::edge_residual_capacity_t>::type;
    using ReverseMap = typename boost::property_map<FlowGraph, boost::edge_reverse_t>::type;
    using FlowEdgeDesc = typename boost::graph_traits<FlowGraph>::edge_descriptor;

    FlowGraph flow_graph;
    CapacityMap capacity = boost::get(boost::edge_capacity, flow_graph);
    ReverseMap reverse = boost::get(boost::edge_reverse, flow_graph);
    const auto all_nodes = nodes();
    std::unordered_map<NodeID, std::size_t> node_to_index;
    for (std::size_t i = 0; i < all_nodes.size(); ++i) { boost::add_vertex(flow_graph); node_to_index[all_nodes[i]] = i; }

    std::map<std::pair<NodeID, NodeID>, FlowEdgeDesc> original_edges;
    for (const auto& [u, v] : edge_pairs()) {
        auto [e, added] = boost::add_edge(node_to_index.at(u), node_to_index.at(v), flow_graph);
        auto [rev, rev_added] = boost::add_edge(node_to_index.at(v), node_to_index.at(u), flow_graph);
        (void)added; (void)rev_added;
        capacity[e] = static_cast<long>(get_edge_numeric_attr(u, v, capacity_attr));
        capacity[rev] = 0;
        reverse[e] = rev;
        reverse[rev] = e;
        original_edges[{u, v}] = e;
    }

    const long flow_value = boost::push_relabel_max_flow(flow_graph, node_to_index.at(source_id), node_to_index.at(target_id));
    ResidualMap residual = boost::get(boost::edge_residual_capacity, flow_graph);
    MaximumFlowResult<NodeID> result;
    result.value = flow_value;
    for (const auto& [key, edge_desc] : original_edges) result.flow[key] = capacity[edge_desc] - residual[edge_desc];
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::maximum_flow(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr) const { return edmonds_karp_maximum_flow(source_id, target_id, capacity_attr); }

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::minimum_cut(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr) const {
    if (!has_node(source_id) || !has_node(target_id)) throw std::runtime_error("Source or target node not found in graph.");
    using FlowTraits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
    using FlowGraph = boost::adjacency_list<
        boost::vecS, boost::vecS, boost::directedS, boost::no_property,
        boost::property<boost::edge_capacity_t, long,
            boost::property<boost::edge_residual_capacity_t, long,
                boost::property<boost::edge_reverse_t, typename FlowTraits::edge_descriptor>>>>;
    using CapacityMap = typename boost::property_map<FlowGraph, boost::edge_capacity_t>::type;
    using ResidualMap = typename boost::property_map<FlowGraph, boost::edge_residual_capacity_t>::type;
    using ReverseMap = typename boost::property_map<FlowGraph, boost::edge_reverse_t>::type;

    FlowGraph flow_graph;
    CapacityMap capacity = boost::get(boost::edge_capacity, flow_graph);
    ReverseMap reverse = boost::get(boost::edge_reverse, flow_graph);
    const auto all_nodes = nodes();
    std::unordered_map<NodeID, std::size_t> node_to_index;
    std::vector<NodeID> index_to_node;
    index_to_node.reserve(all_nodes.size());
    for (std::size_t i = 0; i < all_nodes.size(); ++i) { boost::add_vertex(flow_graph); node_to_index[all_nodes[i]] = i; index_to_node.push_back(all_nodes[i]); }
    for (const auto& [u, v] : edge_pairs()) {
        auto [e, added] = boost::add_edge(node_to_index.at(u), node_to_index.at(v), flow_graph);
        auto [rev, rev_added] = boost::add_edge(node_to_index.at(v), node_to_index.at(u), flow_graph);
        (void)added; (void)rev_added;
        capacity[e] = static_cast<long>(get_edge_numeric_attr(u, v, capacity_attr));
        capacity[rev] = 0;
        reverse[e] = rev;
        reverse[rev] = e;
    }
    const auto source_index = node_to_index.at(source_id);
    const auto target_index = node_to_index.at(target_id);
    const long cut_value = boost::edmonds_karp_max_flow(flow_graph, source_index, target_index);
    ResidualMap residual = boost::get(boost::edge_residual_capacity, flow_graph);
    std::vector<bool> visited(all_nodes.size(), false);
    std::queue<std::size_t> q;
    visited[source_index] = true;
    q.push(source_index);
    while (!q.empty()) {
        const auto current = q.front();
        q.pop();
        for (auto [eit, eend] = boost::out_edges(current, flow_graph); eit != eend; ++eit) {
            const auto next = boost::target(*eit, flow_graph);
            if (!visited[next] && residual[*eit] > 0) { visited[next] = true; q.push(next); }
        }
    }
    MinimumCutResult<NodeID> result;
    result.value = cut_value;
    for (std::size_t i = 0; i < index_to_node.size(); ++i) {
        if (visited[i]) result.reachable.push_back(index_to_node[i]);
        else result.non_reachable.push_back(index_to_node[i]);
    }
    for (const auto& [u, v] : edge_pairs()) if (visited[node_to_index.at(u)] && !visited[node_to_index.at(v)]) result.cut_edges.emplace_back(u, v);
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::max_flow_min_cost_cycle_canceling(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr, const std::string& weight_attr) const {
    if (!has_node(source_id) || !has_node(target_id)) throw std::runtime_error("Source or target node not found in graph.");
    using FlowTraits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
    using FlowGraph = boost::adjacency_list<
        boost::vecS, boost::vecS, boost::directedS, boost::no_property,
        boost::property<boost::edge_weight_t, long,
            boost::property<boost::edge_capacity_t, long,
                boost::property<boost::edge_residual_capacity_t, long,
                    boost::property<boost::edge_reverse_t, typename FlowTraits::edge_descriptor>>>>>;
    using WeightMap = typename boost::property_map<FlowGraph, boost::edge_weight_t>::type;
    using CapacityMap = typename boost::property_map<FlowGraph, boost::edge_capacity_t>::type;
    using ResidualMap = typename boost::property_map<FlowGraph, boost::edge_residual_capacity_t>::type;
    using ReverseMap = typename boost::property_map<FlowGraph, boost::edge_reverse_t>::type;
    using FlowEdgeDesc = typename boost::graph_traits<FlowGraph>::edge_descriptor;

    FlowGraph flow_graph;
    WeightMap weight = boost::get(boost::edge_weight, flow_graph);
    CapacityMap capacity = boost::get(boost::edge_capacity, flow_graph);
    ReverseMap reverse = boost::get(boost::edge_reverse, flow_graph);
    const auto all_nodes = nodes();
    std::unordered_map<NodeID, std::size_t> node_to_index;
    for (std::size_t i = 0; i < all_nodes.size(); ++i) { boost::add_vertex(flow_graph); node_to_index[all_nodes[i]] = i; }
    std::map<std::pair<NodeID, NodeID>, FlowEdgeDesc> original_edges;
    for (const auto& [u, v] : edge_pairs()) {
        auto [e, added] = boost::add_edge(node_to_index.at(u), node_to_index.at(v), flow_graph);
        auto [rev, rev_added] = boost::add_edge(node_to_index.at(v), node_to_index.at(u), flow_graph);
        (void)added; (void)rev_added;
        weight[e] = static_cast<long>(get_edge_numeric_attr(u, v, weight_attr));
        weight[rev] = -weight[e];
        capacity[e] = static_cast<long>(get_edge_numeric_attr(u, v, capacity_attr));
        capacity[rev] = 0;
        reverse[e] = rev;
        reverse[rev] = e;
        original_edges[{u, v}] = e;
    }
    boost::push_relabel_max_flow(flow_graph, node_to_index.at(source_id), node_to_index.at(target_id));
    boost::cycle_canceling(flow_graph);
    const long cost = boost::find_flow_cost(flow_graph);
    ResidualMap residual = boost::get(boost::edge_residual_capacity, flow_graph);
    MinCostMaxFlowResult<NodeID> result;
    result.cost = cost;
    for (const auto& [key, edge_desc] : original_edges) {
        result.edge_flows[key] = capacity[edge_desc] - residual[edge_desc];
        if (key.first == source_id) result.flow += result.edge_flows[key];
    }
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
long Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::push_relabel_maximum_flow(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr, const std::string& weight_attr) const {
    if (!has_node(source_id) || !has_node(target_id)) throw std::runtime_error("Source or target node not found in graph.");
    auto state = std::make_unique<detail::MinCostFlowState<NodeID>>();
    state->source_id = source_id;
    state->target_id = target_id;
    const auto all_nodes = nodes();
    for (std::size_t i = 0; i < all_nodes.size(); ++i) { boost::add_vertex(state->flow_graph); state->node_to_index[all_nodes[i]] = i; }
    for (const auto& [u, v] : edge_pairs()) {
        auto [e, added] = boost::add_edge(state->node_to_index.at(u), state->node_to_index.at(v), state->flow_graph);
        auto [rev, rev_added] = boost::add_edge(state->node_to_index.at(v), state->node_to_index.at(u), state->flow_graph);
        (void)added; (void)rev_added;
        state->weight[e] = static_cast<long>(get_edge_numeric_attr(u, v, weight_attr));
        state->weight[rev] = -state->weight[e];
        state->capacity[e] = static_cast<long>(get_edge_numeric_attr(u, v, capacity_attr));
        state->capacity[rev] = 0;
        state->reverse[e] = rev;
        state->reverse[rev] = e;
        state->original_edges[{u, v}] = e;
    }
    state->value = boost::push_relabel_max_flow(state->flow_graph, state->node_to_index.at(source_id), state->node_to_index.at(target_id));
    auto& cache = detail::min_cost_flow_cache<Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>>();
    cache[static_cast<const void*>(this)] = std::move(state);
    return cache.at(static_cast<const void*>(this))->value;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::cycle_canceling(const std::string& weight_attr) const {
    auto& cache = detail::min_cost_flow_cache<Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>>();
    auto it = cache.find(static_cast<const void*>(this));
    if (it == cache.end()) throw std::runtime_error("Cycle-canceling state unavailable: run push_relabel_maximum_flow(...) first.");
    auto& state = *it->second;
    for (const auto& [key, edge_desc] : state.original_edges) {
        state.weight[edge_desc] = static_cast<long>(get_edge_numeric_attr(key.first, key.second, weight_attr));
        const auto rev = state.reverse[edge_desc];
        state.weight[rev] = -state.weight[edge_desc];
    }
    boost::cycle_canceling(state.flow_graph);
    state.cost = boost::find_flow_cost(state.flow_graph);
    return state.cost;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::successive_shortest_path_nonnegative_weights(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr, const std::string& weight_attr) const {
    if (!has_node(source_id) || !has_node(target_id)) throw std::runtime_error("Source or target node not found in graph.");
    using FlowTraits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
    using FlowGraph = boost::adjacency_list<
        boost::vecS, boost::vecS, boost::directedS, boost::no_property,
        boost::property<boost::edge_weight_t, long,
            boost::property<boost::edge_capacity_t, long,
                boost::property<boost::edge_residual_capacity_t, long,
                    boost::property<boost::edge_reverse_t, typename FlowTraits::edge_descriptor>>>>>;
    using WeightMap = typename boost::property_map<FlowGraph, boost::edge_weight_t>::type;
    using CapacityMap = typename boost::property_map<FlowGraph, boost::edge_capacity_t>::type;
    using ResidualMap = typename boost::property_map<FlowGraph, boost::edge_residual_capacity_t>::type;
    using ReverseMap = typename boost::property_map<FlowGraph, boost::edge_reverse_t>::type;
    using FlowEdgeDesc = typename boost::graph_traits<FlowGraph>::edge_descriptor;

    FlowGraph flow_graph;
    WeightMap weight = boost::get(boost::edge_weight, flow_graph);
    CapacityMap capacity = boost::get(boost::edge_capacity, flow_graph);
    ReverseMap reverse = boost::get(boost::edge_reverse, flow_graph);
    const auto all_nodes = nodes();
    std::unordered_map<NodeID, std::size_t> node_to_index;
    for (std::size_t i = 0; i < all_nodes.size(); ++i) { boost::add_vertex(flow_graph); node_to_index[all_nodes[i]] = i; }
    std::map<std::pair<NodeID, NodeID>, FlowEdgeDesc> original_edges;
    for (const auto& [u, v] : edge_pairs()) {
        auto [e, added] = boost::add_edge(node_to_index.at(u), node_to_index.at(v), flow_graph);
        auto [rev, rev_added] = boost::add_edge(node_to_index.at(v), node_to_index.at(u), flow_graph);
        (void)added; (void)rev_added;
        weight[e] = static_cast<long>(get_edge_numeric_attr(u, v, weight_attr));
        weight[rev] = -weight[e];
        capacity[e] = static_cast<long>(get_edge_numeric_attr(u, v, capacity_attr));
        capacity[rev] = 0;
        reverse[e] = rev;
        reverse[rev] = e;
        original_edges[{u, v}] = e;
    }
    boost::successive_shortest_path_nonnegative_weights(flow_graph, node_to_index.at(source_id), node_to_index.at(target_id));
    ResidualMap residual = boost::get(boost::edge_residual_capacity, flow_graph);
    long flow_value = 0;
    for (auto [eit, eend] = boost::out_edges(node_to_index.at(source_id), flow_graph); eit != eend; ++eit) flow_value += capacity[*eit] - residual[*eit];
    const long cost = boost::find_flow_cost(flow_graph);
    MinCostMaxFlowResult<NodeID> result;
    result.flow = flow_value;
    result.cost = cost;
    for (const auto& [key, edge_desc] : original_edges) result.edge_flows[key] = capacity[edge_desc] - residual[edge_desc];
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::max_flow_min_cost_successive_shortest_path(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr, const std::string& weight_attr) const {
    return successive_shortest_path_nonnegative_weights(source_id, target_id, capacity_attr, weight_attr);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::max_flow_min_cost(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr, const std::string& weight_attr) const {
    return max_flow_min_cost_cycle_canceling(source_id, target_id, capacity_attr, weight_attr);
}

} // namespace nxpp
