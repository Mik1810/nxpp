#include<cstdlib>
#include<iostream>
#include<vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>

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

    std::vector<int> dist(boost::num_vertices(G));
    std::vector<GT::vertex_descriptor> pred(boost::num_vertices(G));


    boost::bellman_ford_shortest_paths(G, boost::root_vertex(0)
                                          .distance_map(make_iterator_property_map(dist.begin(), get(boost::vertex_index, G)))
                                          .predecessor_map(make_iterator_property_map(pred.begin(), get(boost::vertex_index, G))));

    for(unsigned int i=0; i<boost::num_vertices(G); i++)
        std::cout << "Distance to " << i << ": " << dist[i] << " Parent: " <<  pred[i] << "\n";
 
    return EXIT_SUCCESS;
}

