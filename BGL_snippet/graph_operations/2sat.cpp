#include<cstdlib>
#include<iostream>
#include<vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/strong_components.hpp>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> Graph;

//Maps each literal to an id between 0 and 2n-1
int to_vertex_id(int i){ return (i>0)?(2*i-2):(-2*i-1); }

int solve()
{
    int n,m;
    std::cin >> n >> m;
 
    Graph G(2*n);   
    for(int i=0; i<m; i++)
    {
        int x, y;
        std::cin >> x >> y;

        boost::add_edge(to_vertex_id(-x), to_vertex_id(y), G);
        boost::add_edge(to_vertex_id(-y), to_vertex_id(x), G);
    }

    std::vector<int> comp(boost::num_vertices(G));
    boost::strong_components(G, boost::make_iterator_property_map(comp.begin(), get(boost::vertex_index, G)));

    for(int i=1; i<=n; i++)
        if(comp[to_vertex_id(i)] == comp[to_vertex_id(-i)])
            return false;
 
    return true;
}

int main()
{
    std::cout << (solve()?"Satisfiable\n":"Not satisfiable\n");
    
    return EXIT_SUCCESS;
}

