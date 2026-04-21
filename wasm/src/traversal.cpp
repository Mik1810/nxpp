#include "../include/nxpp_wasm/traversal.hpp"

namespace nxpp_wasm {

namespace {

template <typename BindingT>
void bind_common_traversal_api(emscripten::class_<BindingT>& binding) {
    binding
        .function("bfsEdges", &BindingT::bfs_edges)
        .function("bfsTree", &BindingT::bfs_tree)
        .function("bfsSuccessors", &BindingT::bfs_successors)
        .function("dfsEdges", &BindingT::dfs_edges)
        .function("dfsTree", &BindingT::dfs_tree)
        .function("dfsPredecessors", &BindingT::dfs_predecessors)
        .function("dfsSuccessors", &BindingT::dfs_successors);
}

} // namespace

void bind_traversal_api(emscripten::class_<GraphIntBinding>& binding) {
    bind_common_traversal_api(binding);
}

void bind_traversal_api(emscripten::class_<GraphStrBinding>& binding) {
    bind_common_traversal_api(binding);
}

void bind_traversal_api(emscripten::class_<DiGraphIntBinding>& binding) {
    bind_common_traversal_api(binding);
}

void bind_traversal_api(emscripten::class_<DiGraphStrBinding>& binding) {
    bind_common_traversal_api(binding);
}

void bind_traversal_api(emscripten::class_<MultiGraphIntBinding>& binding) {
    bind_common_traversal_api(binding);
}

void bind_traversal_api(emscripten::class_<MultiGraphStrBinding>& binding) {
    bind_common_traversal_api(binding);
}

void bind_traversal_api(emscripten::class_<MultiDiGraphIntBinding>& binding) {
    bind_common_traversal_api(binding);
}

void bind_traversal_api(emscripten::class_<MultiDiGraphStrBinding>& binding) {
    bind_common_traversal_api(binding);
}

} // namespace nxpp_wasm
