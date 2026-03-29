#include <cstdlib>
#include <iostream>

#include "../../include/nxpp/graph.hpp"

int main() {
    nxpp::WeightedDiGraphInt G;
    G.add_nodes_from({0, 1, 2, 3, 4});
    G.add_edge(0, 1, 5);

    for (const auto& [u, v, w] : G.edges()) {
        std::cout << "Edge from " << u << " to " << v << " with weight " << static_cast<int>(w) << "\n";
    }

    return EXIT_SUCCESS;
}
