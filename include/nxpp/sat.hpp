#pragma once

#include "components.hpp"

namespace nxpp {

inline int to_2sat_vertex_id(int literal) {
    return (literal > 0) ? (2 * literal - 2) : (-2 * literal - 1);
}

inline bool two_sat_satisfiable(int num_variables, const std::vector<std::pair<int, int>>& clauses) {
    using SatGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS>;

    SatGraph g(2 * num_variables);
    for (const auto& [x, y] : clauses) {
        boost::add_edge(to_2sat_vertex_id(-x), to_2sat_vertex_id(y), g);
        boost::add_edge(to_2sat_vertex_id(-y), to_2sat_vertex_id(x), g);
    }

    std::vector<int> comp(boost::num_vertices(g));
    boost::strong_components(g, boost::make_iterator_property_map(comp.begin(), boost::get(boost::vertex_index, g)));

    for (int i = 1; i <= num_variables; ++i) {
        if (comp[to_2sat_vertex_id(i)] == comp[to_2sat_vertex_id(-i)]) {
            return false;
        }
    }
    return true;
}

} // namespace nxpp
