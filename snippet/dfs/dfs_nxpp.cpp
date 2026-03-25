#include <cstdlib>
#include <functional>
#include <iostream>
#include <unordered_map>

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

    std::unordered_map<int, int> color;
    for (int i = 0; i < 5; ++i) {
        color[i] = 0;
    }

    std::function<void(int)> dfs = [&](int u) {
        color[u] = 1;
        for (const int v : G.successors(u)) {
            if (color[v] == 0) {
                std::cout << "Discovered tree edge from " << u << " to " << v << "\n";
                dfs(v);
            } else if (color[v] == 1) {
                std::cout << "Discovered back edge from " << u << " to " << v << "\n";
            }
        }
        color[u] = 2;
    };

    dfs(0);
    return EXIT_SUCCESS;
}
