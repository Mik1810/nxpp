#include <cstdlib>
#include <iostream>

#include "../../include/nxpp/graph.hpp"

int main() {
    nxpp::WeightedDiGraphInt G;
    G.add_nodes_from({0, 1, 2, 3, 4});
    G.add_edge(0, 1, 5);

    for (const auto& [u, v, w] : G.weighted_edges()) {
        std::cout << "Edge from " << u << " to " << v << " with weight " << w << "\n";
    }

    return EXIT_SUCCESS;
}
