#include <iostream>

#include <nxpp.hpp>

int main() {
    nxpp::GraphInt graph;
    graph.add_edge(0, 1, 2);
    if (!graph.has_edge(0, 1)) {
        return 1;
    }
    std::cout << "vendored-ok\n";
    return 0;
}
