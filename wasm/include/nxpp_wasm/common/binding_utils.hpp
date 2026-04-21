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

template <typename TryGetFn, typename MissingFn>
emscripten::val read_attribute_value(TryGetFn&& try_get, bool has_attribute, MissingFn&& on_missing) {
    if (const auto value = try_get.template operator()<std::string>(); value.has_value()) {
        return emscripten::val(*value);
    }
    if (const auto value = try_get.template operator()<bool>(); value.has_value()) {
        return emscripten::val(*value);
    }
    if (const auto value = try_get.template operator()<double>(); value.has_value()) {
        return emscripten::val(*value);
    }
    if (const auto value = try_get.template operator()<float>(); value.has_value()) {
        return emscripten::val(*value);
    }
    if (const auto value = try_get.template operator()<int>(); value.has_value()) {
        return emscripten::val(*value);
    }
    if (const auto value = try_get.template operator()<long>(); value.has_value()) {
        return emscripten::val(static_cast<double>(*value));
    }
    if (const auto value = try_get.template operator()<long long>(); value.has_value()) {
        return emscripten::val(static_cast<double>(*value));
    }
    if (has_attribute) {
        throw std::runtime_error("WASM graph attribute conversion failed: unsupported stored attribute type.");
    }

    return on_missing();
}

template <typename SetterFn>
void write_attribute_value(const emscripten::val& value, SetterFn&& set) {
    const std::string type = js_type_of(value);
    if (type == "string") {
        set(value.as<std::string>());
        return;
    }
    if (type == "number") {
        set(as_weight(value));
        return;
    }
    if (type == "boolean") {
        set(value.as<bool>());
        return;
    }
    throw std::runtime_error("WASM graph attribute values must be string, number, or boolean.");
}

template <typename NodeT>
emscripten::val to_js_edge_list(const std::vector<std::pair<NodeT, NodeT>>& edges) {
    emscripten::val array = emscripten::val::array();
    for (std::size_t i = 0; i < edges.size(); ++i) {
        emscripten::val entry = emscripten::val::object();
        entry.set("source", emscripten::val(edges[i].first));
        entry.set("target", emscripten::val(edges[i].second));
        array.set(i, entry);
    }
    return array;
}

template <typename NodeT>
emscripten::val to_js_successor_entries(const nxpp::indexed_lookup_map<NodeT, std::vector<NodeT>>& map) {
    emscripten::val array = emscripten::val::array();
    std::size_t index = 0;
    for (const auto& [node, successors] : map) {
        emscripten::val entry = emscripten::val::object();
        entry.set("node", emscripten::val(node));
        entry.set("successors", to_js_array(successors));
        array.set(index++, entry);
    }
    return array;
}

template <typename NodeT>
emscripten::val to_js_predecessor_entries(const nxpp::indexed_lookup_map<NodeT, NodeT>& map) {
    emscripten::val array = emscripten::val::array();
    std::size_t index = 0;
    for (const auto& [node, predecessor] : map) {
        emscripten::val entry = emscripten::val::object();
        entry.set("node", emscripten::val(node));
        entry.set("predecessor", emscripten::val(predecessor));
        array.set(index++, entry);
    }
    return array;
}

template <typename NodeT, typename DistanceT>
emscripten::val to_js_distance_entries(const std::map<NodeT, DistanceT>& map) {
    emscripten::val array = emscripten::val::array();
    std::size_t index = 0;
    for (const auto& [node, distance] : map) {
        emscripten::val entry = emscripten::val::object();
        entry.set("node", emscripten::val(node));
        entry.set("distance", emscripten::val(static_cast<double>(distance)));
        array.set(index++, entry);
    }
    return array;
}

template <typename NodeT>
emscripten::val to_js_predecessor_entries(const std::map<NodeT, NodeT>& map) {
    emscripten::val array = emscripten::val::array();
    std::size_t index = 0;
    for (const auto& [node, predecessor] : map) {
        emscripten::val entry = emscripten::val::object();
        entry.set("node", emscripten::val(node));
        entry.set("predecessor", emscripten::val(predecessor));
        array.set(index++, entry);
    }
    return array;
}

