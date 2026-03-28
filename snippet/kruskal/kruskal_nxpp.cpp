#include <cstdlib>
#include <iostream>

#include "../../include/nxpp.hpp"

int main() {
    nxpp::WeightedGraphInt G;

    G.add_edge(0, 2, 3);
    G.add_edge(1, 0, 2);
    G.add_edge(1, 2, 2);
    G.add_edge(1, 4, 5);
    G.add_edge(2, 3, 1);
    G.add_edge(3, 4, 5);

    // Default behavior: const auto result = G.minimum_spanning_tree();
    // It uses the Kruskal algorithm by default, but you can specify the algorithm explicitly:

    const auto result = G.kruskal_minimum_spanning_tree();
    // ALTERNATIVELY: const auto result = G.prim_minimum_spanning_tree(0);

    for (const auto& [u, v] : result) {
        std::cout << "Edge from " << u << " to " << v << "\n";
    }

    return EXIT_SUCCESS;
}
