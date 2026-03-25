#include <cstdlib>
#include <iostream>
#include <limits>

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

    const auto matrix = nxpp::floyd_warshall_all_pairs_shortest_paths(G);
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            const double value = matrix.at(i).at(j);
            if (value == std::numeric_limits<double>::max()) {
                std::cout << std::numeric_limits<int>::max() << " ";
            } else {
                std::cout << static_cast<int>(value) << " ";
            }
        }
        std::cout << "\n";
    }

    return EXIT_SUCCESS;
}
