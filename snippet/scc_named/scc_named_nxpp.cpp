#include <cstdlib>
#include <iostream>

#include "../../include/nxpp.hpp"

int main() {
    nxpp::UnweightedDiGraphInt G;
    G.add_nodes_from({0, 1, 2, 3, 4});

    G.add_edge(0, 2);
    G.add_edge(1, 0);
    G.add_edge(1, 4);
    G.add_edge(2, 1);
    G.add_edge(2, 3);
    G.add_edge(3, 2);
    G.add_edge(3, 4);

    auto comp = nxpp::strongly_connected_component_roots(G);
    for (int i = 0; i < 5; ++i) {
        std::cout << "The representative of the SCC of vertex " << i
                  << " is " << comp[i] << "\n";
    }

    return EXIT_SUCCESS;
}
