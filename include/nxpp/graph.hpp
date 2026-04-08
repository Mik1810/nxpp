#pragma once

#if !__has_include(<boost/graph/adjacency_list.hpp>)
#error "FATAL: nxpp requires the Boost Graph Library (BGL)."
#endif

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#include <boost/graph/adjacency_list.hpp>
#include <boost/functional/hash.hpp>
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
#include <functional>
#include <vector>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <string>
#include <algorithm>
#include <cmath>
#include <concepts>
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

} // namespace nxpp

namespace boost {

enum vertex_wrapper_index_t { vertex_wrapper_index };
BOOST_INSTALL_PROPERTY(vertex, wrapper_index);

} // namespace boost

namespace nxpp {

template <typename GraphType, bool HasWeight>
struct built_in_weight_traits;

namespace detail {

template <typename T>
concept node_id_orderable =
    requires(const T& lhs, const T& rhs) {
        { std::less<T>{}(lhs, rhs) } -> std::convertible_to<bool>;
    };

} // namespace detail

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

template <typename Key, typename Value>
class lookup_map {
public:
    using storage_type = std::map<Key, Value>;
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

template <typename Key, typename Value>
class indexed_lookup_map {
public:
    using storage_type = std::vector<std::pair<Key, Value>>;
    using iterator = typename storage_type::iterator;
    using const_iterator = typename storage_type::const_iterator;

    void reserve(std::size_t count) {
        data.reserve(count);
    }

    void push_back(const Key& key, const Value& value) {
        data.emplace_back(key, value);
    }

    void push_back(const Key& key, Value&& value) {
        data.emplace_back(key, std::move(value));
    }

    Value& at(const Key& key) {
        auto it = find(key);
        if (it == data.end()) {
            throw std::out_of_range("indexed_lookup_map::at");
        }
        return it->second;
    }

    const Value& at(const Key& key) const {
        auto it = find(key);
        if (it == data.end()) {
            throw std::out_of_range("indexed_lookup_map::at");
        }
        return it->second;
    }

    Value& operator[](const Key& key) {
        return at(key);
    }

    const Value& operator[](const Key& key) const {
        return at(key);
    }

    bool contains(const Key& key) const {
        return find(key) != data.end();
    }

    iterator find(const Key& key) {
        auto it = lower_bound_for(key);
        if (it == data.end() || key < it->first) {
            return data.end();
        }
        return it;
    }

    const_iterator find(const Key& key) const {
        auto it = lower_bound_for(key);
        if (it == data.end() || key < it->first) {
            return data.end();
        }
        return it;
    }

    iterator begin() { return data.begin(); }
    iterator end() { return data.end(); }
    const_iterator begin() const { return data.begin(); }
    const_iterator end() const { return data.end(); }
    const_iterator cbegin() const { return data.cbegin(); }
    const_iterator cend() const { return data.cend(); }

    bool empty() const { return data.empty(); }
    std::size_t size() const { return data.size(); }

private:
    iterator lower_bound_for(const Key& key) {
        return std::lower_bound(
            data.begin(),
            data.end(),
            key,
            [](const auto& entry, const Key& value) { return entry.first < value; }
        );
    }

    const_iterator lower_bound_for(const Key& key) const {
        return std::lower_bound(
            data.begin(),
            data.end(),
            key,
            [](const auto& entry, const Key& value) { return entry.first < value; }
        );
    }

    storage_type data;
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
/**
 * @brief Graph wrapper around Boost Graph Library with Python-inspired helpers.
 *
 * The template parameters control directedness, multigraph support, built-in
 * edge weights, and the underlying Boost selectors. Public methods operate on
 * stable wrapper-side node IDs instead of raw Boost descriptors.
 */
class Graph {
public:
    using NodeType = NodeID;
    using EdgeWeightType = EdgeWeight;
    using OutEdgeListSelector = OutEdgeSelector;
    using VertexListSelector = VertexSelector;
    using DirectedSelector = typename std::conditional<Directed, boost::bidirectionalS, boost::undirectedS>::type;
    using VertexProperty = boost::property<
        boost::vertex_name_t,
        NodeID,
        boost::property<boost::vertex_wrapper_index_t, std::size_t>
    >;
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
    using WrapperIndexMap = typename boost::property_map<GraphType, boost::vertex_wrapper_index_t>::type;
    using VertexIndexMap = WrapperIndexMap;
    using EdgeIdMap = typename boost::property_map<GraphType, boost::edge_index_t>::type;
    using IdMap = std::map<NodeID, VertexDesc>;
    using AttrMap = std::map<std::string, std::any>;
    using NodeAttrStorage = std::map<NodeID, AttrMap>;
    using EdgeAttrStorage = std::map<std::size_t, AttrMap>;
    static constexpr bool is_directed = Directed;
    static constexpr bool has_builtin_edge_weight = Weighted;

private:
    static_assert(
        std::copy_constructible<NodeID>,
        "nxpp::Graph requires NodeID to be copy-constructible because node IDs are stored and materialized across wrapper-owned containers."
    );

    static_assert(
        std::equality_comparable<NodeID>,
        "nxpp::Graph requires NodeID to support operator== because several wrapper result helpers compare reconstructed node IDs directly."
    );

    static_assert(
        detail::node_id_orderable<NodeID>,
        "nxpp::Graph requires NodeID to be orderable with std::less because wrapper-owned maps and key-sorted result containers rely on that ordering."
    );

    GraphType g;
    WeightMap weight_map;
    VertexNameMap vertex_name_map;
    VertexIndexMap vertex_index_map;
    EdgeIdMap edge_id_map;
    IdMap id_to_bgl;
    std::vector<NodeID> bgl_to_id;
    std::size_t next_edge_id = 0;
    NodeAttrStorage node_properties;
    EdgeAttrStorage edge_properties;

private:
    static_assert(
        !(Multi && std::is_same_v<OutEdgeSelector, boost::setS>),
        "nxpp does not support Multi=true with boost::setS because setS suppresses parallel edges."
    );

    using EdgeAttrMap = AttrMap;

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

    template <typename Value, typename BuildValue>
    indexed_lookup_map<NodeID, Value> build_node_indexed_result(BuildValue&& build_value) const {
        indexed_lookup_map<NodeID, Value> result;
        result.reserve(id_to_bgl.size());
        for (const auto& [id, desc] : id_to_bgl) {
            result.push_back(id, build_value(desc));
        }
        return result;
    }

