#include<cstdlib>
#include<iostream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> Graph;
typedef boost::graph_traits<Graph> GT;

int main()
{
    Graph G(5);

    boost::add_edge(0, 2, G);
    boost::add_edge(1, 0, G);
    boost::add_edge(1, 4, G);
    boost::add_edge(2, 1, G);
    boost::add_edge(2, 3, G);
    boost::add_edge(3, 2, G);
    boost::add_edge(3, 4, G);
    
    //boost::edges(G) returns a std::pair<GT::edge_iterator, GT::edge_iterator>
    for(auto [eit, eend] = boost::edges(G); eit != eend; eit++)
        std::cout << "Edge from " << boost::source(*eit, G) << " to " << boost::target(*eit, G) << "\n";

    //boost::edges(G) returns a std::pair<GT::out_edge_iterator, GT::out_edge_iterator>
    for(auto [eit, eend] = boost::out_edges(2, G); eit != eend; eit++)
        std::cout << "Out edge from " << boost::source(*eit, G) << " to " << boost::target(*eit, G) << "\n";

    return EXIT_SUCCESS;
}

