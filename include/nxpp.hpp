#ifndef NXPP_HPP
#define NXPP_HPP

#if !__has_include(<boost/graph/adjacency_list.hpp>)
#error "FATAL: nxpp requires the Boost Graph Library (BGL)."
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
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/find_flow_cost.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
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
#include <utility>
#include <iostream>
#include <any>
#include <map>
#include <iomanip>
#include <random>
#include <optional>
#include <limits>
#include <queue>

namespace nxpp {

// Python Emulation Utilities

// Python-style print() emulation: print(a, b, c) with spaces and auto-newline.
inline void print() {
    std::cout << "\n";
}

template <typename First, typename... Rest>
inline void print(First&& first, Rest&&... rest) {
    if constexpr (std::is_floating_point_v<std::decay_t<First>>) {
        std::cout << std::fixed << std::setprecision(1) << first;
    } else {
        std::cout << std::forward<First>(first);
    }

    if constexpr (sizeof...(rest) > 0) {
        std::cout << " ";
        print(std::forward<Rest>(rest)...);
    } else {
        std::cout << "\n";
    }
}

// Core Graph Class

template <typename NodeID = std::string, typename EdgeWeight = double, bool Directed = false, bool Multi = false>
class Graph {
public:
    using NodeType = NodeID;
    using DirectedSelector = typename std::conditional<Directed, boost::bidirectionalS, boost::undirectedS>::type;
    using EdgeProperty = boost::property<boost::edge_weight_t, EdgeWeight,
                                         boost::property<boost::edge_index_t, std::size_t>>;
    using GraphType = boost::adjacency_list<boost::vecS, boost::vecS, DirectedSelector,
                                            boost::no_property, EdgeProperty>;
    using VertexDesc = typename boost::graph_traits<GraphType>::vertex_descriptor;
    using EdgeDesc = typename boost::graph_traits<GraphType>::edge_descriptor;
    using WeightMap = typename boost::property_map<GraphType, boost::edge_weight_t>::type;
    using EdgeIdMap = typename boost::property_map<GraphType, boost::edge_index_t>::type;
    static constexpr bool is_directed = Directed;

private:
    GraphType g;
    WeightMap weight_map;
    EdgeIdMap edge_id_map;
    std::unordered_map<NodeID, VertexDesc> id_to_bgl;
    std::vector<NodeID> bgl_to_id;
    std::size_t next_edge_id = 0;

public:
    std::unordered_map<NodeID, std::unordered_map<std::string, std::any>> node_properties;
    std::unordered_map<std::size_t, std::unordered_map<std::string, std::any>> edge_properties;

private:
    using EdgeAttrMap = std::unordered_map<std::string, std::any>;

    VertexDesc get_or_create_vertex(const NodeID& id) {
        auto it = id_to_bgl.find(id);
        if (it != id_to_bgl.end()) {
            return it->second;
        }
        VertexDesc v = boost::add_vertex(g);
        id_to_bgl[id] = v;
        if (v >= bgl_to_id.size()) {
            bgl_to_id.resize(v + 1);
        }
        bgl_to_id[v] = id;
        return v;
    }

    std::size_t get_edge_id(EdgeDesc e) const {
        return boost::get(edge_id_map, e);
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
            edge_attr_map[key] = value;
        }
    }

    void assign_edge_attrs(EdgeDesc e, std::initializer_list<std::pair<std::string, std::any>> attrs) {
        auto& edge_attr_map = edge_properties[get_edge_id(e)];
        for (const auto& [key, value] : attrs) {
            edge_attr_map[key] = value;
        }
    }

    void assign_edge_attr(EdgeDesc e, const std::pair<std::string, std::any>& attr) {
        edge_properties[get_edge_id(e)][attr.first] = attr.second;
    }

public:
    Graph() : g(), weight_map(boost::get(boost::edge_weight, g)), edge_id_map(boost::get(boost::edge_index, g)) {}

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

    void add_edge(const NodeID& u, const NodeID& v, EdgeWeight w, const EdgeAttrMap& attrs) {
        add_edge(u, v, w);
        assign_edge_attrs(get_edge_desc(u, v), attrs);
    }

    void add_edge(const NodeID& u, const NodeID& v, const EdgeAttrMap& attrs) {
        add_edge(u, v, 1.0, attrs);
    }

    void add_edge(const NodeID& u, const NodeID& v, EdgeWeight w, const std::pair<std::string, std::any>& attr) {
        add_edge(u, v, w);
        assign_edge_attr(get_edge_desc(u, v), attr);
    }

    void add_edge(const NodeID& u, const NodeID& v, const std::pair<std::string, std::any>& attr) {
        add_edge(u, v, 1.0, attr);
    }

    void add_edge(const NodeID& u, const NodeID& v, EdgeWeight w, std::initializer_list<std::pair<std::string, std::any>> attrs) {
        add_edge(u, v, w);
        assign_edge_attrs(get_edge_desc(u, v), attrs);
    }

    void add_edge(const NodeID& u, const NodeID& v, std::initializer_list<std::pair<std::string, std::any>> attrs) {
        add_edge(u, v, 1.0, attrs);
    }

    void add_edges_from(const std::vector<std::tuple<NodeID, NodeID, EdgeWeight>>& edges) {
        for (const auto& edge : edges) {
            add_edge(std::get<0>(edge), std::get<1>(edge), std::get<2>(edge));
        }
    }

    void add_edges_from(const std::vector<std::pair<NodeID, NodeID>>& edges) {
        for (const auto& edge : edges) {
            add_edge(edge.first, edge.second, 1.0);
        }
    }

    void clear() {
        g = GraphType();
        id_to_bgl.clear();
        bgl_to_id.clear();
        node_properties.clear();
        edge_properties.clear();
        weight_map = boost::get(boost::edge_weight, g);
        edge_id_map = boost::get(boost::edge_index, g);
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
        edge_properties.erase(get_edge_id(e));
        boost::remove_edge(it_u->second, it_v->second, g);
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
        
        id_to_bgl.erase(it);
        node_properties.erase(u);

        // When using vecS, remove_vertex invalidates all descriptors > v
        // because all subsequent vertices are shifted down by 1.
        // We must re-sync all NodeID -> index mappings for shifted vertices.
        bgl_to_id.erase(bgl_to_id.begin() + v);
        
        // Full re-sync of the ID to BGL map
        for (size_t i = 0; i < bgl_to_id.size(); ++i) {
            id_to_bgl[bgl_to_id[i]] = (VertexDesc)i;
        }
    }

