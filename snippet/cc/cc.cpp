#include<cstdlib>
#include<iostream>
#include<vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/connected_components.hpp>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> Graph;
typedef boost::graph_traits<Graph> GT;

int main()
{
    Graph G(5);

    boost::add_edge(0, 2, G);
    boost::add_edge(1, 0, G);
    boost::add_edge(1, 2, G);
    boost::add_edge(3, 4, G);
 
    std::vector<GT::vertices_size_type> comp(boost::num_vertices(G));
    boost::connected_components(G, boost::make_iterator_property_map(comp.begin(), get(boost::vertex_index, G)));

    for(int i=0; i<boost::num_vertices(G); i++)
        std::cout << "Vertex " << i << " is in component " << comp[i] << "\n";
 
    return EXIT_SUCCESS;
}

