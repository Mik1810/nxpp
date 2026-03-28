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

    // Default behavior: const auto result = nxpp::minimum_spanning_tree(G);
    // It uses the Kruskal algorithm by default, but you can specify the algorithm explicitly:

    auto parent = nxpp::prim_minimum_spanning_tree(G, 0);

    for (int i = 0; i < nxpp::num_vertices(G); ++i) {
        std::cout << "Parent of " << i << ": " << parent[i] << "\n";
    }

    return EXIT_SUCCESS;
}
