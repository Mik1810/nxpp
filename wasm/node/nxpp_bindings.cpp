#include <cmath>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include <emscripten/bind.h>
#include <emscripten/val.h>

#include "include/nxpp.hpp"

namespace nxpp_wasm_node {

namespace {

enum class NodeIdKind {
    integer,
    string,
};

using IntDiGraph = nxpp::Graph<int, double, true>;
using StringDiGraph = nxpp::WeightedDiGraph;
using IntDijkstraResult = nxpp::SingleSourceShortestPathResult<int, double>;
using StringDijkstraResult = nxpp::SingleSourceShortestPathResult<std::string, double>;
using IntEndpoints = std::pair<int, int>;
using StringEndpoints = std::pair<std::string, std::string>;

std::string js_type_of(const emscripten::val& value) {
    return value.typeOf().as<std::string>();
}

bool is_integral_number(const emscripten::val& value) {
    if (js_type_of(value) != "number") {
        return false;
    }

    const double numeric = value.as<double>();
    return ::isfinite(numeric)
        && std::floor(numeric) == numeric
        && numeric >= static_cast<double>(std::numeric_limits<int>::min())
        && numeric <= static_cast<double>(std::numeric_limits<int>::max());
}

bool is_string_value(const emscripten::val& value) {
    return js_type_of(value) == "string";
}

NodeIdKind classify_node_id(const emscripten::val& value) {
    if (is_integral_number(value)) {
        return NodeIdKind::integer;
    }
    if (is_string_value(value)) {
        return NodeIdKind::string;
    }
    throw std::runtime_error("WASM graph node IDs must be integer-valued numbers or strings.");
}

int as_int_node_id(const emscripten::val& value) {
    if (!is_integral_number(value)) {
        throw std::runtime_error("WASM graph expects integer-valued numeric node IDs for this graph.");
    }
    return value.as<int>();
}

std::string as_string_node_id(const emscripten::val& value) {
    if (!is_string_value(value)) {
        throw std::runtime_error("WASM graph expects string node IDs for this graph.");
    }
    return value.as<std::string>();
}

double as_weight(const emscripten::val& value) {
    if (js_type_of(value) != "number") {
        throw std::runtime_error("WASM graph edge weights must be numeric.");
    }
    const double numeric = value.as<double>();
    if (!::isfinite(numeric)) {
        throw std::runtime_error("WASM graph edge weights must be finite.");
    }
    return numeric;
}

emscripten::val to_js_array(const std::vector<int>& values) {
    emscripten::val array = emscripten::val::array();
    for (std::size_t i = 0; i < values.size(); ++i) {
        array.set(i, emscripten::val(values[i]));
    }
    return array;
}

emscripten::val to_js_array(const std::vector<std::string>& values) {
    emscripten::val array = emscripten::val::array();
    for (std::size_t i = 0; i < values.size(); ++i) {
        array.set(i, emscripten::val(values[i]));
    }
    return array;
}

emscripten::val to_js_array(const std::vector<std::size_t>& values) {
    emscripten::val array = emscripten::val::array();
    for (std::size_t i = 0; i < values.size(); ++i) {
        array.set(i, emscripten::val(static_cast<double>(values[i])));
    }
    return array;
}

void throw_missing_node() {
    throw std::runtime_error("Node lookup failed: node not found.");
}

void throw_missing_edge() {
    throw std::runtime_error("Edge lookup failed: edge not found.");
}

void throw_missing_edge_id() {
    throw std::runtime_error("Edge lookup failed: edge_id not found.");
}

} // namespace

class DijkstraResultBinding {
public:
    using ResultVariant = std::variant<IntDijkstraResult, StringDijkstraResult>;

    explicit DijkstraResultBinding(IntDijkstraResult result)
        : result_(std::in_place_type<IntDijkstraResult>, std::move(result)) {}

    explicit DijkstraResultBinding(StringDijkstraResult result)
        : result_(std::in_place_type<StringDijkstraResult>, std::move(result)) {}

    bool has_path_to(const emscripten::val& target) const {
        return std::visit(
            [&](const auto& result) {
                using ResultType = std::decay_t<decltype(result)>;
                if constexpr (std::is_same_v<ResultType, IntDijkstraResult>) {
                    return result.has_path_to(as_int_node_id(target));
                } else {
                    return result.has_path_to(as_string_node_id(target));
                }
            },
            result_);
    }

