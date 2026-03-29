#include <cstdlib>
#include <iostream>

#include "../../include/nxpp/graph.hpp"

int main() {
    nxpp::UnweightedDiGraphInt G;

    G.add_edge(0, 2);
    G.add_edge(1, 0);
    G.add_edge(1, 4);
    G.add_edge(2, 1);
    G.add_edge(2, 3);
    G.add_edge(3, 2);
    G.add_edge(3, 4);

    for (const auto& [u, v] : G.edges()) {
        std::cout << "Edge from " << u << " to " << v << "\n";
    }

    for (const int v : G.successors(2)) {
        std::cout << "Out edge from 2 to " << v << "\n";
    }

    return EXIT_SUCCESS;
}
