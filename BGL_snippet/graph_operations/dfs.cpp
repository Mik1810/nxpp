#include<cstdlib>
#include<iostream>
#include<vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/depth_first_search.hpp>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> Graph;
typedef boost::graph_traits<Graph> GT;

class custom_visitor : public boost::default_dfs_visitor 
{
public:    
    void tree_edge(GT::edge_descriptor e, const Graph& G)
    {
        std::cout << "Discovered tree edge from " << boost::source(e, G) << " to " << boost::target(e, G) << "\n";
    } 
    
    void back_edge(GT::edge_descriptor e, const Graph& G)
    {
        std::cout << "Discovered back edge from " << boost::source(e, G) << " to " << boost::target(e, G) << "\n";
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
    boost::depth_first_search(G, boost::root_vertex(0).visitor(vis));
 
    return EXIT_SUCCESS;
}

