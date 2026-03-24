#ifndef NXPP_HPP
#define NXPP_HPP

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/strong_components.hpp>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <string>
#include <algorithm>
#include <utility>
#include <iostream>

namespace nxpp {

// Python Emulation Utilities

// Emula esattamente il print() di Python: print(a, b, c) con spazi e a capo automatici.
inline void print() {
    std::cout << "\n";
}

template <typename First, typename... Rest>
inline void print(First&& first, Rest&&... rest) {
    std::cout << std::forward<First>(first);
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
    using GraphType = boost::adjacency_list<boost::vecS, boost::vecS, DirectedSelector,
                                            boost::no_property, boost::property<boost::edge_weight_t, EdgeWeight>>;
    using VertexDesc = typename boost::graph_traits<GraphType>::vertex_descriptor;
    using EdgeDesc = typename boost::graph_traits<GraphType>::edge_descriptor;
    using WeightMap = typename boost::property_map<GraphType, boost::edge_weight_t>::type;

private:
    GraphType g;
    WeightMap weight_map;
    std::unordered_map<NodeID, VertexDesc> id_to_bgl;
    std::vector<NodeID> bgl_to_id;

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

public:
    Graph() : g(), weight_map(boost::get(boost::edge_weight, g)) {}

    void add_node(const NodeID& id) {
        get_or_create_vertex(id);
    }

    void add_nodes_from(const std::vector<NodeID>& nodes) {
        for (const auto& n : nodes) {
            add_node(n);
        }
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
        weight_map = boost::get(boost::edge_weight, g);
    }

    void remove_edge(const NodeID& u, const NodeID& v) {
        auto it_u = id_to_bgl.find(u);
        auto it_v = id_to_bgl.find(v);
        if (it_u == id_to_bgl.end() || it_v == id_to_bgl.end()) {
            throw std::runtime_error("NetworkXError: The node is not in the graph.");
        }
        boost::remove_edge(it_u->second, it_v->second, g);
    }

    void remove_node(const NodeID& u) {
        auto it = id_to_bgl.find(u);
        if (it == id_to_bgl.end()) {
            throw std::runtime_error("NetworkXError: The node is not in the graph.");
        }
        VertexDesc v = it->second;
        
        // Boost graph pulisce tutti gli archi entranti/uscenti e rimuove il nodo
        boost::clear_vertex(v, g);
        boost::remove_vertex(v, g);
        
        id_to_bgl.erase(it);

        // Se non era l'ultimo nodo, BGL (con vecS) fa uno swap interno per colmare il buco
        // Dobbiamo aggiornare le mappe posizionali con il nodo spostato!
        size_t last_v_index = bgl_to_id.size() - 1;
        if (v != last_v_index) {
            NodeID moved_node_id = bgl_to_id[last_v_index];
            bgl_to_id[v] = moved_node_id;
            id_to_bgl[moved_node_id] = v;
        }
        bgl_to_id.pop_back();
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

    bool has_node(const NodeID& u) const {
        return id_to_bgl.find(u) != id_to_bgl.end();
    }

    bool has_edge(const NodeID& u, const NodeID& v) const {
        auto it_u = id_to_bgl.find(u);
        auto it_v = id_to_bgl.find(v);
        if (it_u == id_to_bgl.end() || it_v == id_to_bgl.end()) return false;
        auto [e, exists] = boost::edge(it_u->second, it_v->second, g);
        return exists;
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

    // --- Proxy Pattern per simulare G[u][v] = weight ---
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

    NodeProxy operator[](const NodeID& u) {
        if (!has_node(u)) {
            add_node(u);
        }
        return NodeProxy(this, u);
    }
};

// NetworkX-style Aliases

// Aliases per grafi Base
template <typename NodeID = std::string, typename EdgeWeight = double>
using DiGraph = Graph<NodeID, EdgeWeight, true, false>;

// Aliases per MultiGraph
template <typename NodeID = std::string, typename EdgeWeight = double>
using MultiGraph = Graph<NodeID, EdgeWeight, false, true>;

template <typename NodeID = std::string, typename EdgeWeight = double>
using MultiDiGraph = Graph<NodeID, EdgeWeight, true, true>;

// Ereditarietà per la retrocompatibilità
using DiGraphStr = DiGraph<std::string, double>;
using GraphStr = Graph<std::string, double, false, false>;


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


// Algorithms: Shortest Paths

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
auto dijkstra_path_length(const GraphWrapper& G, const typename GraphWrapper::NodeType& source_id) {
    using NodeID = typename GraphWrapper::NodeType;
    using VertexDesc = typename GraphWrapper::VertexDesc;
    auto& g = G.get_impl();
    auto& id_to_bgl = G.get_id_to_bgl_map();
    auto& bgl_to_id = G.get_bgl_to_id_map();

    if (id_to_bgl.find(source_id) == id_to_bgl.end()) {
        throw std::runtime_error("Source node not found in graph.");
    }
    VertexDesc source = id_to_bgl.at(source_id);

    size_t n = boost::num_vertices(g);
    std::vector<double> dist(n);

    boost::dijkstra_shortest_paths(g, source, 
        boost::distance_map(boost::make_iterator_property_map(dist.begin(), boost::get(boost::vertex_index, g))));

    std::unordered_map<NodeID, double> result;
    for(size_t i = 0; i < n; ++i) {
        result[bgl_to_id[i]] = dist[i];
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

} // namespace nxpp

#endif // NXPP_HPP