    double distance_to(const emscripten::val& target) const {
        return std::visit(
            [&](const auto& result) -> double {
                using ResultType = std::decay_t<decltype(result)>;
                if constexpr (std::is_same_v<ResultType, IntDijkstraResult>) {
                    return result.distance.at(as_int_node_id(target));
                } else {
                    return result.distance.at(as_string_node_id(target));
                }
            },
            result_);
    }

    emscripten::val path_to(const emscripten::val& target) const {
        return std::visit(
            [&](const auto& result) {
                using ResultType = std::decay_t<decltype(result)>;
                if constexpr (std::is_same_v<ResultType, IntDijkstraResult>) {
                    return to_js_array(result.path_to(as_int_node_id(target)));
                } else {
                    return to_js_array(result.path_to(as_string_node_id(target)));
                }
            },
            result_);
    }

    emscripten::val reachable_nodes() const {
        return std::visit(
            [](const auto& result) {
                using ResultType = std::decay_t<decltype(result)>;
                if constexpr (std::is_same_v<ResultType, IntDijkstraResult>) {
                    std::vector<int> keys;
                    keys.reserve(result.distance.size());
                    for (const auto& entry : result.distance) {
                        keys.push_back(entry.first);
                    }
                    return to_js_array(keys);
                } else {
                    std::vector<std::string> keys;
                    keys.reserve(result.distance.size());
                    for (const auto& entry : result.distance) {
                        keys.push_back(entry.first);
                    }
                    return to_js_array(keys);
                }
            },
            result_);
    }

private:
    ResultVariant result_;
};

class EdgeEndpointsBinding {
public:
    using EndpointVariant = std::variant<IntEndpoints, StringEndpoints>;

    explicit EdgeEndpointsBinding(IntEndpoints endpoints)
        : endpoints_(std::in_place_type<IntEndpoints>, std::move(endpoints)) {}

    explicit EdgeEndpointsBinding(StringEndpoints endpoints)
        : endpoints_(std::in_place_type<StringEndpoints>, std::move(endpoints)) {}

    emscripten::val source() const {
        return std::visit(
            [](const auto& endpoints) {
                return emscripten::val(endpoints.first);
            },
            endpoints_);
    }

    emscripten::val target() const {
        return std::visit(
            [](const auto& endpoints) {
                return emscripten::val(endpoints.second);
            },
            endpoints_);
    }

private:
    EndpointVariant endpoints_;
};

class DiGraphBinding {
public:
    void add_node(const emscripten::val& id) {
        const NodeIdKind kind = ensure_backend_for_first_use(id);
        if (kind == NodeIdKind::integer) {
            int_graph_.add_node(as_int_node_id(id));
        } else {
            string_graph_.add_node(as_string_node_id(id));
        }
    }

    void add_edge(const emscripten::val& source, const emscripten::val& target, const emscripten::val& weight) {
        const NodeIdKind kind = ensure_backend_for_first_use(source, target);
        const double parsed_weight = as_weight(weight);
        if (kind == NodeIdKind::integer) {
            int_graph_.add_edge(as_int_node_id(source), as_int_node_id(target), parsed_weight);
        } else {
            string_graph_.add_edge(as_string_node_id(source), as_string_node_id(target), parsed_weight);
        }
    }

    bool has_node(const emscripten::val& id) const {
        if (!kind_) {
            classify_node_id(id);
            return false;
        }
        if (*kind_ == NodeIdKind::integer) {
            return int_graph_.has_node(as_int_node_id(id));
        }
        return string_graph_.has_node(as_string_node_id(id));
    }

    bool has_edge(const emscripten::val& source, const emscripten::val& target) const {
        if (!kind_) {
            ensure_consistent_id_pair(source, target);
            return false;
        }
        if (*kind_ == NodeIdKind::integer) {
            return int_graph_.has_edge(as_int_node_id(source), as_int_node_id(target));
        }
        return string_graph_.has_edge(as_string_node_id(source), as_string_node_id(target));
    }

    bool has_edge_id(std::size_t edge_id) const {
        if (!kind_) {
            return false;
        }
        if (*kind_ == NodeIdKind::integer) {
            return int_graph_.has_edge_id(edge_id);
        }
        return string_graph_.has_edge_id(edge_id);
    }