    std::vector<NodeID> neighbors(const NodeID& u) const {
        auto it = id_to_bgl.find(u);
        if (it == id_to_bgl.end()) {
            throw std::runtime_error("NetworkXError: The node is not in the graph.");
        }
        std::vector<NodeID> res;
        for (auto [e, eend] = boost::out_edges(it->second, g); e != eend; ++e) {
            res.push_back(bgl_to_id[boost::target(*e, g)]);
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
                res.push_back(bgl_to_id[boost::source(*e, g)]);
            }
        } else {
            for (auto [e, eend] = boost::out_edges(it->second, g); e != eend; ++e) {
                res.push_back(bgl_to_id[boost::target(*e, g)]);
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

    double get_edge_numeric_attr(const NodeID& u, const NodeID& v, const std::string& key) const {
        if (key == "weight") {
            return static_cast<double>(get_edge_weight(u, v));
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

    EdgeWeight get_edge_weight(const NodeID& u, const NodeID& v) const {
        auto it_u = id_to_bgl.find(u);
        auto it_v = id_to_bgl.find(v);
        if (it_u == id_to_bgl.end() || it_v == id_to_bgl.end()) throw std::runtime_error("Node not found in graph");
        auto [e, exists] = boost::edge(it_u->second, it_v->second, g);
        if (!exists) throw std::runtime_error("Edge not found in graph");
        return weight_map[e];
    }

    std::vector<NodeID> nodes() const {
        std::vector<NodeID> res;
        for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
            res.push_back(bgl_to_id[*v]);
        }
        return res;
    }

    std::vector<std::tuple<NodeID, NodeID, EdgeWeight>> edges() const {
        std::vector<std::tuple<NodeID, NodeID, EdgeWeight>> res;
        for (auto [e, eend] = boost::edges(g); e != eend; ++e) {
            NodeID source_id = bgl_to_id[boost::source(*e, g)];
            NodeID target_id = bgl_to_id[boost::target(*e, g)];
            res.emplace_back(source_id, target_id, weight_map[*e]);
        }
        return res;
    }

    // Accessors for BGL algorithms
    const GraphType& get_impl() const { return g; }
    const std::vector<NodeID>& get_bgl_to_id_map() const { return bgl_to_id; }
    const std::unordered_map<NodeID, VertexDesc>& get_id_to_bgl_map() const { return id_to_bgl; }

    // Proxy Pattern per simulare G[u][v] = weight
    struct EdgeAttrProxy {
        Graph* graph;
        NodeID u, v;
        std::string key;

        template <typename T>
        EdgeAttrProxy& operator=(const T& val) {
            if (!graph->has_edge(u, v)) graph->add_edge(u, v, 1.0);
            auto e = graph->get_edge_desc(u, v);
            graph->edge_properties[graph->get_edge_id(e)][key] = std::any(val);
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
        
        EdgeProxy& operator=(EdgeWeight w) {
            graph->add_edge(u, v, w);
            return *this;
        }

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
            graph->node_properties[u][key] = std::any(val);
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
using GraphInt = Graph<int>;
using DiGraphInt = Graph<int, double, true>;
using GraphStr = Graph<std::string>;
using MultiGraphInt = Graph<int, double, false, true>;
using MultiDiGraphInt = Graph<int, double, true, true>;

using DiGraph = Graph<std::string, double, true>;
using MultiGraph = Graph<std::string, double, false, true>;
using MultiDiGraph = Graph<std::string, double, true, true>;

template <typename NodeID>
struct MaximumFlowResult {
    long value = 0;
    std::map<std::pair<NodeID, NodeID>, long> flow;
};

template <typename NodeID>
struct MinCostMaxFlowResult {
    long value = 0;
    long cost = 0;
    std::map<std::pair<NodeID, NodeID>, long> flow;
};

template <typename NodeID>
struct MinimumCutResult {
    long value = 0;
    std::vector<NodeID> reachable;
    std::vector<NodeID> non_reachable;
    std::vector<std::pair<NodeID, NodeID>> cut_edges;
};

template <typename NodeID>
struct SingleSourceShortestPathResult {
    std::unordered_map<NodeID, double> distance;
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

template <typename GraphWrapper>
class default_graph_visitor {
public:
    using NodeID = typename GraphWrapper::NodeType;

    void examine_vertex(const NodeID&, const GraphWrapper&) {}
    void tree_edge(const NodeID&, const NodeID&, const GraphWrapper&) {}
    void back_edge(const NodeID&, const NodeID&, const GraphWrapper&) {}
};


// Algorithms: Traversals

template <typename NodeID, typename Edge>
class GenericBfsEdgeVisitor : public boost::default_bfs_visitor {
public:
    GenericBfsEdgeVisitor(std::vector<std::pair<NodeID, NodeID>>& edges, const std::vector<NodeID>& bgl_to_id)
        : tree_edges(edges), bgl_to_id(bgl_to_id) {}

    template <typename G>
    void tree_edge(Edge e, const G& g) {
        NodeID u = bgl_to_id[boost::source(e, g)];
        NodeID v = bgl_to_id[boost::target(e, g)];
        tree_edges.emplace_back(u, v);
    }
private:
    std::vector<std::pair<NodeID, NodeID>>& tree_edges;
    const std::vector<NodeID>& bgl_to_id;
};

template <typename GraphWrapper>
auto bfs_edges(const GraphWrapper& G, const typename GraphWrapper::NodeType& start) {
    using NodeID = typename GraphWrapper::NodeType;
    std::vector<std::pair<NodeID, NodeID>> edges;
    using GraphType = typename GraphWrapper::GraphType;
    using EdgeType = typename boost::graph_traits<GraphType>::edge_descriptor;
    
    if (!G.has_node(start)) throw std::runtime_error("Start node not found in graph");
    auto start_bgl = G.get_id_to_bgl_map().at(start);

    GenericBfsEdgeVisitor<NodeID, EdgeType> vis(edges, G.get_bgl_to_id_map());
    boost::breadth_first_search(G.get_impl(), start_bgl, boost::visitor(vis));
    return edges;
}

template <typename GraphWrapper>
auto bfs_tree(const GraphWrapper& G, const typename GraphWrapper::NodeType& start) {
    using NodeID = typename GraphWrapper::NodeType;
    Graph<NodeID, double, GraphWrapper::is_directed> tree;

    if (!G.has_node(start)) {
        throw std::runtime_error("Start node not found in graph");
    }

    tree.add_node(start);
    for (const auto& [u, v] : bfs_edges(G, start)) {
        tree.add_edge(u, v);
    }
    return tree;
}

template <typename GraphWrapper>
auto bfs_successors(const GraphWrapper& G, const typename GraphWrapper::NodeType& start) {
    using NodeID = typename GraphWrapper::NodeType;

    if (!G.has_node(start)) {
        throw std::runtime_error("Start node not found in graph");
    }

    std::unordered_map<NodeID, std::vector<NodeID>> result;
    for (const auto& [u, v] : bfs_edges(G, start)) {
        result[u].push_back(v);
    }
    return result;
}

template <typename NodeID, typename Edge, typename OnVertex, typename OnTreeEdge>
class GenericBfsVisitVisitor : public boost::default_bfs_visitor {
public:
    GenericBfsVisitVisitor(const std::vector<NodeID>& bgl_to_id, OnVertex& on_vertex, OnTreeEdge& on_tree_edge)
        : bgl_to_id(bgl_to_id), on_vertex(on_vertex), on_tree_edge(on_tree_edge) {}

    template <typename G>
    void examine_vertex(typename boost::graph_traits<G>::vertex_descriptor u, const G&) const {
        on_vertex(bgl_to_id[u]);
    }

    template <typename G>
    void tree_edge(Edge e, const G& g) const {
        on_tree_edge(bgl_to_id[boost::source(e, g)], bgl_to_id[boost::target(e, g)]);
    }

private:
    const std::vector<NodeID>& bgl_to_id;
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
        bfs_dispatch_examine_vertex(visitor, graph_wrapper.get_bgl_to_id_map()[u], graph_wrapper);
    }

    template <typename G>
    void tree_edge(Edge e, const G& g) const {
        const auto& bgl_to_id = graph_wrapper.get_bgl_to_id_map();
        bfs_dispatch_tree_edge(
            visitor,
            bgl_to_id[boost::source(e, g)],
            bgl_to_id[boost::target(e, g)],
            graph_wrapper
        );
    }

private:
    const GraphWrapper& graph_wrapper;
    Visitor& visitor;
};

template <typename GraphWrapper, typename Visitor>
void breadth_first_search(const GraphWrapper& G, const typename GraphWrapper::NodeType& start, Visitor& visitor) {
    using NodeID = typename GraphWrapper::NodeType;
    using GraphType = typename GraphWrapper::GraphType;
    using EdgeType = typename boost::graph_traits<GraphType>::edge_descriptor;

    if (!G.has_node(start)) throw std::runtime_error("Start node not found in graph");
    auto start_bgl = G.get_id_to_bgl_map().at(start);

    GenericBfsObjectVisitor<NodeID, EdgeType, GraphWrapper, Visitor> vis(G, visitor);
    boost::breadth_first_search(G.get_impl(), start_bgl, boost::visitor(vis));
}

template <typename GraphWrapper, typename OnVertex, typename OnTreeEdge>
void bfs_visit(const GraphWrapper& G, const typename GraphWrapper::NodeType& start, OnVertex&& on_vertex, OnTreeEdge&& on_tree_edge) {
    struct callback_visitor {
        OnVertex& on_vertex;
        OnTreeEdge& on_tree_edge;

        void examine_vertex(const typename GraphWrapper::NodeType& u, const GraphWrapper&) {
            on_vertex(u);
        }

        void tree_edge(const typename GraphWrapper::NodeType& u, const typename GraphWrapper::NodeType& v, const GraphWrapper&) {
            on_tree_edge(u, v);
        }
    };

    auto on_vertex_fn = std::forward<OnVertex>(on_vertex);
    auto on_tree_edge_fn = std::forward<OnTreeEdge>(on_tree_edge);
    callback_visitor visitor{on_vertex_fn, on_tree_edge_fn};
    breadth_first_search(G, start, visitor);
}

template <typename NodeID, typename Edge>
class GenericDfsEdgeVisitor : public boost::default_dfs_visitor {
public:
    GenericDfsEdgeVisitor(std::vector<std::pair<NodeID, NodeID>>& edges, const std::vector<NodeID>& bgl_to_id)
        : tree_edges(edges), bgl_to_id(bgl_to_id) {}

    template <typename G>
    void tree_edge(Edge e, const G& g) {
        NodeID u = bgl_to_id[boost::source(e, g)];
        NodeID v = bgl_to_id[boost::target(e, g)];
        tree_edges.emplace_back(u, v);
    }
private:
    std::vector<std::pair<NodeID, NodeID>>& tree_edges;
    const std::vector<NodeID>& bgl_to_id;
};

template <typename GraphWrapper>
auto dfs_edges(const GraphWrapper& G, const typename GraphWrapper::NodeType& start) {
    using NodeID = typename GraphWrapper::NodeType;
    std::vector<std::pair<NodeID, NodeID>> edges;
    using GraphType = typename GraphWrapper::GraphType;
    using EdgeType = typename boost::graph_traits<GraphType>::edge_descriptor;
    
    if (!G.has_node(start)) throw std::runtime_error("Start node not found in graph");
    auto start_bgl = G.get_id_to_bgl_map().at(start);

    GenericDfsEdgeVisitor<NodeID, EdgeType> vis(edges, G.get_bgl_to_id_map());
    boost::depth_first_search(G.get_impl(), boost::root_vertex(start_bgl).visitor(vis));
    return edges;
}

template <typename GraphWrapper>
auto dfs_tree(const GraphWrapper& G, const typename GraphWrapper::NodeType& start) {
    using NodeID = typename GraphWrapper::NodeType;
    Graph<NodeID, double, GraphWrapper::is_directed> tree;

    if (!G.has_node(start)) {
        throw std::runtime_error("Start node not found in graph");
    }

    tree.add_node(start);
    for (const auto& [u, v] : dfs_edges(G, start)) {
        tree.add_edge(u, v);
    }
    return tree;
}

template <typename GraphWrapper>
auto dfs_predecessors(const GraphWrapper& G, const typename GraphWrapper::NodeType& start) {
    using NodeID = typename GraphWrapper::NodeType;

    if (!G.has_node(start)) {
        throw std::runtime_error("Start node not found in graph");
    }

    std::unordered_map<NodeID, NodeID> result;
    for (const auto& [u, v] : dfs_edges(G, start)) {
        result[v] = u;
    }
    return result;
}

template <typename GraphWrapper>
auto dfs_successors(const GraphWrapper& G, const typename GraphWrapper::NodeType& start) {
    using NodeID = typename GraphWrapper::NodeType;

    if (!G.has_node(start)) {
        throw std::runtime_error("Start node not found in graph");
    }

    std::unordered_map<NodeID, std::vector<NodeID>> result;
    for (const auto& [u, v] : dfs_edges(G, start)) {
        result[u].push_back(v);
    }
    return result;
}

template <typename NodeID, typename VertexDesc>
std::unordered_map<NodeID, std::vector<NodeID>> build_single_source_paths(
    const std::vector<NodeID>& bgl_to_id,
    const std::vector<double>& dist,
    const std::vector<VertexDesc>& pred
) {
    std::unordered_map<NodeID, std::vector<NodeID>> paths;
    const auto unreachable = std::numeric_limits<double>::max();

    for (size_t i = 0; i < bgl_to_id.size(); ++i) {
        const NodeID target_id = bgl_to_id[i];
        if (dist[i] == unreachable) {
            continue;
        }

        std::vector<NodeID> path;
        VertexDesc current = static_cast<VertexDesc>(i);
        while (true) {
            path.push_back(bgl_to_id[current]);
            if (pred[current] == current) {
                break;
            }
            current = pred[current];
        }
        std::reverse(path.begin(), path.end());
        paths[target_id] = std::move(path);
    }

    return paths;
}

template <typename NodeID, typename Edge, typename OnTreeEdge, typename OnBackEdge>
class GenericDfsVisitVisitor : public boost::default_dfs_visitor {
public:
    GenericDfsVisitVisitor(const std::vector<NodeID>& bgl_to_id, OnTreeEdge& on_tree_edge, OnBackEdge& on_back_edge)
        : bgl_to_id(bgl_to_id), on_tree_edge(on_tree_edge), on_back_edge(on_back_edge) {}

    template <typename G>
    void tree_edge(Edge e, const G& g) const {
        on_tree_edge(bgl_to_id[boost::source(e, g)], bgl_to_id[boost::target(e, g)]);
    }

    template <typename G>
    void back_edge(Edge e, const G& g) const {
        on_back_edge(bgl_to_id[boost::source(e, g)], bgl_to_id[boost::target(e, g)]);
    }

private:
    const std::vector<NodeID>& bgl_to_id;
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
        const auto& bgl_to_id = graph_wrapper.get_bgl_to_id_map();
        dfs_dispatch_tree_edge(
            visitor,
            bgl_to_id[boost::source(e, g)],
            bgl_to_id[boost::target(e, g)],
            graph_wrapper
        );
    }

    template <typename G>
    void back_edge(Edge e, const G& g) const {
        const auto& bgl_to_id = graph_wrapper.get_bgl_to_id_map();
        dfs_dispatch_back_edge(
            visitor,
            bgl_to_id[boost::source(e, g)],
            bgl_to_id[boost::target(e, g)],
            graph_wrapper
        );
    }

private:
    const GraphWrapper& graph_wrapper;
    Visitor& visitor;
};

template <typename GraphWrapper, typename Visitor>
void depth_first_search(const GraphWrapper& G, const typename GraphWrapper::NodeType& start, Visitor& visitor) {
    using NodeID = typename GraphWrapper::NodeType;
    using GraphType = typename GraphWrapper::GraphType;
    using EdgeType = typename boost::graph_traits<GraphType>::edge_descriptor;

    if (!G.has_node(start)) throw std::runtime_error("Start node not found in graph");
    auto start_bgl = G.get_id_to_bgl_map().at(start);

    GenericDfsObjectVisitor<NodeID, EdgeType, GraphWrapper, Visitor> vis(G, visitor);
    boost::depth_first_search(G.get_impl(), boost::root_vertex(start_bgl).visitor(vis));
}

template <typename GraphWrapper, typename OnTreeEdge, typename OnBackEdge>
void dfs_visit(const GraphWrapper& G, const typename GraphWrapper::NodeType& start, OnTreeEdge&& on_tree_edge, OnBackEdge&& on_back_edge) {
    struct callback_visitor {
        OnTreeEdge& on_tree_edge;
        OnBackEdge& on_back_edge;

        void tree_edge(const typename GraphWrapper::NodeType& u, const typename GraphWrapper::NodeType& v, const GraphWrapper&) {
            on_tree_edge(u, v);
        }

        void back_edge(const typename GraphWrapper::NodeType& u, const typename GraphWrapper::NodeType& v, const GraphWrapper&) {
            on_back_edge(u, v);
        }
    };

    auto on_tree_edge_fn = std::forward<OnTreeEdge>(on_tree_edge);
    auto on_back_edge_fn = std::forward<OnBackEdge>(on_back_edge);
    callback_visitor visitor{on_tree_edge_fn, on_back_edge_fn};
    depth_first_search(G, start, visitor);
}


// Algorithms: Shortest Paths

template <typename GraphWrapper>
auto shortest_path(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id) {
    using NodeID = typename GraphWrapper::NodeType;
    using VertexDesc = typename GraphWrapper::VertexDesc;

    const auto& g = G.get_impl();
    const auto& id_to_bgl = G.get_id_to_bgl_map();
    const auto& bgl_to_id = G.get_bgl_to_id_map();

    if (id_to_bgl.find(source_id) == id_to_bgl.end() || id_to_bgl.find(target_id) == id_to_bgl.end()) {
        throw std::runtime_error("Source or target node not found in graph.");
    }

    VertexDesc source = id_to_bgl.at(source_id);
    VertexDesc target = id_to_bgl.at(target_id);

    if (source == target) {
        return std::vector<NodeID>{source_id};
    }

    const size_t n = boost::num_vertices(g);
    std::vector<bool> visited(n, false);
    std::vector<VertexDesc> pred(n);
    std::queue<VertexDesc> q;

    for (size_t i = 0; i < n; ++i) {
        pred[i] = static_cast<VertexDesc>(i);
    }

    visited[source] = true;
    q.push(source);

    while (!q.empty()) {
        VertexDesc current = q.front();
        q.pop();

        for (auto [e, eend] = boost::out_edges(current, g); e != eend; ++e) {
            VertexDesc next = boost::target(*e, g);
            if (!visited[next]) {
                visited[next] = true;
                pred[next] = current;
                if (next == target) {
                    q = {};
                    break;
                }
                q.push(next);
            }
        }
    }

    if (!visited[target]) {
        throw std::runtime_error("Node not reachable");
    }

    std::vector<NodeID> path;
    for (VertexDesc curr = target; curr != source; curr = pred[curr]) {
        path.push_back(bgl_to_id[curr]);
    }
    path.push_back(bgl_to_id[source]);
    std::reverse(path.begin(), path.end());
    return path;
}

template <typename GraphWrapper>
auto dijkstra_path(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id) {
    using NodeID = typename GraphWrapper::NodeType;
    using VertexDesc = typename GraphWrapper::VertexDesc;
    auto& g = G.get_impl();
    auto& id_to_bgl = G.get_id_to_bgl_map();
    auto& bgl_to_id = G.get_bgl_to_id_map();

    if (id_to_bgl.find(source_id) == id_to_bgl.end() || id_to_bgl.find(target_id) == id_to_bgl.end()) {
        throw std::runtime_error("Source or target node not found in graph.");
    }
    VertexDesc source = id_to_bgl.at(source_id);
    VertexDesc target = id_to_bgl.at(target_id);

    size_t n = boost::num_vertices(g);
    std::vector<double> dist(n);
    std::vector<VertexDesc> pred(n);

    boost::dijkstra_shortest_paths(g, source, 
        boost::distance_map(boost::make_iterator_property_map(dist.begin(), boost::get(boost::vertex_index, g)))
        .predecessor_map(boost::make_iterator_property_map(pred.begin(), boost::get(boost::vertex_index, g))));

    if (pred[target] == target && source != target) {
        throw std::runtime_error("Node not reachable");
    }

    std::vector<NodeID> path;
    for (VertexDesc curr = target; curr != source; curr = pred[curr]) {
        path.push_back(bgl_to_id[curr]);
    }
    path.push_back(bgl_to_id[source]);
    std::reverse(path.begin(), path.end());
    return path;
}

template <typename GraphWrapper>
auto single_source_dijkstra(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id) {
    using NodeID = typename GraphWrapper::NodeType;
    using VertexDesc = typename GraphWrapper::VertexDesc;

    const auto& g = G.get_impl();
    const auto& id_to_bgl = G.get_id_to_bgl_map();
    const auto& bgl_to_id = G.get_bgl_to_id_map();

    if (id_to_bgl.find(source_id) == id_to_bgl.end()) {
        throw std::runtime_error("Source node not found in graph.");
    }

    const VertexDesc source = id_to_bgl.at(source_id);
    const size_t n = boost::num_vertices(g);
    std::vector<double> dist(n, std::numeric_limits<double>::max());
    std::vector<VertexDesc> pred(n);
    for (size_t i = 0; i < n; ++i) {
        pred[i] = static_cast<VertexDesc>(i);
    }

    boost::dijkstra_shortest_paths(
        g,
        source,
        boost::distance_map(boost::make_iterator_property_map(dist.begin(), boost::get(boost::vertex_index, g)))
            .predecessor_map(boost::make_iterator_property_map(pred.begin(), boost::get(boost::vertex_index, g)))
    );

    SingleSourceShortestPathResult<NodeID> result;
    for (size_t i = 0; i < n; ++i) {
        result.distance[bgl_to_id[i]] = dist[i];
        result.predecessor[bgl_to_id[i]] = bgl_to_id[pred[i]];
    }
    result.paths = build_single_source_paths<NodeID>(bgl_to_id, dist, pred);
    return result;
}

template <typename GraphWrapper>
auto shortest_path(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& weight) {
    if (weight.empty()) {
        return shortest_path(G, source_id, target_id);
    }
    if (weight == "weight") {
        return dijkstra_path(G, source_id, target_id);
    }
    throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
}

template <typename GraphWrapper>
auto dijkstra_path(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& weight) {
    if (weight.empty() || weight == "weight") {
        return dijkstra_path(G, source_id, target_id);
    }
    throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
}

template <typename GraphWrapper>
double shortest_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id) {
    const auto path = shortest_path(G, source_id, target_id);
    if (path.empty()) {
        throw std::runtime_error("Node not reachable");
    }
    return static_cast<double>(path.size() - 1);
}

template <typename GraphWrapper>
double shortest_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& weight) {
    if (weight.empty()) {
        return shortest_path_length(G, source_id, target_id);
    }
    if (weight == "weight") {
        return dijkstra_path_length(G, source_id, target_id);
    }
    throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
}

template <typename GraphWrapper>
auto dijkstra_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id) {
    return single_source_dijkstra(G, source_id).distance;
}

template <typename GraphWrapper>
double dijkstra_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id) {
    const auto distances = dijkstra_path_length(G, source_id);
    auto it = distances.find(target_id);
    if (it == distances.end()) {
        throw std::runtime_error("Target node not found in graph.");
    }
    return it->second;
}

template <typename GraphWrapper>
double dijkstra_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& weight) {
    if (weight.empty() || weight == "weight") {
        return dijkstra_path_length(G, source_id, target_id);
    }
    throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
}

