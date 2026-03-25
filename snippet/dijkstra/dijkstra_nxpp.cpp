#include <cstdlib>
#include <iostream>

#include "../../include/nxpp.hpp"

int main() {
    nxpp::Graph<int, double, true> G;

    G.add_edge(0, 2, 3);
    G.add_edge(1, 0, 2);
    G.add_edge(1, 4, 5);
    G.add_edge(2, 1, 2);
    G.add_edge(2, 3, 1);
    G.add_edge(3, 2, 4);
    G.add_edge(3, 4, 5);

    const auto distances = nxpp::dijkstra_path_length(G, 0);
    for (int i = 0; i < 5; ++i) {
        if (i == 0) {
            std::cout << "Distance to 0: 0 Parent: 0\n";
            continue;
        }
        const auto path = nxpp::dijkstra_path(G, 0, i);
        std::cout << "Distance to " << i << ": " << static_cast<int>(distances.at(i))
                  << " Parent: " << path[path.size() - 2] << "\n";
    }

    return EXIT_SUCCESS;
}
