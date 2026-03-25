#include <cstdlib>
#include <iostream>

#include "../../include/nxpp.hpp"

int main() {
    nxpp::Graph<int, double, true> G;
    G.add_nodes_from({0, 1, 2, 3, 4, 5});

    G.add_edge(0, 1, 2);
    G[0][1]["capacity"] = 1L;
    G.add_edge(0, 3, 3);
    G[0][3]["capacity"] = 2L;
    G.add_edge(1, 2, 5);
    G[1][2]["capacity"] = 3L;
    G.add_edge(1, 4, 1);
    G[1][4]["capacity"] = 4L;
    G.add_edge(2, 5, 3);
    G[2][5]["capacity"] = 2L;
    G.add_edge(3, 1, 1);
    G[3][1]["capacity"] = 1L;
    G.add_edge(3, 2, 6);
    G[3][2]["capacity"] = 2L;
    G.add_edge(4, 5, 1);
    G[4][5]["capacity"] = 2L;

    const auto result = nxpp::max_flow_min_cost_cycle_canceling(G, 0, 5);
    std::cout << "The maximum flow from 0 to 5 is " << result.value << "\n";
    std::cout << "The minimum cost of a max flow from 0 to 5 is " << result.cost << "\n";
    return EXIT_SUCCESS;
}
