#ifndef NXPP_HPP
#define NXPP_HPP

#if !__has_include(<boost/graph/adjacency_list.hpp>)
#error "FATAL: nxpp requires the Boost Graph Library (BGL)."
#endif

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/strong_components.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/graph/dag_shortest_paths.hpp>
#include <boost/graph/floyd_warshall_shortest.hpp>
#include <boost/graph/edmonds_karp_max_flow.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/find_flow_cost.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/functional/hash.hpp>
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <string>
#include <algorithm>
#include <cmath>
#include <utility>
#include <iostream>
#include <any>
#include <map>
#include <random>
#include <optional>
#include <limits>
#include <queue>
#include <memory>

namespace nxpp {

template <typename GraphType, bool HasWeight>
struct built_in_weight_traits;

template <typename GraphType>
struct built_in_weight_traits<GraphType, true> {
    using map_type = typename boost::property_map<GraphType, boost::edge_weight_t>::type;

    static map_type get(GraphType& g) {
        return boost::get(boost::edge_weight, g);
    }
};

template <typename GraphType>
struct built_in_weight_traits<GraphType, false> {
    struct map_type {};

    static map_type get(GraphType&) {
        return {};
    }
};

// Core Graph Class

template <
    typename NodeID = std::string,
    typename EdgeWeight = double,
    bool Directed = false,
    bool Multi = false,
    bool Weighted = true,
    typename OutEdgeSelector = boost::vecS,
    typename VertexSelector = boost::vecS
>
class Graph {
public:
    using NodeType = NodeID;
    using EdgeWeightType = EdgeWeight;
    using OutEdgeListSelector = OutEdgeSelector;
    using VertexListSelector = VertexSelector;
    using DirectedSelector = typename std::conditional<Directed, boost::bidirectionalS, boost::undirectedS>::type;
    using VertexProperty = boost::property<boost::vertex_name_t, NodeID>;
    using EdgeProperty = typename std::conditional<
        Weighted,
        boost::property<boost::edge_weight_t, EdgeWeight, boost::property<boost::edge_index_t, std::size_t>>,
        boost::property<boost::edge_index_t, std::size_t>
    >::type;
    using GraphType = boost::adjacency_list<OutEdgeSelector, VertexSelector, DirectedSelector,
                                            VertexProperty, EdgeProperty>;
    using VertexDesc = typename boost::graph_traits<GraphType>::vertex_descriptor;
    using EdgeDesc = typename boost::graph_traits<GraphType>::edge_descriptor;
    using WeightMap = typename built_in_weight_traits<GraphType, Weighted>::map_type;
    using VertexNameMap = typename boost::property_map<GraphType, boost::vertex_name_t>::type;
    using EdgeIdMap = typename boost::property_map<GraphType, boost::edge_index_t>::type;
    using VertexIndexStorage = std::unordered_map<VertexDesc, std::size_t, boost::hash<VertexDesc>>;
    using VertexIndexMap = boost::associative_property_map<VertexIndexStorage>;
    static constexpr bool is_directed = Directed;
    static constexpr bool has_builtin_edge_weight = Weighted;

private:
    GraphType g;
    WeightMap weight_map;
    VertexNameMap vertex_name_map;
    EdgeIdMap edge_id_map;
    VertexIndexStorage vertex_index_storage;
    VertexIndexMap vertex_index_map;
    std::unordered_map<NodeID, VertexDesc> id_to_bgl;
    std::vector<NodeID> bgl_to_id;
    std::size_t next_edge_id = 0;

public:
    std::unordered_map<NodeID, std::unordered_map<std::string, std::any>> node_properties;
    std::unordered_map<std::size_t, std::unordered_map<std::string, std::any>> edge_properties;

private:
    static_assert(
        !(Multi && std::is_same_v<OutEdgeSelector, boost::setS>),
        "nxpp does not support Multi=true with boost::setS because setS suppresses parallel edges."
    );

    using EdgeAttrMap = std::unordered_map<std::string, std::any>;

    static std::any normalize_attr_any(std::any value) {
        if (value.type() == typeid(const char*)) {
            return std::string(std::any_cast<const char*>(value));
        }
        if (value.type() == typeid(char*)) {
            return std::string(std::any_cast<char*>(value));
        }
        return value;
    }

    template <typename T>
    static std::any make_attr_any(const T& value) {
        if constexpr (std::is_convertible_v<T, const char*> && !std::is_same_v<std::decay_t<T>, std::string>) {
            return std::string(value);
        } else {
            return std::any(value);
        }
    }

    std::size_t vertex_index_of(VertexDesc v) const {
        return boost::get(vertex_index_map, v);
    }

    const NodeID& node_id_of(VertexDesc v) const {
        return boost::get(vertex_name_map, v);
    }

    void rebuild_vertex_maps() {
        id_to_bgl.clear();
        bgl_to_id.clear();

        std::size_t index = 0;
        for (auto [v, vend] = boost::vertices(g); v != vend; ++v, ++index) {
            boost::put(vertex_index_map, *v, index);
            NodeID id = boost::get(vertex_name_map, *v);
            id_to_bgl[id] = *v;
            bgl_to_id.push_back(id);
        }
    }

    VertexDesc get_or_create_vertex(const NodeID& id) {
        auto it = id_to_bgl.find(id);
        if (it != id_to_bgl.end()) {
            return it->second;
        }
        VertexDesc v = boost::add_vertex(g);
        boost::put(vertex_name_map, v, id);
        boost::put(vertex_index_map, v, bgl_to_id.size());
        id_to_bgl[id] = v;
        bgl_to_id.push_back(id);
        return v;
    }

    std::size_t get_edge_id(EdgeDesc e) const {
        return boost::get(edge_id_map, e);
    }

    std::optional<EdgeDesc> try_find_edge_desc_by_id(std::size_t edge_id) const {
        for (auto [e, eend] = boost::edges(g); e != eend; ++e) {
            if (get_edge_id(*e) == edge_id) {
                return *e;
            }
        }
        return std::nullopt;
    }

    EdgeDesc get_edge_desc_by_id(std::size_t edge_id) const {
        auto edge_desc = try_find_edge_desc_by_id(edge_id);
        if (!edge_desc.has_value()) {
            throw std::runtime_error("Edge not found");
        }
        return *edge_desc;
    }

    std::vector<std::size_t> collect_edge_ids_between(VertexDesc u, VertexDesc v) const {
        std::vector<std::size_t> edge_ids;
        for (auto [e, eend] = boost::out_edges(u, g); e != eend; ++e) {
            if (boost::target(*e, g) == v) {
                edge_ids.push_back(get_edge_id(*e));
            }
        }
        if constexpr (!Directed) {
            if (u != v) {
                for (auto [e, eend] = boost::out_edges(v, g); e != eend; ++e) {
                    if (boost::target(*e, g) == u) {
                        edge_ids.push_back(get_edge_id(*e));
                    }
                }
            }
        }
        return edge_ids;
    }

    void erase_incident_edge_properties(VertexDesc v) {
        std::vector<std::size_t> edge_ids;
        for (auto [e, eend] = boost::edges(g); e != eend; ++e) {
            if (boost::source(*e, g) == v || boost::target(*e, g) == v) {
                edge_ids.push_back(get_edge_id(*e));
            }
        }
        for (auto edge_id : edge_ids) {
            edge_properties.erase(edge_id);
        }
    }

    void assign_edge_attrs(EdgeDesc e, const EdgeAttrMap& attrs) {
        auto& edge_attr_map = edge_properties[get_edge_id(e)];
        for (const auto& [key, value] : attrs) {
            edge_attr_map[key] = normalize_attr_any(value);
        }
    }

    void assign_edge_attrs(EdgeDesc e, std::initializer_list<std::pair<std::string, std::any>> attrs) {
        auto& edge_attr_map = edge_properties[get_edge_id(e)];
        for (const auto& [key, value] : attrs) {
            edge_attr_map[key] = normalize_attr_any(value);
        }
    }

    void assign_edge_attr(EdgeDesc e, const std::pair<std::string, std::any>& attr) {
        edge_properties[get_edge_id(e)][attr.first] = normalize_attr_any(attr.second);
    }

public:
    Graph()
        : g(),
          weight_map(built_in_weight_traits<GraphType, Weighted>::get(g)),
          vertex_name_map(boost::get(boost::vertex_name, g)),
          edge_id_map(boost::get(boost::edge_index, g)),
          vertex_index_storage(),
          vertex_index_map(vertex_index_storage) {}

    void add_node(const NodeID& id) {
        get_or_create_vertex(id);
    }

    void add_nodes_from(const std::vector<NodeID>& nodes) {
        for (const auto& n : nodes) {
            add_node(n);
        }
    }

    EdgeDesc get_edge_desc(const NodeID& u, const NodeID& v) const {
        auto it_u = id_to_bgl.find(u);
        auto it_v = id_to_bgl.find(v);
        if (it_u == id_to_bgl.end() || it_v == id_to_bgl.end()) throw std::runtime_error("Node not found");
        auto [e, exists] = boost::edge(it_u->second, it_v->second, g);
        if (!exists) throw std::runtime_error("Edge not found");
        return e;
    }

    bool has_edge(const NodeID& u, const NodeID& v) const {
        auto it_u = id_to_bgl.find(u);
        auto it_v = id_to_bgl.find(v);
        if (it_u == id_to_bgl.end() || it_v == id_to_bgl.end()) return false;
        auto [e, exists] = boost::edge(it_u->second, it_v->second, g);
        return exists;
    }

    bool has_edge_id(std::size_t edge_id) const {
        return try_find_edge_desc_by_id(edge_id).has_value();
    }

    std::vector<std::size_t> edge_ids() const {
        std::vector<std::size_t> ids;
        for (auto [e, eend] = boost::edges(g); e != eend; ++e) {
            ids.push_back(get_edge_id(*e));
        }
        return ids;
    }

    std::vector<std::size_t> edge_ids(const NodeID& u, const NodeID& v) const {
        auto it_u = id_to_bgl.find(u);
        auto it_v = id_to_bgl.find(v);
        if (it_u == id_to_bgl.end() || it_v == id_to_bgl.end()) {
            return {};
        }
        return collect_edge_ids_between(it_u->second, it_v->second);
    }

    std::pair<NodeID, NodeID> get_edge_endpoints(std::size_t edge_id) const {
        auto e = get_edge_desc_by_id(edge_id);
        return {
            node_id_of(boost::source(e, g)),
            node_id_of(boost::target(e, g))
        };
    }

    template <bool W = Weighted>
    requires(W)
    void add_edge(const NodeID& u, const NodeID& v, EdgeWeight w = 1.0) {
        VertexDesc bu = get_or_create_vertex(u);
        VertexDesc bv = get_or_create_vertex(v);
        
        if constexpr (!Multi) {
            auto [e, exists] = boost::edge(bu, bv, g);
            if (exists) {
                weight_map[e] = w;
                return;
            }
        }
        
        auto [e, added] = boost::add_edge(bu, bv, g);
        weight_map[e] = w;
        edge_id_map[e] = next_edge_id++;
    }

