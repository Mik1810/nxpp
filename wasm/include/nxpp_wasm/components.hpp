#pragma once

#include <emscripten/bind.h>

#include "graph.hpp"
#include "multigraph.hpp"

namespace nxpp_wasm {

void register_components_bindings();

void bind_connected_components_api(emscripten::class_<GraphIntBinding>& binding);
void bind_connected_components_api(emscripten::class_<GraphStrBinding>& binding);
void bind_connected_components_api(emscripten::class_<MultiGraphIntBinding>& binding);
void bind_connected_components_api(emscripten::class_<MultiGraphStrBinding>& binding);

void bind_strongly_connected_components_api(emscripten::class_<DiGraphIntBinding>& binding);
void bind_strongly_connected_components_api(emscripten::class_<DiGraphStrBinding>& binding);
void bind_strongly_connected_components_api(emscripten::class_<MultiDiGraphIntBinding>& binding);
void bind_strongly_connected_components_api(emscripten::class_<MultiDiGraphStrBinding>& binding);

} // namespace nxpp_wasm