template <typename GraphWrapper>
auto bellman_ford_path(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id) {
    using NodeID = typename GraphWrapper::NodeType;
    using VertexDesc = typename GraphWrapper::VertexDesc;

    const auto& g = G.get_impl();
    const auto& id_to_bgl = G.get_id_to_bgl_map();
    const auto& bgl_to_id = G.get_bgl_to_id_map();

    if (id_to_bgl.find(source_id) == id_to_bgl.end() || id_to_bgl.find(target_id) == id_to_bgl.end()) {
        throw std::runtime_error("Source or target node not found in graph.");
    }

    VertexDesc source = id_to_bgl.at(source_id);
    VertexDesc target = id_to_bgl.at(target_id);

    const size_t n = boost::num_vertices(g);
    std::vector<double> dist(n, std::numeric_limits<double>::max());
    std::vector<VertexDesc> pred(n);

    for (size_t i = 0; i < n; ++i) {
        pred[i] = static_cast<VertexDesc>(i);
    }
    dist[source] = 0.0;

    const bool ok = boost::bellman_ford_shortest_paths(
        g,
        static_cast<int>(n),
        boost::weight_map(boost::get(boost::edge_weight, g))
            .distance_map(boost::make_iterator_property_map(dist.begin(), boost::get(boost::vertex_index, g)))
            .predecessor_map(boost::make_iterator_property_map(pred.begin(), boost::get(boost::vertex_index, g)))
            .root_vertex(source)
    );

    if (!ok) {
        throw std::runtime_error("Bellman-Ford failed: negative cycle detected.");
    }

    if (dist[target] == std::numeric_limits<double>::max()) {
        throw std::runtime_error("Node not reachable");
    }

    std::vector<NodeID> path;
    for (VertexDesc curr = target; curr != source; curr = pred[curr]) {
        path.push_back(bgl_to_id[curr]);
    }
    path.push_back(bgl_to_id[source]);
    std::reverse(path.begin(), path.end());
    return path;
}

