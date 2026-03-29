#pragma once

#include "flow.hpp"

namespace nxpp {

// Generators

template <typename GraphType = Graph<int>>
GraphType complete_graph(size_t n) {
    GraphType G;
    using NodeID = typename GraphType::NodeType;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (i != j) {
                G.add_edge(static_cast<NodeID>(i), static_cast<NodeID>(j));
            }
        }
    }
    return G;
}

template <typename GraphType = Graph<int>>
GraphType path_graph(size_t n) {
    GraphType G;
    using NodeID = typename GraphType::NodeType;
    for (size_t i = 0; i < n - 1; ++i) {
        G.add_edge(static_cast<NodeID>(i), static_cast<NodeID>(i + 1));
    }
    return G;
}

template <typename GraphType = Graph<int>>
GraphType erdos_renyi_graph(size_t n, double p, int seed = 42) {
    GraphType G;
    using NodeID = typename GraphType::NodeType;
    std::mt19937 gen(seed);
    std::uniform_real_distribution<> dis(0.0, 1.0);

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < (GraphType::is_directed ? n : i); ++j) {
            if (i == j) continue;
            if (dis(gen) < p) {
                G.add_edge(static_cast<NodeID>(i), static_cast<NodeID>(j));
            }
        }
    }
    return G;
}

} // namespace nxpp
