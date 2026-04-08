#pragma once

/**
 * @file components.hpp
 * @brief Connected-component and strong-component helpers plus compatibility aliases.
 *
 * @defgroup nxpp_components Components
 * @brief Connected-component and strong-component wrappers and aliases.
 */

#include <boost/graph/connected_components.hpp>
#include <boost/graph/strong_components.hpp>

#include "graph.hpp"

namespace nxpp {

// Algorithms: Components

template <typename GraphWrapper>
/**
 * @brief Deprecated free-function alias for `G.connected_component_groups()`.
 *
 * @param G Graph wrapper on which to compute connected-component groups.
 * @return The same grouped connected-component result returned by
 * `G.connected_component_groups()`.
 */
[[deprecated("Use G.connected_component_groups() instead.")]]
auto connected_component_groups(const GraphWrapper& G) {
    return G.connected_component_groups();
}

template <typename GraphWrapper>
/**
 * @brief Deprecated free-function alias for `G.connected_components()`.
 *
 * @param G Graph wrapper on which to compute the component map.
 * @return The same node-to-component map returned by `G.connected_components()`.
 */
[[deprecated("Use G.connected_components() instead.")]]
auto connected_components(const GraphWrapper& G) {
    return G.connected_components();
}

template <typename GraphWrapper>
/**
 * @brief Deprecated free-function alias for `G.strongly_connected_component_groups()`.
 *
 * @param G Graph wrapper on which to compute grouped SCC output.
 * @return The same grouped SCC result returned by
 * `G.strongly_connected_component_groups()`.
 */
[[deprecated("Use G.strongly_connected_component_groups() instead.")]]
auto strongly_connected_component_groups(const GraphWrapper& G) {
    return G.strongly_connected_component_groups();
}

template <typename GraphWrapper>
/**
 * @brief Deprecated free-function alias for `G.strong_component_map()`.
 *
 * @param G Graph wrapper on which to compute the SCC index map.
 * @return The same node-to-strong-component map returned by
 * `G.strong_component_map()`.
 */
[[deprecated("Use G.strong_component_map() instead.")]]
auto strong_component_map(const GraphWrapper& G) {
    return G.strong_component_map();
}

template <typename GraphWrapper>
/**
 * @brief Deprecated free-function alias for `G.strong_components()`.
 *
 * @param G Graph wrapper on which to compute strong-component roots.
 * @return The same node-to-root mapping returned by `G.strong_components()`.
 */
[[deprecated("Use G.strong_components() instead.")]]
auto strong_components(const GraphWrapper& G) {
    return G.strong_components();
}

template <typename GraphWrapper>
/**
 * @brief Deprecated free-function alias for `G.connected_component_map()`.
 *
 * @param G Graph wrapper on which to compute the connected-component map.
 * @return The same node-to-component map returned by
 * `G.connected_component_map()`.
 */
[[deprecated("Use G.connected_component_map() instead.")]]
auto connected_component_map(const GraphWrapper& G) {
    return G.connected_component_map();
}

template <typename GraphWrapper>
/**
 * @brief Deprecated free-function alias for `G.strongly_connected_components()`.
 *
 * @param G Graph wrapper on which to compute grouped SCC output.
 * @return The same grouped SCC result returned by
 * `G.strongly_connected_components()`.
 */
[[deprecated("Use G.strongly_connected_components() instead.")]]
auto strongly_connected_components(const GraphWrapper& G) {
    return G.strongly_connected_components();
}

template <typename GraphWrapper>
/**
 * @brief Deprecated free-function alias for `G.strongly_connected_component_map()`.
 *
 * @param G Graph wrapper on which to compute the SCC index map.
 * @return The same node-to-component map returned by
 * `G.strongly_connected_component_map()`.
 */
[[deprecated("Use G.strongly_connected_component_map() instead.")]]
auto strongly_connected_component_map(const GraphWrapper& G) {
    return G.strongly_connected_component_map();
}

template <typename GraphWrapper>
/**
 * @brief Deprecated free-function alias for `G.strongly_connected_component_roots()`.
 *
 * @param G Graph wrapper on which to compute SCC representative roots.
 * @return The same node-to-root mapping returned by
 * `G.strongly_connected_component_roots()`.
 */
[[deprecated("Use G.strongly_connected_component_roots() instead.")]]
auto strongly_connected_component_roots(const GraphWrapper& G) {
    return G.strongly_connected_component_roots();
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::connected_component_groups() const {
    const int n = static_cast<int>(boost::num_vertices(g));
    std::vector<int> comp(n);
    std::vector<boost::default_color_type> color(n);
    const int num = boost::connected_components(
        g,
        boost::make_iterator_property_map(comp.begin(), vertex_index_map),
        boost::color_map(boost::make_iterator_property_map(color.begin(), vertex_index_map))
            .vertex_index_map(vertex_index_map)
    );
    std::vector<std::vector<NodeID>> components(num);
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
        const auto index = static_cast<int>(get_vertex_index(*v));
        components[comp[index]].push_back(get_node_id(*v));
    }
    return components;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::connected_components() const {
    const int n = static_cast<int>(boost::num_vertices(g));
    std::vector<int> comp(n);
    std::vector<boost::default_color_type> color(n);
    boost::connected_components(
        g,
        boost::make_iterator_property_map(comp.begin(), vertex_index_map),
        boost::color_map(boost::make_iterator_property_map(color.begin(), vertex_index_map))
            .vertex_index_map(vertex_index_map)
    );
    return build_node_indexed_result<int>([&](VertexDesc v) {
        return comp[get_vertex_index(v)];
    });
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::strongly_connected_component_groups() const {
    const int n = static_cast<int>(boost::num_vertices(g));
    std::vector<int> comp(n);
    std::vector<boost::default_color_type> color(n);
    const int num = boost::strong_components(
        g,
        boost::make_iterator_property_map(comp.begin(), vertex_index_map),
        boost::color_map(boost::make_iterator_property_map(color.begin(), vertex_index_map))
            .vertex_index_map(vertex_index_map)
    );
    std::vector<std::vector<NodeID>> components(num);
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
        const auto index = static_cast<int>(get_vertex_index(*v));
        components[comp[index]].push_back(get_node_id(*v));
    }
    return components;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::strong_component_map() const {
    const int n = static_cast<int>(boost::num_vertices(g));
    std::vector<int> comp(n);
    std::vector<boost::default_color_type> color(n);
    boost::strong_components(
        g,
        boost::make_iterator_property_map(comp.begin(), vertex_index_map),
        boost::color_map(boost::make_iterator_property_map(color.begin(), vertex_index_map))
            .vertex_index_map(vertex_index_map)
    );
    return build_node_indexed_result<int>([&](VertexDesc v) {
        return comp[get_vertex_index(v)];
    });
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::strong_components() const {
    const int n = static_cast<int>(boost::num_vertices(g));
    std::vector<int> comp(n);
    std::vector<VertexDesc> roots(n);
    std::vector<boost::default_color_type> color(n);
    boost::strong_components(
        g,
        boost::make_iterator_property_map(comp.begin(), vertex_index_map),
        boost::root_map(boost::make_iterator_property_map(roots.begin(), vertex_index_map))
            .color_map(boost::make_iterator_property_map(color.begin(), vertex_index_map))
            .vertex_index_map(vertex_index_map)
    );
    return build_node_indexed_result<NodeID>([&](VertexDesc v) {
        return get_node_id(roots[get_vertex_index(v)]);
    });
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::connected_component_map() const { return connected_components(); }

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::strongly_connected_components() const { return strongly_connected_component_groups(); }

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::strongly_connected_component_map() const { return strong_component_map(); }

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::strongly_connected_component_roots() const { return strong_components(); }

} // namespace nxpp
