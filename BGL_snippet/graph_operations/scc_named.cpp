#include<cstdlib>
#include<iostream>
#include<vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/strong_components.hpp>

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

    std::vector<GT::vertices_size_type> comp(boost::num_vertices(G));
    std::vector<GT::vertex_descriptor> roots(boost::num_vertices(G));

    boost::strong_components(G, make_iterator_property_map(comp.begin(), get(boost::vertex_index, G)),
                            root_map(make_iterator_property_map(roots.begin(), get(boost::vertex_index, G))));

    for(int i=0; i<boost::num_vertices(G); i++)
        std::cout << "The representative of the SCC of vertex " << i
                   << " is " << roots[i] << "\n";
 
    return EXIT_SUCCESS;
}

