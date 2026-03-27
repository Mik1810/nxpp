#include <cstdlib>
#include <iostream>

#include "../../include/nxpp.hpp"

int main() {
    nxpp::UnweightedDiGraphInt G;
    G.add_nodes_from({0, 1, 2, 3, 4});

    G.add_edge(0, 2);
    G.add_edge(1, 0);
    G.add_edge(1, 2);
    G.add_edge(1, 4);
    G.add_edge(2, 3);
    G.add_edge(3, 4);

    for (const int node : nxpp::topological_sort(G)) {
        std::cout << node << " ";
    }
    std::cout << "\n";

    return EXIT_SUCCESS;
}