template <typename GraphWrapper>
auto single_source_bellman_ford(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id) {
    using NodeID = typename GraphWrapper::NodeType;
    using VertexDesc = typename GraphWrapper::VertexDesc;

    const auto& g = G.get_impl();
    const auto& id_to_bgl = G.get_id_to_bgl_map();
    const auto& bgl_to_id = G.get_bgl_to_id_map();

    if (id_to_bgl.find(source_id) == id_to_bgl.end()) {
        throw std::runtime_error("Source node not found in graph.");
    }

    const VertexDesc source = id_to_bgl.at(source_id);
    const size_t n = boost::num_vertices(g);
    std::vector<double> dist(n, std::numeric_limits<double>::max());
    std::vector<VertexDesc> pred(n);
    for (size_t i = 0; i < n; ++i) {
        pred[i] = static_cast<VertexDesc>(i);
    }
    dist[source] = 0.0;

    const bool ok = boost::bellman_ford_shortest_paths(
        g,
        static_cast<int>(n),
        boost::weight_map(boost::get(boost::edge_weight, g))
            .distance_map(boost::make_iterator_property_map(dist.begin(), boost::get(boost::vertex_index, g)))
            .predecessor_map(boost::make_iterator_property_map(pred.begin(), boost::get(boost::vertex_index, g)))
            .root_vertex(source)
    );

    if (!ok) {
        throw std::runtime_error("Bellman-Ford failed: negative cycle detected.");
    }

    SingleSourceShortestPathResult<NodeID> result;
    for (size_t i = 0; i < n; ++i) {
        result.distance[bgl_to_id[i]] = dist[i];
        result.predecessor[bgl_to_id[i]] = bgl_to_id[pred[i]];
    }
    result.paths = build_single_source_paths<NodeID>(bgl_to_id, dist, pred);
    return result;
}

