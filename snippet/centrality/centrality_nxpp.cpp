#include <cstdlib>
#include <iomanip>
#include <iostream>

#include "../../include/nxpp/centrality.hpp"

int main() {
    nxpp::DiGraphInt G;

    G.add_edge(0, 1, 1);
    G.add_edge(0, 2, 1);
    G.add_edge(1, 2, 1);
    G.add_edge(2, 0, 1);
    G.add_edge(2, 3, 1);
    G.add_edge(3, 2, 1);

    const auto degree = G.degree_centrality();
    const auto rank = G.pagerank();
    const auto between = G.betweenness_centrality();

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "Degree centrality\n";
    for (const auto& [node, score] : degree) {
        std::cout << node << ": " << score << "\n";
    }

    std::cout << "\nPageRank\n";
    for (const auto& [node, score] : rank) {
        std::cout << node << ": " << score << "\n";
    }

    std::cout << "\nBetweenness centrality\n";
    for (const auto& [node, score] : between) {
        std::cout << node << ": " << score << "\n";
    }

    return EXIT_SUCCESS;
}
