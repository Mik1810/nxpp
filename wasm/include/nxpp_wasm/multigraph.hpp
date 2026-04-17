#pragma once

#include "common/binding_utils.hpp"

namespace nxpp_wasm {

using EdgeEndpointsIntBinding = common::EdgeEndpointsBindingT<int>;
using EdgeEndpointsStrBinding = common::EdgeEndpointsBindingT<std::string>;

using MultiGraphIntBinding = common::MultiGraphBindingBase<int, false>;
using MultiGraphStrBinding = common::MultiGraphBindingBase<std::string, false>;
using MultiDiGraphIntBinding = common::MultiGraphBindingBase<int, true>;
using MultiDiGraphStrBinding = common::MultiGraphBindingBase<std::string, true>;

void register_multigraph_bindings();

} // namespace nxpp_wasm
