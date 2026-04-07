#pragma once

#include "graph.hpp"

namespace nxpp {

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::degree_centrality() const {
    const auto node_count = boost::num_vertices(g);
    std::vector<double> centrality_by_index(static_cast<std::size_t>(node_count), 0.0);
    if (node_count == 0) {
        return indexed_lookup_map<NodeID, double>{};
    }
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
        centrality_by_index[get_vertex_index(*v)] = 0.0;
    }
    if (node_count <= 1) {
        return build_node_indexed_result<double>([&](VertexDesc v) {
            return centrality_by_index[get_vertex_index(v)];
        });
    }
    const double scale = 1.0 / static_cast<double>(node_count - 1);
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
        double degree = 0.0;
        if constexpr (Directed) {
            degree = static_cast<double>(boost::in_degree(*v, g) + boost::out_degree(*v, g));
        } else {
            degree = static_cast<double>(boost::degree(*v, g));
        }
        centrality_by_index[get_vertex_index(*v)] = degree * scale;
    }
    return build_node_indexed_result<double>([&](VertexDesc v) {
        return centrality_by_index[get_vertex_index(v)];
    });
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::pagerank() const {
    const auto node_count = boost::num_vertices(g);
    if (node_count == 0) {
        return indexed_lookup_map<NodeID, double>{};
    }

    const double n = static_cast<double>(node_count);
    const double damping = 0.85;
    std::vector<double> rank(static_cast<std::size_t>(node_count), 1.0 / n);
    std::vector<double> next(static_cast<std::size_t>(node_count), 0.0);

    for (int iteration = 0; iteration < 20; ++iteration) {
        const double base = (1.0 - damping) / n;
        std::fill(next.begin(), next.end(), base);

        double dangling_sum = 0.0;
        for (auto [u, uend] = boost::vertices(g); u != uend; ++u) {
            const auto out = boost::out_degree(*u, g);
            const auto index = get_vertex_index(*u);
            if (out == 0) {
                dangling_sum += rank[index];
                continue;
            }

            const double share = damping * rank[index] / static_cast<double>(out);
            for (auto [adj, adj_end] = boost::adjacent_vertices(*u, g); adj != adj_end; ++adj) {
                next[get_vertex_index(*adj)] += share;
            }
        }

        if (dangling_sum > 0.0) {
            const double dangling_share = damping * dangling_sum / n;
            for (double& value : next) {
                value += dangling_share;
            }
        }

        rank.swap(next);
    }

    return build_node_indexed_result<double>([&](VertexDesc v) {
        return rank[get_vertex_index(v)];
    });
}

template <typename GraphWrapper>
/// @brief Deprecated free-function alias for pagerank().
[[deprecated("Use G.pagerank() instead.")]]
auto pagerank(const GraphWrapper& G) {
    return G.pagerank();
}

template <typename GraphWrapper>
/// @brief Deprecated free-function alias for degree_centrality().
[[deprecated("Use G.degree_centrality() instead.")]]
auto degree_centrality(const GraphWrapper& G) {
    return G.degree_centrality();
}

} // namespace nxpp
