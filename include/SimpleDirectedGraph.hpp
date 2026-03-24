#ifndef SIMPLE_DIRECTED_GRAPH_HPP
#define SIMPLE_DIRECTED_GRAPH_HPP

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <vector>
#include <algorithm>
#include <iostream>

// Classe per grafi diretti
class SimpleDirectedGraph {
public:
    using GraphType = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS>;
    using Node = boost::graph_traits<GraphType>::vertex_descriptor;
    using Edge = boost::graph_traits<GraphType>::edge_descriptor;

    SimpleDirectedGraph() : g() {}
    virtual ~SimpleDirectedGraph() {}

    Node add_node() {
        return boost::add_vertex(g);
    }

    void add_nodes_from(const std::vector<Node>& nodes) {
        for (size_t i = 0; i < nodes.size(); ++i) {
            add_node();
        }
    }

    Edge add_edge(Node u, Node v) {
        return boost::add_edge(u, v, g).first;
    }

    void add_edges_from(const std::vector<std::pair<Node, Node>>& edges) {
        for (const auto& edge : edges) {
            add_edge(edge.first, edge.second);
        }
    }

    std::vector<Node> nodes() const {
        std::vector<Node> result;
        for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
            result.push_back(*v);
        }
        return result;
    }

    std::vector<std::pair<Node, Node>> edges() const {
        std::vector<std::pair<Node, Node>> result;
        for (auto [e, eend] = boost::edges(g); e != eend; ++e) {
            result.emplace_back(boost::source(*e, g), boost::target(*e, g));
        }
        return result;
    }

    // Metodo per l'ordinamento topologico
    std::vector<Node> topological_sort() const {
        std::vector<Node> result;
        try {
            boost::topological_sort(g, std::back_inserter(result));
            std::reverse(result.begin(), result.end());
        } catch (const boost::not_a_dag& e) {
            std::cerr << "Errore: Il grafo contiene cicli, impossibile effettuare l'ordinamento topologico.\n";
            result.clear();
        }
        return result;
    }

    const GraphType& get_impl() const { return g; }

protected:
    GraphType g;
};

// Alias per grafo diretto non pesato
using SimpleUnweightedDirectedGraph = SimpleDirectedGraph;

#endif // SIMPLE_DIRECTED_GRAPH_HPP