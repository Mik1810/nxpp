#include<cstdlib>
#include<iostream>
#include<vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/find_flow_cost.hpp>

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> Traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
    boost::property<boost::edge_weight_t, long,
        boost::property<boost::edge_capacity_t, long,
            boost::property<boost::edge_residual_capacity_t, long,
                boost::property<boost::edge_reverse_t, Traits::edge_descriptor> >>>> Graph;

typedef boost::property_map<Graph, boost::edge_weight_t>::type weight_map;
typedef boost::property_map<Graph, boost::edge_capacity_t>::type capacity_map;
typedef boost::property_map<Graph, boost::edge_residual_capacity_t>::type residual_map;
typedef boost::property_map<Graph, boost::edge_reverse_t>::type reverse_map;

//Helper class to add edges more conveniently
class EdgeAdder
{
    Graph &G;
    weight_map weight;
    capacity_map capacity;
    reverse_map reverse;
    
public:
    explicit EdgeAdder(Graph &G) : G(G)
    {
        weight = boost::get(boost::edge_weight, G);
        capacity = boost::get(boost::edge_capacity, G);
        reverse = boost::get(boost::edge_reverse, G);
    }
    
    void add_edge(long u, long v, long c, long w)
    {
        auto [e, added] = boost::add_edge(u, v, G);
        auto [rev, rev_added] = boost::add_edge(v, u, G);
        
        weight[e] = w;
        weight[rev] = -w;
        
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

    edge_adder.add_edge(0, 1, 1, 2);
    edge_adder.add_edge(0, 3, 2, 3);
    edge_adder.add_edge(1, 2, 3, 5);
    edge_adder.add_edge(1, 4, 4, 1);
    edge_adder.add_edge(2, 5, 2, 3);
    edge_adder.add_edge(3, 1, 1, 1);
    edge_adder.add_edge(3, 2, 2, 6);
    edge_adder.add_edge(4, 5, 2, 1);
       
    long flow = boost::push_relabel_max_flow(G, 0, 5);
    std::cout << "The maximum flow from 0 to 5 is " << flow << "\n";

    boost::cycle_canceling(G);
    std::cout << "The minimum cost of a max flow from 0 to 5 is " << boost::find_flow_cost(G) << "\n";

    return EXIT_SUCCESS;
}


