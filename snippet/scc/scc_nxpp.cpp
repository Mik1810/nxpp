#include <cstdlib>
#include <iostream>
#include <unordered_map>

#include "../../include/nxpp.hpp"

int main() {
    nxpp::UnweightedDiGraphInt G;

    G.add_edge(0, 2);
    G.add_edge(1, 0);
    G.add_edge(1, 4);
    G.add_edge(2, 1);
    G.add_edge(2, 3);
    G.add_edge(3, 2);
    G.add_edge(3, 4);

    const auto comp_index = G.strong_component_map();

    for (int i = 0; i < G.num_vertices(); ++i) {
        std::cout << "Vertex " << i << " is in component " << comp_index[i] << "\n";
    }

    return EXIT_SUCCESS;
}