    template <typename Value>
    indexed_lookup_map<NodeID, Value> build_sparse_node_indexed_result(
        const std::vector<std::optional<Value>>& values
    ) const {
        indexed_lookup_map<NodeID, Value> result;
        for (const auto& [id, desc] : id_to_bgl) {
            const auto index = get_vertex_index(desc);
            if (index < values.size() && values[index].has_value()) {
                result.push_back(id, *values[index]);
            }
        }
        return result;
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
            throw std::runtime_error("Edge lookup failed: edge not found.");
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
          vertex_index_map(boost::get(boost::vertex_wrapper_index, g)),
          edge_id_map(boost::get(boost::edge_index, g)),
          id_to_bgl() {}

    /**
     * @brief Ensures that a node with the given ID exists in the graph.
     *
     * This is idempotent: if the node is already present, the call leaves the
     * graph unchanged. Use it when you want to materialize isolated nodes
     * explicitly instead of relying on edge insertion to create them.
     */
    void add_node(const NodeID& id) {
        get_or_create_vertex(id);
    }

    /**
     * @brief Adds a batch of nodes from a vector of node IDs.
     *
     * Each element is forwarded to add_node(), so duplicate IDs are ignored in
     * the same way as repeated single-node insertion.
     */
    void add_nodes_from(const std::vector<NodeID>& nodes) {
        for (const auto& n : nodes) {
            add_node(n);
        }
    }

    /**
     * @brief Returns the underlying Boost edge descriptor for an existing edge.
     *
     * This is mainly for advanced integrations with raw Boost algorithms. Most
     * users should prefer the higher-level wrapper methods and edge-id helpers.
     */
    EdgeDesc get_edge_desc(const NodeID& u, const NodeID& v) const {
        auto it_u = id_to_bgl.find(u);
        auto it_v = id_to_bgl.find(v);
        if (it_u == id_to_bgl.end() || it_v == id_to_bgl.end()) throw std::runtime_error("Node lookup failed: node not found.");
        auto [e, exists] = boost::edge(it_u->second, it_v->second, g);
        if (!exists) throw std::runtime_error("Edge lookup failed: edge not found.");
        return e;
    }

    /**
     * @brief Returns whether at least one edge exists between two endpoints.
     *
     * In multigraphs this answers an existence question only: it does not tell
     * you which parallel edge matched. Use edge_ids(u, v) for precise
     * parallel-edge inspection.
     */
    bool has_edge(const NodeID& u, const NodeID& v) const {
        auto it_u = id_to_bgl.find(u);
        auto it_v = id_to_bgl.find(v);
        if (it_u == id_to_bgl.end() || it_v == id_to_bgl.end()) return false;
        auto [e, exists] = boost::edge(it_u->second, it_v->second, g);
        return exists;
    }

    /// Returns whether an edge with the given wrapper-managed ID exists.
    bool has_edge_id(std::size_t edge_id) const;
    /// Returns all wrapper-managed edge IDs currently present in the graph.
    std::vector<std::size_t> edge_ids() const;
    /// Returns all wrapper-managed edge IDs between the given endpoints.
    std::vector<std::size_t> edge_ids(const NodeID& u, const NodeID& v) const;
    /// Returns the endpoints of an edge identified by its wrapper-managed ID.
    std::pair<NodeID, NodeID> get_edge_endpoints(std::size_t edge_id) const;

    template <bool W = Weighted>
    requires(W)
    /**
     * @brief Adds or updates a weighted edge.
     *
     * Missing endpoint nodes are created automatically. In simple graphs,
     * inserting an existing edge updates its stored built-in weight instead of
     * creating a parallel edge.
     *
     * @param u Source node ID.
     * @param v Target node ID.
     * @param w Edge weight to store.
     */
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
    /**
     * @brief Adds a weighted edge and returns its wrapper-managed edge ID.
     *
     * This is the precise insertion form to prefer when you need stable edge
     * identity later, especially in multigraphs.
     */
    std::size_t add_edge_with_id(const NodeID& u, const NodeID& v, EdgeWeight w = 1.0);

    template <bool W = Weighted>
    requires(!W)
    /**
     * @brief Adds an unweighted edge.
     *
     * Missing endpoint nodes are created automatically. In simple graphs,
     * inserting an existing edge is a no-op instead of creating a duplicate.
     *
     * @param u Source node ID.
     * @param v Target node ID.
     */
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
    /**
     * @brief Adds an unweighted edge and returns its wrapper-managed edge ID.
     *
     * This is useful when later lookups or mutations should target one
     * specific edge through the explicit edge-id API.
     */
    std::size_t add_edge_with_id(const NodeID& u, const NodeID& v);


    /**
     * @brief Adds or updates a weighted edge and attaches an attribute map.
     *
     * The built-in weight is stored in the graph edge property, while the
     * supplied attributes are copied into the wrapper-managed edge attribute
     * storage. In multigraphs this remains an endpoint-based convenience form,
     * not a precise single-parallel-edge insertion/update API.
     */
    template <bool W = Weighted>
    requires(W)
    void add_edge(const NodeID& u, const NodeID& v, EdgeWeight w, const EdgeAttrMap& attrs);

    /**
     * @brief Adds an unweighted edge and attaches an attribute map.
     *
     * This overload is the attribute-bearing counterpart of the plain
     * unweighted add_edge(u, v) form. In multigraphs it remains endpoint-based
     * convenience API rather than a precise single-edge handle.
     */
    void add_edge(const NodeID& u, const NodeID& v, const EdgeAttrMap& attrs);

    template <bool W = Weighted>
    requires(W)
    /**
     * @brief Adds or updates a weighted edge and attaches one attribute.
     *
     * This is a compact convenience overload when a full attribute map would
     * be unnecessarily verbose. In multigraphs it should still be treated as
     * an endpoint-based convenience form rather than a precise edge-instance
     * API.
     */
    void add_edge(const NodeID& u, const NodeID& v, EdgeWeight w, const std::pair<std::string, std::any>& attr);

    /// Adds an unweighted edge and attaches one attribute.
    void add_edge(const NodeID& u, const NodeID& v, const std::pair<std::string, std::any>& attr);

    template <bool W = Weighted>
    requires(W)
    /**
     * @brief Adds or updates a weighted edge and attaches an initializer-list of attributes.
     *
     * This overload is convenient for literal-style calls such as
     * `add_edge(u, v, w, {{"capacity", 3}, {"label", "fast"}})`. In
     * multigraphs it remains an endpoint-based convenience form rather than a
     * precise edge-instance API.
     */
    void add_edge(const NodeID& u, const NodeID& v, EdgeWeight w, std::initializer_list<std::pair<std::string, std::any>> attrs);

    /// Adds an unweighted edge and attaches an initializer-list of attributes.
    void add_edge(const NodeID& u, const NodeID& v, std::initializer_list<std::pair<std::string, std::any>> attrs);
    template <bool W = Weighted>
    requires(W)
    /// Adds a batch of weighted edges from `(u, v, w)` tuples.
    void add_edges_from(const std::vector<std::tuple<NodeID, NodeID, EdgeWeight>>& edges) {
        for (const auto& edge : edges) {
            add_edge(std::get<0>(edge), std::get<1>(edge), std::get<2>(edge));
        }
    }

    /// Adds a batch of unweighted edges from `(u, v)` pairs.
    void add_edges_from(const std::vector<std::pair<NodeID, NodeID>>& edges) {
        for (const auto& edge : edges) {
            add_edge(edge.first, edge.second);
        }
    }

    /**
     * @brief Clears the entire graph and all wrapper-managed metadata.
     *
     * After this call, nodes, edges, edge IDs, built-in weights, attributes,
     * and translation tables are reset to the same logical state as a freshly
     * default-constructed graph.
     */
    void clear() {
        g = GraphType();
        id_to_bgl.clear();
        bgl_to_id.clear();
        node_properties.clear();
        edge_properties.clear();
        weight_map = built_in_weight_traits<GraphType, Weighted>::get(g);
        vertex_name_map = boost::get(boost::vertex_name, g);
        vertex_index_map = boost::get(boost::vertex_wrapper_index, g);
        edge_id_map = boost::get(boost::edge_index, g);
        next_edge_id = 0;
    }

    /**
     * @brief Removes all edges between two endpoints.
     *
     * In simple graphs this removes the unique matching edge. In multigraphs
     * it removes every parallel edge between the same endpoints and cleans the
     * associated wrapper-managed edge attributes.
     *
     * @throws std::runtime_error If either node is missing or no such edge exists.
     */
    void remove_edge(const NodeID& u, const NodeID& v) {
        auto it_u = id_to_bgl.find(u);
        auto it_v = id_to_bgl.find(v);
        if (it_u == id_to_bgl.end() || it_v == id_to_bgl.end()) {
            throw std::runtime_error("Node lookup failed: node not found.");
        }
        auto [e, exists] = boost::edge(it_u->second, it_v->second, g);
        if (!exists) {
            throw std::runtime_error("Edge lookup failed: edge not found.");
        }
        for (auto edge_id : collect_edge_ids_between(it_u->second, it_v->second)) {
            edge_properties.erase(edge_id);
        }
        boost::remove_edge(it_u->second, it_v->second, g);
    }

    /**
     * @brief Removes a single edge by its wrapper-managed edge ID.
     *
     * This is the precise removal form to prefer in multigraphs.
     */
    void remove_edge(std::size_t edge_id);

    /**
     * @brief Removes a node and all of its incident edges.
     *
     * Wrapper-side node/edge attributes and maintained translation/index maps
     * are updated too, so the graph stays internally consistent after
     * descriptor remapping.
     *
     * @throws std::runtime_error If the node is not present.
     */
    void remove_node(const NodeID& u) {
        auto it = id_to_bgl.find(u);
        if (it == id_to_bgl.end()) {
            throw std::runtime_error("Node lookup failed: node not found.");
        }
        VertexDesc v = it->second;

        erase_incident_edge_properties(v);
        boost::clear_vertex(v, g);
        boost::remove_vertex(v, g);
        
        node_properties.erase(u);
        rebuild_vertex_maps();
    }

    /**
     * @brief Returns the outgoing neighbors of a node.
     *
     * For undirected graphs this is the standard adjacency view. For directed
     * graphs this corresponds to successor nodes.
     */
    std::vector<NodeID> neighbors(const NodeID& u) const {
        auto it = id_to_bgl.find(u);
        if (it == id_to_bgl.end()) {
            throw std::runtime_error("Node lookup failed: node not found.");
        }
        std::vector<NodeID> res;
        for (auto [e, eend] = boost::out_edges(it->second, g); e != eend; ++e) {
            res.push_back(node_id_of(boost::target(*e, g)));
        }
        return res;
    }

    /// Alias for neighbors(), mainly to mirror directed-graph terminology.
    std::vector<NodeID> successors(const NodeID& u) const {
        return neighbors(u);
    }

    /**
     * @brief Returns predecessor nodes for directed graphs.
     *
     * For undirected graphs this falls back to the ordinary adjacency view, so
     * the result matches neighbors(u).
     */
    std::vector<NodeID> predecessors(const NodeID& u) const {
        auto it = id_to_bgl.find(u);
        if (it == id_to_bgl.end()) {
            throw std::runtime_error("Node lookup failed: node not found.");
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

    /// Returns whether the graph already contains the given node ID.
    bool has_node(const NodeID& u) const {
        return id_to_bgl.find(u) != id_to_bgl.end();
    }


    /**
     * @brief Returns whether a node has the named attribute.
     *
     * Missing nodes are treated as "attribute not present" rather than as an
     * exceptional case.
     */
    bool has_node_attr(const NodeID& u, const std::string& key) const;
    /**
     * @brief Returns whether an endpoint-selected edge has the named attribute.
     *
     * In multigraphs this follows the same `(u, v)` caveat as the other
     * endpoint-based edge accessors and should not be treated as precise
     * parallel-edge identification.
     */
    bool has_edge_attr(const NodeID& u, const NodeID& v, const std::string& key) const;
    /// Returns whether an edge-ID-selected edge has the named attribute.
    bool has_edge_attr(std::size_t edge_id, const std::string& key) const;

    template <typename T>
    /**
     * @brief Reads a typed node attribute.
     *
     * This throws if the node has no attributes, the key is missing, or the
     * stored value cannot be converted to the requested type.
     */
    T get_node_attr(const NodeID& u, const std::string& key) const;

    template <typename T>
    /**
     * @brief Reads a typed edge attribute selected by endpoints.
     *
     * Prefer the edge-id overload in multigraphs when you need stable
     * per-parallel-edge access.
     */
    T get_edge_attr(const NodeID& u, const NodeID& v, const std::string& key) const;

    template <typename T>
    /**
     * @brief Reads a typed edge attribute selected by edge ID.
     *
     * This is the precise typed read path for multigraph edge metadata.
     */
    T get_edge_attr(std::size_t edge_id, const std::string& key) const;

    template <typename T>
    /**
     * @brief Attempts to read a typed node attribute without throwing.
     *
     * Returns `std::nullopt` for missing nodes, missing keys, and type
     * mismatches.
     */
    std::optional<T> try_get_node_attr(const NodeID& u, const std::string& key) const;

    template <typename T>
    /**
     * @brief Attempts to read a typed edge attribute selected by endpoints.
     *
     * Returns `std::nullopt` instead of throwing when the edge, key, or type
     * does not match.
     */
    std::optional<T> try_get_edge_attr(const NodeID& u, const NodeID& v, const std::string& key) const;

    template <typename T>
    /// Attempts to read a typed edge attribute selected by edge ID.
    std::optional<T> try_get_edge_attr(std::size_t edge_id, const std::string& key) const;

    /**
     * @brief Reads an endpoint-selected edge attribute as a numeric value.
     *
     * The special key `"weight"` resolves to the built-in edge-weight property
     * when the graph is weighted.
     */
    double get_edge_numeric_attr(const NodeID& u, const NodeID& v, const std::string& key) const;
    /// Reads an edge attribute as a numeric value selected by edge ID.
    double get_edge_numeric_attr(std::size_t edge_id, const std::string& key) const;

    template <bool W = Weighted>
    requires(W)
    /**
     * @brief Returns the built-in edge weight selected by endpoints.
     *
     * In multigraphs this follows the wrapper's endpoint-based edge-selection
     * semantics and is therefore not a stable way to identify one specific
     * parallel edge.
     */
    EdgeWeight get_edge_weight(const NodeID& u, const NodeID& v) const;

    template <bool W = Weighted>
    requires(W)
    /// Returns the built-in edge weight selected by edge ID.
    EdgeWeight get_edge_weight(std::size_t edge_id) const;

    template <bool W = Weighted>
    requires(W)
    /// Updates the built-in weight of an edge identified by edge ID.
    void set_edge_weight(std::size_t edge_id, EdgeWeight w);

    template <typename T>
    /**
     * @brief Stores a typed attribute on an edge identified by edge ID.
     *
     * String-like values are normalized through the wrapper's attribute
     * storage helpers in the same way as other public attribute APIs.
     */
    void set_edge_attr(std::size_t edge_id, const std::string& key, const T& value);

    /**
     * @brief Returns all node IDs currently stored in the graph.
     *
     * The order follows the wrapper-maintained vertex order used by the public
     * API helpers.
     */
    std::vector<NodeID> nodes() const {
        std::vector<NodeID> res;
        for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
            res.push_back(node_id_of(*v));
        }
        return res;
    }

    /**
     * @brief Returns the public edge list.
     *
     * Weighted graphs expose `(u, v, w)` tuples, while unweighted graphs expose
     * `(u, v)` pairs. The return type follows the `Weighted` template flag.
     */
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

    /// Returns all edges as endpoint pairs, ignoring built-in weights.
    std::vector<std::pair<NodeID, NodeID>> edge_pairs() const {
        std::vector<std::pair<NodeID, NodeID>> res;
        for (auto [e, eend] = boost::edges(g); e != eend; ++e) {
            NodeID source_id = node_id_of(boost::source(*e, g));
            NodeID target_id = node_id_of(boost::target(*e, g));
            res.emplace_back(source_id, target_id);
        }
        return res;
    }

    /// Exposes the underlying Boost graph for advanced integrations.
    const GraphType& get_impl() const { return g; }
    /// Returns the maintained vertex-index-to-node-ID translation table.
    const std::vector<NodeID>& get_bgl_to_id_map() const { return bgl_to_id; }
    /// Returns the maintained node-ID-to-vertex translation table.
    const IdMap& get_id_to_bgl_map() const { return id_to_bgl; }
    /// Returns the wrapper-side node ID associated with a Boost vertex descriptor.
    const NodeID& get_node_id(VertexDesc v) const { return node_id_of(v); }
    /// Returns the wrapper-maintained dense vertex index used by algorithms.
    std::size_t get_vertex_index(VertexDesc v) const { return vertex_index_of(v); }

    // Proxy Pattern per simulare G[u][v] = weight
    struct EdgeAttrProxy {
        Graph* graph;
        NodeID u, v;
        std::string key;

        /// Sets attribute @p key on edge (u,v), creating the edge if it does not exist.
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
        
        /// Assigns edge weight, creating the edge (u,v) if it does not exist.
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

        /// Sets attribute @p key on node @p u, creating the node if it does not exist.
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

    /**
     * @brief Returns the proxy used for `G[u][v]` edge-access syntax.
     *
     * This keeps the NetworkX-inspired indexing style available for reads and
     * writes on existing-graph operations.
     */
    NodeProxy operator[](const NodeID& u) {
        if (!has_node(u)) {
            add_node(u);
        }
        return NodeProxy(this, u);
    }

    /**
     * @brief Returns the proxy used for `G.node(u)[key]` node-attribute syntax.
     *
     * The node is created implicitly when it does not exist yet.
     */
    NodeAttrBaseProxy node(const NodeID& u) {
        if (!has_node(u) && !Multi) add_node(u);
        else if (!has_node(u)) add_node(u);
        return NodeAttrBaseProxy(this, u);
    }

    /**
     * @brief Returns the tree edges discovered by breadth-first search.
     *
     * @param start Node ID used as the BFS root.
     * @return A `std::vector<std::pair<NodeID, NodeID>>` containing the ordered
     * BFS tree edges as `(parent, child)` pairs.
     * @throws std::runtime_error If @p start is not present in the graph.
     */
    auto bfs_edges(const NodeID& start) const;
    /**
     * @brief Materializes the breadth-first-search tree rooted at @p start.
     *
     * The returned graph contains the root node and each tree edge reported by
     * @ref bfs_edges.
     *
     * @param start Node ID used as the BFS root.
     * @return A `Graph<NodeID, double, Directed>` containing only the BFS tree.
     * @throws std::runtime_error If @p start is not present in the graph.
     */
    auto bfs_tree(const NodeID& start) const;
    /**
     * @brief Groups BFS tree children by their discovered parent.
     *
     * Only nodes that actually gain at least one tree child appear in the
     * sparse result map.
     *
     * @param start Node ID used as the BFS root.
     * @return A sparse node-indexed map from each BFS parent to a
     * `std::vector<NodeID>` of discovered children.
     * @throws std::runtime_error If @p start is not present in the graph.
     */
    auto bfs_successors(const NodeID& start) const;
    /**
     * @brief Runs breadth-first search with an object-style visitor.
     *
     * The visitor may implement wrapper-level hooks such as
     * `examine_vertex(u, G)` and `tree_edge(u, v, G)`.
     *
     * @tparam Visitor Visitor object type accepted by the wrapper dispatcher.
     * @param start Node ID used as the BFS root.
     * @param visitor Visitor instance that receives traversal callbacks.
     * @throws std::runtime_error If @p start is not present in the graph.
     */
    template <typename Visitor>
    void breadth_first_search(const NodeID& start, Visitor& visitor) const;
    /**
     * @brief Runs breadth-first search with lightweight callback hooks.
     *
     * This is a convenience wrapper over @ref breadth_first_search that avoids
     * defining a dedicated visitor type when only vertex and tree-edge events
     * are needed.
     *
     * @tparam OnVertex Callback invoked as `on_vertex(u)`.
     * @tparam OnTreeEdge Callback invoked as `on_tree_edge(u, v)`.
     * @param start Node ID used as the BFS root.
     * @param on_vertex Callback for discovered vertices.
     * @param on_tree_edge Callback for BFS tree edges.
     * @throws std::runtime_error If @p start is not present in the graph.
     */
    template <typename OnVertex, typename OnTreeEdge>
    void bfs_visit(const NodeID& start, OnVertex&& on_vertex, OnTreeEdge&& on_tree_edge) const;
    /**
     * @brief Returns the tree edges discovered by depth-first search.
     *
     * @param start Node ID used as the DFS root.
     * @return A `std::vector<std::pair<NodeID, NodeID>>` containing the ordered
     * DFS tree edges as `(parent, child)` pairs.
     * @throws std::runtime_error If @p start is not present in the graph.
     */
    auto dfs_edges(const NodeID& start) const;
    /**
     * @brief Materializes the depth-first-search tree rooted at @p start.
     *
     * The returned graph contains the root node and each tree edge reported by
     * @ref dfs_edges.
     *
     * @param start Node ID used as the DFS root.
     * @return A `Graph<NodeID, double, Directed>` containing only the DFS tree.
     * @throws std::runtime_error If @p start is not present in the graph.
     */
    auto dfs_tree(const NodeID& start) const;
    /**
     * @brief Returns the DFS predecessor assigned to each discovered node.
     *
     * The sparse result omits the root node and any node not reached from
     * @p start.
     *
     * @param start Node ID used as the DFS root.
     * @return A sparse node-indexed map from each discovered node to its parent `NodeID`.
     * @throws std::runtime_error If @p start is not present in the graph.
     */
    auto dfs_predecessors(const NodeID& start) const;
    /**
     * @brief Groups DFS tree children by their discovered parent.
     *
     * Only nodes that actually gain at least one tree child appear in the
     * sparse result map.
     *
     * @param start Node ID used as the DFS root.
     * @return A sparse node-indexed map from each DFS parent to a
     * `std::vector<NodeID>` of discovered children.
     * @throws std::runtime_error If @p start is not present in the graph.
     */
    auto dfs_successors(const NodeID& start) const;
    /**
     * @brief Runs depth-first search with an object-style visitor.
     *
     * The visitor may implement wrapper-level hooks such as
     * `tree_edge(u, v, G)` and `back_edge(u, v, G)`.
     *
     * @tparam Visitor Visitor object type accepted by the wrapper dispatcher.
     * @param start Node ID used as the DFS root.
     * @param visitor Visitor instance that receives traversal callbacks.
     * @throws std::runtime_error If @p start is not present in the graph.
     */
    template <typename Visitor>
    void depth_first_search(const NodeID& start, Visitor& visitor) const;
    /**
     * @brief Runs depth-first search with lightweight callback hooks.
     *
     * This is a convenience wrapper over @ref depth_first_search that avoids
     * defining a dedicated visitor type when only tree-edge and back-edge
     * events are needed.
     *
     * @tparam OnTreeEdge Callback invoked as `on_tree_edge(u, v)`.
     * @tparam OnBackEdge Callback invoked as `on_back_edge(u, v)`.
     * @param start Node ID used as the DFS root.
     * @param on_tree_edge Callback for DFS tree edges.
     * @param on_back_edge Callback for DFS back edges.
     * @throws std::runtime_error If @p start is not present in the graph.
     */
    template <typename OnTreeEdge, typename OnBackEdge>
    void dfs_visit(const NodeID& start, OnTreeEdge&& on_tree_edge, OnBackEdge&& on_back_edge) const;

    /**
     * @brief Computes an unweighted shortest path between two nodes.
     *
     * @param source_id Source node ID.
     * @param target_id Target node ID.
     * @return A `std::vector<NodeID>` describing the path from source to target.
     * @throws std::runtime_error If either node is missing or the target is unreachable.
     */
    auto shortest_path(const NodeID& source_id, const NodeID& target_id) const;
    /**
     * @brief Computes a shortest path using the built-in edge-weight property.
     *
     * This overload accepts the compatibility-shaped `"weight"` name, but it
     * does not support arbitrary user-defined weight keys.
     *
     * @param source_id Source node ID.
     * @param target_id Target node ID.
     * @param weight Compatibility name for the built-in edge-weight property.
     * @return A `std::vector<NodeID>` describing the path from source to target.
     * @throws std::runtime_error If either node is missing, the target is unreachable, or the weight name is unsupported.
     */
    auto shortest_path(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const;
    /**
     * @brief Returns the length of an unweighted shortest path.
     *
     * @param source_id Source node ID.
     * @param target_id Target node ID.
     * @return The path length as a `double`, measured in edge count.
     * @throws std::runtime_error If either node is missing or the target is unreachable.
     */
    double shortest_path_length(const NodeID& source_id, const NodeID& target_id) const;
    /**
     * @brief Returns the length of a shortest path using the built-in edge-weight property.
     *
     * This overload accepts the compatibility-shaped `"weight"` name, but it
     * does not support arbitrary user-defined weight keys.
     *
     * @param source_id Source node ID.
     * @param target_id Target node ID.
     * @param weight Compatibility name for the built-in edge-weight property.
     * @return The weighted shortest-path length as a `double`.
     * @throws std::runtime_error If either node is missing, the target is unreachable, or the weight name is unsupported.
     */
    double shortest_path_length(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const;

    /**
     * @brief Computes the shortest path using the built-in edge-weight property.
     *
     * @tparam W Internal enable/disable gate for weighted graph specializations.
     * @param source_id Source node ID.
     * @param target_id Target node ID.
     * @return A `std::vector<NodeID>` describing the shortest path from source to target.
     */
    template <bool W = Weighted>
    requires(W)
    auto dijkstra_path(const NodeID& source_id, const NodeID& target_id) const;

    /**
     * @brief Computes the shortest path using the built-in edge-weight property.
     *
     * This overload accepts the compatibility-shaped `"weight"` name, not an
     * arbitrary user-defined weight attribute.
     *
     * @tparam W Internal enable/disable gate for weighted graph specializations.
     * @param source_id Source node ID.
     * @param target_id Target node ID.
     * @param weight Compatibility name for the built-in edge-weight property.
     * @return A `std::vector<NodeID>` describing the shortest path from source to target.
     */
    template <bool W = Weighted>
    requires(W)
    auto dijkstra_path(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const;

    /**
     * @brief Returns distances and predecessors for all nodes reachable from a source.
     *
     * @tparam W Internal enable/disable gate for weighted graph specializations.
     * @param source_id Source node ID.
     * @return A @ref SingleSourceShortestPathResult carrying ordered
     * `distance` and `predecessor` maps keyed by `NodeID`.
     */
    template <bool W = Weighted>
    requires(W)
    auto dijkstra_shortest_paths(const NodeID& source_id) const;

    /**
     * @brief Returns built-in-weight shortest-path distances from a source node.
     *
     * @tparam W Internal enable/disable gate for weighted graph specializations.
     * @param source_id Source node ID.
     * @return An ordered `std::map<NodeID, EdgeWeight>` of distances from @p source_id.
     */
    template <bool W = Weighted>
    requires(W)
    auto dijkstra_path_length(const NodeID& source_id) const;

    /**
     * @brief Returns the built-in-weight shortest-path length between two nodes.
     *
     * @tparam W Internal enable/disable gate for weighted graph specializations.
     * @param source_id Source node ID.
     * @param target_id Target node ID.
     * @return The shortest-path distance as `EdgeWeight`.
     */
    template <bool W = Weighted>
    requires(W)
    auto dijkstra_path_length(const NodeID& source_id, const NodeID& target_id) const;

    /**
     * @brief Returns the shortest-path length using the built-in edge-weight property.
     *
     * This overload accepts the compatibility-shaped `"weight"` name, not an
     * arbitrary user-defined weight attribute.
     *
     * @tparam W Internal enable/disable gate for weighted graph specializations.
     * @param source_id Source node ID.
     * @param target_id Target node ID.
     * @param weight Compatibility name for the built-in edge-weight property.
     * @return The shortest-path distance as `EdgeWeight`.
     */
    template <bool W = Weighted>
    requires(W)
    auto dijkstra_path_length(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const;

    /**
     * @brief Computes a shortest path using Bellman-Ford and built-in weights.
     *
     * @tparam W Internal enable/disable gate for weighted graph specializations.
     * @param source_id Source node ID.
     * @param target_id Target node ID.
     * @return A `std::vector<NodeID>` describing the shortest path from source to target.
     */
    template <bool W = Weighted>
    requires(W)
    auto bellman_ford_path(const NodeID& source_id, const NodeID& target_id) const;

    /**
     * @brief Returns distances and predecessors for all nodes using Bellman-Ford.
     *
     * @tparam W Internal enable/disable gate for weighted graph specializations.
     * @param source_id Source node ID.
     * @return A @ref SingleSourceShortestPathResult carrying ordered
     * `distance` and `predecessor` maps keyed by `NodeID`.
     */
    template <bool W = Weighted>
    requires(W)
    auto bellman_ford_shortest_paths(const NodeID& source_id) const;

    /**
     * @brief Computes a shortest path using Bellman-Ford and a named edge attribute.
     *
     * @tparam W Internal enable/disable gate for weighted graph specializations.
     * @param source_id Source node ID.
     * @param target_id Target node ID.
     * @param weight Name of the edge attribute interpreted as a numeric weight.
     * @return A `std::vector<NodeID>` describing the shortest path from source to target.
     */
    template <bool W = Weighted>
    requires(W)
    auto bellman_ford_path(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const;

    /**
     * @brief Returns the Bellman-Ford shortest-path length with built-in weights.
     *
     * @tparam W Internal enable/disable gate for weighted graph specializations.
     * @param source_id Source node ID.
     * @param target_id Target node ID.
     * @return The shortest-path distance as `EdgeWeight`.
     */
    template <bool W = Weighted>
    requires(W)
    auto bellman_ford_path_length(const NodeID& source_id, const NodeID& target_id) const;

    /**
     * @brief Returns the Bellman-Ford shortest-path length with a named edge attribute.
     *
     * @tparam W Internal enable/disable gate for weighted graph specializations.
     * @param source_id Source node ID.
     * @param target_id Target node ID.
     * @param weight Name of the edge attribute interpreted as a numeric weight.
     * @return The shortest-path distance as `EdgeWeight`.
     */
    template <bool W = Weighted>
    requires(W)
    auto bellman_ford_path_length(const NodeID& source_id, const NodeID& target_id, const std::string& weight) const;

    /**
     * @brief Returns shortest-path distances in a directed acyclic graph.
     *
     * @tparam W Internal enable/disable gate for weighted graph specializations.
     * @param source_id Source node ID.
     * @return A @ref SingleSourceShortestPathResult carrying ordered
     * `distance` and `predecessor` maps keyed by `NodeID`.
     */
    template <bool W = Weighted>
    requires(W)
    auto dag_shortest_paths(const NodeID& source_id) const;

    /**
     * @brief Returns all-pairs shortest-path distances as a dense matrix.
     *
     * The returned matrix follows the wrapper's stable node-ordering, so row
     * and column positions correspond to the order reported by @ref nodes.
     *
     * @tparam W Internal enable/disable gate for weighted graph specializations.
     * @return A dense `std::vector<std::vector<EdgeWeight>>` shortest-path
     * matrix ordered by the stable node order reported by @ref nodes.
     */
    template <bool W = Weighted>
    requires(W)
    auto floyd_warshall_all_pairs_shortest_paths() const;

    /**
     * @brief Returns all-pairs shortest-path distances keyed by node IDs.
     *
     * This wrapper expands the dense Floyd-Warshall matrix into nested ordered
     * maps so callers can address results directly by source and target ID.
     *
     * @tparam W Internal enable/disable gate for weighted graph specializations.
     * @return A nested `std::map<NodeID, std::map<NodeID, EdgeWeight>>`
     * keyed first by source node and then by target node.
     */
    template <bool W = Weighted>
    requires(W)
    auto floyd_warshall_all_pairs_shortest_paths_map() const;

    /// Groups each connected component as a vector of node IDs.
    auto connected_component_groups() const;
    /// Returns the number of connected components in an undirected graph.
    auto connected_components() const;
    /// Groups each strongly connected component as a vector of node IDs.
    auto strongly_connected_component_groups() const;
    /// Returns the component index assigned to each node in a directed graph.
    auto strong_component_map() const;
    /// Returns the number of strongly connected components in a directed graph.
    auto strong_components() const;
    /// Returns the connected-component index assigned to each node.
    auto connected_component_map() const;
    /// Alias for strongly_connected_component_groups().
    auto strongly_connected_components() const;
    /// Alias for strong_component_map().
    auto strongly_connected_component_map() const;
    /// Returns one representative root node for each strong component.
    auto strongly_connected_component_roots() const;
    /// Returns a topological ordering for a directed acyclic graph.
    auto topological_sort() const;

    /**
     * @brief Returns the edges selected by Kruskal's minimum-spanning-tree algorithm.
     *
     * @tparam W Internal enable/disable gate for weighted graph specializations.
     * @return A `std::vector<std::pair<NodeID, NodeID>>` containing the
     * selected spanning-tree edges as `(u, v)` pairs.
     */
    template <bool W = Weighted>
    requires(W)
    auto kruskal_minimum_spanning_tree() const;

    /**
     * @brief Returns the parent map produced by Prim's minimum-spanning-tree algorithm.
     *
     * @tparam W Internal enable/disable gate for weighted graph specializations.
     * @param root_id Root node used to seed Prim's algorithm.
     * @return An ordered `std::map<NodeID, NodeID>` parent map for the spanning tree.
     */
    template <bool W = Weighted>
    requires(W)
    auto prim_minimum_spanning_tree(const NodeID& root_id) const;

    /**
     * @brief Convenience alias for @ref kruskal_minimum_spanning_tree().
     *
     * @tparam W Internal enable/disable gate for weighted graph specializations.
     * @return A `std::vector<std::pair<NodeID, NodeID>>` containing the
     * selected spanning-tree edges as `(u, v)` pairs.
     */
    template <bool W = Weighted>
    requires(W)
    auto minimum_spanning_tree() const;

    /**
     * @brief Convenience alias for `prim_minimum_spanning_tree(root_id)`.
     *
     * @tparam W Internal enable/disable gate for weighted graph specializations.
     * @param root_id Root node used to seed Prim's algorithm.
     * @return An ordered `std::map<NodeID, NodeID>` parent map for the spanning tree.
     */
    template <bool W = Weighted>
    requires(W)
    auto minimum_spanning_tree(const NodeID& root_id) const;

    /**
     * @brief Computes a maximum flow using the Edmonds-Karp algorithm.
     *
     * @param source_id Source node ID.
     * @param target_id Sink node ID.
     * @param capacity_attr Name of the numeric edge attribute used as capacity.
     * @return A @ref MaximumFlowResult containing the total flow value and the
     * per-edge flow assignment keyed by `(u, v)`.
     */
    auto edmonds_karp_maximum_flow(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr = "capacity") const;
    /**
     * @brief Computes a maximum flow using push-relabel and returns edge assignments.
     *
     * @param source_id Source node ID.
     * @param target_id Sink node ID.
     * @param capacity_attr Name of the numeric edge attribute used as capacity.
     * @return A @ref MaximumFlowResult containing the total flow value and the
     * per-edge flow assignment keyed by `(u, v)`.
     */
    auto push_relabel_maximum_flow_result(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr = "capacity") const;
    /**
     * @brief Computes the maximum flow value and edge assignments using the named capacity attribute.
     *
     * @param source_id Source node ID.
     * @param target_id Sink node ID.
     * @param capacity_attr Name of the numeric edge attribute used as capacity.
     * @return A @ref MaximumFlowResult containing the total flow value and the
     * per-edge flow assignment keyed by `(u, v)`.
     */
    auto maximum_flow(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr = "capacity") const;
    /**
     * @brief Computes a minimum cut between source and target using the named capacity attribute.
     *
     * @param source_id Source node ID.
     * @param target_id Sink node ID.
     * @param capacity_attr Name of the numeric edge attribute used as capacity.
     * @return A @ref MinimumCutResult containing the cut value, the two partitions,
     * and the cut edges crossing from reachable to non-reachable nodes.
     */
    auto minimum_cut(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr = "capacity") const;
    /**
     * @brief Computes a max-flow min-cost result using cycle canceling.
     *
     * @param source_id Source node ID.
     * @param target_id Sink node ID.
     * @param capacity_attr Name of the numeric edge attribute used as capacity.
     * @param weight_attr Name of the numeric edge attribute used as cost. The default `"weight"` refers to the built-in edge-weight property.
     * @return A @ref MinCostMaxFlowResult containing total flow, total cost,
     * and the per-edge flow assignment keyed by `(u, v)`.
     */
    auto max_flow_min_cost_cycle_canceling(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") const;
    /// Runs push-relabel and caches the residual state for follow-up min-cost routines. The default `"weight"` still refers to the built-in edge-weight property.
    long push_relabel_maximum_flow(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") const;
    /**
     * @brief Runs cycle canceling on the previously cached residual network.
     *
     * @param weight_attr Name of the numeric edge attribute used as cost. The default `"weight"` refers to the built-in edge-weight property.
     * @return A @ref MinCostMaxFlowResult containing total flow, total cost,
     * and the per-edge flow assignment keyed by `(u, v)`.
     */
    auto cycle_canceling(const std::string& weight_attr = "weight") const;
    /**
     * @brief Computes a max-flow min-cost result using successive shortest path.
     *
     * @param source_id Source node ID.
     * @param target_id Sink node ID.
     * @param capacity_attr Name of the numeric edge attribute used as capacity.
     * @param weight_attr Name of the numeric edge attribute used as cost. The default `"weight"` refers to the built-in edge-weight property.
     * @return A @ref MinCostMaxFlowResult containing total flow, total cost,
     * and the per-edge flow assignment keyed by `(u, v)`.
     */
    auto successive_shortest_path_nonnegative_weights(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") const;
    /**
     * @brief Convenience alias for successive_shortest_path_nonnegative_weights().
     *
     * @param source_id Source node ID.
     * @param target_id Sink node ID.
     * @param capacity_attr Name of the numeric edge attribute used as capacity.
     * @param weight_attr Name of the numeric edge attribute used as cost. The default `"weight"` refers to the built-in edge-weight property.
     * @return A @ref MinCostMaxFlowResult containing total flow, total cost,
     * and the per-edge flow assignment keyed by `(u, v)`.
     */
    auto max_flow_min_cost_successive_shortest_path(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") const;
    /**
     * @brief Convenience alias for the default max-flow min-cost wrapper.
     *
     * @param source_id Source node ID.
     * @param target_id Sink node ID.
     * @param capacity_attr Name of the numeric edge attribute used as capacity.
     * @param weight_attr Name of the numeric edge attribute used as cost. The default `"weight"` refers to the built-in edge-weight property.
     * @return A @ref MinCostMaxFlowResult containing total flow, total cost,
     * and the per-edge flow assignment keyed by `(u, v)`.
     */
    auto max_flow_min_cost(const NodeID& source_id, const NodeID& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") const;

    /// @brief Returns the number of vertices currently stored in the graph.
    /// @return The current vertex count as an `int`.
    auto num_vertices() const;
    /**
     * @brief Computes normalized degree centrality for every node.
     *
     * For undirected graphs this uses the standard degree. For directed graphs
     * it uses `in_degree + out_degree`. The result is normalized by `n - 1`,
     * matching the usual NetworkX-style convention.
     *
     * @return An `indexed_lookup_map<NodeID, double>` keyed by public node ID
     * that stores one normalized degree-centrality score per node.
     */
    auto degree_centrality() const;
    /**
     * @brief Computes PageRank scores for every node.
     *
     * The current wrapper uses a small fixed-iteration implementation with
     * damping factor `0.85` and explicit redistribution of dangling-node mass.
     * It is intentionally conservative and returns a ready-to-consume result
     * keyed by public node ID instead of exposing lower-level property-map
     * plumbing.
     *
     * @return An `indexed_lookup_map<NodeID, double>` keyed by public node ID
     * containing one PageRank score per node.
     */
    auto pagerank() const;
    /**
     * @brief Computes normalized betweenness centrality for every node.
     *
     * The current implementation uses a self-contained Brandes shortest-path
     * accumulation pass and normalizes the result with semantics matching
     * `betweenness_centrality(G, normalized=True)` in NetworkX.
     *
     * @return An `indexed_lookup_map<NodeID, double>` keyed by public node ID
     * containing one normalized betweenness-centrality score per node.
     */
    auto betweenness_centrality() const;
};

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::num_vertices() const {
    return static_cast<int>(boost::num_vertices(g));
}


template <typename GraphWrapper>
/// @brief Deprecated free-function alias for num_vertices().
[[deprecated("Use G.num_vertices() instead.")]]
auto num_vertices(const GraphWrapper& G) {
    return G.num_vertices();
}


/**
 * @brief Explicit weighted alias presets for the primary @ref Graph template.
 *
 * These aliases are the clearest names when a caller wants weightedness,
 * directedness, and multigraph support to be visible directly in the type.
 */
using WeightedGraphInt = Graph<int, int>;
using WeightedDiGraphInt = Graph<int, int, true>;
using WeightedGraphStr = Graph<std::string>;
using WeightedMultiGraphInt = Graph<int, int, false, true>;
using WeightedMultiDiGraphInt = Graph<int, int, true, true>;

using WeightedDiGraph = Graph<std::string, double, true>;
using WeightedMultiGraph = Graph<std::string, double, false, true>;
using WeightedMultiDiGraph = Graph<std::string, double, true, true>;

/**
 * @brief Compatibility-friendly short aliases for the common weighted presets.
 *
 * These are thin synonyms of the corresponding `Weighted*` aliases. They stay
 * in the public API for ergonomic parity, but the `Weighted*` forms are the
 * more explicit names in the current docs.
 */
using GraphInt = Graph<int, int>;
using DiGraphInt = Graph<int, int, true>;
using GraphStr = Graph<std::string>;
using MultiGraphInt = Graph<int, int, false, true>;
using MultiDiGraphInt = Graph<int, int, true, true>;

using DiGraph = Graph<std::string, double, true>;
using MultiGraph = Graph<std::string, double, false, true>;
using MultiDiGraph = Graph<std::string, double, true, true>;

/**
 * @brief Explicit unweighted alias presets for the primary @ref Graph template.
 *
 * These aliases keep the default selectors but disable the built-in edge-weight
 * property, making the unweighted choice visible at the type level.
 */
using UnweightedGraphInt = Graph<int, double, false, false, false>;
using UnweightedDiGraphInt = Graph<int, double, true, false, false>;
using UnweightedGraphStr = Graph<std::string, double, false, false, false>;
using UnweightedDiGraph = Graph<std::string, double, true, false, false>;
using UnweightedMultiGraphInt = Graph<int, double, false, true, false>;
using UnweightedMultiDiGraphInt = Graph<int, double, true, true, false>;
using UnweightedMultiGraph = Graph<std::string, double, false, true, false>;
using UnweightedMultiDiGraph = Graph<std::string, double, true, true, false>;

} // namespace nxpp
