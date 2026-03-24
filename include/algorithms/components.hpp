#ifndef NXPP_COMPONENTS_HPP
#define NXPP_COMPONENTS_HPP

#include <boost/graph/connected_components.hpp>
#include <boost/graph/strong_components.hpp>
#include <vector>

namespace nxpp {

// Per grafi non orientati
template <typename GraphWrapper>
std::vector<std::vector<int>> connected_components(const GraphWrapper& G) {
    auto& g = G.get_impl();
    int n = boost::num_vertices(g);
    std::vector<int> comp(n);
    int num = boost::connected_components(g, boost::make_iterator_property_map(comp.begin(), boost::get(boost::vertex_index, g)));
    
    std::vector<std::vector<int>> components(num);
    for (int i = 0; i < n; ++i) {
        components[comp[i]].push_back(i);
    }
    return components;
}

// Per grafi orientati
template <typename GraphWrapper>
std::vector<std::vector<int>> strongly_connected_components(const GraphWrapper& G) {
    auto& g = G.get_impl();
    int n = boost::num_vertices(g);
    std::vector<int> comp(n);
    int num = boost::strong_components(g, boost::make_iterator_property_map(comp.begin(), boost::get(boost::vertex_index, g)));
    
    std::vector<std::vector<int>> components(num);
    for (int i = 0; i < n; ++i) {
        components[comp[i]].push_back(i);
    }
    return components;
}

} // namespace nxpp

#endif // NXPP_COMPONENTS_HPP
