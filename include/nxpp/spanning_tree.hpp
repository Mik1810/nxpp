#pragma once

/**
 * @file spanning_tree.hpp
 * @brief Minimum-spanning-tree helpers, topological sort, and related aliases.
 */

#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>

#include "graph.hpp"

namespace nxpp {

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
/**
 * @brief Deprecated free-function alias for `G.kruskal_minimum_spanning_tree()`.
 *
 * @param G Graph wrapper on which to compute the Kruskal MST edge list.
 * @return The same MST edge list returned by
 * `G.kruskal_minimum_spanning_tree()`.
 */
[[deprecated("Use G.kruskal_minimum_spanning_tree() instead.")]]
auto kruskal_minimum_spanning_tree(const GraphWrapper& G) {
    return G.kruskal_minimum_spanning_tree();
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
/**
 * @brief Deprecated free-function alias for `G.prim_minimum_spanning_tree(root)`.
 *
 * @param G Graph wrapper on which to compute the rooted Prim MST.
 * @param root_id Root node ID used for the Prim traversal.
 * @return The same node-to-parent MST map returned by
 * `G.prim_minimum_spanning_tree(root_id)`.
 */
[[deprecated("Use G.prim_minimum_spanning_tree(root) instead.")]]
auto prim_minimum_spanning_tree(const GraphWrapper& G, const typename GraphWrapper::NodeType& root_id) {
    return G.prim_minimum_spanning_tree(root_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
/**
 * @brief Deprecated free-function alias for `G.minimum_spanning_tree()`.
 *
 * @param G Graph wrapper on which to compute the default MST result.
 * @return The same default MST result returned by `G.minimum_spanning_tree()`.
 */
[[deprecated("Use G.minimum_spanning_tree() instead.")]]
auto minimum_spanning_tree(const GraphWrapper& G) {
    return G.minimum_spanning_tree();
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
/**
 * @brief Deprecated free-function alias for `G.minimum_spanning_tree(root)`.
 *
 * @param G Graph wrapper on which to compute the rooted MST result.
 * @param root_id Root node ID used by the rooted MST wrapper.
 * @return The same rooted MST result returned by
 * `G.minimum_spanning_tree(root_id)`.
 */
[[deprecated("Use G.minimum_spanning_tree(root) instead.")]]
auto minimum_spanning_tree(const GraphWrapper& G, const typename GraphWrapper::NodeType& root_id) {
    return G.minimum_spanning_tree(root_id);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::kruskal_minimum_spanning_tree() const {
    std::vector<EdgeDesc> mst_edges;
    boost::kruskal_minimum_spanning_tree(g, std::back_inserter(mst_edges));
    std::vector<std::pair<NodeID, NodeID>> result;
    result.reserve(mst_edges.size());
    for (const auto& e : mst_edges) result.emplace_back(get_node_id(boost::source(e, g)), get_node_id(boost::target(e, g)));
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::prim_minimum_spanning_tree(const NodeID& root_id) const {
    if (id_to_bgl.find(root_id) == id_to_bgl.end()) throw std::runtime_error("Minimum spanning tree failed: root node not found.");
    std::vector<VertexDesc> parent(boost::num_vertices(g));
    std::vector<boost::default_color_type> color(boost::num_vertices(g));
    boost::prim_minimum_spanning_tree(
        g,
        boost::make_iterator_property_map(parent.begin(), vertex_index_map),
        boost::root_vertex(id_to_bgl.at(root_id))
            .vertex_index_map(vertex_index_map)
            .color_map(boost::make_iterator_property_map(color.begin(), vertex_index_map))
    );
    std::map<NodeID, NodeID> result;
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
        const auto index = get_vertex_index(*v);
        result[get_node_id(*v)] = get_node_id(parent[index]);
    }
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::minimum_spanning_tree() const { return kruskal_minimum_spanning_tree(); }

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::minimum_spanning_tree(const NodeID& root_id) const { return prim_minimum_spanning_tree(root_id); }

} // namespace nxpp
