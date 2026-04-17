#pragma once

#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <emscripten/bind.h>
#include <emscripten/val.h>

#include "include/nxpp.hpp"
#include "errors.hpp"
#include "js_convert.hpp"

namespace nxpp_wasm::common {

template <typename NodeT>
struct NodeJsPolicy;

template <>
struct NodeJsPolicy<int> {
    static int to_node_id(const emscripten::val& value) {
        return as_int_node_id(value, "*Int graph node IDs must be integer-valued JS numbers.");
    }
};

template <>
struct NodeJsPolicy<std::string> {
    static std::string to_node_id(const emscripten::val& value) {
        return as_string_node_id(value, "*Str graph node IDs must be JS strings.");
    }
};

template <typename NodeT>
class EdgeEndpointsBindingT {
public:
    explicit EdgeEndpointsBindingT(std::pair<NodeT, NodeT> endpoints)
        : endpoints_(std::move(endpoints)) {}

    emscripten::val source() const {
        return emscripten::val(endpoints_.first);
    }

    emscripten::val target() const {
        return emscripten::val(endpoints_.second);
    }

private:
    std::pair<NodeT, NodeT> endpoints_;
};

template <typename NodeT, bool Directed>
class SimpleGraphBindingBase {
public:
    using GraphType = nxpp::Graph<NodeT, double, Directed, false>;

    void add_node(const emscripten::val& id) {
        graph_.add_node(as_node_id(id));
    }

    void add_edge(const emscripten::val& source, const emscripten::val& target, const emscripten::val& weight) {
        graph_.add_edge(as_node_id(source), as_node_id(target), as_weight(weight));
    }

    bool has_node(const emscripten::val& id) const {
        return graph_.has_node(as_node_id(id));
    }

    bool has_edge(const emscripten::val& source, const emscripten::val& target) const {
        return graph_.has_edge(as_node_id(source), as_node_id(target));
    }

    emscripten::val nodes() const {
        return to_js_array(graph_.nodes());
    }

    emscripten::val neighbors(const emscripten::val& id) const {
        return to_js_array(graph_.neighbors(as_node_id(id)));
    }

    void remove_node(const emscripten::val& id) {
        graph_.remove_node(as_node_id(id));
    }

    void remove_edge(const emscripten::val& source, const emscripten::val& target) {
        graph_.remove_edge(as_node_id(source), as_node_id(target));
    }

    double get_edge_weight(const emscripten::val& source, const emscripten::val& target) const {
        return graph_.get_edge_weight(as_node_id(source), as_node_id(target));
    }

    void set_edge_weight(const emscripten::val& source, const emscripten::val& target, const emscripten::val& weight) {
        const NodeT u = as_node_id(source);
        const NodeT v = as_node_id(target);
        const auto ids = graph_.edge_ids(u, v);
        if (ids.empty()) {
            throw_missing_edge();
        }
        graph_.set_edge_weight(ids.front(), as_weight(weight));
    }

    void clear() {
        graph_.clear();
    }

protected:
    static NodeT as_node_id(const emscripten::val& value) {
        return NodeJsPolicy<NodeT>::to_node_id(value);
    }

private:
    GraphType graph_;
};

template <typename NodeT, bool Directed>
class MultiGraphBindingBase {
public:
    using GraphType = nxpp::Graph<NodeT, double, Directed, true>;
    using EndpointsBindingType = EdgeEndpointsBindingT<NodeT>;

    void add_node(const emscripten::val& id) {
        graph_.add_node(as_node_id(id));
    }

    void add_edge(const emscripten::val& source, const emscripten::val& target, const emscripten::val& weight) {
        graph_.add_edge(as_node_id(source), as_node_id(target), as_weight(weight));
    }

    bool has_node(const emscripten::val& id) const {
        return graph_.has_node(as_node_id(id));
    }

    bool has_edge(const emscripten::val& source, const emscripten::val& target) const {
        return graph_.has_edge(as_node_id(source), as_node_id(target));
    }

    emscripten::val nodes() const {
        return to_js_array(graph_.nodes());
    }

    emscripten::val neighbors(const emscripten::val& id) const {
        return to_js_array(graph_.neighbors(as_node_id(id)));
    }

    void remove_node(const emscripten::val& id) {
        graph_.remove_node(as_node_id(id));
    }

    void remove_edge(const emscripten::val& source, const emscripten::val& target) {
        graph_.remove_edge(as_node_id(source), as_node_id(target));
    }

    double get_edge_weight(const emscripten::val& source, const emscripten::val& target) const {
        return graph_.get_edge_weight(as_node_id(source), as_node_id(target));
    }

    void set_edge_weight(const emscripten::val& source, const emscripten::val& target, const emscripten::val& weight) {
        const NodeT u = as_node_id(source);
        const NodeT v = as_node_id(target);
        const auto ids = graph_.edge_ids(u, v);
        if (ids.empty()) {
            throw_missing_edge();
        }
        graph_.set_edge_weight(ids.front(), as_weight(weight));
    }

    bool has_edge_id(std::size_t edge_id) const {
        return graph_.has_edge_id(edge_id);
    }

    emscripten::val edge_ids() const {
        return to_js_array(graph_.edge_ids());
    }

    emscripten::val edge_ids_between(const emscripten::val& source, const emscripten::val& target) const {
        return to_js_array(graph_.edge_ids(as_node_id(source), as_node_id(target)));
    }

    EndpointsBindingType get_edge_endpoints(std::size_t edge_id) const {
        return EndpointsBindingType(graph_.get_edge_endpoints(edge_id));
    }

    double get_edge_weight_by_id(std::size_t edge_id) const {
        return graph_.get_edge_weight(edge_id);
    }

    void set_edge_weight_by_id(std::size_t edge_id, const emscripten::val& weight) {
        graph_.set_edge_weight(edge_id, as_weight(weight));
    }

    void remove_edge_by_id(std::size_t edge_id) {
        graph_.remove_edge(edge_id);
    }

    void clear() {
        graph_.clear();
    }

protected:
    static NodeT as_node_id(const emscripten::val& value) {
        return NodeJsPolicy<NodeT>::to_node_id(value);
    }

private:
    GraphType graph_;
};

template <typename BindingT>
void bind_simple_graph_api(emscripten::class_<BindingT>& binding) {
    binding
        .function("addNode", &BindingT::add_node)
        .function("addEdge", &BindingT::add_edge)
        .function("hasNode", &BindingT::has_node)
        .function("hasEdge", &BindingT::has_edge)
        .function("nodes", &BindingT::nodes)
        .function("neighbors", &BindingT::neighbors)
        .function("removeNode", &BindingT::remove_node)
        .function("removeEdge", &BindingT::remove_edge)
        .function("getEdgeWeight", &BindingT::get_edge_weight)
        .function("setEdgeWeight", &BindingT::set_edge_weight)
        .function("clear", &BindingT::clear);
}

template <typename BindingT>
void bind_multigraph_api(emscripten::class_<BindingT>& binding) {
    bind_simple_graph_api(binding);
    binding
        .function("hasEdgeId", &BindingT::has_edge_id)
        .function("edgeIds", &BindingT::edge_ids)
        .function("edgeIdsBetween", &BindingT::edge_ids_between)
        .function("getEdgeEndpoints", &BindingT::get_edge_endpoints)
        .function("getEdgeWeightById", &BindingT::get_edge_weight_by_id)
        .function("setEdgeWeightById", &BindingT::set_edge_weight_by_id)
        .function("removeEdgeById", &BindingT::remove_edge_by_id);
}

} // namespace nxpp_wasm::common
