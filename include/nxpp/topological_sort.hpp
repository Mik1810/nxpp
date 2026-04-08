#pragma once

/**
 * @file topological_sort.hpp
 * @brief Thin topological-sort compatibility aliases for the graph wrapper.
 *
 * @ingroup nxpp_spanning
 */

#include <boost/graph/topological_sort.hpp>

#include "graph.hpp"

namespace nxpp {

template <typename GraphWrapper>
/**
 * @brief Deprecated free-function alias for `G.topological_sort()`.
 *
 * @param G Graph wrapper on which to compute a topological ordering.
 * @return The same node-order vector returned by `G.topological_sort()`.
 */
[[deprecated("Use G.topological_sort() instead.")]]
auto topological_sort(const GraphWrapper& G) {
    return G.topological_sort();
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::topological_sort() const {
    std::vector<VertexDesc> rev_order;
    std::vector<boost::default_color_type> color(boost::num_vertices(g));
    boost::topological_sort(
        g,
        std::back_inserter(rev_order),
        boost::color_map(boost::make_iterator_property_map(color.begin(), vertex_index_map))
    );
    std::vector<NodeID> order;
    order.reserve(rev_order.size());
    for (auto it = rev_order.rbegin(); it != rev_order.rend(); ++it) {
        order.push_back(get_node_id(*it));
    }
    return order;
}

} // namespace nxpp
