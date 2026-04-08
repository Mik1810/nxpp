#pragma once

/**
 * @file generators.hpp
 * @brief Graph-construction helpers such as complete, path, and Erdos-Renyi graphs.
 *
 * @defgroup nxpp_generators Generators
 * @brief Graph-construction helpers and random graph generators.
 */

#include "graph.hpp"

namespace nxpp {

// Generators

template <typename GraphType = Graph<int>>
/**
 * @brief Builds the complete graph on node IDs 0 through n - 1.
 *
 * @param n Number of nodes to generate.
 * @return A graph where every distinct node pair is connected.
 */
GraphType complete_graph(size_t n) {
    GraphType G;
    using NodeID = typename GraphType::NodeType;
    static_assert(
        std::constructible_from<NodeID, std::size_t>,
        "nxpp::complete_graph requires GraphType::NodeType to be constructible from std::size_t because it synthesizes node IDs 0..n-1."
    );
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
/**
 * @brief Builds a simple path on node IDs 0 through n - 1.
 *
 * @param n Number of nodes to generate.
 * @return A graph containing the path 0-1-...-(n-1).
 */
GraphType path_graph(size_t n) {
    GraphType G;
    using NodeID = typename GraphType::NodeType;
    static_assert(
        std::constructible_from<NodeID, std::size_t>,
        "nxpp::path_graph requires GraphType::NodeType to be constructible from std::size_t because it synthesizes node IDs 0..n-1."
    );
    for (size_t i = 0; i < n - 1; ++i) {
        G.add_edge(static_cast<NodeID>(i), static_cast<NodeID>(i + 1));
    }
    return G;
}

template <typename GraphType = Graph<int>>
/**
 * @brief Builds an Erdos-Renyi random graph on node IDs 0 through n - 1.
 *
 * @param n Number of nodes to generate.
 * @param p Probability of adding each candidate edge.
 * @param seed Random seed used by the internal generator.
 * @return A randomly generated graph of the requested type.
 */
GraphType erdos_renyi_graph(size_t n, double p, int seed = 42) {
    GraphType G;
    using NodeID = typename GraphType::NodeType;
    static_assert(
        std::constructible_from<NodeID, std::size_t>,
        "nxpp::erdos_renyi_graph requires GraphType::NodeType to be constructible from std::size_t because it synthesizes node IDs 0..n-1."
    );
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
