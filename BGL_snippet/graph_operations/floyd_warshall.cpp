#include<cstdlib>
#include<iostream>
#include<vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/floyd_warshall_shortest.hpp>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property, boost::property<boost::edge_weight_t, int>> Graph;
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
    edge_adder.add_edge(1, 4, 5);
    edge_adder.add_edge(2, 1, 2);
    edge_adder.add_edge(2, 3, 1);
    edge_adder.add_edge(3, 2, 4);
    edge_adder.add_edge(3, 4, 5);

    std::vector<std::vector<int>> M(boost::num_vertices(G), std::vector<int>(boost::num_vertices(G)));
    boost::floyd_warshall_all_pairs_shortest_paths(G, M);

    for(auto row : M)
    {
        for(int d : row)
            std::cout << d << " ";
        std::cout << "\n";
    }
 
    return EXIT_SUCCESS;
}

