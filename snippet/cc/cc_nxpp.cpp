#include <cstdlib>
#include <iostream>
#include <unordered_map>

#include "../../include/nxpp.hpp"

int main() {
    nxpp::UnweightedGraphInt G;

    G.add_edge(0, 2);
    G.add_edge(1, 0);
    G.add_edge(1, 2);
    G.add_edge(3, 4);

    const auto comp_index = G.connected_components();

    for (std::size_t i = 0; i < static_cast<std::size_t>(G.num_vertices()); ++i) {
        std::cout << "Vertex " << i << " is in component " << comp_index[i] << "\n";
    }

    return EXIT_SUCCESS;
}
