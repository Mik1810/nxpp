#include <stdexcept>
#include <vector>

#include <emscripten/bind.h>

#include "include/nxpp.hpp"

namespace nxpp_wasm_node {

class DiGraphBinding {
public:
    void add_node(int id) {
        graph_.add_node(id);
    }

    void add_edge(int source, int target, int weight) {
        graph_.add_edge(source, target, weight);
    }

    bool has_node(int id) const {
        return graph_.has_node(id);
    }

    bool has_edge(int source, int target) const {
        return graph_.has_edge(source, target);
    }

    std::vector<int> nodes() const {
        return graph_.nodes();
    }

    int dijkstra_distance(int source, int target) const {
        const auto result = graph_.dijkstra_shortest_paths(source);
        return result.distance.at(target);
    }

    std::vector<int> dijkstra_path(int source, int target) const {
        const auto result = graph_.dijkstra_shortest_paths(source);
        return result.path_to(target);
    }

    void clear() {
        graph_.clear();
    }

private:
    nxpp::WeightedDiGraphInt graph_;
};

} // namespace nxpp_wasm_node

EMSCRIPTEN_BINDINGS(nxpp_node_module) {
    emscripten::register_vector<int>("IntVector");

    emscripten::class_<nxpp_wasm_node::DiGraphBinding>("DiGraph")
        .constructor<>()
        .function("addNode", &nxpp_wasm_node::DiGraphBinding::add_node)
        .function("addEdge", &nxpp_wasm_node::DiGraphBinding::add_edge)
        .function("hasNode", &nxpp_wasm_node::DiGraphBinding::has_node)
        .function("hasEdge", &nxpp_wasm_node::DiGraphBinding::has_edge)
        .function("nodes", &nxpp_wasm_node::DiGraphBinding::nodes)
        .function("dijkstraDistance", &nxpp_wasm_node::DiGraphBinding::dijkstra_distance)
        .function("dijkstraPath", &nxpp_wasm_node::DiGraphBinding::dijkstra_path)
        .function("clear", &nxpp_wasm_node::DiGraphBinding::clear);
}
