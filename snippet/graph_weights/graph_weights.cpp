#include<cstdlib>
#include<iostream>
#include<utility>

#include<boost/graph/adjacency_list.hpp>
#include<boost/graph/graph_traits.hpp>


typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property, boost::property<boost::edge_weight_t, int>> Graph;
typedef boost::graph_traits<Graph> GT;
typedef boost::property_map<Graph, boost::edge_weight_t>::type weight_map;

int main()
{
    Graph G(5);

    weight_map weights = boost::get(boost::edge_weight, G);

    std::pair<GT::edge_descriptor, bool> e = boost::add_edge(0, 1 ,G);
    weights[e.first] = 5; 
    
    for(auto [eit, eend] = boost::edges(G); eit != eend; eit++)
        std::cout << "Edge from " << boost::source(*eit, G)  << " to " << boost::target(*eit, G) << " with weight " << weights[*eit] << "\n";
}

