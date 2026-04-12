#include <stdexcept>
#include <utility>
#include <vector>

#include <emscripten/bind.h>

#include "include/nxpp.hpp"

namespace nxpp_wasm_node {

class DijkstraResultBinding {
public:
    using ResultType = nxpp::SingleSourceShortestPathResult<int, int>;

    explicit DijkstraResultBinding(ResultType result)
        : result_(std::move(result)) {}

    bool has_path_to(int target) const {
        return result_.has_path_to(target);
    }

    int distance_to(int target) const {
        return result_.distance.at(target);
    }

    std::vector<int> path_to(int target) const {
        return result_.path_to(target);
    }

    std::vector<int> reachable_nodes() const {
        std::vector<int> keys;
        keys.reserve(result_.distance.size());
        for (const auto& entry : result_.distance) {
            keys.push_back(entry.first);
        }
        return keys;
    }

private:
    ResultType result_;
};

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

    DijkstraResultBinding dijkstra_shortest_paths(int source) const {
        return DijkstraResultBinding(graph_.dijkstra_shortest_paths(source));
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

    emscripten::class_<nxpp_wasm_node::DijkstraResultBinding>("DijkstraResult")
        .function("hasPathTo", &nxpp_wasm_node::DijkstraResultBinding::has_path_to)
        .function("distanceTo", &nxpp_wasm_node::DijkstraResultBinding::distance_to)
        .function("pathTo", &nxpp_wasm_node::DijkstraResultBinding::path_to)
        .function("reachableNodes", &nxpp_wasm_node::DijkstraResultBinding::reachable_nodes);

    emscripten::class_<nxpp_wasm_node::DiGraphBinding>("DiGraph")
        .constructor<>()
        .function("addNode", &nxpp_wasm_node::DiGraphBinding::add_node)
        .function("addEdge", &nxpp_wasm_node::DiGraphBinding::add_edge)
        .function("hasNode", &nxpp_wasm_node::DiGraphBinding::has_node)
        .function("hasEdge", &nxpp_wasm_node::DiGraphBinding::has_edge)
        .function("nodes", &nxpp_wasm_node::DiGraphBinding::nodes)
        .function("dijkstraShortestPaths", &nxpp_wasm_node::DiGraphBinding::dijkstra_shortest_paths)
        .function("clear", &nxpp_wasm_node::DiGraphBinding::clear);
}
