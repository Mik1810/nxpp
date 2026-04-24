#include "../include/nxpp_wasm/spanning_tree.hpp"

namespace nxpp_wasm {

namespace {

template <typename BindingT>
void bind_common_spanning_tree_api(emscripten::class_<BindingT>& binding) {
    binding
        .function("kruskalMinimumSpanningTree", &BindingT::kruskal_minimum_spanning_tree)
        .function("primMinimumSpanningTree", &BindingT::prim_minimum_spanning_tree);
}

} // namespace

void register_spanning_tree_bindings() {}

void bind_spanning_tree_api(emscripten::class_<GraphIntBinding>& binding) {
    bind_common_spanning_tree_api(binding);
}

void bind_spanning_tree_api(emscripten::class_<GraphStrBinding>& binding) {
    bind_common_spanning_tree_api(binding);
}

void bind_spanning_tree_api(emscripten::class_<DiGraphIntBinding>& binding) {
    bind_common_spanning_tree_api(binding);
}

void bind_spanning_tree_api(emscripten::class_<DiGraphStrBinding>& binding) {
    bind_common_spanning_tree_api(binding);
}

void bind_spanning_tree_api(emscripten::class_<MultiGraphIntBinding>& binding) {
    bind_common_spanning_tree_api(binding);
}

void bind_spanning_tree_api(emscripten::class_<MultiGraphStrBinding>& binding) {
    bind_common_spanning_tree_api(binding);
}

void bind_spanning_tree_api(emscripten::class_<MultiDiGraphIntBinding>& binding) {
    bind_common_spanning_tree_api(binding);
}

void bind_spanning_tree_api(emscripten::class_<MultiDiGraphStrBinding>& binding) {
    bind_common_spanning_tree_api(binding);
}

} // namespace nxpp_wasm
