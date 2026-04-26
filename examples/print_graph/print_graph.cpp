#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "../../include/nxpp.hpp"

int main() {
    nxpp::WeightedDiGraphStr graph;
    const std::vector<std::string> nodes = {
        "Milan",
        "Rome",
        "Naples",
        "Turin",
        "Florence",
        "Venice",
        "Bologna",
        "Palermo",
    };

    for (const auto& node : nodes) {
        graph.add_node(node);
    }

    std::mt19937 rng(7);
    std::bernoulli_distribution keep_edge(0.28);
    std::uniform_int_distribution<int> weight_dist(1, 9);

    for (const auto& source : nodes) {
        for (const auto& target : nodes) {
            if (source == target) {
                continue;
            }
            if (keep_edge(rng)) {
                const int weight = weight_dist(rng);
                graph.add_edge(source, target, weight, {{"label", weight}});
            }
        }
    }

    nxpp::viz::write_dot(graph, "examples/print_graph/print_graph.dot");

    const int result = std::system("dot -Tsvg examples/print_graph/print_graph.dot -o examples/print_graph/print_graph.svg");
    if (result != 0) {
        std::cerr << "failed to generate examples/print_graph/print_graph.svg; install Graphviz and ensure 'dot' is available\n";
        return EXIT_FAILURE;
    }

    std::cout << "wrote examples/print_graph/print_graph.dot\n";
    std::cout << "wrote examples/print_graph/print_graph.svg\n";
    return EXIT_SUCCESS;
}