    emscripten::val nodes() const {
        if (!kind_) {
            return emscripten::val::array();
        }
        if (*kind_ == NodeIdKind::integer) {
            return to_js_array(int_graph_.nodes());
        }
        return to_js_array(string_graph_.nodes());
    }

    emscripten::val edge_ids() const {
        if (!kind_) {
            return emscripten::val::array();
        }
        if (*kind_ == NodeIdKind::integer) {
            return to_js_array(int_graph_.edge_ids());
        }
        return to_js_array(string_graph_.edge_ids());
    }

    emscripten::val edge_ids_between(const emscripten::val& source, const emscripten::val& target) const {
        if (!kind_) {
            ensure_consistent_id_pair(source, target);
            return emscripten::val::array();
        }
        if (*kind_ == NodeIdKind::integer) {
            return to_js_array(int_graph_.edge_ids(as_int_node_id(source), as_int_node_id(target)));
        }
        return to_js_array(string_graph_.edge_ids(as_string_node_id(source), as_string_node_id(target)));
    }

    emscripten::val neighbors(const emscripten::val& id) const {
        if (!kind_) {
            classify_node_id(id);
            throw_missing_node();
        }
        if (*kind_ == NodeIdKind::integer) {
            return to_js_array(int_graph_.neighbors(as_int_node_id(id)));
        }
        return to_js_array(string_graph_.neighbors(as_string_node_id(id)));
    }

    void remove_node(const emscripten::val& id) {
        if (!kind_) {
            classify_node_id(id);
            throw_missing_node();
        }
        if (*kind_ == NodeIdKind::integer) {
            int_graph_.remove_node(as_int_node_id(id));
        } else {
            string_graph_.remove_node(as_string_node_id(id));
        }
    }

    void remove_edge(const emscripten::val& source, const emscripten::val& target) {
        if (!kind_) {
            ensure_consistent_id_pair(source, target);
            throw_missing_edge();
        }
        if (*kind_ == NodeIdKind::integer) {
            int_graph_.remove_edge(as_int_node_id(source), as_int_node_id(target));
        } else {
            string_graph_.remove_edge(as_string_node_id(source), as_string_node_id(target));
        }
    }

    double get_edge_weight(const emscripten::val& source, const emscripten::val& target) const {
        if (!kind_) {
            ensure_consistent_id_pair(source, target);
            throw_missing_edge();
        }
        if (*kind_ == NodeIdKind::integer) {
            return int_graph_.get_edge_weight(as_int_node_id(source), as_int_node_id(target));
        }
        return string_graph_.get_edge_weight(as_string_node_id(source), as_string_node_id(target));
    }

    EdgeEndpointsBinding get_edge_endpoints(std::size_t edge_id) const {
        if (!kind_) {
            throw_missing_edge_id();
        }
        if (*kind_ == NodeIdKind::integer) {
            return EdgeEndpointsBinding(int_graph_.get_edge_endpoints(edge_id));
        }
        return EdgeEndpointsBinding(string_graph_.get_edge_endpoints(edge_id));
    }

    double get_edge_weight_by_id(std::size_t edge_id) const {
        if (!kind_) {
            throw_missing_edge_id();
        }
        if (*kind_ == NodeIdKind::integer) {
            return int_graph_.get_edge_weight(edge_id);
        }
        return string_graph_.get_edge_weight(edge_id);
    }

    void set_edge_weight_by_id(std::size_t edge_id, const emscripten::val& weight) {
        if (!kind_) {
            throw_missing_edge_id();
        }
        const double parsed_weight = as_weight(weight);
        if (*kind_ == NodeIdKind::integer) {
            int_graph_.set_edge_weight(edge_id, parsed_weight);
        } else {
            string_graph_.set_edge_weight(edge_id, parsed_weight);
        }
    }

    DijkstraResultBinding dijkstra_shortest_paths(const emscripten::val& source) const {
        if (!kind_) {
            classify_node_id(source);
            throw_missing_node();
        }
        if (*kind_ == NodeIdKind::integer) {
            return DijkstraResultBinding(int_graph_.dijkstra_shortest_paths(as_int_node_id(source)));
        }
        return DijkstraResultBinding(string_graph_.dijkstra_shortest_paths(as_string_node_id(source)));
    }

