#include "../include/nxpp_wasm/components.hpp"

namespace nxpp_wasm {

void register_components_bindings() {
}

void bind_connected_components_api(emscripten::class_<GraphIntBinding>& binding) {
    binding.function("connectedComponents", &GraphIntBinding::connected_components);
}

void bind_connected_components_api(emscripten::class_<GraphStrBinding>& binding) {
    binding.function("connectedComponents", &GraphStrBinding::connected_components);
}

void bind_connected_components_api(emscripten::class_<MultiGraphIntBinding>& binding) {
    binding.function("connectedComponents", &MultiGraphIntBinding::connected_components);
}

void bind_connected_components_api(emscripten::class_<MultiGraphStrBinding>& binding) {
    binding.function("connectedComponents", &MultiGraphStrBinding::connected_components);
}

void bind_strongly_connected_components_api(emscripten::class_<DiGraphIntBinding>& binding) {
    binding.function("stronglyConnectedComponents", &DiGraphIntBinding::strongly_connected_components);
}

void bind_strongly_connected_components_api(emscripten::class_<DiGraphStrBinding>& binding) {
    binding.function("stronglyConnectedComponents", &DiGraphStrBinding::strongly_connected_components);
}

void bind_strongly_connected_components_api(emscripten::class_<MultiDiGraphIntBinding>& binding) {
    binding.function("stronglyConnectedComponents", &MultiDiGraphIntBinding::strongly_connected_components);
}

void bind_strongly_connected_components_api(emscripten::class_<MultiDiGraphStrBinding>& binding) {
    binding.function("stronglyConnectedComponents", &MultiDiGraphStrBinding::strongly_connected_components);
}

} // namespace nxpp_wasm