template <typename GraphWrapper>
auto bellman_ford_path(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& weight) {
    if (weight.empty() || weight == "weight") {
        return bellman_ford_path(G, source_id, target_id);
    }
    throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
}

template <typename GraphWrapper>
double bellman_ford_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id) {
    const auto path = bellman_ford_path(G, source_id, target_id);
    if (path.empty()) {
        throw std::runtime_error("Node not reachable");
    }

    double total = 0.0;
    for (size_t i = 0; i + 1 < path.size(); ++i) {
        total += static_cast<double>(G.get_edge_weight(path[i], path[i + 1]));
    }
    return total;
}

template <typename GraphWrapper>
double bellman_ford_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& weight) {
    if (weight.empty() || weight == "weight") {
        return bellman_ford_path_length(G, source_id, target_id);
    }
    throw std::runtime_error("Unsupported weight attribute: nxpp currently supports only the built-in edge weight property named 'weight'.");
}

template <typename GraphWrapper>
auto dag_shortest_paths(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id) {
    using NodeID = typename GraphWrapper::NodeType;
    using VertexDesc = typename GraphWrapper::VertexDesc;

    const auto& g = G.get_impl();
    const auto& id_to_bgl = G.get_id_to_bgl_map();
    const auto& bgl_to_id = G.get_bgl_to_id_map();

    if (id_to_bgl.find(source_id) == id_to_bgl.end()) {
        throw std::runtime_error("Source node not found in graph.");
    }

    VertexDesc source = id_to_bgl.at(source_id);
    const size_t n = boost::num_vertices(g);
    std::vector<double> dist(n, std::numeric_limits<double>::max());

    boost::dag_shortest_paths(
        g,
        source,
        boost::distance_map(boost::make_iterator_property_map(dist.begin(), boost::get(boost::vertex_index, g)))
    );

    std::unordered_map<NodeID, double> result;
    for (size_t i = 0; i < n; ++i) {
        result[bgl_to_id[i]] = dist[i];
    }
    return result;
}

template <typename GraphWrapper>
auto single_source_dag_shortest_paths(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id) {
    using NodeID = typename GraphWrapper::NodeType;
    using VertexDesc = typename GraphWrapper::VertexDesc;

    const auto& g = G.get_impl();
    const auto& id_to_bgl = G.get_id_to_bgl_map();
    const auto& bgl_to_id = G.get_bgl_to_id_map();

    if (id_to_bgl.find(source_id) == id_to_bgl.end()) {
        throw std::runtime_error("Source node not found in graph.");
    }

    const VertexDesc source = id_to_bgl.at(source_id);
    const size_t n = boost::num_vertices(g);
    std::vector<double> dist(n, std::numeric_limits<double>::max());
    std::vector<VertexDesc> pred(n);
    for (size_t i = 0; i < n; ++i) {
        pred[i] = static_cast<VertexDesc>(i);
    }

    boost::dag_shortest_paths(
        g,
        source,
        boost::distance_map(boost::make_iterator_property_map(dist.begin(), boost::get(boost::vertex_index, g)))
            .predecessor_map(boost::make_iterator_property_map(pred.begin(), boost::get(boost::vertex_index, g)))
    );

    SingleSourceShortestPathResult<NodeID> result;
    for (size_t i = 0; i < n; ++i) {
        result.distance[bgl_to_id[i]] = dist[i];
        result.predecessor[bgl_to_id[i]] = bgl_to_id[pred[i]];
    }
    result.paths = build_single_source_paths<NodeID>(bgl_to_id, dist, pred);
    return result;
}

