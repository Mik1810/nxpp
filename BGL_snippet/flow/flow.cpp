#include<cstdlib>
#include<iostream>
#include<vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/edmonds_karp_max_flow.hpp>
//ALTERNATIVELY #include <boost/graph/push_relabel_max_flow.hpp>

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> Traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
    boost::property<boost::edge_capacity_t, long,
        boost::property<boost::edge_residual_capacity_t, long,
            boost::property<boost::edge_reverse_t, Traits::edge_descriptor> >>> Graph;

typedef boost::property_map<Graph, boost::edge_capacity_t>::type capacity_map;
typedef boost::property_map<Graph, boost::edge_residual_capacity_t>::type residual_map;
typedef boost::property_map<Graph, boost::edge_reverse_t>::type reverse_map;

//Helper class to add edges more conveniently
class EdgeAdder
{
    Graph &G;
    capacity_map capacity;
    reverse_map reverse;
    
public:
    explicit EdgeAdder(Graph &G) : G(G)
    {
        capacity = boost::get(boost::edge_capacity, G);
        reverse = boost::get(boost::edge_reverse, G);
    }
    
    void add_edge(long u, long v, long c)
    {
        auto [e, added] = boost::add_edge(u, v, G);
        auto [rev, rev_added] = boost::add_edge(v, u, G);
        
        capacity[e] = c;
        capacity[rev] = 0;
        
        reverse[e] = rev;
        reverse[rev] = e;
    }
};

int main()
{
    Graph G(6);
    EdgeAdder edge_adder(G);

    edge_adder.add_edge(0, 1, 1);
    edge_adder.add_edge(0, 3, 2);
    edge_adder.add_edge(1, 2, 3);
    edge_adder.add_edge(1, 4, 4);
    edge_adder.add_edge(2, 5, 2);
    edge_adder.add_edge(3, 2, 2);
    edge_adder.add_edge(4, 5, 2);
       
    long flow = boost::edmonds_karp_max_flow(G, 0, 5);
    //ALTERNATIVELY long flow = boost::push_relabel_max_flow(G, 0, 5);
    std::cout << "The maximum flow from 0 to 5 is " << flow << "\n";

    capacity_map capacity = boost::get(boost::edge_capacity, G);
    residual_map residual_capacity = boost::get(boost::edge_residual_capacity, G);
    
    auto [e, found] = boost::edge(1, 4, G);
    std::cout << "The flow across edge (1, 4) is: " << capacity[e] - residual_capacity[e] << "\n";
    
    return EXIT_SUCCESS;
}

