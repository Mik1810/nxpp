#include <cstdlib>
#include <iostream>
#include "../../include/nxpp/traversal.hpp"

class custom_visitor : public nxpp::visitor {
public:
    void examine_vertex(int u, const auto&) {
        std::cout << "Visiting vertex " << u << "\n";
    }

    void tree_edge(int u, int v, const auto&) {
        std::cout << "Discovered tree edge from " << u << " to " << v << "\n";
    }
};

int main() {
    nxpp::UnweightedDiGraphInt G;

    G.add_edge(0, 2);
    G.add_edge(1, 0);
    G.add_edge(1, 4);
    G.add_edge(2, 1);
    G.add_edge(2, 3);
    G.add_edge(3, 2);
    G.add_edge(4, 3);

    custom_visitor vis;
    G.breadth_first_search(0, vis);

    return EXIT_SUCCESS;
}
