#include "../include/nxpp_wasm/attributes.hpp"

namespace nxpp_wasm {

namespace {

template <typename BindingT>
void bind_common_attribute_api(emscripten::class_<BindingT>& binding) {
    binding
        .function("hasNodeAttr", &BindingT::has_node_attr)
        .function("getNodeAttr", &BindingT::get_node_attr)
        .function("tryGetNodeAttr", &BindingT::try_get_node_attr)
        .function("setNodeAttr", &BindingT::set_node_attr)
        .function("hasEdgeAttr", &BindingT::has_edge_attr)
        .function("getEdgeAttr", &BindingT::get_edge_attr)
        .function("tryGetEdgeAttr", &BindingT::try_get_edge_attr)
        .function("setEdgeAttr", &BindingT::set_edge_attr)
        .function("getEdgeNumericAttr", &BindingT::get_edge_numeric_attr);
}

template <typename BindingT>
void bind_precise_multigraph_attribute_api(emscripten::class_<BindingT>& binding) {
    bind_common_attribute_api(binding);
    binding
        .function("hasEdgeAttrById", &BindingT::has_edge_attr_by_id)
        .function("getEdgeAttrById", &BindingT::get_edge_attr_by_id)
        .function("tryGetEdgeAttrById", &BindingT::try_get_edge_attr_by_id)
        .function("setEdgeAttrById", &BindingT::set_edge_attr_by_id)
        .function("getEdgeNumericAttrById", &BindingT::get_edge_numeric_attr_by_id);
}

} // namespace

void bind_attribute_api(emscripten::class_<GraphIntBinding>& binding) {
    bind_common_attribute_api(binding);
}

void bind_attribute_api(emscripten::class_<GraphStrBinding>& binding) {
    bind_common_attribute_api(binding);
}

void bind_attribute_api(emscripten::class_<DiGraphIntBinding>& binding) {
    bind_common_attribute_api(binding);
}

void bind_attribute_api(emscripten::class_<DiGraphStrBinding>& binding) {
    bind_common_attribute_api(binding);
}

void bind_attribute_api(emscripten::class_<MultiGraphIntBinding>& binding) {
    bind_precise_multigraph_attribute_api(binding);
}

void bind_attribute_api(emscripten::class_<MultiGraphStrBinding>& binding) {
    bind_precise_multigraph_attribute_api(binding);
}

void bind_attribute_api(emscripten::class_<MultiDiGraphIntBinding>& binding) {
    bind_precise_multigraph_attribute_api(binding);
}

void bind_attribute_api(emscripten::class_<MultiDiGraphStrBinding>& binding) {
    bind_precise_multigraph_attribute_api(binding);
}

} // namespace nxpp_wasm
