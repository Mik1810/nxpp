#pragma once

#include "common/binding_utils.hpp"

namespace nxpp_wasm {

using MultiGraphIntBinding = common::MultiGraphBindingBase<int, false>;
using MultiGraphStrBinding = common::MultiGraphBindingBase<std::string, false>;
using MultiDiGraphIntBinding = common::MultiGraphBindingBase<int, true>;
using MultiDiGraphStrBinding = common::MultiGraphBindingBase<std::string, true>;

void register_multigraph_bindings();

} // namespace nxpp_wasm
