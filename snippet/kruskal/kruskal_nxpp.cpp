#include <cstdlib>
#include <iostream>

#include "../../include/nxpp.hpp"

int main() {
    nxpp::WeightedGraphInt G;
    G.add_nodes_from({0, 1, 2, 3, 4});

    G.add_edge(0, 2, 3);
    G.add_edge(1, 0, 2);
    G.add_edge(1, 2, 2);
    G.add_edge(1, 4, 5);
    G.add_edge(2, 3, 1);
    G.add_edge(3, 4, 5);

    // Default behavior: const auto mst = nxpp::minimum_spanning_tree(G);
    // It uses Kruskal by default, but you can still call the explicit algorithm wrapper if you prefer.

    for (const auto& [u, v] : nxpp::minimum_spanning_tree(G)) {
        std::cout << "Edge from " << u << " to " << v << "\n";
    }

    return EXIT_SUCCESS;
}
