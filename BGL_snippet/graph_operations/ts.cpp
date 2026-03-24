#include<cstdlib>
#include<iostream>
#include<vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/topological_sort.hpp>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> Graph;
typedef boost::graph_traits<Graph> GT;

int main()
{
    Graph G(5);

    boost::add_edge(0, 2, G);
    boost::add_edge(1, 0, G);
    boost::add_edge(1, 2, G);
    boost::add_edge(1, 4, G);
    boost::add_edge(2, 3, G);
    boost::add_edge(3, 4, G);
 
    std::vector<GT::vertex_descriptor> rev_order;
    boost::topological_sort(G, std::back_inserter(rev_order));
    
    for(auto it = rev_order.rbegin(); it!=rev_order.rend(); it++)
        std::cout << *it << " ";
        
    std::cout << "\n";
 
    return EXIT_SUCCESS;
}

