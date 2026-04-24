#include <cstdlib>
#include <iostream>

#include "../../include/nxpp.hpp"

int main() {
    nxpp::DiGraphInt g;
    g.add_edge(0, 1, 2);
    std::cout << g.to_dot_string();
    return EXIT_SUCCESS;
}
