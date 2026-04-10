#include <iostream>

#include <nxpp.hpp>

int main() {
    nxpp::GraphInt graph;
    graph.add_edge(0, 1, 3);
    if (!graph.has_edge(0, 1)) {
        return 1;
    }
    std::cout << "installed-ok\n";
    return 0;
}
