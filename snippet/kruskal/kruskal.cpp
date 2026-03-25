#include<cstdlib>
#include<iostream>
#include<vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_weight_t, int>> Graph;
typedef boost::graph_traits<Graph> GT;
typedef boost::property_map<Graph, boost::edge_weight_t>::type weight_map;


//Helper class to add edges more conveniently (keeps a reference to G and the weight map)
class EdgeAdder
{
    Graph &G;
    weight_map weights;
    
public:
    explicit EdgeAdder(Graph &G) : G(G)
    {
        weights = boost::get(boost::edge_weight, G);
    }
    
    std::pair<GT::edge_descriptor, bool> add_edge(int u, int v, int w)
    {
        auto [e, added] = boost::add_edge(u, v ,G);
        if(added)
            weights[e] = w; 
            
        return {e, added};
    }
};

int main()
{
    Graph G(5);
    EdgeAdder edge_adder(G);


    edge_adder.add_edge(0, 2, 3);
    edge_adder.add_edge(1, 0, 2);
    edge_adder.add_edge(1, 2, 2);
    edge_adder.add_edge(1, 4, 5);
    edge_adder.add_edge(2, 3, 1);
    edge_adder.add_edge(3, 4, 5);

    std::vector<GT::edge_descriptor> edges;

    boost::kruskal_minimum_spanning_tree(G, std::back_inserter(edges));

    for(GT::edge_descriptor e : edges)
            std::cout << "Edge from " << boost::source(e, G) << " to " << boost::target(e, G) << "\n"; 

    return EXIT_SUCCESS;
}

