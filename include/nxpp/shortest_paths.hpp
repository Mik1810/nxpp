#pragma once

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include <boost/graph/dag_shortest_paths.hpp>
#include <boost/graph/floyd_warshall_shortest.hpp>

#include "graph.hpp"

namespace nxpp {

template <typename NodeID, typename Distance = double>
struct SingleSourceShortestPathResult {
    std::unordered_map<NodeID, Distance> distance;
    std::unordered_map<NodeID, NodeID> predecessor;
    std::unordered_map<NodeID, std::vector<NodeID>> paths;
};

template <typename GraphWrapper, typename Distance, typename VertexDesc>
std::unordered_map<typename GraphWrapper::NodeType, std::vector<typename GraphWrapper::NodeType>> build_single_source_paths(
    const GraphWrapper& graph_wrapper,
    const std::vector<Distance>& dist,
    const std::vector<VertexDesc>& pred
) {
    using NodeID = typename GraphWrapper::NodeType;
    std::unordered_map<NodeID, std::vector<NodeID>> paths;
    const auto unreachable = std::numeric_limits<Distance>::max();
    const auto& bgl_to_id = graph_wrapper.get_bgl_to_id_map();

    for (size_t i = 0; i < bgl_to_id.size(); ++i) {
        const NodeID target_id = bgl_to_id[i];
        if (dist[i] == unreachable) {
            continue;
        }

        std::vector<NodeID> path;
        VertexDesc current = graph_wrapper.get_id_to_bgl_map().at(target_id);
        while (true) {
            path.push_back(graph_wrapper.get_node_id(current));
            const auto current_index = graph_wrapper.get_vertex_index(current);
            if (pred[current_index] == current) {
                break;
            }
            current = pred[current_index];
        }
        std::reverse(path.begin(), path.end());
        paths[target_id] = std::move(path);
    }

    return paths;
}

template <typename Distance>
Distance normalize_weighted_distance(Distance value) {
    if constexpr (std::is_integral_v<Distance> && std::is_signed_v<Distance>) {
        if (value == std::numeric_limits<Distance>::lowest()) {
            return std::numeric_limits<Distance>::max();
        }
    }
    return value;
}

template <typename Distance>
using shortest_path_calc_type = std::conditional_t<std::is_integral_v<Distance>, double, Distance>;

template <typename Distance, typename CalcDistance>
Distance convert_shortest_path_distance(CalcDistance value) {
    if constexpr (std::is_same_v<CalcDistance, Distance>) {
        return normalize_weighted_distance(value);
    } else {
        if constexpr (std::is_integral_v<Distance> && std::is_floating_point_v<CalcDistance>) {
            if (!std::isfinite(value) || value >= static_cast<CalcDistance>(std::numeric_limits<Distance>::max())) {
                return std::numeric_limits<Distance>::max();
            }
            if (value <= static_cast<CalcDistance>(std::numeric_limits<Distance>::lowest())) {
                return std::numeric_limits<Distance>::lowest();
            }
        }
        if (value == std::numeric_limits<CalcDistance>::max()) {
            return std::numeric_limits<Distance>::max();
        }
        return static_cast<Distance>(value);
    }
}

// Algorithms: Shortest Paths

template <typename GraphWrapper>
[[deprecated("Use G.shortest_path(source, target) instead.")]]
auto shortest_path(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id) {
    return G.shortest_path(source_id, target_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.dijkstra_path(source, target) instead.")]]
auto dijkstra_path(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id) {
    return G.dijkstra_path(source_id, target_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.dijkstra_shortest_paths(source) instead.")]]
auto dijkstra_shortest_paths(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id) {
    return G.dijkstra_shortest_paths(source_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.dijkstra_shortest_paths(source) instead.")]]
auto single_source_dijkstra(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id) {
    return G.dijkstra_shortest_paths(source_id);
}

template <typename GraphWrapper>
[[deprecated("Use G.shortest_path(source, target, weight) instead.")]]
auto shortest_path(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& weight) {
    return G.shortest_path(source_id, target_id, weight);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.dijkstra_path(source, target, weight) instead.")]]
auto dijkstra_path(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& weight) {
    return G.dijkstra_path(source_id, target_id, weight);
}

template <typename GraphWrapper>
[[deprecated("Use G.shortest_path_length(source, target) instead.")]]
double shortest_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id) {
    return G.shortest_path_length(source_id, target_id);
}

template <typename GraphWrapper>
[[deprecated("Use G.shortest_path_length(source, target, weight) instead.")]]
double shortest_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& weight) {
    return G.shortest_path_length(source_id, target_id, weight);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.dijkstra_path_length(source) instead.")]]
auto dijkstra_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id) {
    return G.dijkstra_path_length(source_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.dijkstra_path_length(source, target) instead.")]]
auto dijkstra_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id) {
    return G.dijkstra_path_length(source_id, target_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.dijkstra_path_length(source, target, weight) instead.")]]
auto dijkstra_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& weight) {
    return G.dijkstra_path_length(source_id, target_id, weight);
}


template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::shortest_path(const NodeID& source_id, const NodeID& target_id) const {
    if (id_to_bgl.find(source_id) == id_to_bgl.end() || id_to_bgl.find(target_id) == id_to_bgl.end()) {
        throw std::runtime_error("Source or target node not found in graph.");
    }

    const VertexDesc source = id_to_bgl.at(source_id);
    const VertexDesc target = id_to_bgl.at(target_id);

    if (source == target) {
        return std::vector<NodeID>{source_id};
    }

    const size_t n = boost::num_vertices(g);
    const auto source_index = get_vertex_index(source);
    const auto target_index = get_vertex_index(target);
    std::vector<bool> visited(n, false);
    std::vector<VertexDesc> pred(n);
    std::queue<VertexDesc> q;

    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
        pred[get_vertex_index(*v)] = *v;
    }

    visited[source_index] = true;
    q.push(source);

    while (!q.empty()) {
        const VertexDesc current = q.front();
        q.pop();

        for (auto [e, eend] = boost::out_edges(current, g); e != eend; ++e) {
            const VertexDesc next = boost::target(*e, g);
            const auto next_index = get_vertex_index(next);
            if (!visited[next_index]) {
                visited[next_index] = true;
                pred[next_index] = current;
                if (next == target) {
                    q = {};
                    break;
                }
                q.push(next);
            }
        }
    }

    if (!visited[target_index]) {
        throw std::runtime_error("Node not reachable");
    }

    std::vector<NodeID> path;
    for (VertexDesc curr = target; curr != source; curr = pred[get_vertex_index(curr)]) {
        path.push_back(get_node_id(curr));
    }
    path.push_back(get_node_id(source));
    std::reverse(path.begin(), path.end());
    return path;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::shortest_path(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const {
    if (weight.empty()) {
        return shortest_path(source_id, target_id);
    }
    if (weight == "weight") {
        if constexpr (Weighted) {
            return dijkstra_path(source_id, target_id);
        } else {
            throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
        }
    }
    throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
double Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::shortest_path_length(const NodeID& source_id, const NodeID& target_id) const {
    const auto path = shortest_path(source_id, target_id);
    if (path.empty()) {
        throw std::runtime_error("Node not reachable");
    }
    return static_cast<double>(path.size() - 1);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
double Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::shortest_path_length(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const {
    if (weight.empty()) {
        return shortest_path_length(source_id, target_id);
    }
    if (weight == "weight") {
        if constexpr (Weighted) {
            return dijkstra_path_length(source_id, target_id);
        } else {
            throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
        }
    }
    throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dijkstra_path(const NodeID& source_id, const NodeID& target_id) const {
    if (id_to_bgl.find(source_id) == id_to_bgl.end() || id_to_bgl.find(target_id) == id_to_bgl.end()) {
        throw std::runtime_error("Source or target node not found in graph.");
    }

    const VertexDesc source = id_to_bgl.at(source_id);
    const VertexDesc target = id_to_bgl.at(target_id);
    const size_t n = boost::num_vertices(g);
    const auto target_index = get_vertex_index(target);
    std::vector<EdgeWeight> dist(n);
    std::vector<VertexDesc> pred(n);

    boost::dijkstra_shortest_paths(
        g,
        source,
        boost::distance_map(boost::make_iterator_property_map(dist.begin(), vertex_index_map))
            .predecessor_map(boost::make_iterator_property_map(pred.begin(), vertex_index_map))
            .vertex_index_map(vertex_index_map)
    );

    if (pred[target_index] == target && source != target) {
        throw std::runtime_error("Node not reachable");
    }

    std::vector<NodeID> path;
    for (VertexDesc curr = target; curr != source; curr = pred[get_vertex_index(curr)]) {
        path.push_back(get_node_id(curr));
    }
    path.push_back(get_node_id(source));
    std::reverse(path.begin(), path.end());
    return path;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dijkstra_path(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const {
    if (weight.empty() || weight == "weight") {
        return dijkstra_path(source_id, target_id);
    }
    throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dijkstra_shortest_paths(const NodeID& source_id) const {
    if (id_to_bgl.find(source_id) == id_to_bgl.end()) {
        throw std::runtime_error("Source node not found in graph.");
    }

    const VertexDesc source = id_to_bgl.at(source_id);
    const size_t n = boost::num_vertices(g);
    std::vector<EdgeWeight> dist(n, std::numeric_limits<EdgeWeight>::max());
    std::vector<VertexDesc> pred(n);
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
        pred[get_vertex_index(*v)] = *v;
    }

    boost::dijkstra_shortest_paths(
        g,
        source,
        boost::distance_map(boost::make_iterator_property_map(dist.begin(), vertex_index_map))
            .predecessor_map(boost::make_iterator_property_map(pred.begin(), vertex_index_map))
            .vertex_index_map(vertex_index_map)
    );

    SingleSourceShortestPathResult<NodeID, EdgeWeight> result;
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
        const auto index = get_vertex_index(*v);
        const auto& id = get_node_id(*v);
        result.distance[id] = dist[index];
        result.predecessor[id] = get_node_id(pred[index]);
    }
    result.paths = build_single_source_paths(*this, dist, pred);
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dijkstra_path_length(const NodeID& source_id) const {
    return dijkstra_shortest_paths(source_id).distance;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dijkstra_path_length(const NodeID& source_id, const NodeID& target_id) const {
    const auto distances = dijkstra_path_length(source_id);
    const auto it = distances.find(target_id);
    if (it == distances.end()) {
        throw std::runtime_error("Target node not found in graph.");
    }
    return it->second;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dijkstra_path_length(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const {
    if (weight.empty() || weight == "weight") {
        return dijkstra_path_length(source_id, target_id);
    }
    throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.bellman_ford_path(source, target) instead.")]]
auto bellman_ford_path(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id) {
    return G.bellman_ford_path(source_id, target_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.bellman_ford_shortest_paths(source) instead.")]]
auto bellman_ford_shortest_paths(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id) {
    return G.bellman_ford_shortest_paths(source_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.bellman_ford_shortest_paths(source) instead.")]]
auto single_source_bellman_ford(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id) {
    return G.bellman_ford_shortest_paths(source_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.bellman_ford_path(source, target, weight) instead.")]]
auto bellman_ford_path(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& weight) {
    return G.bellman_ford_path(source_id, target_id, weight);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.bellman_ford_path_length(source, target) instead.")]]
auto bellman_ford_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id) {
    return G.bellman_ford_path_length(source_id, target_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.bellman_ford_path_length(source, target, weight) instead.")]]
auto bellman_ford_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& weight) {
    return G.bellman_ford_path_length(source_id, target_id, weight);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.dag_shortest_paths(source) instead.")]]
auto dag_shortest_paths(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id) {
    return G.dag_shortest_paths(source_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.floyd_warshall_all_pairs_shortest_paths() instead.")]]
auto floyd_warshall_all_pairs_shortest_paths(const GraphWrapper& G) {
    return G.floyd_warshall_all_pairs_shortest_paths();
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.floyd_warshall_all_pairs_shortest_paths_map() instead.")]]
auto floyd_warshall_all_pairs_shortest_paths_map(const GraphWrapper& G) {
    return G.floyd_warshall_all_pairs_shortest_paths_map();
}


template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::bellman_ford_path(const NodeID& source_id, const NodeID& target_id) const {
    if (id_to_bgl.find(source_id) == id_to_bgl.end() || id_to_bgl.find(target_id) == id_to_bgl.end()) {
        throw std::runtime_error("Source or target node not found in graph.");
    }

    const VertexDesc source = id_to_bgl.at(source_id);
    const VertexDesc target = id_to_bgl.at(target_id);
    const size_t n = boost::num_vertices(g);
    const auto source_index = get_vertex_index(source);
    const auto target_index = get_vertex_index(target);
    std::vector<EdgeWeight> dist(n, std::numeric_limits<EdgeWeight>::max());
    std::vector<VertexDesc> pred(n);
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) pred[get_vertex_index(*v)] = *v;
    dist[source_index] = EdgeWeight{};

    const bool ok = boost::bellman_ford_shortest_paths(
        g,
        static_cast<int>(n),
        boost::weight_map(boost::get(boost::edge_weight, g))
            .distance_map(boost::make_iterator_property_map(dist.begin(), vertex_index_map))
            .predecessor_map(boost::make_iterator_property_map(pred.begin(), vertex_index_map))
            .root_vertex(source)
    );
    if (!ok) throw std::runtime_error("Bellman-Ford failed: negative cycle detected.");
    if (dist[target_index] == std::numeric_limits<EdgeWeight>::max()) throw std::runtime_error("Node not reachable");

    std::vector<NodeID> path;
    for (VertexDesc curr = target; curr != source; curr = pred[get_vertex_index(curr)]) path.push_back(get_node_id(curr));
    path.push_back(get_node_id(source));
    std::reverse(path.begin(), path.end());
    return path;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::bellman_ford_shortest_paths(const NodeID& source_id) const {
    if (id_to_bgl.find(source_id) == id_to_bgl.end()) {
        throw std::runtime_error("Source node not found in graph.");
    }

    const VertexDesc source = id_to_bgl.at(source_id);
    const size_t n = boost::num_vertices(g);
    const auto source_index = get_vertex_index(source);
    std::vector<EdgeWeight> dist(n, std::numeric_limits<EdgeWeight>::max());
    std::vector<VertexDesc> pred(n);
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) pred[get_vertex_index(*v)] = *v;
    dist[source_index] = EdgeWeight{};

    const bool ok = boost::bellman_ford_shortest_paths(
        g,
        static_cast<int>(n),
        boost::weight_map(boost::get(boost::edge_weight, g))
            .distance_map(boost::make_iterator_property_map(dist.begin(), vertex_index_map))
            .predecessor_map(boost::make_iterator_property_map(pred.begin(), vertex_index_map))
            .root_vertex(source)
    );
    if (!ok) throw std::runtime_error("Bellman-Ford failed: negative cycle detected.");

    SingleSourceShortestPathResult<NodeID, EdgeWeight> result;
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
        const auto index = get_vertex_index(*v);
        const auto& id = get_node_id(*v);
        result.distance[id] = dist[index];
        result.predecessor[id] = get_node_id(pred[index]);
    }
    result.paths = build_single_source_paths(*this, dist, pred);
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::bellman_ford_path(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const {
    if (weight.empty() || weight == "weight") return bellman_ford_path(source_id, target_id);
    throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::bellman_ford_path_length(const NodeID& source_id, const NodeID& target_id) const {
    EdgeWeight total{};
    const auto path = bellman_ford_path(source_id, target_id);
    for (size_t i = 0; i + 1 < path.size(); ++i) total += get_edge_weight(path[i], path[i + 1]);
    return total;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::bellman_ford_path_length(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const {
    if (weight.empty() || weight == "weight") return bellman_ford_path_length(source_id, target_id);
    throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dag_shortest_paths(const NodeID& source_id) const {
    using CalcDistance = shortest_path_calc_type<EdgeWeight>;
    if (id_to_bgl.find(source_id) == id_to_bgl.end()) throw std::runtime_error("Source node not found in graph.");

    const VertexDesc source = id_to_bgl.at(source_id);
    const size_t n = boost::num_vertices(g);
    std::vector<CalcDistance> dist(n, std::numeric_limits<CalcDistance>::max());
    std::vector<VertexDesc> pred(n);
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) pred[get_vertex_index(*v)] = *v;

    boost::dag_shortest_paths(
        g, source,
        boost::distance_map(boost::make_iterator_property_map(dist.begin(), vertex_index_map))
            .predecessor_map(boost::make_iterator_property_map(pred.begin(), vertex_index_map))
            .distance_compare(std::less<CalcDistance>())
            .distance_combine(boost::closed_plus<CalcDistance>(std::numeric_limits<CalcDistance>::max()))
            .distance_inf(std::numeric_limits<CalcDistance>::max())
            .distance_zero(CalcDistance{})
    );

    std::vector<EdgeWeight> normalized_dist(n);
    for (size_t i = 0; i < n; ++i) normalized_dist[i] = convert_shortest_path_distance<EdgeWeight>(dist[i]);

    SingleSourceShortestPathResult<NodeID, EdgeWeight> result;
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
        const auto index = get_vertex_index(*v);
        const auto& id = get_node_id(*v);
        result.distance[id] = normalized_dist[index];
        result.predecessor[id] = get_node_id(pred[index]);
    }
    result.paths = build_single_source_paths(*this, normalized_dist, pred);
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::floyd_warshall_all_pairs_shortest_paths() const {
    using CalcDistance = std::conditional_t<std::is_integral_v<EdgeWeight>, long long, EdgeWeight>;
    const size_t n = boost::num_vertices(g);
    const CalcDistance inf = std::numeric_limits<CalcDistance>::max() / 4;
    std::vector<std::vector<CalcDistance>> internal_matrix(n, std::vector<CalcDistance>(n, inf));
    for (size_t i = 0; i < n; ++i) internal_matrix[i][i] = CalcDistance{};

    for (auto [e, eend] = boost::edges(g); e != eend; ++e) {
        const size_t u = get_vertex_index(boost::source(*e, g));
        const size_t v = get_vertex_index(boost::target(*e, g));
        const CalcDistance w = static_cast<CalcDistance>(boost::get(boost::edge_weight, g, *e));
        internal_matrix[u][v] = std::min(internal_matrix[u][v], w);
        if constexpr (!Directed) internal_matrix[v][u] = std::min(internal_matrix[v][u], w);
    }

    for (size_t k = 0; k < n; ++k) for (size_t i = 0; i < n; ++i) if (internal_matrix[i][k] != inf)
        for (size_t j = 0; j < n; ++j) if (internal_matrix[k][j] != inf) {
            const CalcDistance through_k = internal_matrix[i][k] + internal_matrix[k][j];
            if (through_k < internal_matrix[i][j]) internal_matrix[i][j] = through_k;
        }

    std::vector<size_t> order(n);
    for (size_t i = 0; i < n; ++i) order[i] = i;
    if constexpr (std::is_arithmetic_v<NodeID> || std::is_same_v<NodeID, std::string>) {
        std::sort(order.begin(), order.end(), [&](size_t lhs, size_t rhs) { return bgl_to_id[lhs] < bgl_to_id[rhs]; });
    }

    std::vector<std::vector<EdgeWeight>> matrix(n, std::vector<EdgeWeight>(n));
    for (size_t i = 0; i < n; ++i) for (size_t j = 0; j < n; ++j)
        matrix[i][j] = internal_matrix[order[i]][order[j]] == inf ? std::numeric_limits<EdgeWeight>::max() : static_cast<EdgeWeight>(internal_matrix[order[i]][order[j]]);
    return matrix;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::floyd_warshall_all_pairs_shortest_paths_map() const {
    const size_t n = boost::num_vertices(g);
    std::vector<size_t> order(n);
    for (size_t i = 0; i < n; ++i) order[i] = i;
    if constexpr (std::is_arithmetic_v<NodeID> || std::is_same_v<NodeID, std::string>) {
        std::sort(order.begin(), order.end(), [&](size_t lhs, size_t rhs) { return bgl_to_id[lhs] < bgl_to_id[rhs]; });
    }
    const auto matrix = floyd_warshall_all_pairs_shortest_paths();
    std::unordered_map<NodeID, std::unordered_map<NodeID, EdgeWeight>> result;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            result[bgl_to_id[order[i]]][bgl_to_id[order[j]]] = matrix[i][j];
        }
    }
    return result;
}
} // namespace nxpp
