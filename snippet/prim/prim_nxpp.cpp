#include <cstdlib>
#include <iostream>

#include "../../include/nxpp.hpp"

int main() {
    nxpp::Graph<int> G;
    G.add_nodes_from({0, 1, 2, 3, 4});

    G.add_edge(0, 2, 3);
    G.add_edge(1, 0, 2);
    G.add_edge(1, 2, 2);
    G.add_edge(1, 4, 5);
    G.add_edge(2, 3, 1);
    G.add_edge(3, 4, 5);

    const auto parent = nxpp::prim_minimum_spanning_tree(G, 0);
    for (int i = 0; i < 5; ++i) {
        std::cout << "Parent of " << i << ": " << parent.at(i) << "\n";
    }

    return EXIT_SUCCESS;
}
