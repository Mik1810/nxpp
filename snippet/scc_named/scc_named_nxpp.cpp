#include <cstdlib>
#include <iostream>

#include "../../include/nxpp/components.hpp"

int main() {
    nxpp::UnweightedDiGraphInt G;

    G.add_edge(0, 2);
    G.add_edge(1, 0);
    G.add_edge(1, 4);
    G.add_edge(2, 1);
    G.add_edge(2, 3);
    G.add_edge(3, 2);
    G.add_edge(3, 4);

    auto comp = G.strong_components();
    for (int i = 0; i < G.num_vertices(); ++i) {
        std::cout << "The representative of the SCC of vertex " << i
                  << " is " << comp[i] << "\n";
    }

    return EXIT_SUCCESS;
}
