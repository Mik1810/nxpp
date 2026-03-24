#ifndef NXPP_SHORTEST_PATHS_HPP
#define NXPP_SHORTEST_PATHS_HPP

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include <vector>
#include <stdexcept>
#include <algorithm>

namespace nxpp {

// Ritorna il percorso da source a target usando Dijkstra
template <typename GraphWrapper>
std::vector<int> dijkstra_path(const GraphWrapper& G, int source, int target) {
    auto& g = G.get_impl();
    int n = boost::num_vertices(g);
    std::vector<int> dist(n);
    std::vector<int> pred(n);

    boost::dijkstra_shortest_paths(g, source, 
        boost::distance_map(boost::make_iterator_property_map(dist.begin(), boost::get(boost::vertex_index, g)))
        .predecessor_map(boost::make_iterator_property_map(pred.begin(), boost::get(boost::vertex_index, g))));

    if (pred[target] == target && source != target) {
        throw std::runtime_error("Node not reachable");
    }

    std::vector<int> path;
    for (int curr = target; curr != source; curr = pred[curr]) {
        path.push_back(curr);
    }
    path.push_back(source);
    std::reverse(path.begin(), path.end());
    return path;
}

// Ritorna le distanze minime da source a tutti gli altri nodi
template <typename GraphWrapper>
std::vector<int> dijkstra_path_length(const GraphWrapper& G, int source) {
    auto& g = G.get_impl();
    int n = boost::num_vertices(g);
    std::vector<int> dist(n);

    // Dijkstra necessita di un distance_map
    boost::dijkstra_shortest_paths(g, source, 
        boost::distance_map(boost::make_iterator_property_map(dist.begin(), boost::get(boost::vertex_index, g))));

    return dist;
}

} // namespace nxpp

#endif // NXPP_SHORTEST_PATHS_HPP
