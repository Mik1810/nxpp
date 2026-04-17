#include "../../include/nxpp_wasm/common/errors.hpp"

#include <stdexcept>

namespace nxpp_wasm::common {

void throw_missing_node() {
    throw std::runtime_error("Node lookup failed: node not found.");
}

void throw_missing_edge() {
    throw std::runtime_error("Edge lookup failed: edge not found.");
}

void throw_missing_edge_id() {
    throw std::runtime_error("Edge lookup failed: edge_id not found.");
}

} // namespace nxpp_wasm::common
