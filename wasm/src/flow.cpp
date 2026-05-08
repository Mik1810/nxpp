#include "../include/nxpp_wasm/flow.hpp"

namespace nxpp_wasm {

namespace {

template <typename BindingT>
void bind_common_flow_api(emscripten::class_<BindingT>& binding) {
    binding
        .function("maximumFlow", &BindingT::maximum_flow)
        .function("minimumCut", &BindingT::minimum_cut)
        .function("maxFlowMinCost", &BindingT::max_flow_min_cost)
        .function("maxFlowMinCostSuccessiveShortestPath", &BindingT::max_flow_min_cost_successive_shortest_path)
        .function("pushRelabelMaximumFlow", &BindingT::push_relabel_maximum_flow)
        .function("cycleCanceling", &BindingT::cycle_canceling);
}

} // namespace

void register_flow_bindings() {
}

void bind_flow_api(emscripten::class_<GraphIntBinding>& binding) {
    bind_common_flow_api(binding);
}

void bind_flow_api(emscripten::class_<GraphStrBinding>& binding) {
    bind_common_flow_api(binding);
}

void bind_flow_api(emscripten::class_<DiGraphIntBinding>& binding) {
    bind_common_flow_api(binding);
}

void bind_flow_api(emscripten::class_<DiGraphStrBinding>& binding) {
    bind_common_flow_api(binding);
}

void bind_flow_api(emscripten::class_<MultiGraphIntBinding>& binding) {
    bind_common_flow_api(binding);
}

void bind_flow_api(emscripten::class_<MultiGraphStrBinding>& binding) {
    bind_common_flow_api(binding);
}

void bind_flow_api(emscripten::class_<MultiDiGraphIntBinding>& binding) {
    bind_common_flow_api(binding);
}

void bind_flow_api(emscripten::class_<MultiDiGraphStrBinding>& binding) {
    bind_common_flow_api(binding);
}

} // namespace nxpp_wasm
