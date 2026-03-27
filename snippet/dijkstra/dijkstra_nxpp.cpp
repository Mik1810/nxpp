#include <cstdlib>
#include <iostream>

#include "../../include/nxpp.hpp"

int main() {
    nxpp::WeightedDiGraphInt G;

    G.add_edge(0, 2, 3);
    G.add_edge(1, 0, 2);
    G.add_edge(1, 4, 5);
    G.add_edge(2, 1, 2);
    G.add_edge(2, 3, 1);
    G.add_edge(3, 2, 4);
    G.add_edge(3, 4, 5);

    auto result = nxpp::dijkstra_shortest_paths(G, 0);
    for (int i = 0; i < nxpp::num_vertices(G); ++i) {
        const int result_distance = result.distance[i];
        std::cout << "Distance to " << i << ": " << result_distance
                  << " Parent: " << result.predecessor[i] << "\n";
    }

    return EXIT_SUCCESS;
}
