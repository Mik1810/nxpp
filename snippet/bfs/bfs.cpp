#include<cstdlib>
#include<iostream>
#include<vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/breadth_first_search.hpp>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> Graph;
typedef boost::graph_traits<Graph> GT;

class custom_visitor : public boost::default_bfs_visitor 
{
public:
    void examine_vertex(GT::vertex_descriptor u, const Graph& G)
    {
        std::cout << "Visiting vertex " << u << "\n";
    }
    
    void tree_edge(GT::edge_descriptor e, const Graph& G)
    {
        std::cout << "Discovered tree edge from " << boost::source(e, G) << " to " << boost::target(e, G) << "\n";
    } 
};


int main()
{
    Graph G(5);

    boost::add_edge(0, 2, G);
    boost::add_edge(1, 0, G);
    boost::add_edge(1, 4, G);
    boost::add_edge(2, 1, G);
    boost::add_edge(2, 3, G);
    boost::add_edge(3, 2, G);
    boost::add_edge(4, 3, G);
    
     
    custom_visitor vis;
    boost::breadth_first_search(G, 0, boost::visitor(vis));
 
    return EXIT_SUCCESS;
}

