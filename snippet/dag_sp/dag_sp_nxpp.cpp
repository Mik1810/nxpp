#include <cstdlib>
#include <iostream>
#include <limits>

#include "../../include/nxpp.hpp"

int main() {
    nxpp::WeightedDiGraphInt G;

    G.add_edge(0, 2, 3);
    G.add_edge(1, 0, 2);
    G.add_edge(1, 2, 2);
    G.add_edge(1, 4, 5);
    G.add_edge(2, 3, 1);
    G.add_edge(3, 4, 5);

    const auto result = nxpp::single_source_dag_shortest_paths(G, 0);
    for (int i = 0; i < nxpp::num_vertices(G); ++i) {
        const double distance = result.distance.at(i);
        const int printable_distance =
            distance == std::numeric_limits<double>::max()
                ? std::numeric_limits<int>::max()
                : static_cast<int>(distance);
        std::cout << "Distance to " << i << ": " << printable_distance
                  << " Parent: " << result.predecessor.at(i) << "\n";
    }

    return EXIT_SUCCESS;
}
