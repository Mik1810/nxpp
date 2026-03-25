#include <cstdlib>
#include <iostream>
#include <unordered_map>

#include "../../include/nxpp.hpp"

int main() {
    nxpp::Graph<int> G;
    G.add_nodes_from({0, 1, 2, 3, 4});

    G.add_edge(0, 2);
    G.add_edge(1, 0);
    G.add_edge(1, 2);
    G.add_edge(3, 4);

    const auto components = nxpp::connected_components(G);
    std::unordered_map<int, int> comp_index;
    for (int i = 0; i < static_cast<int>(components.size()); ++i) {
        for (const int node : components[i]) {
            comp_index[node] = i;
        }
    }

    for (int i = 0; i < 5; ++i) {
        std::cout << "Vertex " << i << " is in component " << comp_index[i] << "\n";
    }

    return EXIT_SUCCESS;
}
