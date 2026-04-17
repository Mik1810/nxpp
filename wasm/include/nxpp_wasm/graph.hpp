#pragma once

#include "common/binding_utils.hpp"

namespace nxpp_wasm {

using GraphIntBinding = common::SimpleGraphBindingBase<int, false>;
using GraphStrBinding = common::SimpleGraphBindingBase<std::string, false>;
using DiGraphIntBinding = common::SimpleGraphBindingBase<int, true>;
using DiGraphStrBinding = common::SimpleGraphBindingBase<std::string, true>;

void register_graph_bindings();

} // namespace nxpp_wasm
