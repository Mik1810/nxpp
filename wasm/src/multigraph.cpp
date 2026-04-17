#include "../include/nxpp_wasm/multigraph.hpp"

#include <emscripten/bind.h>

namespace nxpp_wasm {

void register_multigraph_bindings() {
    emscripten::class_<EdgeEndpointsIntBinding> edge_endpoints_int("EdgeEndpointsInt");
    edge_endpoints_int
        .function("source", &EdgeEndpointsIntBinding::source)
        .function("target", &EdgeEndpointsIntBinding::target);

    emscripten::class_<EdgeEndpointsStrBinding> edge_endpoints_str("EdgeEndpointsStr");
    edge_endpoints_str
        .function("source", &EdgeEndpointsStrBinding::source)
        .function("target", &EdgeEndpointsStrBinding::target);

    emscripten::class_<MultiGraphIntBinding> multigraph_int("MultiGraphInt");
    multigraph_int.constructor<>();
    common::bind_multigraph_api(multigraph_int);

    emscripten::class_<MultiGraphStrBinding> multigraph_str("MultiGraphStr");
    multigraph_str.constructor<>();
    common::bind_multigraph_api(multigraph_str);

    emscripten::class_<MultiDiGraphIntBinding> multidigraph_int("MultiDiGraphInt");
    multidigraph_int.constructor<>();
    common::bind_multigraph_api(multidigraph_int);

    emscripten::class_<MultiDiGraphStrBinding> multidigraph_str("MultiDiGraphStr");
    multidigraph_str.constructor<>();
    common::bind_multigraph_api(multidigraph_str);
}

} // namespace nxpp_wasm
