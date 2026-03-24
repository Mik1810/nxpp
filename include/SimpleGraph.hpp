#ifndef SIMPLE_GRAPH_HPP
#define SIMPLE_GRAPH_HPP

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <vector>
#include <iostream>

// Classe base per grafi non diretti
class SimpleGraph {
public:
    using GraphType = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
    using Node = boost::graph_traits<GraphType>::vertex_descriptor;
    using Edge = boost::graph_traits<GraphType>::edge_descriptor;

    SimpleGraph() : g() {}
    virtual ~SimpleGraph() {}

    virtual Node add_node() {
        return boost::add_vertex(g);
    }

    virtual void add_nodes_from(const std::vector<Node>& nodes) {
        for (size_t i = 0; i < nodes.size(); ++i) {
            add_node();
        }
    }

    virtual Edge add_edge(Node u, Node v) {
        return boost::add_edge(u, v, g).first;
    }

    virtual void add_edges_from(const std::vector<std::pair<Node, Node>>& edges) {
        for (const auto& edge : edges) {
            add_edge(edge.first, edge.second);
        }
    }

    virtual std::vector<Node> nodes() const {
        std::vector<Node> result;
        for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
            result.push_back(*v);
        }
        return result;
    }

    virtual std::vector<std::pair<Node, Node>> edges() const {
        std::vector<std::pair<Node, Node>> result;
        for (auto [e, eend] = boost::edges(g); e != eend; ++e) {
            result.emplace_back(boost::source(*e, g), boost::target(*e, g));
        }
        return result;
    }

    const GraphType& get_impl() const { return g; }

protected:
    GraphType g;
};

// Alias per grafo non diretto non pesato
using SimpleUnweightedUndirectedGraph = SimpleGraph;

#endif // SIMPLE_GRAPH_HPP