template <typename GraphWrapper>
auto floyd_warshall_all_pairs_shortest_paths(const GraphWrapper& G) {
    using NodeID = typename GraphWrapper::NodeType;

    const auto& g = G.get_impl();
    const auto& bgl_to_id = G.get_bgl_to_id_map();
    const size_t n = boost::num_vertices(g);

    std::vector<std::vector<double>> internal_matrix(n, std::vector<double>(n));
    boost::floyd_warshall_all_pairs_shortest_paths(g, internal_matrix);

    std::vector<std::size_t> order(n);
    for (std::size_t i = 0; i < n; ++i) {
        order[i] = i;
    }

    if constexpr (std::is_arithmetic_v<NodeID> || std::is_same_v<NodeID, std::string>) {
        std::sort(order.begin(), order.end(), [&](std::size_t lhs, std::size_t rhs) {
            return bgl_to_id[lhs] < bgl_to_id[rhs];
        });
    }

    std::vector<std::vector<double>> matrix(n, std::vector<double>(n));
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            const double value = internal_matrix[order[i]][order[j]];
            matrix[i][j] = value == std::numeric_limits<double>::max()
                ? static_cast<double>(std::numeric_limits<int>::max())
                : value;
        }
    }

    return matrix;
}

template <typename GraphWrapper>
auto floyd_warshall_all_pairs_shortest_paths_map(const GraphWrapper& G) {
    using NodeID = typename GraphWrapper::NodeType;

    const auto& bgl_to_id = G.get_bgl_to_id_map();
    const auto& g = G.get_impl();
    const size_t n = boost::num_vertices(g);

    std::vector<std::size_t> order(n);
    for (std::size_t i = 0; i < n; ++i) {
        order[i] = i;
    }

    if constexpr (std::is_arithmetic_v<NodeID> || std::is_same_v<NodeID, std::string>) {
        std::sort(order.begin(), order.end(), [&](std::size_t lhs, std::size_t rhs) {
            return bgl_to_id[lhs] < bgl_to_id[rhs];
        });
    }

    const auto matrix = floyd_warshall_all_pairs_shortest_paths(G);

    std::unordered_map<NodeID, std::unordered_map<NodeID, double>> result;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            result[bgl_to_id[order[i]]][bgl_to_id[order[j]]] = matrix[i][j];
        }
    }
    return result;
}


// Algorithms: Components

template <typename GraphWrapper>
auto connected_components(const GraphWrapper& G) {
    using NodeID = typename GraphWrapper::NodeType;
    auto& g = G.get_impl();
    auto& bgl_to_id = G.get_bgl_to_id_map();

    int n = boost::num_vertices(g);
    std::vector<int> comp(n);
    int num = boost::connected_components(g, boost::make_iterator_property_map(comp.begin(), boost::get(boost::vertex_index, g)));
    
    std::vector<std::vector<NodeID>> components(num);
    for (int i = 0; i < n; ++i) {
        components[comp[i]].push_back(bgl_to_id[i]);
    }
    return components;
}

template <typename GraphWrapper>
auto connected_component_map(const GraphWrapper& G) {
    using NodeID = typename GraphWrapper::NodeType;

    const auto& g = G.get_impl();
    const auto& bgl_to_id = G.get_bgl_to_id_map();

    const int n = static_cast<int>(boost::num_vertices(g));
    std::vector<int> comp(n);
    boost::connected_components(g, boost::make_iterator_property_map(comp.begin(), boost::get(boost::vertex_index, g)));

    lookup_map<NodeID, int> result;
    for (int i = 0; i < n; ++i) {
        result[bgl_to_id[i]] = comp[i];
    }
    return result;
}

template <typename GraphWrapper>
auto strongly_connected_components(const GraphWrapper& G) {
    using NodeID = typename GraphWrapper::NodeType;
    auto& g = G.get_impl();
    auto& bgl_to_id = G.get_bgl_to_id_map();

    int n = boost::num_vertices(g);
    std::vector<int> comp(n);
    int num = boost::strong_components(g, boost::make_iterator_property_map(comp.begin(), boost::get(boost::vertex_index, g)));
    
    std::vector<std::vector<NodeID>> components(num);
    for (int i = 0; i < n; ++i) {
        components[comp[i]].push_back(bgl_to_id[i]);
    }
    return components;
}

template <typename GraphWrapper>
auto strongly_connected_component_map(const GraphWrapper& G) {
    using NodeID = typename GraphWrapper::NodeType;

    const auto& g = G.get_impl();
    const auto& bgl_to_id = G.get_bgl_to_id_map();

    const int n = static_cast<int>(boost::num_vertices(g));
    std::vector<int> comp(n);
    boost::strong_components(g, boost::make_iterator_property_map(comp.begin(), boost::get(boost::vertex_index, g)));

    lookup_map<NodeID, int> result;
    for (int i = 0; i < n; ++i) {
        result[bgl_to_id[i]] = comp[i];
    }
    return result;
}

template <typename GraphWrapper>
auto strongly_connected_component_roots(const GraphWrapper& G) {
    using NodeID = typename GraphWrapper::NodeType;

    const auto& g = G.get_impl();
    const auto& bgl_to_id = G.get_bgl_to_id_map();
    const int n = static_cast<int>(boost::num_vertices(g));

    std::vector<int> comp(n);
    std::vector<typename GraphWrapper::VertexDesc> roots(n);
    boost::strong_components(
        g,
        boost::make_iterator_property_map(comp.begin(), boost::get(boost::vertex_index, g)),
        boost::root_map(boost::make_iterator_property_map(roots.begin(), boost::get(boost::vertex_index, g)))
    );

    std::unordered_map<NodeID, NodeID> result;
    for (int i = 0; i < n; ++i) {
        result[bgl_to_id[i]] = bgl_to_id[roots[i]];
    }
    return result;
}

template <typename GraphWrapper>
auto topological_sort(const GraphWrapper& G) {
    using NodeID = typename GraphWrapper::NodeType;

    std::vector<typename GraphWrapper::VertexDesc> rev_order;
    boost::topological_sort(G.get_impl(), std::back_inserter(rev_order));

    std::vector<NodeID> order;
    order.reserve(rev_order.size());
    for (auto it = rev_order.rbegin(); it != rev_order.rend(); ++it) {
        order.push_back(G.get_bgl_to_id_map()[*it]);
    }
    return order;
}

template <typename GraphWrapper>
auto kruskal_minimum_spanning_tree(const GraphWrapper& G) {
    using NodeID = typename GraphWrapper::NodeType;
    using EdgeDesc = typename GraphWrapper::EdgeDesc;

    std::vector<EdgeDesc> mst_edges;
    boost::kruskal_minimum_spanning_tree(G.get_impl(), std::back_inserter(mst_edges));

    std::vector<std::pair<NodeID, NodeID>> result;
    result.reserve(mst_edges.size());
    const auto& impl = G.get_impl();
    const auto& bgl_to_id = G.get_bgl_to_id_map();
    for (const auto& e : mst_edges) {
        result.emplace_back(bgl_to_id[boost::source(e, impl)], bgl_to_id[boost::target(e, impl)]);
    }
    return result;
}

