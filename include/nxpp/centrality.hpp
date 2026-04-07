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

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::betweenness_centrality() const {
    const auto node_count = boost::num_vertices(g);
    if (node_count == 0) {
        return indexed_lookup_map<NodeID, double>{};
    }

    const std::size_t n = static_cast<std::size_t>(node_count);
    std::vector<double> bc(n, 0.0);

    // Brandes algorithm: for each source, run BFS to count shortest paths,
    // then back-propagate dependency scores to accumulate betweenness.
    for (auto [s_it, s_end] = boost::vertices(g); s_it != s_end; ++s_it) {
        const VertexDesc s = *s_it;
        const std::size_t s_idx = get_vertex_index(s);

        // Vertices in order of non-increasing distance from s (for back-propagation).
        std::vector<std::size_t> stack;
        stack.reserve(n);

        // Predecessors on shortest paths from s, indexed by vertex index.
        std::vector<std::vector<std::size_t>> pred(n);

        // Number of shortest paths from s to each vertex.
        std::vector<double> sigma(n, 0.0);
        sigma[s_idx] = 1.0;

        // BFS distance from s (-1 means unvisited).
        std::vector<int> dist(n, -1);
        dist[s_idx] = 0;

        std::queue<VertexDesc> bfs_queue;
        bfs_queue.push(s);

        while (!bfs_queue.empty()) {
            const VertexDesc v = bfs_queue.front();
            bfs_queue.pop();
            const std::size_t v_idx = get_vertex_index(v);
            stack.push_back(v_idx);

            for (auto [w_it, w_end] = boost::adjacent_vertices(v, g); w_it != w_end; ++w_it) {
                const std::size_t w_idx = get_vertex_index(*w_it);

                if (dist[w_idx] < 0) {
                    bfs_queue.push(*w_it);
                    dist[w_idx] = dist[v_idx] + 1;
                }
                if (dist[w_idx] == dist[v_idx] + 1) {
                    sigma[w_idx] += sigma[v_idx];
                    pred[w_idx].push_back(v_idx);
                }
            }
        }

        // Back-propagate dependency in reverse BFS order.
        std::vector<double> delta(n, 0.0);
        while (!stack.empty()) {
            const std::size_t w_idx = stack.back();
            stack.pop_back();
            for (const std::size_t v_idx : pred[w_idx]) {
                if (sigma[w_idx] > 0.0) {
                    delta[v_idx] += (sigma[v_idx] / sigma[w_idx]) * (1.0 + delta[w_idx]);
                }
            }
            if (w_idx != s_idx) {
                bc[w_idx] += delta[w_idx];
            }
        }
    }

    // Normalize by the number of ordered (directed) or unordered (undirected) pairs,
    // matching NetworkX betweenness_centrality(G, normalized=True) semantics.
    if (node_count > 2) {
        const double denom = static_cast<double>(node_count - 1) * static_cast<double>(node_count - 2);
        const double scale = Directed ? 1.0 / denom : 2.0 / denom;
        for (double& v : bc) v *= scale;
    }

    return build_node_indexed_result<double>([&](VertexDesc v) {
        return bc[get_vertex_index(v)];
    });
}

template <typename GraphWrapper>
/// @brief Deprecated free-function alias for betweenness_centrality().
[[deprecated("Use G.betweenness_centrality() instead.")]]
auto betweenness_centrality(const GraphWrapper& G) {
    return G.betweenness_centrality();
}

} // namespace nxpp
