#include "../include/nxpp_wasm/centrality.hpp"

namespace nxpp_wasm {

namespace {

template <typename BindingT>
void bind_common_centrality_api(emscripten::class_<BindingT>& binding) {
    binding
        .function("degreeCentrality", &BindingT::degree_centrality)
        .function("pagerank", &BindingT::pagerank)
        .function("betweennessCentrality", &BindingT::betweenness_centrality);
}

} // namespace

void register_centrality_bindings() {
}

void bind_centrality_api(emscripten::class_<GraphIntBinding>& binding) {
    bind_common_centrality_api(binding);
}

void bind_centrality_api(emscripten::class_<GraphStrBinding>& binding) {
    bind_common_centrality_api(binding);
}

void bind_centrality_api(emscripten::class_<DiGraphIntBinding>& binding) {
    bind_common_centrality_api(binding);
}

void bind_centrality_api(emscripten::class_<DiGraphStrBinding>& binding) {
    bind_common_centrality_api(binding);
}

void bind_centrality_api(emscripten::class_<MultiGraphIntBinding>& binding) {
    bind_common_centrality_api(binding);
}

void bind_centrality_api(emscripten::class_<MultiGraphStrBinding>& binding) {
    bind_common_centrality_api(binding);
}

void bind_centrality_api(emscripten::class_<MultiDiGraphIntBinding>& binding) {
    bind_common_centrality_api(binding);
}

void bind_centrality_api(emscripten::class_<MultiDiGraphStrBinding>& binding) {
    bind_common_centrality_api(binding);
}

} // namespace nxpp_wasm
