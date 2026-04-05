#pragma once

#include "graph.hpp"

namespace nxpp {

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
void Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::add_edge(
    const NodeID& u,
    const NodeID& v,
    EdgeWeight w,
    const EdgeAttrMap& attrs
) {
    add_edge(u, v, w);
    assign_edge_attrs(get_edge_desc(u, v), attrs);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
void Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::add_edge(
    const NodeID& u,
    const NodeID& v,
    const EdgeAttrMap& attrs
) {
    if constexpr (Weighted) {
        add_edge(u, v, static_cast<EdgeWeight>(1.0), attrs);
    } else {
        add_edge(u, v);
        assign_edge_attrs(get_edge_desc(u, v), attrs);
    }
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
void Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::add_edge(
    const NodeID& u,
    const NodeID& v,
    EdgeWeight w,
    const std::pair<std::string, std::any>& attr
) {
    add_edge(u, v, w);
    assign_edge_attr(get_edge_desc(u, v), attr);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
void Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::add_edge(
    const NodeID& u,
    const NodeID& v,
    const std::pair<std::string, std::any>& attr
) {
    if constexpr (Weighted) {
        add_edge(u, v, static_cast<EdgeWeight>(1.0), attr);
    } else {
        add_edge(u, v);
        assign_edge_attr(get_edge_desc(u, v), attr);
    }
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
void Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::add_edge(
    const NodeID& u,
    const NodeID& v,
    EdgeWeight w,
    std::initializer_list<std::pair<std::string, std::any>> attrs
) {
    add_edge(u, v, w);
    assign_edge_attrs(get_edge_desc(u, v), attrs);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
void Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::add_edge(
    const NodeID& u,
    const NodeID& v,
    std::initializer_list<std::pair<std::string, std::any>> attrs
) {
    if constexpr (Weighted) {
        add_edge(u, v, static_cast<EdgeWeight>(1.0), attrs);
    } else {
        add_edge(u, v);
        assign_edge_attrs(get_edge_desc(u, v), attrs);
    }
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
bool Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::has_node_attr(const NodeID& u, const std::string& key) const {
    auto node_it = node_properties.find(u);
    if (node_it == node_properties.end()) {
        return false;
    }
    return node_it->second.find(key) != node_it->second.end();
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
bool Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::has_edge_attr(const NodeID& u, const NodeID& v, const std::string& key) const {
    if (!has_edge(u, v)) {
        return false;
    }
    auto e = get_edge_desc(u, v);
    auto edge_it = edge_properties.find(get_edge_id(e));
    if (edge_it == edge_properties.end()) {
        return false;
    }
    return edge_it->second.find(key) != edge_it->second.end();
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
bool Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::has_edge_attr(std::size_t edge_id, const std::string& key) const {
    auto edge_it = edge_properties.find(edge_id);
    if (edge_it == edge_properties.end()) {
        return false;
    }
    return edge_it->second.find(key) != edge_it->second.end();
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <typename T>
T Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::get_node_attr(const NodeID& u, const std::string& key) const {
    auto node_it = node_properties.find(u);
    if (node_it == node_properties.end()) {
        throw std::runtime_error("Node attribute lookup failed: node has no attributes.");
    }
    auto attr_it = node_it->second.find(key);
    if (attr_it == node_it->second.end()) {
        throw std::runtime_error("Node attribute lookup failed: key not found.");
    }
    try {
        return std::any_cast<T>(attr_it->second);
    } catch (const std::bad_any_cast&) {
        throw std::runtime_error("Node attribute lookup failed: stored type does not match requested type.");
    }
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <typename T>
T Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::get_edge_attr(const NodeID& u, const NodeID& v, const std::string& key) const {
    auto e = get_edge_desc(u, v);
    auto edge_it = edge_properties.find(get_edge_id(e));
    if (edge_it == edge_properties.end()) {
        throw std::runtime_error("Edge attribute lookup failed: edge has no attributes.");
    }
    auto attr_it = edge_it->second.find(key);
    if (attr_it == edge_it->second.end()) {
        throw std::runtime_error("Edge attribute lookup failed: key not found.");
    }
    try {
        return std::any_cast<T>(attr_it->second);
    } catch (const std::bad_any_cast&) {
        throw std::runtime_error("Edge attribute lookup failed: stored type does not match requested type.");
    }
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <typename T>
std::optional<T> Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::try_get_node_attr(const NodeID& u, const std::string& key) const {
    auto node_it = node_properties.find(u);
    if (node_it == node_properties.end()) {
        return std::nullopt;
    }
    auto attr_it = node_it->second.find(key);
    if (attr_it == node_it->second.end()) {
        return std::nullopt;
    }
    if (const auto* value = std::any_cast<T>(&(attr_it->second))) {
        return *value;
    }
    return std::nullopt;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <typename T>
std::optional<T> Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::try_get_edge_attr(const NodeID& u, const NodeID& v, const std::string& key) const {
    if (!has_edge(u, v)) {
        return std::nullopt;
    }
    auto e = get_edge_desc(u, v);
    auto edge_it = edge_properties.find(get_edge_id(e));
    if (edge_it == edge_properties.end()) {
        return std::nullopt;
    }
    auto attr_it = edge_it->second.find(key);
    if (attr_it == edge_it->second.end()) {
        return std::nullopt;
    }
    if (const auto* value = std::any_cast<T>(&(attr_it->second))) {
        return *value;
    }
    return std::nullopt;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
double Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::get_edge_numeric_attr(const NodeID& u, const NodeID& v, const std::string& key) const {
    if (key == "weight") {
        if constexpr (Weighted) {
            return static_cast<double>(get_edge_weight(u, v));
        } else {
            throw std::runtime_error("Edge attribute lookup failed: graph has no built-in edge weight.");
        }
    }

    auto e = get_edge_desc(u, v);
    auto edge_it = edge_properties.find(get_edge_id(e));
    if (edge_it == edge_properties.end()) {
        throw std::runtime_error("Edge attribute lookup failed: edge has no attributes.");
    }
    auto attr_it = edge_it->second.find(key);
    if (attr_it == edge_it->second.end()) {
        throw std::runtime_error("Edge attribute lookup failed: key not found.");
    }

    const auto& value = attr_it->second;
    if (const auto* vptr = std::any_cast<int>(&value)) return static_cast<double>(*vptr);
    if (const auto* vptr = std::any_cast<long>(&value)) return static_cast<double>(*vptr);
    if (const auto* vptr = std::any_cast<long long>(&value)) return static_cast<double>(*vptr);
    if (const auto* vptr = std::any_cast<float>(&value)) return static_cast<double>(*vptr);
    if (const auto* vptr = std::any_cast<double>(&value)) return *vptr;

    throw std::runtime_error("Edge attribute lookup failed: stored value is not numeric.");
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
EdgeWeight Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::get_edge_weight(const NodeID& u, const NodeID& v) const {
    auto it_u = id_to_bgl.find(u);
    auto it_v = id_to_bgl.find(v);
    if (it_u == id_to_bgl.end() || it_v == id_to_bgl.end()) throw std::runtime_error("Node lookup failed: node not found.");
    auto [e, exists] = boost::edge(it_u->second, it_v->second, g);
    if (!exists) throw std::runtime_error("Edge lookup failed: edge not found.");
    return weight_map[e];
}

} // namespace nxpp
