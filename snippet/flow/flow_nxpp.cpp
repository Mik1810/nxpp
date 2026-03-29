#include <cstdlib>
#include <iostream>

#include "../../include/nxpp/flow.hpp"

int main() {
    nxpp::UnweightedDiGraphInt G;

    G.add_edge(0, 1, {"capacity", 1});
    G.add_edge(0, 3, {"capacity", 2});
    G.add_edge(1, 2, {"capacity", 3});
    G.add_edge(1, 4, {"capacity", 4});
    G.add_edge(2, 5, {"capacity", 2});
    G.add_edge(3, 2, {"capacity", 2});
    G.add_edge(4, 5, {"capacity", 2});

    // Default behavior: const auto result = G.maximum_flow(0, 5);
    // It uses the Edmonds-Karp algorithm by default, but you can specify the algorithm explicitly:

    auto result = G.edmonds_karp_maximum_flow(0, 5);
    
    std::cout << "The maximum flow from 0 to 5 is " << result.value << "\n";
    std::cout << "The flow across edge (1, 4) is: " << result.flow[{1, 4}] << "\n";
    return EXIT_SUCCESS;
}