template <typename ResultT>
emscripten::val to_js_single_source_shortest_path_result(const ResultT& result) {
    emscripten::val object = emscripten::val::object();
    object.set("distance", to_js_distance_entries(result.distance));
    object.set("predecessor", to_js_predecessor_entries(result.predecessor));
    return object;
}

template <typename MatrixT>
emscripten::val to_js_distance_matrix(const MatrixT& matrix) {
    emscripten::val rows = emscripten::val::array();
    for (std::size_t row_index = 0; row_index < matrix.size(); ++row_index) {
        emscripten::val row = emscripten::val::array();
        for (std::size_t column_index = 0; column_index < matrix[row_index].size(); ++column_index) {
            row.set(column_index, emscripten::val(static_cast<double>(matrix[row_index][column_index])));
        }
        rows.set(row_index, row);
    }
    return rows;
}

template <typename NodeT, typename DistanceT>
emscripten::val to_js_all_pairs_distance_entries(const std::map<NodeT, std::map<NodeT, DistanceT>>& map) {
    emscripten::val sources = emscripten::val::array();
    std::size_t source_index = 0;
    for (const auto& [source, distances] : map) {
        emscripten::val source_entry = emscripten::val::object();
        source_entry.set("source", emscripten::val(source));

        emscripten::val distance_entries = emscripten::val::array();
        std::size_t distance_index = 0;
        for (const auto& [target, distance] : distances) {
            emscripten::val distance_entry = emscripten::val::object();
            distance_entry.set("target", emscripten::val(target));
            distance_entry.set("distance", emscripten::val(static_cast<double>(distance)));
            distance_entries.set(distance_index++, distance_entry);
        }

        source_entry.set("distances", distance_entries);
        sources.set(source_index++, source_entry);
    }
    return sources;
}

template <typename TreeGraphT>
emscripten::val to_js_traversal_tree(const TreeGraphT& tree) {
    emscripten::val result = emscripten::val::object();
    result.set("nodes", to_js_array(tree.nodes()));
    result.set("edges", to_js_edge_list(tree.edge_pairs()));
    return result;
}

