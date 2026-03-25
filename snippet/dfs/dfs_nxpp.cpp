#include <cstdlib>
#include <iostream>

#include "../../include/nxpp.hpp"

int main() {
    nxpp::Graph<int, double, true> G;
    G.add_nodes_from({0, 1, 2, 3, 4});

    G.add_edge(0, 2);
    G.add_edge(1, 0);
    G.add_edge(1, 4);
    G.add_edge(2, 1);
    G.add_edge(2, 3);
    G.add_edge(3, 2);
    G.add_edge(4, 3);

    nxpp::dfs_visit(
        G,
        0,
        [](int u, int v) { std::cout << "Discovered tree edge from " << u << " to " << v << "\n"; },
        [](int u, int v) { std::cout << "Discovered back edge from " << u << " to " << v << "\n"; }
    );
    return EXIT_SUCCESS;
}
