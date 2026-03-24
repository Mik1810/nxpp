#ifndef SIMPLE_WEIGHTED_GRAPH_BASE_HPP
#define SIMPLE_WEIGHTED_GRAPH_BASE_HPP

#include <boost/graph/adjacency_list.hpp>
#include "EdgeTriple.hpp"
#include <vector>

// Template per grafi ponderati
template <typename DirectedS, typename WeightType>
class SimpleWeightedGraphBase {
public:
    using GraphType = boost::adjacency_list<boost::vecS, boost::vecS, DirectedS,
                                            boost::no_property, boost::property<boost::edge_weight_t, WeightType>>;
    using Node = typename boost::graph_traits<GraphType>::vertex_descriptor;
    using Edge = typename boost::graph_traits<GraphType>::edge_descriptor;
    using WeightMap = typename boost::property_map<GraphType, boost::edge_weight_t>::type;
    using WeightedEdge = EdgeTriple<Node, Node, WeightType>;

    SimpleWeightedGraphBase() : g(), weight_map(boost::get(boost::edge_weight, g)) {}
    virtual ~SimpleWeightedGraphBase() {}

    Node add_node() {
        return boost::add_vertex(g);
    }

    void add_nodes_from(const std::vector<Node>& nodes) {
        for (size_t i = 0; i < nodes.size(); ++i) {
            add_node();
        }
    }

    Edge add_edge(Node u, Node v, WeightType weight) {
        Edge e = boost::add_edge(u, v, g).first;
        weight_map[e] = weight;
        return e;
    }

    void add_edges_from(const std::vector<std::tuple<Node, Node, WeightType>>& edges) {
        for (const auto& edge : edges) {
            add_edge(std::get<0>(edge), std::get<1>(edge), std::get<2>(edge));
        }
    }

    std::vector<Node> nodes() const {
        std::vector<Node> result;
        for (auto [v, vend] = boost::vertices(g); v != vend; ++v) {
            result.push_back(*v);
        }
        return result;
    }

    std::vector<WeightedEdge> edges() const {
        std::vector<WeightedEdge> result;
        for (auto [e, eend] = boost::edges(g); e != eend; ++e) {
            result.emplace_back(boost::source(*e, g), boost::target(*e, g), weight_map[*e]);
        }
        return result;
    }

    const GraphType& get_impl() const { return g; }
    const WeightMap& get_weight_map() const { return weight_map; }

protected:
    GraphType g;
    WeightMap weight_map;
};

#endif // SIMPLE_WEIGHTED_GRAPH_BASE_HPP