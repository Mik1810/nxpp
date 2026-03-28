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

    // Default behavior: const auto result = nxpp::minimum_spanning_tree(G);
    // It uses the Kruskal algorithm by default, but you can specify the algorithm explicitly:

    const auto result = nxpp::kruskal_minimum_spanning_tree(G);
    // ALTERNATIVELY: const auto result = nxpp::prim_minimum_spanning_tree(G, 0);

    for (const auto& [u, v] : result) {
        std::cout << "Edge from " << u << " to " << v << "\n";
    }

    return EXIT_SUCCESS;
}
