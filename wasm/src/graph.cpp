#include "../include/nxpp_wasm/graph.hpp"

#include <emscripten/bind.h>

namespace nxpp_wasm {

void register_graph_bindings() {
    emscripten::class_<GraphIntBinding> graph_int("GraphInt");
    graph_int.constructor<>();
    common::bind_simple_graph_api(graph_int);

    emscripten::class_<GraphStrBinding> graph_str("GraphStr");
    graph_str.constructor<>();
    common::bind_simple_graph_api(graph_str);

    emscripten::class_<DiGraphIntBinding> digraph_int("DiGraphInt");
    digraph_int.constructor<>();
    common::bind_simple_graph_api(digraph_int);

    emscripten::class_<DiGraphStrBinding> digraph_str("DiGraphStr");
    digraph_str.constructor<>();
    common::bind_simple_graph_api(digraph_str);
}

} // namespace nxpp_wasm
