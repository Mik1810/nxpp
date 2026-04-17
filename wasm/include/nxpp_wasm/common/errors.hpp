#pragma once

namespace nxpp_wasm::common {

[[noreturn]] void throw_missing_node();
[[noreturn]] void throw_missing_edge();
[[noreturn]] void throw_missing_edge_id();

} // namespace nxpp_wasm::common