template <typename NodeT>
emscripten::val build_js_traversal_tree(const NodeT& start, const std::vector<std::pair<NodeT, NodeT>>& edges) {
    std::vector<NodeT> nodes;
    nodes.push_back(start);
    auto ensure_node = [&](const NodeT& node) {
        if (std::find(nodes.begin(), nodes.end(), node) == nodes.end()) {
            nodes.push_back(node);
        }
    };
    for (const auto& [source, target] : edges) {
        ensure_node(source);
        ensure_node(target);
    }

    emscripten::val result = emscripten::val::object();
    result.set("nodes", to_js_array(nodes));
    result.set("edges", to_js_edge_list(edges));
    return result;
}

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

    bool has_node_attr(const emscripten::val& id, const std::string& key) const {
        return graph_.has_node_attr(as_node_id(id), key);
    }

    emscripten::val get_node_attr(const emscripten::val& id, const std::string& key) const {
        const NodeT node_id = as_node_id(id);
        return read_attribute_value(
            [&]<typename T>() { return graph_.template try_get_node_attr<T>(node_id, key); },
            graph_.has_node_attr(node_id, key),
            [&]() -> emscripten::val {
                return emscripten::val(graph_.template get_node_attr<std::string>(node_id, key));
            }
        );
    }

    emscripten::val try_get_node_attr(const emscripten::val& id, const std::string& key) const {
        const NodeT node_id = as_node_id(id);
        return read_attribute_value(
            [&]<typename T>() { return graph_.template try_get_node_attr<T>(node_id, key); },
            graph_.has_node_attr(node_id, key),
            []() -> emscripten::val { return emscripten::val::null(); }
        );
    }

    void set_node_attr(const emscripten::val& id, const std::string& key, const emscripten::val& value) {
        const NodeT node_id = as_node_id(id);
        write_attribute_value(value, [&](const auto& converted_value) {
            graph_.node(node_id)[key] = converted_value;
        });
    }

    bool has_edge_attr(const emscripten::val& source, const emscripten::val& target, const std::string& key) const {
        return graph_.has_edge_attr(as_node_id(source), as_node_id(target), key);
    }

    emscripten::val get_edge_attr(const emscripten::val& source, const emscripten::val& target, const std::string& key) const {
        const NodeT u = as_node_id(source);
        const NodeT v = as_node_id(target);
        return read_attribute_value(
            [&]<typename T>() { return graph_.template try_get_edge_attr<T>(u, v, key); },
            graph_.has_edge_attr(u, v, key),
            [&]() -> emscripten::val {
                return emscripten::val(graph_.template get_edge_attr<std::string>(u, v, key));
            }
        );
    }

    emscripten::val try_get_edge_attr(const emscripten::val& source, const emscripten::val& target, const std::string& key) const {
        const NodeT u = as_node_id(source);
        const NodeT v = as_node_id(target);
        return read_attribute_value(
            [&]<typename T>() { return graph_.template try_get_edge_attr<T>(u, v, key); },
            graph_.has_edge_attr(u, v, key),
            []() -> emscripten::val { return emscripten::val::null(); }
        );
    }

    void set_edge_attr(const emscripten::val& source, const emscripten::val& target, const std::string& key, const emscripten::val& value) {
        const NodeT u = as_node_id(source);
        const NodeT v = as_node_id(target);
        write_attribute_value(value, [&](const auto& converted_value) {
            graph_[u][v][key] = converted_value;
        });
    }

    double get_edge_numeric_attr(const emscripten::val& source, const emscripten::val& target, const std::string& key) const {
        return graph_.get_edge_numeric_attr(as_node_id(source), as_node_id(target), key);
    }

    emscripten::val bfs_edges(const emscripten::val& start) const {
        return to_js_edge_list(graph_.bfs_edges(as_node_id(start)));
    }

    emscripten::val bfs_tree(const emscripten::val& start) const {
        const NodeT start_id = as_node_id(start);
        const auto edges = graph_.bfs_edges(start_id);
        return build_js_traversal_tree(start_id, edges);
    }

    emscripten::val bfs_successors(const emscripten::val& start) const {
        return to_js_successor_entries(graph_.bfs_successors(as_node_id(start)));
    }

    emscripten::val dfs_edges(const emscripten::val& start) const {
        return to_js_edge_list(graph_.dfs_edges(as_node_id(start)));
    }

    emscripten::val dfs_tree(const emscripten::val& start) const {
        const NodeT start_id = as_node_id(start);
        const auto edges = graph_.dfs_edges(start_id);
        return build_js_traversal_tree(start_id, edges);
    }

    emscripten::val dfs_predecessors(const emscripten::val& start) const {
        return to_js_predecessor_entries(graph_.dfs_predecessors(as_node_id(start)));
    }

    emscripten::val dfs_successors(const emscripten::val& start) const {
        return to_js_successor_entries(graph_.dfs_successors(as_node_id(start)));
    }

    emscripten::val shortest_path(const emscripten::val& source, const emscripten::val& target) const {
        return to_js_array(graph_.shortest_path(as_node_id(source), as_node_id(target)));
    }

    emscripten::val shortest_path_weighted(const emscripten::val& source, const emscripten::val& target, const std::string& weight_key) const {
        return to_js_array(graph_.shortest_path(as_node_id(source), as_node_id(target), weight_key));
    }

    double shortest_path_length(const emscripten::val& source, const emscripten::val& target) const {
        return graph_.shortest_path_length(as_node_id(source), as_node_id(target));
    }

    double shortest_path_length_weighted(const emscripten::val& source, const emscripten::val& target, const std::string& weight_key) const {
        return graph_.shortest_path_length(as_node_id(source), as_node_id(target), weight_key);
    }

    emscripten::val dijkstra_path(const emscripten::val& source, const emscripten::val& target) const {
        return to_js_array(graph_.dijkstra_path(as_node_id(source), as_node_id(target)));
    }

    emscripten::val dijkstra_path_weighted(const emscripten::val& source, const emscripten::val& target, const std::string& weight_key) const {
        return to_js_array(graph_.dijkstra_path(as_node_id(source), as_node_id(target), weight_key));
    }

    emscripten::val dijkstra_shortest_paths(const emscripten::val& source) const {
        return to_js_single_source_shortest_path_result(graph_.dijkstra_shortest_paths(as_node_id(source)));
    }

    emscripten::val dijkstra_path_lengths(const emscripten::val& source) const {
        return to_js_distance_entries(graph_.dijkstra_path_length(as_node_id(source)));
    }

    double dijkstra_path_length(const emscripten::val& source, const emscripten::val& target) const {
        return static_cast<double>(graph_.dijkstra_path_length(as_node_id(source), as_node_id(target)));
    }

    double dijkstra_path_length_weighted(const emscripten::val& source, const emscripten::val& target, const std::string& weight_key) const {
        return static_cast<double>(graph_.dijkstra_path_length(as_node_id(source), as_node_id(target), weight_key));
    }

    emscripten::val bellman_ford_path(const emscripten::val& source, const emscripten::val& target) const {
        return to_js_array(graph_.bellman_ford_path(as_node_id(source), as_node_id(target)));
    }

    emscripten::val bellman_ford_path_weighted(const emscripten::val& source, const emscripten::val& target, const std::string& weight_key) const {
        return to_js_array(graph_.bellman_ford_path(as_node_id(source), as_node_id(target), weight_key));
    }

    emscripten::val bellman_ford_shortest_paths(const emscripten::val& source) const {
        return to_js_single_source_shortest_path_result(graph_.bellman_ford_shortest_paths(as_node_id(source)));
    }

    double bellman_ford_path_length(const emscripten::val& source, const emscripten::val& target) const {
        return static_cast<double>(graph_.bellman_ford_path_length(as_node_id(source), as_node_id(target)));
    }

    double bellman_ford_path_length_weighted(const emscripten::val& source, const emscripten::val& target, const std::string& weight_key) const {
        return static_cast<double>(graph_.bellman_ford_path_length(as_node_id(source), as_node_id(target), weight_key));
    }

    emscripten::val dag_shortest_paths(const emscripten::val& source) const {
        return to_js_single_source_shortest_path_result(graph_.dag_shortest_paths(as_node_id(source)));
    }

    emscripten::val floyd_warshall_all_pairs_shortest_paths() const {
        return to_js_distance_matrix(graph_.floyd_warshall_all_pairs_shortest_paths());
    }

    emscripten::val floyd_warshall_all_pairs_shortest_paths_map() const {
        return to_js_all_pairs_distance_entries(graph_.floyd_warshall_all_pairs_shortest_paths_map());
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

    bool has_node_attr(const emscripten::val& id, const std::string& key) const {
        return graph_.has_node_attr(as_node_id(id), key);
    }

    emscripten::val get_node_attr(const emscripten::val& id, const std::string& key) const {
        const NodeT node_id = as_node_id(id);
        return read_attribute_value(
            [&]<typename T>() { return graph_.template try_get_node_attr<T>(node_id, key); },
            graph_.has_node_attr(node_id, key),
            [&]() -> emscripten::val {
                return emscripten::val(graph_.template get_node_attr<std::string>(node_id, key));
            }
        );
    }

    emscripten::val try_get_node_attr(const emscripten::val& id, const std::string& key) const {
        const NodeT node_id = as_node_id(id);
        return read_attribute_value(
            [&]<typename T>() { return graph_.template try_get_node_attr<T>(node_id, key); },
            graph_.has_node_attr(node_id, key),
            []() -> emscripten::val { return emscripten::val::null(); }
        );
    }

    void set_node_attr(const emscripten::val& id, const std::string& key, const emscripten::val& value) {
        const NodeT node_id = as_node_id(id);
        write_attribute_value(value, [&](const auto& converted_value) {
            graph_.node(node_id)[key] = converted_value;
        });
    }

    bool has_edge_attr(const emscripten::val& source, const emscripten::val& target, const std::string& key) const {
        return graph_.has_edge_attr(as_node_id(source), as_node_id(target), key);
    }

    emscripten::val get_edge_attr(const emscripten::val& source, const emscripten::val& target, const std::string& key) const {
        const NodeT u = as_node_id(source);
        const NodeT v = as_node_id(target);
        return read_attribute_value(
            [&]<typename T>() { return graph_.template try_get_edge_attr<T>(u, v, key); },
            graph_.has_edge_attr(u, v, key),
            [&]() -> emscripten::val {
                return emscripten::val(graph_.template get_edge_attr<std::string>(u, v, key));
            }
        );
    }

    emscripten::val try_get_edge_attr(const emscripten::val& source, const emscripten::val& target, const std::string& key) const {
        const NodeT u = as_node_id(source);
        const NodeT v = as_node_id(target);
        return read_attribute_value(
            [&]<typename T>() { return graph_.template try_get_edge_attr<T>(u, v, key); },
            graph_.has_edge_attr(u, v, key),
            []() -> emscripten::val { return emscripten::val::null(); }
        );
    }

    void set_edge_attr(const emscripten::val& source, const emscripten::val& target, const std::string& key, const emscripten::val& value) {
        const NodeT u = as_node_id(source);
        const NodeT v = as_node_id(target);
        write_attribute_value(value, [&](const auto& converted_value) {
            graph_[u][v][key] = converted_value;
        });
    }

    double get_edge_numeric_attr(const emscripten::val& source, const emscripten::val& target, const std::string& key) const {
        return graph_.get_edge_numeric_attr(as_node_id(source), as_node_id(target), key);
    }

    bool has_edge_attr_by_id(std::size_t edge_id, const std::string& key) const {
        return graph_.has_edge_attr(edge_id, key);
    }

    emscripten::val get_edge_attr_by_id(std::size_t edge_id, const std::string& key) const {
        return read_attribute_value(
            [&]<typename T>() { return graph_.template try_get_edge_attr<T>(edge_id, key); },
            graph_.has_edge_attr(edge_id, key),
            [&]() -> emscripten::val {
                return emscripten::val(graph_.template get_edge_attr<std::string>(edge_id, key));
            }
        );
    }

    emscripten::val try_get_edge_attr_by_id(std::size_t edge_id, const std::string& key) const {
        return read_attribute_value(
            [&]<typename T>() { return graph_.template try_get_edge_attr<T>(edge_id, key); },
            graph_.has_edge_attr(edge_id, key),
            []() -> emscripten::val { return emscripten::val::null(); }
        );
    }

    void set_edge_attr_by_id(std::size_t edge_id, const std::string& key, const emscripten::val& value) {
        write_attribute_value(value, [&](const auto& converted_value) {
            graph_.set_edge_attr(edge_id, key, converted_value);
        });
    }

    double get_edge_numeric_attr_by_id(std::size_t edge_id, const std::string& key) const {
        return graph_.get_edge_numeric_attr(edge_id, key);
    }

    emscripten::val bfs_edges(const emscripten::val& start) const {
        return to_js_edge_list(graph_.bfs_edges(as_node_id(start)));
    }

    emscripten::val bfs_tree(const emscripten::val& start) const {
        const NodeT start_id = as_node_id(start);
        const auto edges = graph_.bfs_edges(start_id);
        return build_js_traversal_tree(start_id, edges);
    }

    emscripten::val bfs_successors(const emscripten::val& start) const {
        return to_js_successor_entries(graph_.bfs_successors(as_node_id(start)));
    }

    emscripten::val dfs_edges(const emscripten::val& start) const {
        return to_js_edge_list(graph_.dfs_edges(as_node_id(start)));
    }

    emscripten::val dfs_tree(const emscripten::val& start) const {
        const NodeT start_id = as_node_id(start);
        const auto edges = graph_.dfs_edges(start_id);
        return build_js_traversal_tree(start_id, edges);
    }

    emscripten::val dfs_predecessors(const emscripten::val& start) const {
        return to_js_predecessor_entries(graph_.dfs_predecessors(as_node_id(start)));
    }

    emscripten::val dfs_successors(const emscripten::val& start) const {
        return to_js_successor_entries(graph_.dfs_successors(as_node_id(start)));
    }

    emscripten::val shortest_path(const emscripten::val& source, const emscripten::val& target) const {
        return to_js_array(graph_.shortest_path(as_node_id(source), as_node_id(target)));
    }

    emscripten::val shortest_path_weighted(const emscripten::val& source, const emscripten::val& target, const std::string& weight_key) const {
        return to_js_array(graph_.shortest_path(as_node_id(source), as_node_id(target), weight_key));
    }

    double shortest_path_length(const emscripten::val& source, const emscripten::val& target) const {
        return graph_.shortest_path_length(as_node_id(source), as_node_id(target));
    }

    double shortest_path_length_weighted(const emscripten::val& source, const emscripten::val& target, const std::string& weight_key) const {
        return graph_.shortest_path_length(as_node_id(source), as_node_id(target), weight_key);
    }

    emscripten::val dijkstra_path(const emscripten::val& source, const emscripten::val& target) const {
        return to_js_array(graph_.dijkstra_path(as_node_id(source), as_node_id(target)));
    }

    emscripten::val dijkstra_path_weighted(const emscripten::val& source, const emscripten::val& target, const std::string& weight_key) const {
        return to_js_array(graph_.dijkstra_path(as_node_id(source), as_node_id(target), weight_key));
    }

    emscripten::val dijkstra_shortest_paths(const emscripten::val& source) const {
        return to_js_single_source_shortest_path_result(graph_.dijkstra_shortest_paths(as_node_id(source)));
    }

    emscripten::val dijkstra_path_lengths(const emscripten::val& source) const {
        return to_js_distance_entries(graph_.dijkstra_path_length(as_node_id(source)));
    }

    double dijkstra_path_length(const emscripten::val& source, const emscripten::val& target) const {
        return static_cast<double>(graph_.dijkstra_path_length(as_node_id(source), as_node_id(target)));
    }

    double dijkstra_path_length_weighted(const emscripten::val& source, const emscripten::val& target, const std::string& weight_key) const {
        return static_cast<double>(graph_.dijkstra_path_length(as_node_id(source), as_node_id(target), weight_key));
    }

    emscripten::val bellman_ford_path(const emscripten::val& source, const emscripten::val& target) const {
        return to_js_array(graph_.bellman_ford_path(as_node_id(source), as_node_id(target)));
    }

    emscripten::val bellman_ford_path_weighted(const emscripten::val& source, const emscripten::val& target, const std::string& weight_key) const {
        return to_js_array(graph_.bellman_ford_path(as_node_id(source), as_node_id(target), weight_key));
    }

    emscripten::val bellman_ford_shortest_paths(const emscripten::val& source) const {
        return to_js_single_source_shortest_path_result(graph_.bellman_ford_shortest_paths(as_node_id(source)));
    }

    double bellman_ford_path_length(const emscripten::val& source, const emscripten::val& target) const {
        return static_cast<double>(graph_.bellman_ford_path_length(as_node_id(source), as_node_id(target)));
    }

    double bellman_ford_path_length_weighted(const emscripten::val& source, const emscripten::val& target, const std::string& weight_key) const {
        return static_cast<double>(graph_.bellman_ford_path_length(as_node_id(source), as_node_id(target), weight_key));
    }

    emscripten::val dag_shortest_paths(const emscripten::val& source) const {
        return to_js_single_source_shortest_path_result(graph_.dag_shortest_paths(as_node_id(source)));
    }

    emscripten::val floyd_warshall_all_pairs_shortest_paths() const {
        return to_js_distance_matrix(graph_.floyd_warshall_all_pairs_shortest_paths());
    }

    emscripten::val floyd_warshall_all_pairs_shortest_paths_map() const {
        return to_js_all_pairs_distance_entries(graph_.floyd_warshall_all_pairs_shortest_paths_map());
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