    void clear() {
        if (!kind_) {
            return;
        }
        if (*kind_ == NodeIdKind::integer) {
            int_graph_.clear();
        } else {
            string_graph_.clear();
        }
    }

private:
    NodeIdKind ensure_backend_for_first_use(const emscripten::val& id) {
        const NodeIdKind incoming = classify_node_id(id);
        if (!kind_) {
            kind_ = incoming;
            return incoming;
        }
        if (*kind_ != incoming) {
            throw std::runtime_error("WASM graph does not allow mixing numeric and string node IDs.");
        }
        return *kind_;
    }

    NodeIdKind ensure_backend_for_first_use(const emscripten::val& lhs, const emscripten::val& rhs) {
        const NodeIdKind incoming = ensure_consistent_id_pair(lhs, rhs);
        if (!kind_) {
            kind_ = incoming;
            return incoming;
        }
        if (*kind_ != incoming) {
            throw std::runtime_error("WASM graph does not allow mixing numeric and string node IDs.");
        }
        return *kind_;
    }

    NodeIdKind ensure_consistent_id_pair(const emscripten::val& lhs, const emscripten::val& rhs) const {
        const NodeIdKind lhs_kind = classify_node_id(lhs);
        const NodeIdKind rhs_kind = classify_node_id(rhs);
        if (lhs_kind != rhs_kind) {
            throw std::runtime_error("WASM graph edge endpoints must use the same node ID type.");
        }
        return lhs_kind;
    }

    std::optional<NodeIdKind> kind_;
    IntDiGraph int_graph_;
    StringDiGraph string_graph_;
};

} // namespace nxpp_wasm_node

EMSCRIPTEN_BINDINGS(nxpp_node_module) {
    emscripten::class_<nxpp_wasm_node::DijkstraResultBinding>("DijkstraResult")
        .function("hasPathTo", &nxpp_wasm_node::DijkstraResultBinding::has_path_to)
        .function("distanceTo", &nxpp_wasm_node::DijkstraResultBinding::distance_to)
        .function("pathTo", &nxpp_wasm_node::DijkstraResultBinding::path_to)
        .function("reachableNodes", &nxpp_wasm_node::DijkstraResultBinding::reachable_nodes);

    emscripten::class_<nxpp_wasm_node::EdgeEndpointsBinding>("EdgeEndpoints")
        .function("source", &nxpp_wasm_node::EdgeEndpointsBinding::source)
        .function("target", &nxpp_wasm_node::EdgeEndpointsBinding::target);

    emscripten::class_<nxpp_wasm_node::DiGraphBinding>("DiGraph")
        .constructor<>()
        .function("addNode", &nxpp_wasm_node::DiGraphBinding::add_node)
        .function("addEdge", &nxpp_wasm_node::DiGraphBinding::add_edge)
        .function("hasNode", &nxpp_wasm_node::DiGraphBinding::has_node)
        .function("hasEdge", &nxpp_wasm_node::DiGraphBinding::has_edge)
        .function("hasEdgeId", &nxpp_wasm_node::DiGraphBinding::has_edge_id)
        .function("nodes", &nxpp_wasm_node::DiGraphBinding::nodes)
        .function("edgeIds", &nxpp_wasm_node::DiGraphBinding::edge_ids)
        .function("edgeIdsBetween", &nxpp_wasm_node::DiGraphBinding::edge_ids_between)
        .function("neighbors", &nxpp_wasm_node::DiGraphBinding::neighbors)
        .function("removeNode", &nxpp_wasm_node::DiGraphBinding::remove_node)
        .function("removeEdge", &nxpp_wasm_node::DiGraphBinding::remove_edge)
        .function("getEdgeWeight", &nxpp_wasm_node::DiGraphBinding::get_edge_weight)
        .function("getEdgeEndpoints", &nxpp_wasm_node::DiGraphBinding::get_edge_endpoints)
        .function("getEdgeWeightById", &nxpp_wasm_node::DiGraphBinding::get_edge_weight_by_id)
        .function("setEdgeWeightById", &nxpp_wasm_node::DiGraphBinding::set_edge_weight_by_id)
        .function("dijkstraShortestPaths", &nxpp_wasm_node::DiGraphBinding::dijkstra_shortest_paths)
        .function("clear", &nxpp_wasm_node::DiGraphBinding::clear);
}
