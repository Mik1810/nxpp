#pragma once

#include "attributes.hpp"

namespace nxpp {

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
bool Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::has_edge_id(std::size_t edge_id) const {
    return try_find_edge_desc_by_id(edge_id).has_value();
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
std::vector<std::size_t> Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::edge_ids() const {
    std::vector<std::size_t> ids;
    for (auto [e, eend] = boost::edges(g); e != eend; ++e) {
        ids.push_back(get_edge_id(*e));
    }
    return ids;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
std::vector<std::size_t> Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::edge_ids(const NodeID& u, const NodeID& v) const {
    auto it_u = id_to_bgl.find(u);
    auto it_v = id_to_bgl.find(v);
    if (it_u == id_to_bgl.end() || it_v == id_to_bgl.end()) {
        return {};
    }
    return collect_edge_ids_between(it_u->second, it_v->second);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
std::pair<NodeID, NodeID> Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::get_edge_endpoints(std::size_t edge_id) const {
    auto e = get_edge_desc_by_id(edge_id);
    return {
        node_id_of(boost::source(e, g)),
        node_id_of(boost::target(e, g))
    };
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
std::size_t Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::add_edge_with_id(const NodeID& u, const NodeID& v, EdgeWeight w) {
    VertexDesc bu = get_or_create_vertex(u);
    VertexDesc bv = get_or_create_vertex(v);

    if constexpr (!Multi) {
        auto [e, exists] = boost::edge(bu, bv, g);
        if (exists) {
            weight_map[e] = w;
            return get_edge_id(e);
        }
    }

    auto [e, added] = boost::add_edge(bu, bv, g);
    (void)added;
    weight_map[e] = w;
    edge_id_map[e] = next_edge_id++;
    return get_edge_id(e);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(!W)
std::size_t Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::add_edge_with_id(const NodeID& u, const NodeID& v) {
    VertexDesc bu = get_or_create_vertex(u);
    VertexDesc bv = get_or_create_vertex(v);

    if constexpr (!Multi) {
        auto [e, exists] = boost::edge(bu, bv, g);
        if (exists) {
            return get_edge_id(e);
        }
    }

    auto [e, added] = boost::add_edge(bu, bv, g);
    (void)added;
    edge_id_map[e] = next_edge_id++;
    return get_edge_id(e);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
void Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::remove_edge(std::size_t edge_id) {
    auto edge_desc = try_find_edge_desc_by_id(edge_id);
    if (!edge_desc.has_value()) {
        throw std::runtime_error("NetworkXError: The edge is not in the graph.");
    }
    edge_properties.erase(edge_id);
    boost::remove_edge(*edge_desc, g);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <typename T>
T Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::get_edge_attr(std::size_t edge_id, const std::string& key) const {
    auto edge_it = edge_properties.find(edge_id);
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
std::optional<T> Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::try_get_edge_attr(std::size_t edge_id, const std::string& key) const {
    auto edge_it = edge_properties.find(edge_id);
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
double Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::get_edge_numeric_attr(std::size_t edge_id, const std::string& key) const {
    if (key == "weight") {
        if constexpr (Weighted) {
            return static_cast<double>(get_edge_weight(edge_id));
        } else {
            throw std::runtime_error("Edge attribute lookup failed: graph has no built-in edge weight.");
        }
    }

    auto edge_it = edge_properties.find(edge_id);
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
EdgeWeight Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::get_edge_weight(std::size_t edge_id) const {
    auto e = get_edge_desc_by_id(edge_id);
    return weight_map[e];
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
void Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::set_edge_weight(std::size_t edge_id, EdgeWeight w) {
    auto e = get_edge_desc_by_id(edge_id);
    weight_map[e] = w;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <typename T>
void Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::set_edge_attr(std::size_t edge_id, const std::string& key, const T& value) {
    (void)get_edge_desc_by_id(edge_id);
    edge_properties[edge_id][key] = make_attr_any(value);
}

} // namespace nxpp
