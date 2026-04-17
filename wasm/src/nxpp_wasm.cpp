#include <emscripten/bind.h>

#include "../include/nxpp_wasm.hpp"

EMSCRIPTEN_BINDINGS(nxpp_node_module) {
    nxpp_wasm::register_all_bindings();
}