template <typename GraphWrapper>
auto prim_minimum_spanning_tree(const GraphWrapper& G, const typename GraphWrapper::NodeType& root_id) {
    using NodeID = typename GraphWrapper::NodeType;
    using VertexDesc = typename GraphWrapper::VertexDesc;

    const auto& impl = G.get_impl();
    const auto& id_to_bgl = G.get_id_to_bgl_map();
    const auto& bgl_to_id = G.get_bgl_to_id_map();

    if (id_to_bgl.find(root_id) == id_to_bgl.end()) {
        throw std::runtime_error("Root node not found in graph.");
    }

    std::vector<VertexDesc> parent(boost::num_vertices(impl));
    boost::prim_minimum_spanning_tree(
        impl,
        boost::make_iterator_property_map(parent.begin(), boost::get(boost::vertex_index, impl)),
        boost::root_vertex(id_to_bgl.at(root_id))
    );

    std::unordered_map<NodeID, NodeID> result;
    for (size_t i = 0; i < parent.size(); ++i) {
        result[bgl_to_id[i]] = bgl_to_id[parent[i]];
    }
    return result;
}

template <typename GraphWrapper>
auto edmonds_karp_maximum_flow(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity") {
    using NodeID = typename GraphWrapper::NodeType;

    if (!G.has_node(source_id) || !G.has_node(target_id)) {
        throw std::runtime_error("Source or target node not found in graph.");
    }

    using FlowTraits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
    using FlowGraph = boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::directedS,
        boost::no_property,
        boost::property<
            boost::edge_capacity_t,
            long,
            boost::property<
                boost::edge_residual_capacity_t,
                long,
                boost::property<boost::edge_reverse_t, typename FlowTraits::edge_descriptor>
            >
        >
    >;

    using CapacityMap = typename boost::property_map<FlowGraph, boost::edge_capacity_t>::type;
    using ResidualMap = typename boost::property_map<FlowGraph, boost::edge_residual_capacity_t>::type;
    using ReverseMap = typename boost::property_map<FlowGraph, boost::edge_reverse_t>::type;
    using FlowEdgeDesc = typename boost::graph_traits<FlowGraph>::edge_descriptor;

    FlowGraph flow_graph;
    CapacityMap capacity = boost::get(boost::edge_capacity, flow_graph);
    ReverseMap reverse = boost::get(boost::edge_reverse, flow_graph);

    const auto nodes = G.nodes();
    std::unordered_map<NodeID, std::size_t> node_to_index;
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        boost::add_vertex(flow_graph);
        node_to_index[nodes[i]] = i;
    }

    std::map<std::pair<NodeID, NodeID>, FlowEdgeDesc> original_edges;
    for (const auto& [u, v, w] : G.edges()) {
        (void)w;
        auto [e, added] = boost::add_edge(node_to_index.at(u), node_to_index.at(v), flow_graph);
        auto [rev, rev_added] = boost::add_edge(node_to_index.at(v), node_to_index.at(u), flow_graph);
        (void)added;
        (void)rev_added;

        capacity[e] = static_cast<long>(G.get_edge_numeric_attr(u, v, capacity_attr));
        capacity[rev] = 0;
        reverse[e] = rev;
        reverse[rev] = e;
        original_edges[{u, v}] = e;
    }

    const long flow_value = boost::edmonds_karp_max_flow(
        flow_graph,
        node_to_index.at(source_id),
        node_to_index.at(target_id)
    );

    ResidualMap residual = boost::get(boost::edge_residual_capacity, flow_graph);

    MaximumFlowResult<NodeID> result;
    result.value = flow_value;
    for (const auto& [key, edge_desc] : original_edges) {
        result.flow[key] = capacity[edge_desc] - residual[edge_desc];
    }
    return result;
}

template <typename GraphWrapper>
auto push_relabel_maximum_flow(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity") {
    using NodeID = typename GraphWrapper::NodeType;

    if (!G.has_node(source_id) || !G.has_node(target_id)) {
        throw std::runtime_error("Source or target node not found in graph.");
    }

    using FlowTraits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
    using FlowGraph = boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::directedS,
        boost::no_property,
        boost::property<
            boost::edge_capacity_t,
            long,
            boost::property<
                boost::edge_residual_capacity_t,
                long,
                boost::property<boost::edge_reverse_t, typename FlowTraits::edge_descriptor>
            >
        >
    >;

    using CapacityMap = typename boost::property_map<FlowGraph, boost::edge_capacity_t>::type;
    using ResidualMap = typename boost::property_map<FlowGraph, boost::edge_residual_capacity_t>::type;
    using ReverseMap = typename boost::property_map<FlowGraph, boost::edge_reverse_t>::type;
    using FlowEdgeDesc = typename boost::graph_traits<FlowGraph>::edge_descriptor;

    FlowGraph flow_graph;
    CapacityMap capacity = boost::get(boost::edge_capacity, flow_graph);
    ReverseMap reverse = boost::get(boost::edge_reverse, flow_graph);

    const auto nodes = G.nodes();
    std::unordered_map<NodeID, std::size_t> node_to_index;
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        boost::add_vertex(flow_graph);
        node_to_index[nodes[i]] = i;
    }

    std::map<std::pair<NodeID, NodeID>, FlowEdgeDesc> original_edges;
    for (const auto& [u, v, w] : G.edges()) {
        (void)w;
        auto [e, added] = boost::add_edge(node_to_index.at(u), node_to_index.at(v), flow_graph);
        auto [rev, rev_added] = boost::add_edge(node_to_index.at(v), node_to_index.at(u), flow_graph);
        (void)added;
        (void)rev_added;

        capacity[e] = static_cast<long>(G.get_edge_numeric_attr(u, v, capacity_attr));
        capacity[rev] = 0;
        reverse[e] = rev;
        reverse[rev] = e;
        original_edges[{u, v}] = e;
    }

    const long flow_value = boost::push_relabel_max_flow(
        flow_graph,
        node_to_index.at(source_id),
        node_to_index.at(target_id)
    );

    ResidualMap residual = boost::get(boost::edge_residual_capacity, flow_graph);

    MaximumFlowResult<NodeID> result;
    result.value = flow_value;
    for (const auto& [key, edge_desc] : original_edges) {
        result.flow[key] = capacity[edge_desc] - residual[edge_desc];
    }
    return result;
}

template <typename GraphWrapper>
auto maximum_flow(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity") {
    return edmonds_karp_maximum_flow(G, source_id, target_id, capacity_attr);
}

