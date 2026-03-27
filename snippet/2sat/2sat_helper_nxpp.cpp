#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>

#include "../../include/nxpp.hpp"

int main() {
    int n = 0;
    int m = 0;
    std::cin >> n >> m;

    std::vector<std::pair<int, int>> clauses;
    clauses.reserve(m);
    for (int i = 0; i < m; ++i) {
        int x = 0;
        int y = 0;
        std::cin >> x >> y;
        clauses.emplace_back(x, y);
    }

    std::cout << (nxpp::two_sat_satisfiable(n, clauses) ? "Satisfiable\n" : "Not satisfiable\n");
    return EXIT_SUCCESS;
}