    template <bool W = Weighted>
    requires(W)
    std::size_t add_edge_with_id(const NodeID& u, const NodeID& v, EdgeWeight w = 1.0) {
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

    template <bool W = Weighted>
    requires(!W)
    void add_edge(const NodeID& u, const NodeID& v) {
        VertexDesc bu = get_or_create_vertex(u);
        VertexDesc bv = get_or_create_vertex(v);

        if constexpr (!Multi) {
            auto [e, exists] = boost::edge(bu, bv, g);
            if (exists) {
                return;
            }
        }

        auto [e, added] = boost::add_edge(bu, bv, g);
        (void)added;
        edge_id_map[e] = next_edge_id++;
    }

    template <bool W = Weighted>
    requires(!W)
    std::size_t add_edge_with_id(const NodeID& u, const NodeID& v) {
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

    template <bool W = Weighted>
    requires(W)
    void add_edge(const NodeID& u, const NodeID& v, EdgeWeight w, const EdgeAttrMap& attrs) {
        add_edge(u, v, w);
        assign_edge_attrs(get_edge_desc(u, v), attrs);
    }

    void add_edge(const NodeID& u, const NodeID& v, const EdgeAttrMap& attrs) {
        if constexpr (Weighted) {
            add_edge(u, v, static_cast<EdgeWeight>(1.0), attrs);
        } else {
            add_edge(u, v);
            assign_edge_attrs(get_edge_desc(u, v), attrs);
        }
    }

    template <bool W = Weighted>
    requires(W)
    void add_edge(const NodeID& u, const NodeID& v, EdgeWeight w, const std::pair<std::string, std::any>& attr) {
        add_edge(u, v, w);
        assign_edge_attr(get_edge_desc(u, v), attr);
    }

    void add_edge(const NodeID& u, const NodeID& v, const std::pair<std::string, std::any>& attr) {
        if constexpr (Weighted) {
            add_edge(u, v, static_cast<EdgeWeight>(1.0), attr);
        } else {
            add_edge(u, v);
            assign_edge_attr(get_edge_desc(u, v), attr);
        }
    }

    template <bool W = Weighted>
    requires(W)
    void add_edge(const NodeID& u, const NodeID& v, EdgeWeight w, std::initializer_list<std::pair<std::string, std::any>> attrs) {
        add_edge(u, v, w);
        assign_edge_attrs(get_edge_desc(u, v), attrs);
    }

    void add_edge(const NodeID& u, const NodeID& v, std::initializer_list<std::pair<std::string, std::any>> attrs) {
        if constexpr (Weighted) {
            add_edge(u, v, static_cast<EdgeWeight>(1.0), attrs);
        } else {
            add_edge(u, v);
            assign_edge_attrs(get_edge_desc(u, v), attrs);
        }
    }

    template <bool W = Weighted>
    requires(W)
    void add_edges_from(const std::vector<std::tuple<NodeID, NodeID, EdgeWeight>>& edges) {
        for (const auto& edge : edges) {
            add_edge(std::get<0>(edge), std::get<1>(edge), std::get<2>(edge));
        }
    }

    void add_edges_from(const std::vector<std::pair<NodeID, NodeID>>& edges) {
        for (const auto& edge : edges) {
            add_edge(edge.first, edge.second);
        }
    }

    void clear() {
        g = GraphType();
        id_to_bgl.clear();
        bgl_to_id.clear();
        node_properties.clear();
        edge_properties.clear();
        weight_map = built_in_weight_traits<GraphType, Weighted>::get(g);
        vertex_name_map = boost::get(boost::vertex_name, g);
        edge_id_map = boost::get(boost::edge_index, g);
        vertex_index_storage.clear();
        next_edge_id = 0;
    }

    void remove_edge(const NodeID& u, const NodeID& v) {
        auto it_u = id_to_bgl.find(u);
        auto it_v = id_to_bgl.find(v);
        if (it_u == id_to_bgl.end() || it_v == id_to_bgl.end()) {
            throw std::runtime_error("NetworkXError: The node is not in the graph.");
        }
        auto [e, exists] = boost::edge(it_u->second, it_v->second, g);
        if (!exists) {
            throw std::runtime_error("NetworkXError: The edge is not in the graph.");
        }
        for (auto edge_id : collect_edge_ids_between(it_u->second, it_v->second)) {
            edge_properties.erase(edge_id);
        }
        boost::remove_edge(it_u->second, it_v->second, g);
    }

    void remove_edge(std::size_t edge_id) {
        auto edge_desc = try_find_edge_desc_by_id(edge_id);
        if (!edge_desc.has_value()) {
            throw std::runtime_error("NetworkXError: The edge is not in the graph.");
        }
        edge_properties.erase(edge_id);
        boost::remove_edge(*edge_desc, g);
    }

    void remove_node(const NodeID& u) {
        auto it = id_to_bgl.find(u);
        if (it == id_to_bgl.end()) {
            throw std::runtime_error("NetworkXError: The node is not in the graph.");
        }
        VertexDesc v = it->second;

        erase_incident_edge_properties(v);
        boost::clear_vertex(v, g);
        boost::remove_vertex(v, g);
        
        node_properties.erase(u);
        rebuild_vertex_maps();
    }

    std::vector<NodeID> neighbors(const NodeID& u) const {
        auto it = id_to_bgl.find(u);
        if (it == id_to_bgl.end()) {
            throw std::runtime_error("NetworkXError: The node is not in the graph.");
        }
        std::vector<NodeID> res;
        for (auto [e, eend] = boost::out_edges(it->second, g); e != eend; ++e) {
            res.push_back(node_id_of(boost::target(*e, g)));
        }
        return res;
    }

    std::vector<NodeID> successors(const NodeID& u) const {
        return neighbors(u);
    }

    std::vector<NodeID> predecessors(const NodeID& u) const {
        auto it = id_to_bgl.find(u);
        if (it == id_to_bgl.end()) {
            throw std::runtime_error("NetworkXError: The node is not in the graph.");
        }

        std::vector<NodeID> res;
        if constexpr (Directed) {
            for (auto [e, eend] = boost::in_edges(it->second, g); e != eend; ++e) {
                res.push_back(node_id_of(boost::source(*e, g)));
            }
        } else {
            for (auto [e, eend] = boost::out_edges(it->second, g); e != eend; ++e) {
                res.push_back(node_id_of(boost::target(*e, g)));
            }
        }
        return res;
    }

    bool has_node(const NodeID& u) const {
        return id_to_bgl.find(u) != id_to_bgl.end();
    }

    bool has_node_attr(const NodeID& u, const std::string& key) const {
        auto node_it = node_properties.find(u);
        if (node_it == node_properties.end()) {
            return false;
        }
        return node_it->second.find(key) != node_it->second.end();
    }

    bool has_edge_attr(const NodeID& u, const NodeID& v, const std::string& key) const {
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

    bool has_edge_attr(std::size_t edge_id, const std::string& key) const {
        auto edge_it = edge_properties.find(edge_id);
        if (edge_it == edge_properties.end()) {
            return false;
        }
        return edge_it->second.find(key) != edge_it->second.end();
    }

    template <typename T>
    T get_node_attr(const NodeID& u, const std::string& key) const {
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

    template <typename T>
    T get_edge_attr(const NodeID& u, const NodeID& v, const std::string& key) const {
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

    template <typename T>
    T get_edge_attr(std::size_t edge_id, const std::string& key) const {
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

    template <typename T>
    std::optional<T> try_get_node_attr(const NodeID& u, const std::string& key) const {
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

    template <typename T>
    std::optional<T> try_get_edge_attr(const NodeID& u, const NodeID& v, const std::string& key) const {
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

    template <typename T>
    std::optional<T> try_get_edge_attr(std::size_t edge_id, const std::string& key) const {
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

    double get_edge_numeric_attr(const NodeID& u, const NodeID& v, const std::string& key) const {
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

    double get_edge_numeric_attr(std::size_t edge_id, const std::string& key) const {
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

    template <bool W = Weighted>
    requires(W)
    EdgeWeight get_edge_weight(const NodeID& u, const NodeID& v) const {
        auto it_u = id_to_bgl.find(u);
        auto it_v = id_to_bgl.find(v);
        if (it_u == id_to_bgl.end() || it_v == id_to_bgl.end()) throw std::runtime_error("Node not found in graph");
        auto [e, exists] = boost::edge(it_u->second, it_v->second, g);
        if (!exists) throw std::runtime_error("Edge not found in graph");
        return weight_map[e];
    }

    template <bool W = Weighted>
    requires(W)
    EdgeWeight get_edge_weight(std::size_t edge_id) const {
        auto e = get_edge_desc_by_id(edge_id);
        return weight_map[e];
    }

    template <bool W = Weighted>
    requires(W)
    void set_edge_weight(std::size_t edge_id, EdgeWeight w) {
        auto e = get_edge_desc_by_id(edge_id);
        weight_map[e] = w;
    }

    template <typename T>
    void set_edge_attr(std::size_t edge_id, const std::string& key, const T& value) {
        (void)get_edge_desc_by_id(edge_id);
        edge_properties[edge_id][key] = make_attr_any(value);
    }

    std::vector<NodeID> nodes() const {
        std::vector<NodeID> res;
        for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
            res.push_back(node_id_of(*v));
        }
        return res;
    }

    auto edges() const {
        if constexpr (Weighted) {
            std::vector<std::tuple<NodeID, NodeID, EdgeWeight>> res;
            for (auto [e, eend] = boost::edges(g); e != eend; ++e) {
                NodeID source_id = node_id_of(boost::source(*e, g));
                NodeID target_id = node_id_of(boost::target(*e, g));
                res.emplace_back(source_id, target_id, weight_map[*e]);
            }
            return res;
        } else {
            std::vector<std::pair<NodeID, NodeID>> res;
            for (auto [e, eend] = boost::edges(g); e != eend; ++e) {
                NodeID source_id = node_id_of(boost::source(*e, g));
                NodeID target_id = node_id_of(boost::target(*e, g));
                res.emplace_back(source_id, target_id);
            }
            return res;
        }
    }

    std::vector<std::pair<NodeID, NodeID>> edge_pairs() const {
        std::vector<std::pair<NodeID, NodeID>> res;
        for (auto [e, eend] = boost::edges(g); e != eend; ++e) {
            NodeID source_id = node_id_of(boost::source(*e, g));
            NodeID target_id = node_id_of(boost::target(*e, g));
            res.emplace_back(source_id, target_id);
        }
        return res;
    }

    // Accessors for BGL algorithms
    const GraphType& get_impl() const { return g; }
    const std::vector<NodeID>& get_bgl_to_id_map() const { return bgl_to_id; }
    const std::unordered_map<NodeID, VertexDesc>& get_id_to_bgl_map() const { return id_to_bgl; }
    const NodeID& get_node_id(VertexDesc v) const { return node_id_of(v); }
    std::size_t get_vertex_index(VertexDesc v) const { return vertex_index_of(v); }

    // Proxy Pattern per simulare G[u][v] = weight
    struct EdgeAttrProxy {
        Graph* graph;
        NodeID u, v;
        std::string key;

        template <typename T>
        EdgeAttrProxy& operator=(const T& val) {
            if (!graph->has_edge(u, v)) {
                if constexpr (Weighted) graph->add_edge(u, v, static_cast<EdgeWeight>(1.0));
                else graph->add_edge(u, v);
            }
            auto e = graph->get_edge_desc(u, v);
            graph->edge_properties[graph->get_edge_id(e)][key] = Graph::make_attr_any(val);
            return *this;
        }

        template <typename T>
        operator T() const {
            return graph->template get_edge_attr<T>(u, v, key);
        }
    };

    class EdgeProxy {
        Graph* graph;
        NodeID u, v;
    public:
        EdgeProxy(Graph* g, NodeID u, NodeID v) : graph(g), u(u), v(v) {}
        
        template <bool W = Weighted>
        requires(W)
        EdgeProxy& operator=(EdgeWeight w) {
            graph->add_edge(u, v, w);
            return *this;
        }

        template <bool W = Weighted>
        requires(W)
        operator EdgeWeight() const {
            return graph->get_edge_weight(u, v);
        }

        EdgeAttrProxy operator[](const std::string& key) {
            return {graph, u, v, key};
        }

        EdgeAttrProxy operator[](const char* key) {
            return {graph, u, v, std::string(key)};
        }
    };

    struct NodeAttrProxy {
        Graph* graph;
        NodeID u;
        std::string key;

        template <typename T>
        NodeAttrProxy& operator=(const T& val) {
            if (!graph->has_node(u)) graph->add_node(u);
            graph->node_properties[u][key] = Graph::make_attr_any(val);
            return *this;
        }

        template <typename T>
        operator T() const {
            return graph->template get_node_attr<T>(u, key);
        }
    };

    class NodeProxy {
        Graph* graph;
        NodeID u;
    public:
        NodeProxy(Graph* g, NodeID u) : graph(g), u(u) {}
        
        EdgeProxy operator[](const NodeID& v) {
            return EdgeProxy(graph, u, v);
        }
    };

    class NodeAttrBaseProxy {
        Graph* graph;
        NodeID u;
    public:
        NodeAttrBaseProxy(Graph* g, NodeID u) : graph(g), u(u) {}
        
        NodeAttrProxy operator[](const std::string& key) {
            return {graph, u, key};
        }

        NodeAttrProxy operator[](const char* key) {
            return {graph, u, std::string(key)};
        }
    };

    NodeProxy operator[](const NodeID& u) {
        if (!has_node(u)) {
            add_node(u);
        }
        return NodeProxy(this, u);
    }

    NodeAttrBaseProxy node(const NodeID& u) {
        if (!has_node(u) && !Multi) add_node(u);
        else if (!has_node(u)) add_node(u);
        return NodeAttrBaseProxy(this, u);
    }

    auto bfs_edges(const NodeID& start) const;
    auto bfs_tree(const NodeID& start) const;
    auto bfs_successors(const NodeID& start) const;
    template <typename Visitor>
    void breadth_first_search(const NodeID& start, Visitor& visitor) const;
    template <typename OnVertex, typename OnTreeEdge>
    void bfs_visit(const NodeID& start, OnVertex&& on_vertex, OnTreeEdge&& on_tree_edge) const;
    auto dfs_edges(const NodeID& start) const;
    auto dfs_tree(const NodeID& start) const;
    auto dfs_predecessors(const NodeID& start) const;
    auto dfs_successors(const NodeID& start) const;
    template <typename Visitor>
    void depth_first_search(const NodeID& start, Visitor& visitor) const;
    template <typename OnTreeEdge, typename OnBackEdge>
    void dfs_visit(const NodeID& start, OnTreeEdge&& on_tree_edge, OnBackEdge&& on_back_edge) const;

    auto shortest_path(const NodeID& source_id, const NodeID& target_id) const;
    auto shortest_path(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const;
    double shortest_path_length(const NodeID& source_id, const NodeID& target_id) const;
    double shortest_path_length(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const;

    template <bool W = Weighted>
    requires(W)
    auto dijkstra_path(const NodeID& source_id, const NodeID& target_id) const;

    template <bool W = Weighted>
    requires(W)
    auto dijkstra_path(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const;

    template <bool W = Weighted>
    requires(W)
    auto dijkstra_shortest_paths(const NodeID& source_id) const;

    template <bool W = Weighted>
    requires(W)
    auto dijkstra_path_length(const NodeID& source_id) const;

    template <bool W = Weighted>
    requires(W)
    auto dijkstra_path_length(const NodeID& source_id, const NodeID& target_id) const;

    template <bool W = Weighted>
    requires(W)
    auto dijkstra_path_length(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const;

    template <bool W = Weighted>
    requires(W)
    auto bellman_ford_path(const NodeID& source_id, const NodeID& target_id) const;

    template <bool W = Weighted>
    requires(W)
    auto bellman_ford_shortest_paths(const NodeID& source_id) const;

    template <bool W = Weighted>
    requires(W)
    auto bellman_ford_path(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const;

    template <bool W = Weighted>
    requires(W)
    auto bellman_ford_path_length(const NodeID& source_id, const NodeID& target_id) const;

    template <bool W = Weighted>
    requires(W)
    auto bellman_ford_path_length(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const;

    template <bool W = Weighted>
    requires(W)
    auto dag_shortest_paths(const NodeID& source_id) const;

    template <bool W = Weighted>
    requires(W)
    auto floyd_warshall_all_pairs_shortest_paths() const;

    template <bool W = Weighted>
    requires(W)
    auto floyd_warshall_all_pairs_shortest_paths_map() const;

    auto connected_component_groups() const;
    auto connected_components() const;
    auto strongly_connected_component_groups() const;
    auto strong_component_map() const;
    auto strong_components() const;
    auto connected_component_map() const;
    auto strongly_connected_components() const;
    auto strongly_connected_component_map() const;
    auto strongly_connected_component_roots() const;
    auto topological_sort() const;

    template <bool W = Weighted>
    requires(W)
    auto kruskal_minimum_spanning_tree() const;

    template <bool W = Weighted>
    requires(W)
    auto prim_minimum_spanning_tree(const NodeID& root_id) const;

    template <bool W = Weighted>
    requires(W)
    auto minimum_spanning_tree() const;

    template <bool W = Weighted>
    requires(W)
    auto minimum_spanning_tree(const NodeID& root_id) const;

    auto edmonds_karp_maximum_flow(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr = "capacity") const;
    auto push_relabel_maximum_flow_result(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr = "capacity") const;
    auto maximum_flow(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr = "capacity") const;
    auto minimum_cut(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr = "capacity") const;
    auto max_flow_min_cost_cycle_canceling(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") const;
    long push_relabel_maximum_flow(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") const;
    auto cycle_canceling(const std::string& weight_attr = "weight") const;
    auto successive_shortest_path_nonnegative_weights(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") const;
    auto max_flow_min_cost_successive_shortest_path(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") const;
    auto max_flow_min_cost(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") const;

    auto num_vertices() const;
    auto degree_centrality() const;
};

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

// NetworkX-style Aliases
using WeightedGraphInt = Graph<int, int>;
using WeightedDiGraphInt = Graph<int, int, true>;
using WeightedGraphStr = Graph<std::string>;
using WeightedMultiGraphInt = Graph<int, int, false, true>;
using WeightedMultiDiGraphInt = Graph<int, int, true, true>;

using WeightedDiGraph = Graph<std::string, double, true>;
using WeightedMultiGraph = Graph<std::string, double, false, true>;
using WeightedMultiDiGraph = Graph<std::string, double, true, true>;

using GraphInt = Graph<int, int>;
using DiGraphInt = Graph<int, int, true>;
using GraphStr = Graph<std::string>;
using MultiGraphInt = Graph<int, int, false, true>;
using MultiDiGraphInt = Graph<int, int, true, true>;

using DiGraph = Graph<std::string, double, true>;
using MultiGraph = Graph<std::string, double, false, true>;
using MultiDiGraph = Graph<std::string, double, true, true>;

using UnweightedGraphInt = Graph<int, double, false, false, false>;
using UnweightedDiGraphInt = Graph<int, double, true, false, false>;
using UnweightedGraphStr = Graph<std::string, double, false, false, false>;
using UnweightedDiGraph = Graph<std::string, double, true, false, false>;
using UnweightedMultiGraphInt = Graph<int, double, false, true, false>;
using UnweightedMultiDiGraphInt = Graph<int, double, true, true, false>;
using UnweightedMultiGraph = Graph<std::string, double, false, true, false>;
using UnweightedMultiDiGraph = Graph<std::string, double, true, true, false>;

template <typename NodeID>
struct MaximumFlowResult {
    long value = 0;
    std::map<std::pair<NodeID, NodeID>, long> flow;
};

template <typename NodeID>
struct MinCostMaxFlowResult {
    long flow = 0;
    long cost = 0;
    std::map<std::pair<NodeID, NodeID>, long> edge_flows;
};

namespace detail {

template <typename NodeID>
struct MinCostFlowState {
    using FlowTraits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
    using FlowGraph = boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::directedS,
        boost::no_property,
        boost::property<
            boost::edge_weight_t,
            long,
            boost::property<
                boost::edge_capacity_t,
                long,
                boost::property<
                    boost::edge_residual_capacity_t,
                    long,
                    boost::property<boost::edge_reverse_t, typename FlowTraits::edge_descriptor>
                >
            >
        >
    >;
    using WeightMap = typename boost::property_map<FlowGraph, boost::edge_weight_t>::type;
    using CapacityMap = typename boost::property_map<FlowGraph, boost::edge_capacity_t>::type;
    using ResidualMap = typename boost::property_map<FlowGraph, boost::edge_residual_capacity_t>::type;
    using ReverseMap = typename boost::property_map<FlowGraph, boost::edge_reverse_t>::type;
    using FlowEdgeDesc = typename boost::graph_traits<FlowGraph>::edge_descriptor;

    FlowGraph flow_graph;
    WeightMap weight;
    CapacityMap capacity;
    ResidualMap residual;
    ReverseMap reverse;
    std::unordered_map<NodeID, std::size_t> node_to_index;
    std::map<std::pair<NodeID, NodeID>, FlowEdgeDesc> original_edges;
    NodeID source_id;
    NodeID target_id;
    long value = 0;
    long cost = 0;

    MinCostFlowState()
        : flow_graph(),
          weight(boost::get(boost::edge_weight, flow_graph)),
          capacity(boost::get(boost::edge_capacity, flow_graph)),
          residual(boost::get(boost::edge_residual_capacity, flow_graph)),
          reverse(boost::get(boost::edge_reverse, flow_graph)) {}
};

template <typename GraphWrapper>
auto& min_cost_flow_cache() {
    using NodeID = typename GraphWrapper::NodeType;
    static std::unordered_map<const void*, std::unique_ptr<MinCostFlowState<NodeID>>> cache;
    return cache;
}

} // namespace detail

template <typename NodeID>
struct MinimumCutResult {
    long value = 0;
    std::vector<NodeID> reachable;
    std::vector<NodeID> non_reachable;
    std::vector<std::pair<NodeID, NodeID>> cut_edges;
};

template <typename NodeID, typename Distance = double>
struct SingleSourceShortestPathResult {
    std::unordered_map<NodeID, Distance> distance;
    std::unordered_map<NodeID, NodeID> predecessor;
    std::unordered_map<NodeID, std::vector<NodeID>> paths;
};

template <typename Key, typename Value>
class lookup_map {
public:
    using storage_type = std::unordered_map<Key, Value>;
    using iterator = typename storage_type::iterator;
    using const_iterator = typename storage_type::const_iterator;

    Value& operator[](const Key& key) {
        return data[key];
    }

    const Value& operator[](const Key& key) const {
        return data.at(key);
    }

    Value& at(const Key& key) {
        return data.at(key);
    }

    const Value& at(const Key& key) const {
        return data.at(key);
    }

    iterator begin() { return data.begin(); }
    iterator end() { return data.end(); }
    const_iterator begin() const { return data.begin(); }
    const_iterator end() const { return data.end(); }
    const_iterator cbegin() const { return data.cbegin(); }
    const_iterator cend() const { return data.cend(); }

private:
    storage_type data;
};

class visitor {
public:
    template <typename... Args>
    void examine_vertex(Args&&...) {}

    template <typename... Args>
    void tree_edge(Args&&...) {}

    template <typename... Args>
    void back_edge(Args&&...) {}
};


// Algorithms: Traversals

template <typename GraphWrapper, typename Edge>
class GenericBfsEdgeVisitor : public boost::default_bfs_visitor {
public:
    using NodeID = typename GraphWrapper::NodeType;

    GenericBfsEdgeVisitor(std::vector<std::pair<NodeID, NodeID>>& edges, const GraphWrapper& graph_wrapper)
        : tree_edges(edges), graph_wrapper(graph_wrapper) {}

    template <typename G>
    void tree_edge(Edge e, const G& g) {
        NodeID u = graph_wrapper.get_node_id(boost::source(e, g));
        NodeID v = graph_wrapper.get_node_id(boost::target(e, g));
        tree_edges.emplace_back(u, v);
    }
private:
    std::vector<std::pair<NodeID, NodeID>>& tree_edges;
    const GraphWrapper& graph_wrapper;
};

template <typename GraphWrapper>
[[deprecated("Use G.bfs_edges(start) instead.")]]
auto bfs_edges(const GraphWrapper& G, const typename GraphWrapper::NodeType& start) {
    return G.bfs_edges(start);
}

template <typename GraphWrapper>
[[deprecated("Use G.bfs_tree(start) instead.")]]
auto bfs_tree(const GraphWrapper& G, const typename GraphWrapper::NodeType& start) {
    return G.bfs_tree(start);
}

template <typename GraphWrapper>
[[deprecated("Use G.bfs_successors(start) instead.")]]
auto bfs_successors(const GraphWrapper& G, const typename GraphWrapper::NodeType& start) {
    return G.bfs_successors(start);
}

template <typename GraphWrapper, typename Edge, typename OnVertex, typename OnTreeEdge>
class GenericBfsVisitVisitor : public boost::default_bfs_visitor {
public:
    using NodeID = typename GraphWrapper::NodeType;

    GenericBfsVisitVisitor(const GraphWrapper& graph_wrapper, OnVertex& on_vertex, OnTreeEdge& on_tree_edge)
        : graph_wrapper(graph_wrapper), on_vertex(on_vertex), on_tree_edge(on_tree_edge) {}

    template <typename G>
    void examine_vertex(typename boost::graph_traits<G>::vertex_descriptor u, const G&) const {
        on_vertex(graph_wrapper.get_node_id(u));
    }

    template <typename G>
    void tree_edge(Edge e, const G& g) const {
        on_tree_edge(graph_wrapper.get_node_id(boost::source(e, g)), graph_wrapper.get_node_id(boost::target(e, g)));
    }

private:
    const GraphWrapper& graph_wrapper;
    OnVertex& on_vertex;
    OnTreeEdge& on_tree_edge;
};

template <typename Visitor, typename NodeID, typename GraphWrapper>
void bfs_dispatch_examine_vertex(Visitor& visitor, const NodeID& u, const GraphWrapper& G) {
    if constexpr (requires { visitor.examine_vertex(u, G); }) {
        visitor.examine_vertex(u, G);
    }
}

template <typename Visitor, typename NodeID, typename GraphWrapper>
void bfs_dispatch_tree_edge(Visitor& visitor, const NodeID& u, const NodeID& v, const GraphWrapper& G) {
    if constexpr (requires { visitor.tree_edge(u, v, G); }) {
        visitor.tree_edge(u, v, G);
    }
}

template <typename NodeID, typename Edge, typename GraphWrapper, typename Visitor>
class GenericBfsObjectVisitor : public boost::default_bfs_visitor {
public:
    GenericBfsObjectVisitor(const GraphWrapper& graph_wrapper, Visitor& visitor)
        : graph_wrapper(graph_wrapper), visitor(visitor) {}

    template <typename G>
    void examine_vertex(typename boost::graph_traits<G>::vertex_descriptor u, const G&) const {
        bfs_dispatch_examine_vertex(visitor, graph_wrapper.get_node_id(u), graph_wrapper);
    }

    template <typename G>
    void tree_edge(Edge e, const G& g) const {
        bfs_dispatch_tree_edge(
            visitor,
            graph_wrapper.get_node_id(boost::source(e, g)),
            graph_wrapper.get_node_id(boost::target(e, g)),
            graph_wrapper
        );
    }

private:
    const GraphWrapper& graph_wrapper;
    Visitor& visitor;
};

template <typename GraphWrapper, typename Visitor>
[[deprecated("Use G.breadth_first_search(start, visitor) instead.")]]
void breadth_first_search(const GraphWrapper& G, const typename GraphWrapper::NodeType& start, Visitor& visitor) {
    G.breadth_first_search(start, visitor);
}

template <typename GraphWrapper, typename OnVertex, typename OnTreeEdge>
[[deprecated("Use G.bfs_visit(start, on_vertex, on_tree_edge) instead.")]]
void bfs_visit(const GraphWrapper& G, const typename GraphWrapper::NodeType& start, OnVertex&& on_vertex, OnTreeEdge&& on_tree_edge) {
    G.bfs_visit(start, std::forward<OnVertex>(on_vertex), std::forward<OnTreeEdge>(on_tree_edge));
}

template <typename GraphWrapper, typename Edge>
class GenericDfsEdgeVisitor : public boost::default_dfs_visitor {
public:
    using NodeID = typename GraphWrapper::NodeType;

    GenericDfsEdgeVisitor(std::vector<std::pair<NodeID, NodeID>>& edges, const GraphWrapper& graph_wrapper)
        : tree_edges(edges), graph_wrapper(graph_wrapper) {}

    template <typename G>
    void tree_edge(Edge e, const G& g) {
        NodeID u = graph_wrapper.get_node_id(boost::source(e, g));
        NodeID v = graph_wrapper.get_node_id(boost::target(e, g));
        tree_edges.emplace_back(u, v);
    }
private:
    std::vector<std::pair<NodeID, NodeID>>& tree_edges;
    const GraphWrapper& graph_wrapper;
};

template <typename GraphWrapper>
[[deprecated("Use G.dfs_edges(start) instead.")]]
auto dfs_edges(const GraphWrapper& G, const typename GraphWrapper::NodeType& start) {
    return G.dfs_edges(start);
}

template <typename GraphWrapper>
[[deprecated("Use G.dfs_tree(start) instead.")]]
auto dfs_tree(const GraphWrapper& G, const typename GraphWrapper::NodeType& start) {
    return G.dfs_tree(start);
}

template <typename GraphWrapper>
[[deprecated("Use G.dfs_predecessors(start) instead.")]]
auto dfs_predecessors(const GraphWrapper& G, const typename GraphWrapper::NodeType& start) {
    return G.dfs_predecessors(start);
}

template <typename GraphWrapper>
[[deprecated("Use G.dfs_successors(start) instead.")]]
auto dfs_successors(const GraphWrapper& G, const typename GraphWrapper::NodeType& start) {
    return G.dfs_successors(start);
}

template <typename GraphWrapper, typename Distance, typename VertexDesc>
std::unordered_map<typename GraphWrapper::NodeType, std::vector<typename GraphWrapper::NodeType>> build_single_source_paths(
    const GraphWrapper& graph_wrapper,
    const std::vector<Distance>& dist,
    const std::vector<VertexDesc>& pred
) {
    using NodeID = typename GraphWrapper::NodeType;
    std::unordered_map<NodeID, std::vector<NodeID>> paths;
    const auto unreachable = std::numeric_limits<Distance>::max();
    const auto& bgl_to_id = graph_wrapper.get_bgl_to_id_map();

    for (size_t i = 0; i < bgl_to_id.size(); ++i) {
        const NodeID target_id = bgl_to_id[i];
        if (dist[i] == unreachable) {
            continue;
        }

        std::vector<NodeID> path;
        VertexDesc current = graph_wrapper.get_id_to_bgl_map().at(target_id);
        while (true) {
            path.push_back(graph_wrapper.get_node_id(current));
            const auto current_index = graph_wrapper.get_vertex_index(current);
            if (pred[current_index] == current) {
                break;
            }
            current = pred[current_index];
        }
        std::reverse(path.begin(), path.end());
        paths[target_id] = std::move(path);
    }

    return paths;
}

template <typename Distance>
Distance normalize_weighted_distance(Distance value) {
    if constexpr (std::is_integral_v<Distance> && std::is_signed_v<Distance>) {
        if (value == std::numeric_limits<Distance>::lowest()) {
            return std::numeric_limits<Distance>::max();
        }
    }
    return value;
}

template <typename Distance>
using shortest_path_calc_type = std::conditional_t<std::is_integral_v<Distance>, double, Distance>;

template <typename Distance, typename CalcDistance>
Distance convert_shortest_path_distance(CalcDistance value) {
    if constexpr (std::is_same_v<CalcDistance, Distance>) {
        return normalize_weighted_distance(value);
    } else {
        if constexpr (std::is_integral_v<Distance> && std::is_floating_point_v<CalcDistance>) {
            if (!std::isfinite(value) || value >= static_cast<CalcDistance>(std::numeric_limits<Distance>::max())) {
                return std::numeric_limits<Distance>::max();
            }
            if (value <= static_cast<CalcDistance>(std::numeric_limits<Distance>::lowest())) {
                return std::numeric_limits<Distance>::lowest();
            }
        }
        if (value == std::numeric_limits<CalcDistance>::max()) {
            return std::numeric_limits<Distance>::max();
        }
        return static_cast<Distance>(value);
    }
}

template <typename GraphWrapper, typename Edge, typename OnTreeEdge, typename OnBackEdge>
class GenericDfsVisitVisitor : public boost::default_dfs_visitor {
public:
    GenericDfsVisitVisitor(const GraphWrapper& graph_wrapper, OnTreeEdge& on_tree_edge, OnBackEdge& on_back_edge)
        : graph_wrapper(graph_wrapper), on_tree_edge(on_tree_edge), on_back_edge(on_back_edge) {}

    template <typename G>
    void tree_edge(Edge e, const G& g) const {
        on_tree_edge(graph_wrapper.get_node_id(boost::source(e, g)), graph_wrapper.get_node_id(boost::target(e, g)));
    }

    template <typename G>
    void back_edge(Edge e, const G& g) const {
        on_back_edge(graph_wrapper.get_node_id(boost::source(e, g)), graph_wrapper.get_node_id(boost::target(e, g)));
    }

private:
    const GraphWrapper& graph_wrapper;
    OnTreeEdge& on_tree_edge;
    OnBackEdge& on_back_edge;
};

template <typename Visitor, typename NodeID, typename GraphWrapper>
void dfs_dispatch_tree_edge(Visitor& visitor, const NodeID& u, const NodeID& v, const GraphWrapper& G) {
    if constexpr (requires { visitor.tree_edge(u, v, G); }) {
        visitor.tree_edge(u, v, G);
    }
}

template <typename Visitor, typename NodeID, typename GraphWrapper>
void dfs_dispatch_back_edge(Visitor& visitor, const NodeID& u, const NodeID& v, const GraphWrapper& G) {
    if constexpr (requires { visitor.back_edge(u, v, G); }) {
        visitor.back_edge(u, v, G);
    }
}

template <typename NodeID, typename Edge, typename GraphWrapper, typename Visitor>
class GenericDfsObjectVisitor : public boost::default_dfs_visitor {
public:
    GenericDfsObjectVisitor(const GraphWrapper& graph_wrapper, Visitor& visitor)
        : graph_wrapper(graph_wrapper), visitor(visitor) {}

    template <typename G>
    void tree_edge(Edge e, const G& g) const {
        dfs_dispatch_tree_edge(
            visitor,
            graph_wrapper.get_node_id(boost::source(e, g)),
            graph_wrapper.get_node_id(boost::target(e, g)),
            graph_wrapper
        );
    }

    template <typename G>
    void back_edge(Edge e, const G& g) const {
        dfs_dispatch_back_edge(
            visitor,
            graph_wrapper.get_node_id(boost::source(e, g)),
            graph_wrapper.get_node_id(boost::target(e, g)),
            graph_wrapper
        );
    }

private:
    const GraphWrapper& graph_wrapper;
    Visitor& visitor;
};

template <typename GraphWrapper, typename Visitor>
[[deprecated("Use G.depth_first_search(start, visitor) instead.")]]
void depth_first_search(const GraphWrapper& G, const typename GraphWrapper::NodeType& start, Visitor& visitor) {
    G.depth_first_search(start, visitor);
}

template <typename GraphWrapper, typename OnTreeEdge, typename OnBackEdge>
[[deprecated("Use G.dfs_visit(start, on_tree_edge, on_back_edge) instead.")]]
void dfs_visit(const GraphWrapper& G, const typename GraphWrapper::NodeType& start, OnTreeEdge&& on_tree_edge, OnBackEdge&& on_back_edge) {
    G.dfs_visit(start, std::forward<OnTreeEdge>(on_tree_edge), std::forward<OnBackEdge>(on_back_edge));
}


// Algorithms: Shortest Paths

template <typename GraphWrapper>
[[deprecated("Use G.shortest_path(source, target) instead.")]]
auto shortest_path(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id) {
    return G.shortest_path(source_id, target_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.dijkstra_path(source, target) instead.")]]
auto dijkstra_path(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id) {
    return G.dijkstra_path(source_id, target_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.dijkstra_shortest_paths(source) instead.")]]
auto dijkstra_shortest_paths(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id) {
    return G.dijkstra_shortest_paths(source_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.dijkstra_shortest_paths(source) instead.")]]
auto single_source_dijkstra(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id) {
    return G.dijkstra_shortest_paths(source_id);
}

template <typename GraphWrapper>
[[deprecated("Use G.shortest_path(source, target, weight) instead.")]]
auto shortest_path(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& weight) {
    return G.shortest_path(source_id, target_id, weight);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.dijkstra_path(source, target, weight) instead.")]]
auto dijkstra_path(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& weight) {
    return G.dijkstra_path(source_id, target_id, weight);
}

template <typename GraphWrapper>
[[deprecated("Use G.shortest_path_length(source, target) instead.")]]
double shortest_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id) {
    return G.shortest_path_length(source_id, target_id);
}

template <typename GraphWrapper>
[[deprecated("Use G.shortest_path_length(source, target, weight) instead.")]]
double shortest_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& weight) {
    return G.shortest_path_length(source_id, target_id, weight);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.dijkstra_path_length(source) instead.")]]
auto dijkstra_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id) {
    return G.dijkstra_path_length(source_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.dijkstra_path_length(source, target) instead.")]]
auto dijkstra_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id) {
    return G.dijkstra_path_length(source_id, target_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.dijkstra_path_length(source, target, weight) instead.")]]
auto dijkstra_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& weight) {
    return G.dijkstra_path_length(source_id, target_id, weight);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::bfs_edges(const NodeID& start) const {
    std::vector<std::pair<NodeID, NodeID>> edges;
    using EdgeType = typename boost::graph_traits<GraphType>::edge_descriptor;

    if (!has_node(start)) {
        throw std::runtime_error("Start node not found in graph");
    }

    const auto start_bgl = get_id_to_bgl_map().at(start);
    GenericBfsEdgeVisitor<Graph, EdgeType> vis(edges, *this);
    boost::breadth_first_search(get_impl(), start_bgl, boost::visitor(vis));
    return edges;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::bfs_tree(const NodeID& start) const {
    Graph<NodeID, double, Directed> tree;

    if (!has_node(start)) {
        throw std::runtime_error("Start node not found in graph");
    }

    tree.add_node(start);
    for (const auto& [u, v] : bfs_edges(start)) {
        tree.add_edge(u, v);
    }
    return tree;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::bfs_successors(const NodeID& start) const {
    if (!has_node(start)) throw std::runtime_error("Start node not found in graph");
    std::unordered_map<NodeID, std::vector<NodeID>> result;
    for (const auto& [u, v] : bfs_edges(start)) result[u].push_back(v);
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <typename Visitor>
void Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::breadth_first_search(const NodeID& start, Visitor& visitor) const {
    using EdgeType = typename boost::graph_traits<GraphType>::edge_descriptor;
    if (!has_node(start)) throw std::runtime_error("Start node not found in graph");
    const auto start_bgl = get_id_to_bgl_map().at(start);
    std::vector<boost::default_color_type> color(boost::num_vertices(g));
    GenericBfsObjectVisitor<NodeID, EdgeType, Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>, Visitor> vis(*this, visitor);
    boost::breadth_first_search(
        get_impl(),
        start_bgl,
        boost::visitor(vis).color_map(boost::make_iterator_property_map(color.begin(), vertex_index_map))
    );
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <typename OnVertex, typename OnTreeEdge>
void Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::bfs_visit(const NodeID& start, OnVertex&& on_vertex, OnTreeEdge&& on_tree_edge) const {
    struct callback_visitor {
        OnVertex& on_vertex;
        OnTreeEdge& on_tree_edge;
        void examine_vertex(const NodeID& u, const Graph&) { on_vertex(u); }
        void tree_edge(const NodeID& u, const NodeID& v, const Graph&) { on_tree_edge(u, v); }
    };
    auto on_vertex_fn = std::forward<OnVertex>(on_vertex);
    auto on_tree_edge_fn = std::forward<OnTreeEdge>(on_tree_edge);
    callback_visitor visitor{on_vertex_fn, on_tree_edge_fn};
    breadth_first_search(start, visitor);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dfs_edges(const NodeID& start) const {
    std::vector<std::pair<NodeID, NodeID>> edges;
    using EdgeType = typename boost::graph_traits<GraphType>::edge_descriptor;

    if (!has_node(start)) {
        throw std::runtime_error("Start node not found in graph");
    }

    const auto start_bgl = get_id_to_bgl_map().at(start);
    GenericDfsEdgeVisitor<Graph, EdgeType> vis(edges, *this);
    std::vector<boost::default_color_type> color(boost::num_vertices(g));
    boost::depth_first_search(
        get_impl(),
        boost::root_vertex(start_bgl)
            .visitor(vis)
            .color_map(boost::make_iterator_property_map(color.begin(), vertex_index_map))
    );
    return edges;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dfs_tree(const NodeID& start) const {
    Graph<NodeID, double, Directed> tree;

    if (!has_node(start)) {
        throw std::runtime_error("Start node not found in graph");
    }

    tree.add_node(start);
    for (const auto& [u, v] : dfs_edges(start)) {
        tree.add_edge(u, v);
    }
    return tree;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dfs_predecessors(const NodeID& start) const {
    if (!has_node(start)) throw std::runtime_error("Start node not found in graph");
    std::unordered_map<NodeID, NodeID> result;
    for (const auto& [u, v] : dfs_edges(start)) result[v] = u;
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dfs_successors(const NodeID& start) const {
    if (!has_node(start)) throw std::runtime_error("Start node not found in graph");
    std::unordered_map<NodeID, std::vector<NodeID>> result;
    for (const auto& [u, v] : dfs_edges(start)) result[u].push_back(v);
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <typename Visitor>
void Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::depth_first_search(const NodeID& start, Visitor& visitor) const {
    using EdgeType = typename boost::graph_traits<GraphType>::edge_descriptor;
    if (!has_node(start)) throw std::runtime_error("Start node not found in graph");
    const auto start_bgl = get_id_to_bgl_map().at(start);
    std::vector<boost::default_color_type> color(boost::num_vertices(g));
    GenericDfsObjectVisitor<NodeID, EdgeType, Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>, Visitor> vis(*this, visitor);
    boost::depth_first_search(
        get_impl(),
        boost::root_vertex(start_bgl)
            .visitor(vis)
            .color_map(boost::make_iterator_property_map(color.begin(), vertex_index_map))
    );
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <typename OnTreeEdge, typename OnBackEdge>
void Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dfs_visit(const NodeID& start, OnTreeEdge&& on_tree_edge, OnBackEdge&& on_back_edge) const {
    struct callback_visitor {
        OnTreeEdge& on_tree_edge;
        OnBackEdge& on_back_edge;
        void tree_edge(const NodeID& u, const NodeID& v, const Graph&) { on_tree_edge(u, v); }
        void back_edge(const NodeID& u, const NodeID& v, const Graph&) { on_back_edge(u, v); }
    };
    auto on_tree_edge_fn = std::forward<OnTreeEdge>(on_tree_edge);
    auto on_back_edge_fn = std::forward<OnBackEdge>(on_back_edge);
    callback_visitor visitor{on_tree_edge_fn, on_back_edge_fn};
    depth_first_search(start, visitor);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::shortest_path(const NodeID& source_id, const NodeID& target_id) const {
    if (id_to_bgl.find(source_id) == id_to_bgl.end() || id_to_bgl.find(target_id) == id_to_bgl.end()) {
        throw std::runtime_error("Source or target node not found in graph.");
    }

    const VertexDesc source = id_to_bgl.at(source_id);
    const VertexDesc target = id_to_bgl.at(target_id);

    if (source == target) {
        return std::vector<NodeID>{source_id};
    }

    const size_t n = boost::num_vertices(g);
    const auto source_index = get_vertex_index(source);
    const auto target_index = get_vertex_index(target);
    std::vector<bool> visited(n, false);
    std::vector<VertexDesc> pred(n);
    std::queue<VertexDesc> q;

    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
        pred[get_vertex_index(*v)] = *v;
    }

    visited[source_index] = true;
    q.push(source);

    while (!q.empty()) {
        const VertexDesc current = q.front();
        q.pop();

        for (auto [e, eend] = boost::out_edges(current, g); e != eend; ++e) {
            const VertexDesc next = boost::target(*e, g);
            const auto next_index = get_vertex_index(next);
            if (!visited[next_index]) {
                visited[next_index] = true;
                pred[next_index] = current;
                if (next == target) {
                    q = {};
                    break;
                }
                q.push(next);
            }
        }
    }

    if (!visited[target_index]) {
        throw std::runtime_error("Node not reachable");
    }

    std::vector<NodeID> path;
    for (VertexDesc curr = target; curr != source; curr = pred[get_vertex_index(curr)]) {
        path.push_back(get_node_id(curr));
    }
    path.push_back(get_node_id(source));
    std::reverse(path.begin(), path.end());
    return path;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::shortest_path(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const {
    if (weight.empty()) {
        return shortest_path(source_id, target_id);
    }
    if (weight == "weight") {
        if constexpr (Weighted) {
            return dijkstra_path(source_id, target_id);
        } else {
            throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
        }
    }
    throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
double Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::shortest_path_length(const NodeID& source_id, const NodeID& target_id) const {
    const auto path = shortest_path(source_id, target_id);
    if (path.empty()) {
        throw std::runtime_error("Node not reachable");
    }
    return static_cast<double>(path.size() - 1);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
double Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::shortest_path_length(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const {
    if (weight.empty()) {
        return shortest_path_length(source_id, target_id);
    }
    if (weight == "weight") {
        if constexpr (Weighted) {
            return dijkstra_path_length(source_id, target_id);
        } else {
            throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
        }
    }
    throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dijkstra_path(const NodeID& source_id, const NodeID& target_id) const {
    if (id_to_bgl.find(source_id) == id_to_bgl.end() || id_to_bgl.find(target_id) == id_to_bgl.end()) {
        throw std::runtime_error("Source or target node not found in graph.");
    }

    const VertexDesc source = id_to_bgl.at(source_id);
    const VertexDesc target = id_to_bgl.at(target_id);
    const size_t n = boost::num_vertices(g);
    const auto target_index = get_vertex_index(target);
    std::vector<EdgeWeight> dist(n);
    std::vector<VertexDesc> pred(n);

    boost::dijkstra_shortest_paths(
        g,
        source,
        boost::distance_map(boost::make_iterator_property_map(dist.begin(), vertex_index_map))
            .predecessor_map(boost::make_iterator_property_map(pred.begin(), vertex_index_map))
            .vertex_index_map(vertex_index_map)
    );

    if (pred[target_index] == target && source != target) {
        throw std::runtime_error("Node not reachable");
    }

    std::vector<NodeID> path;
    for (VertexDesc curr = target; curr != source; curr = pred[get_vertex_index(curr)]) {
        path.push_back(get_node_id(curr));
    }
    path.push_back(get_node_id(source));
    std::reverse(path.begin(), path.end());
    return path;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dijkstra_path(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const {
    if (weight.empty() || weight == "weight") {
        return dijkstra_path(source_id, target_id);
    }
    throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dijkstra_shortest_paths(const NodeID& source_id) const {
    if (id_to_bgl.find(source_id) == id_to_bgl.end()) {
        throw std::runtime_error("Source node not found in graph.");
    }

    const VertexDesc source = id_to_bgl.at(source_id);
    const size_t n = boost::num_vertices(g);
    std::vector<EdgeWeight> dist(n, std::numeric_limits<EdgeWeight>::max());
    std::vector<VertexDesc> pred(n);
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
        pred[get_vertex_index(*v)] = *v;
    }

    boost::dijkstra_shortest_paths(
        g,
        source,
        boost::distance_map(boost::make_iterator_property_map(dist.begin(), vertex_index_map))
            .predecessor_map(boost::make_iterator_property_map(pred.begin(), vertex_index_map))
            .vertex_index_map(vertex_index_map)
    );

    SingleSourceShortestPathResult<NodeID, EdgeWeight> result;
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
        const auto index = get_vertex_index(*v);
        const auto& id = get_node_id(*v);
        result.distance[id] = dist[index];
        result.predecessor[id] = get_node_id(pred[index]);
    }
    result.paths = build_single_source_paths(*this, dist, pred);
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dijkstra_path_length(const NodeID& source_id) const {
    return dijkstra_shortest_paths(source_id).distance;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dijkstra_path_length(const NodeID& source_id, const NodeID& target_id) const {
    const auto distances = dijkstra_path_length(source_id);
    const auto it = distances.find(target_id);
    if (it == distances.end()) {
        throw std::runtime_error("Target node not found in graph.");
    }
    return it->second;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dijkstra_path_length(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const {
    if (weight.empty() || weight == "weight") {
        return dijkstra_path_length(source_id, target_id);
    }
    throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.bellman_ford_path(source, target) instead.")]]
auto bellman_ford_path(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id) {
    return G.bellman_ford_path(source_id, target_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.bellman_ford_shortest_paths(source) instead.")]]
auto bellman_ford_shortest_paths(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id) {
    return G.bellman_ford_shortest_paths(source_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.bellman_ford_shortest_paths(source) instead.")]]
auto single_source_bellman_ford(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id) {
    return G.bellman_ford_shortest_paths(source_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.bellman_ford_path(source, target, weight) instead.")]]
auto bellman_ford_path(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& weight) {
    return G.bellman_ford_path(source_id, target_id, weight);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.bellman_ford_path_length(source, target) instead.")]]
auto bellman_ford_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id) {
    return G.bellman_ford_path_length(source_id, target_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.bellman_ford_path_length(source, target, weight) instead.")]]
auto bellman_ford_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& weight) {
    return G.bellman_ford_path_length(source_id, target_id, weight);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.dag_shortest_paths(source) instead.")]]
auto dag_shortest_paths(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id) {
    return G.dag_shortest_paths(source_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.floyd_warshall_all_pairs_shortest_paths() instead.")]]
auto floyd_warshall_all_pairs_shortest_paths(const GraphWrapper& G) {
    return G.floyd_warshall_all_pairs_shortest_paths();
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.floyd_warshall_all_pairs_shortest_paths_map() instead.")]]
auto floyd_warshall_all_pairs_shortest_paths_map(const GraphWrapper& G) {
    return G.floyd_warshall_all_pairs_shortest_paths_map();
}


// Algorithms: Components

template <typename GraphWrapper>
[[deprecated("Use G.connected_component_groups() instead.")]]
auto connected_component_groups(const GraphWrapper& G) {
    return G.connected_component_groups();
}

template <typename GraphWrapper>
[[deprecated("Use G.connected_components() instead.")]]
auto connected_components(const GraphWrapper& G) {
    return G.connected_components();
}

template <typename GraphWrapper>
[[deprecated("Use G.strongly_connected_component_groups() instead.")]]
auto strongly_connected_component_groups(const GraphWrapper& G) {
    return G.strongly_connected_component_groups();
}

template <typename GraphWrapper>
[[deprecated("Use G.strong_component_map() instead.")]]
auto strong_component_map(const GraphWrapper& G) {
    return G.strong_component_map();
}

template <typename GraphWrapper>
[[deprecated("Use G.strong_components() instead.")]]
auto strong_components(const GraphWrapper& G) {
    return G.strong_components();
}

template <typename GraphWrapper>
[[deprecated("Use G.connected_component_map() instead.")]]
auto connected_component_map(const GraphWrapper& G) {
    return G.connected_component_map();
}

template <typename GraphWrapper>
[[deprecated("Use G.strongly_connected_components() instead.")]]
auto strongly_connected_components(const GraphWrapper& G) {
    return G.strongly_connected_components();
}

template <typename GraphWrapper>
[[deprecated("Use G.strongly_connected_component_map() instead.")]]
auto strongly_connected_component_map(const GraphWrapper& G) {
    return G.strongly_connected_component_map();
}

template <typename GraphWrapper>
[[deprecated("Use G.strongly_connected_component_roots() instead.")]]
auto strongly_connected_component_roots(const GraphWrapper& G) {
    return G.strongly_connected_component_roots();
}

template <typename GraphWrapper>
[[deprecated("Use G.topological_sort() instead.")]]
auto topological_sort(const GraphWrapper& G) {
    return G.topological_sort();
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.kruskal_minimum_spanning_tree() instead.")]]
auto kruskal_minimum_spanning_tree(const GraphWrapper& G) {
    return G.kruskal_minimum_spanning_tree();
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.prim_minimum_spanning_tree(root) instead.")]]
auto prim_minimum_spanning_tree(const GraphWrapper& G, const typename GraphWrapper::NodeType& root_id) {
    return G.prim_minimum_spanning_tree(root_id);
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.minimum_spanning_tree() instead.")]]
auto minimum_spanning_tree(const GraphWrapper& G) {
    return G.minimum_spanning_tree();
}

template <typename GraphWrapper>
requires(GraphWrapper::has_builtin_edge_weight)
[[deprecated("Use G.minimum_spanning_tree(root) instead.")]]
auto minimum_spanning_tree(const GraphWrapper& G, const typename GraphWrapper::NodeType& root_id) {
    return G.minimum_spanning_tree(root_id);
}

template <typename GraphWrapper>
[[deprecated("Use G.edmonds_karp_maximum_flow(source, target, capacity_attr) instead.")]]
auto edmonds_karp_maximum_flow(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity") {
    return G.edmonds_karp_maximum_flow(source_id, target_id, capacity_attr);
}

template <typename GraphWrapper>
[[deprecated("Use G.push_relabel_maximum_flow_result(source, target, capacity_attr) instead.")]]
auto push_relabel_maximum_flow_result(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity") {
    return G.push_relabel_maximum_flow_result(source_id, target_id, capacity_attr);
}

template <typename GraphWrapper>
[[deprecated("Use G.maximum_flow(source, target, capacity_attr) instead.")]]
auto maximum_flow(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity") {
    return G.maximum_flow(source_id, target_id, capacity_attr);
}

template <typename GraphWrapper>
[[deprecated("Use G.minimum_cut(source, target, capacity_attr) instead.")]]
auto minimum_cut(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity") {
    return G.minimum_cut(source_id, target_id, capacity_attr);
}

template <typename GraphWrapper>
[[deprecated("Use G.max_flow_min_cost_cycle_canceling(source, target, capacity_attr, weight_attr) instead.")]]
auto max_flow_min_cost_cycle_canceling(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") {
    return G.max_flow_min_cost_cycle_canceling(source_id, target_id, capacity_attr, weight_attr);
}

template <typename GraphWrapper>
[[deprecated("Use G.push_relabel_maximum_flow(source, target, capacity_attr, weight_attr) instead.")]]
long push_relabel_maximum_flow(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") {
    return G.push_relabel_maximum_flow(source_id, target_id, capacity_attr, weight_attr);
}

template <typename GraphWrapper>
[[deprecated("Use G.cycle_canceling(weight_attr) instead.")]]
auto cycle_canceling(const GraphWrapper& G, const std::string& weight_attr = "weight") {
    return G.cycle_canceling(weight_attr);
}

template <typename GraphWrapper>
[[deprecated("Use G.successive_shortest_path_nonnegative_weights(source, target, capacity_attr, weight_attr) instead.")]]
auto successive_shortest_path_nonnegative_weights(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") {
    return G.successive_shortest_path_nonnegative_weights(source_id, target_id, capacity_attr, weight_attr);
}

template <typename GraphWrapper>
[[deprecated("Use G.max_flow_min_cost_successive_shortest_path(source, target, capacity_attr, weight_attr) instead.")]]
auto max_flow_min_cost_successive_shortest_path(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") {
    return G.max_flow_min_cost_successive_shortest_path(source_id, target_id, capacity_attr, weight_attr);
}
template <typename GraphWrapper>
[[deprecated("Use G.max_flow_min_cost(source, target, capacity_attr, weight_attr) instead.")]]
auto max_flow_min_cost(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") {
    return G.max_flow_min_cost(source_id, target_id, capacity_attr, weight_attr);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::bellman_ford_path(const NodeID& source_id, const NodeID& target_id) const {
    if (id_to_bgl.find(source_id) == id_to_bgl.end() || id_to_bgl.find(target_id) == id_to_bgl.end()) {
        throw std::runtime_error("Source or target node not found in graph.");
    }

    const VertexDesc source = id_to_bgl.at(source_id);
    const VertexDesc target = id_to_bgl.at(target_id);
    const size_t n = boost::num_vertices(g);
    const auto source_index = get_vertex_index(source);
    const auto target_index = get_vertex_index(target);
    std::vector<EdgeWeight> dist(n, std::numeric_limits<EdgeWeight>::max());
    std::vector<VertexDesc> pred(n);
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) pred[get_vertex_index(*v)] = *v;
    dist[source_index] = EdgeWeight{};

    const bool ok = boost::bellman_ford_shortest_paths(
        g,
        static_cast<int>(n),
        boost::weight_map(boost::get(boost::edge_weight, g))
            .distance_map(boost::make_iterator_property_map(dist.begin(), vertex_index_map))
            .predecessor_map(boost::make_iterator_property_map(pred.begin(), vertex_index_map))
            .root_vertex(source)
    );
    if (!ok) throw std::runtime_error("Bellman-Ford failed: negative cycle detected.");
    if (dist[target_index] == std::numeric_limits<EdgeWeight>::max()) throw std::runtime_error("Node not reachable");

    std::vector<NodeID> path;
    for (VertexDesc curr = target; curr != source; curr = pred[get_vertex_index(curr)]) path.push_back(get_node_id(curr));
    path.push_back(get_node_id(source));
    std::reverse(path.begin(), path.end());
    return path;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::bellman_ford_shortest_paths(const NodeID& source_id) const {
    if (id_to_bgl.find(source_id) == id_to_bgl.end()) {
        throw std::runtime_error("Source node not found in graph.");
    }

    const VertexDesc source = id_to_bgl.at(source_id);
    const size_t n = boost::num_vertices(g);
    const auto source_index = get_vertex_index(source);
    std::vector<EdgeWeight> dist(n, std::numeric_limits<EdgeWeight>::max());
    std::vector<VertexDesc> pred(n);
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) pred[get_vertex_index(*v)] = *v;
    dist[source_index] = EdgeWeight{};

    const bool ok = boost::bellman_ford_shortest_paths(
        g,
        static_cast<int>(n),
        boost::weight_map(boost::get(boost::edge_weight, g))
            .distance_map(boost::make_iterator_property_map(dist.begin(), vertex_index_map))
            .predecessor_map(boost::make_iterator_property_map(pred.begin(), vertex_index_map))
            .root_vertex(source)
    );
    if (!ok) throw std::runtime_error("Bellman-Ford failed: negative cycle detected.");

    SingleSourceShortestPathResult<NodeID, EdgeWeight> result;
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
        const auto index = get_vertex_index(*v);
        const auto& id = get_node_id(*v);
        result.distance[id] = dist[index];
        result.predecessor[id] = get_node_id(pred[index]);
    }
    result.paths = build_single_source_paths(*this, dist, pred);
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::bellman_ford_path(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const {
    if (weight.empty() || weight == "weight") return bellman_ford_path(source_id, target_id);
    throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::bellman_ford_path_length(const NodeID& source_id, const NodeID& target_id) const {
    EdgeWeight total{};
    const auto path = bellman_ford_path(source_id, target_id);
    for (size_t i = 0; i + 1 < path.size(); ++i) total += get_edge_weight(path[i], path[i + 1]);
    return total;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::bellman_ford_path_length(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const {
    if (weight.empty() || weight == "weight") return bellman_ford_path_length(source_id, target_id);
    throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dag_shortest_paths(const NodeID& source_id) const {
    using CalcDistance = shortest_path_calc_type<EdgeWeight>;
    if (id_to_bgl.find(source_id) == id_to_bgl.end()) throw std::runtime_error("Source node not found in graph.");

    const VertexDesc source = id_to_bgl.at(source_id);
    const size_t n = boost::num_vertices(g);
    std::vector<CalcDistance> dist(n, std::numeric_limits<CalcDistance>::max());
    std::vector<VertexDesc> pred(n);
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) pred[get_vertex_index(*v)] = *v;

    boost::dag_shortest_paths(
        g, source,
        boost::distance_map(boost::make_iterator_property_map(dist.begin(), vertex_index_map))
            .predecessor_map(boost::make_iterator_property_map(pred.begin(), vertex_index_map))
            .distance_compare(std::less<CalcDistance>())
            .distance_combine(boost::closed_plus<CalcDistance>(std::numeric_limits<CalcDistance>::max()))
            .distance_inf(std::numeric_limits<CalcDistance>::max())
            .distance_zero(CalcDistance{})
    );

    std::vector<EdgeWeight> normalized_dist(n);
    for (size_t i = 0; i < n; ++i) normalized_dist[i] = convert_shortest_path_distance<EdgeWeight>(dist[i]);

    SingleSourceShortestPathResult<NodeID, EdgeWeight> result;
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
        const auto index = get_vertex_index(*v);
        const auto& id = get_node_id(*v);
        result.distance[id] = normalized_dist[index];
        result.predecessor[id] = get_node_id(pred[index]);
    }
    result.paths = build_single_source_paths(*this, normalized_dist, pred);
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::floyd_warshall_all_pairs_shortest_paths() const {
    using CalcDistance = std::conditional_t<std::is_integral_v<EdgeWeight>, long long, EdgeWeight>;
    const size_t n = boost::num_vertices(g);
    const CalcDistance inf = std::numeric_limits<CalcDistance>::max() / 4;
    std::vector<std::vector<CalcDistance>> internal_matrix(n, std::vector<CalcDistance>(n, inf));
    for (size_t i = 0; i < n; ++i) internal_matrix[i][i] = CalcDistance{};

    for (auto [e, eend] = boost::edges(g); e != eend; ++e) {
        const size_t u = get_vertex_index(boost::source(*e, g));
        const size_t v = get_vertex_index(boost::target(*e, g));
        const CalcDistance w = static_cast<CalcDistance>(boost::get(boost::edge_weight, g, *e));
        internal_matrix[u][v] = std::min(internal_matrix[u][v], w);
        if constexpr (!Directed) internal_matrix[v][u] = std::min(internal_matrix[v][u], w);
    }

    for (size_t k = 0; k < n; ++k) for (size_t i = 0; i < n; ++i) if (internal_matrix[i][k] != inf)
        for (size_t j = 0; j < n; ++j) if (internal_matrix[k][j] != inf) {
            const CalcDistance through_k = internal_matrix[i][k] + internal_matrix[k][j];
            if (through_k < internal_matrix[i][j]) internal_matrix[i][j] = through_k;
        }

    std::vector<size_t> order(n);
    for (size_t i = 0; i < n; ++i) order[i] = i;
    if constexpr (std::is_arithmetic_v<NodeID> || std::is_same_v<NodeID, std::string>) {
        std::sort(order.begin(), order.end(), [&](size_t lhs, size_t rhs) { return bgl_to_id[lhs] < bgl_to_id[rhs]; });
    }

    std::vector<std::vector<EdgeWeight>> matrix(n, std::vector<EdgeWeight>(n));
    for (size_t i = 0; i < n; ++i) for (size_t j = 0; j < n; ++j)
        matrix[i][j] = internal_matrix[order[i]][order[j]] == inf ? std::numeric_limits<EdgeWeight>::max() : static_cast<EdgeWeight>(internal_matrix[order[i]][order[j]]);
    return matrix;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <bool W>
requires(W)
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::floyd_warshall_all_pairs_shortest_paths_map() const {
    const size_t n = boost::num_vertices(g);
    std::vector<size_t> order(n);
    for (size_t i = 0; i < n; ++i) order[i] = i;
    if constexpr (std::is_arithmetic_v<NodeID> || std::is_same_v<NodeID, std::string>) {
        std::sort(order.begin(), order.end(), [&](size_t lhs, size_t rhs) { return bgl_to_id[lhs] < bgl_to_id[rhs]; });
    }
    const auto matrix = floyd_warshall_all_pairs_shortest_paths();
    std::unordered_map<NodeID, std::unordered_map<NodeID, EdgeWeight>> result;
    for (size_t i = 0; i < n; ++i) for (size_t j = 0; j < n; ++j) result[bgl_to_id[order[i]]][bgl_to_id[order[j]]] = matrix[i][j];
    return result;
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
    lookup_map<NodeID, int> result;
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) result[get_node_id(*v)] = comp[get_vertex_index(*v)];
    return result;
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
    lookup_map<NodeID, int> result;
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) result[get_node_id(*v)] = comp[get_vertex_index(*v)];
    return result;
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
    std::unordered_map<NodeID, NodeID> result;
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
        const auto index = get_vertex_index(*v);
        result[get_node_id(*v)] = get_node_id(roots[index]);
    }
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::connected_component_map() const { return connected_components(); }

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::strongly_connected_components() const { return strongly_connected_component_groups(); }

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::strongly_connected_component_map() const { return strong_component_map(); }

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::strongly_connected_component_roots() const { return strong_components(); }

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
    for (auto it = rev_order.rbegin(); it != rev_order.rend(); ++it) order.push_back(get_node_id(*it));
    return order;
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
    if (id_to_bgl.find(root_id) == id_to_bgl.end()) throw std::runtime_error("Root node not found in graph.");
    std::vector<VertexDesc> parent(boost::num_vertices(g));
    std::vector<boost::default_color_type> color(boost::num_vertices(g));
    boost::prim_minimum_spanning_tree(
        g,
        boost::make_iterator_property_map(parent.begin(), vertex_index_map),
        boost::root_vertex(id_to_bgl.at(root_id))
            .vertex_index_map(vertex_index_map)
            .color_map(boost::make_iterator_property_map(color.begin(), vertex_index_map))
    );
    std::unordered_map<NodeID, NodeID> result;
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

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::edmonds_karp_maximum_flow(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr) const {
    if (!has_node(source_id) || !has_node(target_id)) throw std::runtime_error("Source or target node not found in graph.");
    using FlowTraits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
    using FlowGraph = boost::adjacency_list<
        boost::vecS, boost::vecS, boost::directedS, boost::no_property,
        boost::property<boost::edge_capacity_t, long,
            boost::property<boost::edge_residual_capacity_t, long,
                boost::property<boost::edge_reverse_t, typename FlowTraits::edge_descriptor>>>>;
    using CapacityMap = typename boost::property_map<FlowGraph, boost::edge_capacity_t>::type;
    using ResidualMap = typename boost::property_map<FlowGraph, boost::edge_residual_capacity_t>::type;
    using ReverseMap = typename boost::property_map<FlowGraph, boost::edge_reverse_t>::type;
    using FlowEdgeDesc = typename boost::graph_traits<FlowGraph>::edge_descriptor;

    FlowGraph flow_graph;
    CapacityMap capacity = boost::get(boost::edge_capacity, flow_graph);
    ReverseMap reverse = boost::get(boost::edge_reverse, flow_graph);
    const auto all_nodes = nodes();
    std::unordered_map<NodeID, std::size_t> node_to_index;
    for (std::size_t i = 0; i < all_nodes.size(); ++i) { boost::add_vertex(flow_graph); node_to_index[all_nodes[i]] = i; }

    std::map<std::pair<NodeID, NodeID>, FlowEdgeDesc> original_edges;
    for (const auto& [u, v] : edge_pairs()) {
        auto [e, added] = boost::add_edge(node_to_index.at(u), node_to_index.at(v), flow_graph);
        auto [rev, rev_added] = boost::add_edge(node_to_index.at(v), node_to_index.at(u), flow_graph);
        (void)added; (void)rev_added;
        capacity[e] = static_cast<long>(get_edge_numeric_attr(u, v, capacity_attr));
        capacity[rev] = 0;
        reverse[e] = rev;
        reverse[rev] = e;
        original_edges[{u, v}] = e;
    }

    const long flow_value = boost::edmonds_karp_max_flow(flow_graph, node_to_index.at(source_id), node_to_index.at(target_id));
    ResidualMap residual = boost::get(boost::edge_residual_capacity, flow_graph);
    MaximumFlowResult<NodeID> result;
    result.value = flow_value;
    for (const auto& [key, edge_desc] : original_edges) result.flow[key] = capacity[edge_desc] - residual[edge_desc];
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::push_relabel_maximum_flow_result(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr) const {
    if (!has_node(source_id) || !has_node(target_id)) throw std::runtime_error("Source or target node not found in graph.");
    using FlowTraits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
    using FlowGraph = boost::adjacency_list<
        boost::vecS, boost::vecS, boost::directedS, boost::no_property,
        boost::property<boost::edge_capacity_t, long,
            boost::property<boost::edge_residual_capacity_t, long,
                boost::property<boost::edge_reverse_t, typename FlowTraits::edge_descriptor>>>>;
    using CapacityMap = typename boost::property_map<FlowGraph, boost::edge_capacity_t>::type;
    using ResidualMap = typename boost::property_map<FlowGraph, boost::edge_residual_capacity_t>::type;
    using ReverseMap = typename boost::property_map<FlowGraph, boost::edge_reverse_t>::type;
    using FlowEdgeDesc = typename boost::graph_traits<FlowGraph>::edge_descriptor;

    FlowGraph flow_graph;
    CapacityMap capacity = boost::get(boost::edge_capacity, flow_graph);
    ReverseMap reverse = boost::get(boost::edge_reverse, flow_graph);
    const auto all_nodes = nodes();
    std::unordered_map<NodeID, std::size_t> node_to_index;
    for (std::size_t i = 0; i < all_nodes.size(); ++i) { boost::add_vertex(flow_graph); node_to_index[all_nodes[i]] = i; }

    std::map<std::pair<NodeID, NodeID>, FlowEdgeDesc> original_edges;
    for (const auto& [u, v] : edge_pairs()) {
        auto [e, added] = boost::add_edge(node_to_index.at(u), node_to_index.at(v), flow_graph);
        auto [rev, rev_added] = boost::add_edge(node_to_index.at(v), node_to_index.at(u), flow_graph);
        (void)added; (void)rev_added;
        capacity[e] = static_cast<long>(get_edge_numeric_attr(u, v, capacity_attr));
        capacity[rev] = 0;
        reverse[e] = rev;
        reverse[rev] = e;
        original_edges[{u, v}] = e;
    }

    const long flow_value = boost::push_relabel_max_flow(flow_graph, node_to_index.at(source_id), node_to_index.at(target_id));
    ResidualMap residual = boost::get(boost::edge_residual_capacity, flow_graph);
    MaximumFlowResult<NodeID> result;
    result.value = flow_value;
    for (const auto& [key, edge_desc] : original_edges) result.flow[key] = capacity[edge_desc] - residual[edge_desc];
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::maximum_flow(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr) const { return edmonds_karp_maximum_flow(source_id, target_id, capacity_attr); }

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::minimum_cut(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr) const {
    if (!has_node(source_id) || !has_node(target_id)) throw std::runtime_error("Source or target node not found in graph.");
    using FlowTraits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
    using FlowGraph = boost::adjacency_list<
        boost::vecS, boost::vecS, boost::directedS, boost::no_property,
        boost::property<boost::edge_capacity_t, long,
            boost::property<boost::edge_residual_capacity_t, long,
                boost::property<boost::edge_reverse_t, typename FlowTraits::edge_descriptor>>>>;
    using CapacityMap = typename boost::property_map<FlowGraph, boost::edge_capacity_t>::type;
    using ResidualMap = typename boost::property_map<FlowGraph, boost::edge_residual_capacity_t>::type;
    using ReverseMap = typename boost::property_map<FlowGraph, boost::edge_reverse_t>::type;

    FlowGraph flow_graph;
    CapacityMap capacity = boost::get(boost::edge_capacity, flow_graph);
    ReverseMap reverse = boost::get(boost::edge_reverse, flow_graph);
    const auto all_nodes = nodes();
    std::unordered_map<NodeID, std::size_t> node_to_index;
    std::vector<NodeID> index_to_node;
    index_to_node.reserve(all_nodes.size());
    for (std::size_t i = 0; i < all_nodes.size(); ++i) { boost::add_vertex(flow_graph); node_to_index[all_nodes[i]] = i; index_to_node.push_back(all_nodes[i]); }
    for (const auto& [u, v] : edge_pairs()) {
        auto [e, added] = boost::add_edge(node_to_index.at(u), node_to_index.at(v), flow_graph);
        auto [rev, rev_added] = boost::add_edge(node_to_index.at(v), node_to_index.at(u), flow_graph);
        (void)added; (void)rev_added;
        capacity[e] = static_cast<long>(get_edge_numeric_attr(u, v, capacity_attr));
        capacity[rev] = 0;
        reverse[e] = rev;
        reverse[rev] = e;
    }
    const auto source_index = node_to_index.at(source_id);
    const auto target_index = node_to_index.at(target_id);
    const long cut_value = boost::edmonds_karp_max_flow(flow_graph, source_index, target_index);
    ResidualMap residual = boost::get(boost::edge_residual_capacity, flow_graph);
    std::vector<bool> visited(all_nodes.size(), false);
    std::queue<std::size_t> q;
    visited[source_index] = true;
    q.push(source_index);
    while (!q.empty()) {
        const auto current = q.front();
        q.pop();
        for (auto [eit, eend] = boost::out_edges(current, flow_graph); eit != eend; ++eit) {
            const auto next = boost::target(*eit, flow_graph);
            if (!visited[next] && residual[*eit] > 0) { visited[next] = true; q.push(next); }
        }
    }
    MinimumCutResult<NodeID> result;
    result.value = cut_value;
    for (std::size_t i = 0; i < index_to_node.size(); ++i) {
        if (visited[i]) result.reachable.push_back(index_to_node[i]);
        else result.non_reachable.push_back(index_to_node[i]);
    }
    for (const auto& [u, v] : edge_pairs()) if (visited[node_to_index.at(u)] && !visited[node_to_index.at(v)]) result.cut_edges.emplace_back(u, v);
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::max_flow_min_cost_cycle_canceling(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr, const std::string& weight_attr) const {
    if (!has_node(source_id) || !has_node(target_id)) throw std::runtime_error("Source or target node not found in graph.");
    using FlowTraits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
    using FlowGraph = boost::adjacency_list<
        boost::vecS, boost::vecS, boost::directedS, boost::no_property,
        boost::property<boost::edge_weight_t, long,
            boost::property<boost::edge_capacity_t, long,
                boost::property<boost::edge_residual_capacity_t, long,
                    boost::property<boost::edge_reverse_t, typename FlowTraits::edge_descriptor>>>>>;
    using WeightMap = typename boost::property_map<FlowGraph, boost::edge_weight_t>::type;
    using CapacityMap = typename boost::property_map<FlowGraph, boost::edge_capacity_t>::type;
    using ResidualMap = typename boost::property_map<FlowGraph, boost::edge_residual_capacity_t>::type;
    using ReverseMap = typename boost::property_map<FlowGraph, boost::edge_reverse_t>::type;
    using FlowEdgeDesc = typename boost::graph_traits<FlowGraph>::edge_descriptor;

    FlowGraph flow_graph;
    WeightMap weight = boost::get(boost::edge_weight, flow_graph);
    CapacityMap capacity = boost::get(boost::edge_capacity, flow_graph);
    ReverseMap reverse = boost::get(boost::edge_reverse, flow_graph);
    const auto all_nodes = nodes();
    std::unordered_map<NodeID, std::size_t> node_to_index;
    for (std::size_t i = 0; i < all_nodes.size(); ++i) { boost::add_vertex(flow_graph); node_to_index[all_nodes[i]] = i; }
    std::map<std::pair<NodeID, NodeID>, FlowEdgeDesc> original_edges;
    for (const auto& [u, v] : edge_pairs()) {
        auto [e, added] = boost::add_edge(node_to_index.at(u), node_to_index.at(v), flow_graph);
        auto [rev, rev_added] = boost::add_edge(node_to_index.at(v), node_to_index.at(u), flow_graph);
        (void)added; (void)rev_added;
        weight[e] = static_cast<long>(get_edge_numeric_attr(u, v, weight_attr));
        weight[rev] = -weight[e];
        capacity[e] = static_cast<long>(get_edge_numeric_attr(u, v, capacity_attr));
        capacity[rev] = 0;
        reverse[e] = rev;
        reverse[rev] = e;
        original_edges[{u, v}] = e;
    }
    boost::push_relabel_max_flow(flow_graph, node_to_index.at(source_id), node_to_index.at(target_id));
    boost::cycle_canceling(flow_graph);
    const long cost = boost::find_flow_cost(flow_graph);
    ResidualMap residual = boost::get(boost::edge_residual_capacity, flow_graph);
    MinCostMaxFlowResult<NodeID> result;
    result.cost = cost;
    for (const auto& [key, edge_desc] : original_edges) {
        result.edge_flows[key] = capacity[edge_desc] - residual[edge_desc];
        if (key.first == source_id) result.flow += result.edge_flows[key];
    }
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
long Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::push_relabel_maximum_flow(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr, const std::string& weight_attr) const {
    if (!has_node(source_id) || !has_node(target_id)) throw std::runtime_error("Source or target node not found in graph.");
    auto state = std::make_unique<detail::MinCostFlowState<NodeID>>();
    state->source_id = source_id;
    state->target_id = target_id;
    const auto all_nodes = nodes();
    for (std::size_t i = 0; i < all_nodes.size(); ++i) { boost::add_vertex(state->flow_graph); state->node_to_index[all_nodes[i]] = i; }
    for (const auto& [u, v] : edge_pairs()) {
        auto [e, added] = boost::add_edge(state->node_to_index.at(u), state->node_to_index.at(v), state->flow_graph);
        auto [rev, rev_added] = boost::add_edge(state->node_to_index.at(v), state->node_to_index.at(u), state->flow_graph);
        (void)added; (void)rev_added;
        state->weight[e] = static_cast<long>(get_edge_numeric_attr(u, v, weight_attr));
        state->weight[rev] = -state->weight[e];
        state->capacity[e] = static_cast<long>(get_edge_numeric_attr(u, v, capacity_attr));
        state->capacity[rev] = 0;
        state->reverse[e] = rev;
        state->reverse[rev] = e;
        state->original_edges[{u, v}] = e;
    }
    state->value = boost::push_relabel_max_flow(state->flow_graph, state->node_to_index.at(source_id), state->node_to_index.at(target_id));
    auto& cache = detail::min_cost_flow_cache<Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>>();
    cache[static_cast<const void*>(this)] = std::move(state);
    return cache.at(static_cast<const void*>(this))->value;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::cycle_canceling(const std::string& weight_attr) const {
    auto& cache = detail::min_cost_flow_cache<Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>>();
    auto it = cache.find(static_cast<const void*>(this));
    if (it == cache.end()) throw std::runtime_error("Cycle-canceling state unavailable: run push_relabel_maximum_flow(...) first.");
    auto& state = *it->second;
    for (const auto& [key, edge_desc] : state.original_edges) {
        state.weight[edge_desc] = static_cast<long>(get_edge_numeric_attr(key.first, key.second, weight_attr));
        const auto rev = state.reverse[edge_desc];
        state.weight[rev] = -state.weight[edge_desc];
    }
    boost::cycle_canceling(state.flow_graph);
    state.cost = boost::find_flow_cost(state.flow_graph);
    return state.cost;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::successive_shortest_path_nonnegative_weights(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr, const std::string& weight_attr) const {
    if (!has_node(source_id) || !has_node(target_id)) throw std::runtime_error("Source or target node not found in graph.");
    using FlowTraits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
    using FlowGraph = boost::adjacency_list<
        boost::vecS, boost::vecS, boost::directedS, boost::no_property,
        boost::property<boost::edge_weight_t, long,
            boost::property<boost::edge_capacity_t, long,
                boost::property<boost::edge_residual_capacity_t, long,
                    boost::property<boost::edge_reverse_t, typename FlowTraits::edge_descriptor>>>>>;
    using WeightMap = typename boost::property_map<FlowGraph, boost::edge_weight_t>::type;
    using CapacityMap = typename boost::property_map<FlowGraph, boost::edge_capacity_t>::type;
    using ResidualMap = typename boost::property_map<FlowGraph, boost::edge_residual_capacity_t>::type;
    using ReverseMap = typename boost::property_map<FlowGraph, boost::edge_reverse_t>::type;
    using FlowEdgeDesc = typename boost::graph_traits<FlowGraph>::edge_descriptor;

    FlowGraph flow_graph;
    WeightMap weight = boost::get(boost::edge_weight, flow_graph);
    CapacityMap capacity = boost::get(boost::edge_capacity, flow_graph);
    ReverseMap reverse = boost::get(boost::edge_reverse, flow_graph);
    const auto all_nodes = nodes();
    std::unordered_map<NodeID, std::size_t> node_to_index;
    for (std::size_t i = 0; i < all_nodes.size(); ++i) { boost::add_vertex(flow_graph); node_to_index[all_nodes[i]] = i; }
    std::map<std::pair<NodeID, NodeID>, FlowEdgeDesc> original_edges;
    for (const auto& [u, v] : edge_pairs()) {
        auto [e, added] = boost::add_edge(node_to_index.at(u), node_to_index.at(v), flow_graph);
        auto [rev, rev_added] = boost::add_edge(node_to_index.at(v), node_to_index.at(u), flow_graph);
        (void)added; (void)rev_added;
        weight[e] = static_cast<long>(get_edge_numeric_attr(u, v, weight_attr));
        weight[rev] = -weight[e];
        capacity[e] = static_cast<long>(get_edge_numeric_attr(u, v, capacity_attr));
        capacity[rev] = 0;
        reverse[e] = rev;
        reverse[rev] = e;
        original_edges[{u, v}] = e;
    }
    boost::successive_shortest_path_nonnegative_weights(flow_graph, node_to_index.at(source_id), node_to_index.at(target_id));
    ResidualMap residual = boost::get(boost::edge_residual_capacity, flow_graph);
    long flow_value = 0;
    for (auto [eit, eend] = boost::out_edges(node_to_index.at(source_id), flow_graph); eit != eend; ++eit) flow_value += capacity[*eit] - residual[*eit];
    const long cost = boost::find_flow_cost(flow_graph);
    MinCostMaxFlowResult<NodeID> result;
    result.flow = flow_value;
    result.cost = cost;
    for (const auto& [key, edge_desc] : original_edges) result.edge_flows[key] = capacity[edge_desc] - residual[edge_desc];
    return result;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::max_flow_min_cost_successive_shortest_path(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr, const std::string& weight_attr) const {
    return successive_shortest_path_nonnegative_weights(source_id, target_id, capacity_attr, weight_attr);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::max_flow_min_cost(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr, const std::string& weight_attr) const {
    return max_flow_min_cost_cycle_canceling(source_id, target_id, capacity_attr, weight_attr);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::num_vertices() const {
    return static_cast<int>(boost::num_vertices(g));
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::degree_centrality() const {
    const auto node_count = boost::num_vertices(g);
    std::unordered_map<NodeID, double> centrality;
    if (node_count == 0) return centrality;
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) centrality[get_node_id(*v)] = 0.0;
    if (node_count <= 1) return centrality;
    const double scale = 1.0 / static_cast<double>(node_count - 1);
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
        double degree = 0.0;
        if constexpr (Directed) degree = static_cast<double>(boost::in_degree(*v, g) + boost::out_degree(*v, g));
        else degree = static_cast<double>(boost::degree(*v, g));
        centrality[get_node_id(*v)] = degree * scale;
    }
    return centrality;
}

template <typename GraphWrapper>
[[deprecated("Use G.num_vertices() instead.")]]
auto num_vertices(const GraphWrapper& G) {
    return G.num_vertices();
}

inline int to_2sat_vertex_id(int literal) {
    return (literal > 0) ? (2 * literal - 2) : (-2 * literal - 1);
}

inline bool two_sat_satisfiable(int num_variables, const std::vector<std::pair<int, int>>& clauses) {
    using SatGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS>;

    SatGraph g(2 * num_variables);
    for (const auto& [x, y] : clauses) {
        boost::add_edge(to_2sat_vertex_id(-x), to_2sat_vertex_id(y), g);
        boost::add_edge(to_2sat_vertex_id(-y), to_2sat_vertex_id(x), g);
    }

    std::vector<int> comp(boost::num_vertices(g));
    boost::strong_components(g, boost::make_iterator_property_map(comp.begin(), boost::get(boost::vertex_index, g)));

    for (int i = 1; i <= num_variables; ++i) {
        if (comp[to_2sat_vertex_id(i)] == comp[to_2sat_vertex_id(-i)]) {
            return false;
        }
    }
    return true;
}

template <typename GraphWrapper>
[[deprecated("Use G.degree_centrality() instead.")]]
auto degree_centrality(const GraphWrapper& G) {
    return G.degree_centrality();
}

} // namespace nxpp

#endif // NXPP_HPP
