#include <cstdlib>
#include <iostream>

#include "../../include/nxpp.hpp"

int main() {
    nxpp::WeightedDiGraphInt G;

    G.add_edge(0, 1, 2, {"capacity", 1});
    G.add_edge(0, 3, 3, {"capacity", 2});
    G.add_edge(1, 2, 5, {"capacity", 3});
    G.add_edge(1, 4, 1, {"capacity", 4});
    G.add_edge(2, 5, 3, {"capacity", 2});
    G.add_edge(3, 1, 1, {"capacity", 1});
    G.add_edge(3, 2, 6, {"capacity", 2});
    G.add_edge(4, 5, 1, {"capacity", 2});

    const auto result = G.successive_shortest_path_nonnegative_weights(0, 5);
    std::cout << "The minimum cost of a max flow from 0 to 5 is " << result.cost << "\n";
    std::cout << "The maximum flow from 0 to 5 is " << result.flow << "\n";
    return EXIT_SUCCESS;
}
