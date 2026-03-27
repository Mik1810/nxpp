#include <cstdlib>
#include <iostream>
#include <vector>

#include "../../include/nxpp.hpp"

int to_vertex_id(int i) {
    return (i > 0) ? (2 * i - 2) : (-2 * i - 1);
}

bool solve() {

    int n,m;
    std::cin >> n >> m;

    nxpp::UnweightedDiGraphInt G;
    std::vector<int> nodes;

    for (int i = 0; i < m; ++i) {
        int x = 0;
        int y = 0;
        std::cin >> x >> y;

        G.add_edge(to_vertex_id(-x), to_vertex_id(y));
        G.add_edge(to_vertex_id(-y), to_vertex_id(x));
    }

    auto comp = nxpp::strong_components(G);
    for (int i = 1; i <= n; ++i) {
        if (comp[to_vertex_id(i)] == comp[to_vertex_id(-i)]) {
            return false;
        }
    }

    return true;
}

int main() {
    std::cout << (solve() ? "Satisfiable\n" : "Not satisfiable\n");
    return EXIT_SUCCESS;
}
