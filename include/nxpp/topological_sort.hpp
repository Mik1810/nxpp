#pragma once

#include <boost/graph/topological_sort.hpp>

#include "graph.hpp"

namespace nxpp {

template <typename GraphWrapper>
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