template <typename GraphWrapper>
auto minimum_cut(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity") {
    using NodeID = typename GraphWrapper::NodeType;

    if (!G.has_node(source_id) || !G.has_node(target_id)) {
        throw std::runtime_error("Source or target node not found in graph.");
    }

    using FlowTraits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
    using FlowGraph = boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::directedS,
        boost::no_property,
        boost::property<
            boost::edge_capacity_t,
            long,
            boost::property<
                boost::edge_residual_capacity_t,
                long,
                boost::property<boost::edge_reverse_t, typename FlowTraits::edge_descriptor>
            >
        >
    >;

    using CapacityMap = typename boost::property_map<FlowGraph, boost::edge_capacity_t>::type;
    using ResidualMap = typename boost::property_map<FlowGraph, boost::edge_residual_capacity_t>::type;
    using ReverseMap = typename boost::property_map<FlowGraph, boost::edge_reverse_t>::type;

    FlowGraph flow_graph;
    CapacityMap capacity = boost::get(boost::edge_capacity, flow_graph);
    ReverseMap reverse = boost::get(boost::edge_reverse, flow_graph);

    const auto nodes = G.nodes();
    std::unordered_map<NodeID, std::size_t> node_to_index;
    std::vector<NodeID> index_to_node;
    index_to_node.reserve(nodes.size());
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        boost::add_vertex(flow_graph);
        node_to_index[nodes[i]] = i;
        index_to_node.push_back(nodes[i]);
    }

    for (const auto& [u, v, w] : G.edges()) {
        (void)w;
        auto [e, added] = boost::add_edge(node_to_index.at(u), node_to_index.at(v), flow_graph);
        auto [rev, rev_added] = boost::add_edge(node_to_index.at(v), node_to_index.at(u), flow_graph);
        (void)added;
        (void)rev_added;

        capacity[e] = static_cast<long>(G.get_edge_numeric_attr(u, v, capacity_attr));
        capacity[rev] = 0;
        reverse[e] = rev;
        reverse[rev] = e;
    }

    const auto source_index = node_to_index.at(source_id);
    const auto target_index = node_to_index.at(target_id);
    const long cut_value = boost::edmonds_karp_max_flow(flow_graph, source_index, target_index);

    ResidualMap residual = boost::get(boost::edge_residual_capacity, flow_graph);
    std::vector<bool> visited(nodes.size(), false);
    std::queue<std::size_t> q;
    visited[source_index] = true;
    q.push(source_index);

    while (!q.empty()) {
        const auto current = q.front();
        q.pop();
        for (auto [eit, eend] = boost::out_edges(current, flow_graph); eit != eend; ++eit) {
            const auto next = boost::target(*eit, flow_graph);
            if (!visited[next] && residual[*eit] > 0) {
                visited[next] = true;
                q.push(next);
            }
        }
    }

    MinimumCutResult<NodeID> result;
    result.value = cut_value;
    for (std::size_t i = 0; i < index_to_node.size(); ++i) {
        if (visited[i]) result.reachable.push_back(index_to_node[i]);
        else result.non_reachable.push_back(index_to_node[i]);
    }

    for (const auto& [u, v, w] : G.edges()) {
        (void)w;
        if (visited[node_to_index.at(u)] && !visited[node_to_index.at(v)]) {
            result.cut_edges.emplace_back(u, v);
        }
    }

    return result;
}

template <typename GraphWrapper>
auto max_flow_min_cost_cycle_canceling(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") {
    using NodeID = typename GraphWrapper::NodeType;

    if (!G.has_node(source_id) || !G.has_node(target_id)) {
        throw std::runtime_error("Source or target node not found in graph.");
    }

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
    WeightMap weight = boost::get(boost::edge_weight, flow_graph);
    CapacityMap capacity = boost::get(boost::edge_capacity, flow_graph);
    ReverseMap reverse = boost::get(boost::edge_reverse, flow_graph);

    const auto nodes = G.nodes();
    std::unordered_map<NodeID, std::size_t> node_to_index;
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        boost::add_vertex(flow_graph);
        node_to_index[nodes[i]] = i;
    }

    std::map<std::pair<NodeID, NodeID>, FlowEdgeDesc> original_edges;
    for (const auto& [u, v, w] : G.edges()) {
        (void)w;
        auto [e, added] = boost::add_edge(node_to_index.at(u), node_to_index.at(v), flow_graph);
        auto [rev, rev_added] = boost::add_edge(node_to_index.at(v), node_to_index.at(u), flow_graph);
        (void)added;
        (void)rev_added;

        weight[e] = static_cast<long>(G.get_edge_numeric_attr(u, v, weight_attr));
        weight[rev] = -weight[e];
        capacity[e] = static_cast<long>(G.get_edge_numeric_attr(u, v, capacity_attr));
        capacity[rev] = 0;
        reverse[e] = rev;
        reverse[rev] = e;
        original_edges[{u, v}] = e;
    }

    const long flow_value = boost::push_relabel_max_flow(
        flow_graph,
        node_to_index.at(source_id),
        node_to_index.at(target_id)
    );
    boost::cycle_canceling(flow_graph);
    const long cost = boost::find_flow_cost(flow_graph);

    ResidualMap residual = boost::get(boost::edge_residual_capacity, flow_graph);
    MinCostMaxFlowResult<NodeID> result;
    result.value = flow_value;
    result.cost = cost;
    for (const auto& [key, edge_desc] : original_edges) {
        result.flow[key] = capacity[edge_desc] - residual[edge_desc];
    }
    return result;
}

template <typename GraphWrapper>
auto max_flow_min_cost_successive_shortest_path(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id, const typename GraphWrapper::NodeType& target_id, const std::string& capacity_attr = "capacity", const std::string& weight_attr = "weight") {
    using NodeID = typename GraphWrapper::NodeType;

    if (!G.has_node(source_id) || !G.has_node(target_id)) {
        throw std::runtime_error("Source or target node not found in graph.");
    }

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
    WeightMap weight = boost::get(boost::edge_weight, flow_graph);
    CapacityMap capacity = boost::get(boost::edge_capacity, flow_graph);
    ReverseMap reverse = boost::get(boost::edge_reverse, flow_graph);

    const auto nodes = G.nodes();
    std::unordered_map<NodeID, std::size_t> node_to_index;
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        boost::add_vertex(flow_graph);
        node_to_index[nodes[i]] = i;
    }

    std::map<std::pair<NodeID, NodeID>, FlowEdgeDesc> original_edges;
    for (const auto& [u, v, w] : G.edges()) {
        (void)w;
        auto [e, added] = boost::add_edge(node_to_index.at(u), node_to_index.at(v), flow_graph);
        auto [rev, rev_added] = boost::add_edge(node_to_index.at(v), node_to_index.at(u), flow_graph);
        (void)added;
        (void)rev_added;

        weight[e] = static_cast<long>(G.get_edge_numeric_attr(u, v, weight_attr));
        weight[rev] = -weight[e];
        capacity[e] = static_cast<long>(G.get_edge_numeric_attr(u, v, capacity_attr));
        capacity[rev] = 0;
        reverse[e] = rev;
        reverse[rev] = e;
        original_edges[{u, v}] = e;
    }

    boost::successive_shortest_path_nonnegative_weights(
        flow_graph,
        node_to_index.at(source_id),
        node_to_index.at(target_id)
    );

    ResidualMap residual = boost::get(boost::edge_residual_capacity, flow_graph);
    long flow_value = 0;
    for (auto [eit, eend] = boost::out_edges(node_to_index.at(source_id), flow_graph); eit != eend; ++eit) {
        flow_value += capacity[*eit] - residual[*eit];
    }

    const long cost = boost::find_flow_cost(flow_graph);

    MinCostMaxFlowResult<NodeID> result;
    result.value = flow_value;
    result.cost = cost;
    for (const auto& [key, edge_desc] : original_edges) {
        result.flow[key] = capacity[edge_desc] - residual[edge_desc];
    }
    return result;
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
auto degree_centrality(const GraphWrapper& G) {
    using NodeID = typename GraphWrapper::NodeType;

    const auto& g = G.get_impl();
    const auto& bgl_to_id = G.get_bgl_to_id_map();
    const auto node_count = boost::num_vertices(g);

    std::unordered_map<NodeID, double> centrality;
    if (node_count == 0) {
        return centrality;
    }

    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
        centrality[bgl_to_id[*v]] = 0.0;
    }

    if (node_count <= 1) {
        return centrality;
    }

    const double scale = 1.0 / static_cast<double>(node_count - 1);
    for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
        double degree = 0.0;
        if constexpr (GraphWrapper::is_directed) {
            degree = static_cast<double>(boost::in_degree(*v, g) + boost::out_degree(*v, g));
        } else {
            degree = static_cast<double>(boost::degree(*v, g));
        }
        centrality[bgl_to_id[*v]] = degree * scale;
    }

    return centrality;
}

} // namespace nxpp

#endif // NXPP_HPP
