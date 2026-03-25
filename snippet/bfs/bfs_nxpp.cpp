#include <cstdlib>
#include <iostream>
#include <queue>
#include <unordered_set>

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

    std::queue<int> q;
    std::unordered_set<int> discovered;
    q.push(0);
    discovered.insert(0);
    std::cout << "Visiting vertex 0\n";

    while (!q.empty()) {
        const int u = q.front();
        q.pop();
        for (const int v : G.successors(u)) {
            if (discovered.insert(v).second) {
                std::cout << "Discovered tree edge from " << u << " to " << v << "\n";
                q.push(v);
            }
        }
        if (!q.empty()) {
            std::cout << "Visiting vertex " << q.front() << "\n";
        }
    }

    return EXIT_SUCCESS;
}
