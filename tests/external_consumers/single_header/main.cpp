#include <iostream>

#include <nxpp.hpp>

int main() {
    nxpp::GraphInt graph;
    graph.add_edge(0, 1, 4);
    if (!graph.has_edge(0, 1)) {
        return 1;
    }
    std::cout << "single-header-ok\n";
    return 0;
}
