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

    const auto result = nxpp::single_source_bellman_ford(G, 0);
    for (int i = 0; i < 5; ++i) {
        if (i == 0) {
            std::cout << "Distance to 0: 0 Parent: 0\n";
            continue;
        }
        std::cout << "Distance to " << i << ": " << static_cast<int>(result.distance.at(i))
                  << " Parent: " << result.predecessor.at(i) << "\n";
    }

    return EXIT_SUCCESS;
}
