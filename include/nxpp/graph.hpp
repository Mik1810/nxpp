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

    bool has_edge_id(std::size_t edge_id) const;
    std::vector<std::size_t> edge_ids() const;
    std::vector<std::size_t> edge_ids(const NodeID& u, const NodeID& v) const;
    std::pair<NodeID, NodeID> get_edge_endpoints(std::size_t edge_id) const;

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
    std::size_t add_edge_with_id(const NodeID& u, const NodeID& v, EdgeWeight w = 1.0);

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
    std::size_t add_edge_with_id(const NodeID& u, const NodeID& v);


    template <bool W = Weighted>
    requires(W)
    void add_edge(const NodeID& u, const NodeID& v, EdgeWeight w, const EdgeAttrMap& attrs);

    void add_edge(const NodeID& u, const NodeID& v, const EdgeAttrMap& attrs);

    template <bool W = Weighted>
    requires(W)
    void add_edge(const NodeID& u, const NodeID& v, EdgeWeight w, const std::pair<std::string, std::any>& attr);

    void add_edge(const NodeID& u, const NodeID& v, const std::pair<std::string, std::any>& attr);

    template <bool W = Weighted>
    requires(W)
    void add_edge(const NodeID& u, const NodeID& v, EdgeWeight w, std::initializer_list<std::pair<std::string, std::any>> attrs);

    void add_edge(const NodeID& u, const NodeID& v, std::initializer_list<std::pair<std::string, std::any>> attrs);
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

    void remove_edge(std::size_t edge_id);

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


    bool has_node_attr(const NodeID& u, const std::string& key) const;
    bool has_edge_attr(const NodeID& u, const NodeID& v, const std::string& key) const;
    bool has_edge_attr(std::size_t edge_id, const std::string& key) const;

    template <typename T>
    T get_node_attr(const NodeID& u, const std::string& key) const;

    template <typename T>
    T get_edge_attr(const NodeID& u, const NodeID& v, const std::string& key) const;

    template <typename T>
    T get_edge_attr(std::size_t edge_id, const std::string& key) const;

    template <typename T>
    std::optional<T> try_get_node_attr(const NodeID& u, const std::string& key) const;

    template <typename T>
    std::optional<T> try_get_edge_attr(const NodeID& u, const NodeID& v, const std::string& key) const;

    template <typename T>
    std::optional<T> try_get_edge_attr(std::size_t edge_id, const std::string& key) const;

    double get_edge_numeric_attr(const NodeID& u, const NodeID& v, const std::string& key) const;
    double get_edge_numeric_attr(std::size_t edge_id, const std::string& key) const;

    template <bool W = Weighted>
    requires(W)
    EdgeWeight get_edge_weight(const NodeID& u, const NodeID& v) const;

    template <bool W = Weighted>
    requires(W)
    EdgeWeight get_edge_weight(std::size_t edge_id) const;

    template <bool W = Weighted>
    requires(W)
    void set_edge_weight(std::size_t edge_id, EdgeWeight w);

    template <typename T>
    void set_edge_attr(std::size_t edge_id, const std::string& key, const T& value);
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

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::num_vertices() const {
    return static_cast<int>(boost::num_vertices(g));
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::degree_centrality() const {
    const auto node_count = boost::num_vertices(g);
    std::unordered_map<NodeID, double> centrality;
    if (node_count == 0) return centrality;
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
        centrality[get_node_id(*v)] = 0.0;
    }
    if (node_count <= 1) return centrality;
    const double scale = 1.0 / static_cast<double>(node_count - 1);
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
        double degree = 0.0;
        if constexpr (Directed) {
            degree = static_cast<double>(boost::in_degree(*v, g) + boost::out_degree(*v, g));
        } else {
            degree = static_cast<double>(boost::degree(*v, g));
        }
        centrality[get_node_id(*v)] = degree * scale;
    }
    return centrality;
}

template <typename GraphWrapper>
[[deprecated("Use G.num_vertices() instead.")]]
auto num_vertices(const GraphWrapper& G) {
    return G.num_vertices();
}

template <typename GraphWrapper>
[[deprecated("Use G.degree_centrality() instead.")]]
auto degree_centrality(const GraphWrapper& G) {
    return G.degree_centrality();
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

} // namespace nxpp
