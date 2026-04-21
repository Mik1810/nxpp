#include "../include/nxpp_wasm/shortest_paths.hpp"

namespace nxpp_wasm {

namespace {

template <typename BindingT>
void bind_common_shortest_paths_api(emscripten::class_<BindingT>& binding) {
    binding
        .function("shortestPath", &BindingT::shortest_path)
        .function("shortestPathWeighted", &BindingT::shortest_path_weighted)
        .function("shortestPathLength", &BindingT::shortest_path_length)
        .function("shortestPathLengthWeighted", &BindingT::shortest_path_length_weighted)
        .function("dijkstraPath", &BindingT::dijkstra_path)
        .function("dijkstraPathWeighted", &BindingT::dijkstra_path_weighted)
        .function("dijkstraShortestPaths", &BindingT::dijkstra_shortest_paths)
        .function("dijkstraPathLengths", &BindingT::dijkstra_path_lengths)
        .function("dijkstraPathLength", &BindingT::dijkstra_path_length)
        .function("dijkstraPathLengthWeighted", &BindingT::dijkstra_path_length_weighted)
        .function("bellmanFordPath", &BindingT::bellman_ford_path)
        .function("bellmanFordPathWeighted", &BindingT::bellman_ford_path_weighted)
        .function("bellmanFordShortestPaths", &BindingT::bellman_ford_shortest_paths)
        .function("bellmanFordPathLength", &BindingT::bellman_ford_path_length)
        .function("bellmanFordPathLengthWeighted", &BindingT::bellman_ford_path_length_weighted)
        .function("dagShortestPaths", &BindingT::dag_shortest_paths)
        .function("floydWarshallAllPairsShortestPaths", &BindingT::floyd_warshall_all_pairs_shortest_paths)
        .function("floydWarshallAllPairsShortestPathsMap", &BindingT::floyd_warshall_all_pairs_shortest_paths_map);
}

} // namespace

void bind_shortest_paths_api(emscripten::class_<GraphIntBinding>& binding) {
    bind_common_shortest_paths_api(binding);
}

void bind_shortest_paths_api(emscripten::class_<GraphStrBinding>& binding) {
    bind_common_shortest_paths_api(binding);
}

void bind_shortest_paths_api(emscripten::class_<DiGraphIntBinding>& binding) {
    bind_common_shortest_paths_api(binding);
}

void bind_shortest_paths_api(emscripten::class_<DiGraphStrBinding>& binding) {
    bind_common_shortest_paths_api(binding);
}

void bind_shortest_paths_api(emscripten::class_<MultiGraphIntBinding>& binding) {
    bind_common_shortest_paths_api(binding);
}

void bind_shortest_paths_api(emscripten::class_<MultiGraphStrBinding>& binding) {
    bind_common_shortest_paths_api(binding);
}

void bind_shortest_paths_api(emscripten::class_<MultiDiGraphIntBinding>& binding) {
    bind_common_shortest_paths_api(binding);
}

void bind_shortest_paths_api(emscripten::class_<MultiDiGraphStrBinding>& binding) {
    bind_common_shortest_paths_api(binding);
}

} // namespace nxpp_wasm
