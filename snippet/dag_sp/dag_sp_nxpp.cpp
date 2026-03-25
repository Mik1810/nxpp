#include <cstdlib>
#include <iostream>
#include <limits>

#include "../../include/nxpp.hpp"

int main() {
    nxpp::Graph<int, double, true> G;

    G.add_edge(0, 2, 3);
    G.add_edge(1, 0, 2);
    G.add_edge(1, 2, 2);
    G.add_edge(1, 4, 5);
    G.add_edge(2, 3, 1);
    G.add_edge(3, 4, 5);

    const auto dist = nxpp::dag_shortest_paths(G, 0);
    for (int i = 0; i < 5; ++i) {
        if (i == 0) {
            std::cout << "Distance to 0: 0 Parent: 0\n";
            continue;
        }
        if (dist.at(i) == std::numeric_limits<double>::max()) {
            std::cout << "Distance to " << i << ": " << std::numeric_limits<int>::max()
                      << " Parent: " << i << "\n";
            continue;
        }
        const auto path = nxpp::dijkstra_path(G, 0, i);
        std::cout << "Distance to " << i << ": " << static_cast<int>(dist.at(i))
                  << " Parent: " << path[path.size() - 2] << "\n";
    }

    return EXIT_SUCCESS;
}
