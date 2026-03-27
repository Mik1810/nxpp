#include <cstdlib>
#include <iostream>

#include "../../include/nxpp.hpp"

class custom_visitor : public nxpp::visitor {
public:
    void tree_edge(int u, int v, const auto&) {
        std::cout << "Discovered tree edge from " << u << " to " << v << "\n";
    }

    void back_edge(int u, int v, const auto&) {
        std::cout << "Discovered back edge from " << u << " to " << v << "\n";
    }
};

int main() {
    nxpp::UnweightedDiGraphInt G;
    G.add_nodes_from({0, 1, 2, 3, 4});

    G.add_edge(0, 2);
    G.add_edge(1, 0);
    G.add_edge(1, 4);
    G.add_edge(2, 1);
    G.add_edge(2, 3);
    G.add_edge(3, 2);
    G.add_edge(4, 3);

    custom_visitor vis;
    nxpp::depth_first_search(G, 0, vis);
    return EXIT_SUCCESS;
}
