#pragma once

/**
 * @file traversal.hpp
 * @brief Breadth-first and depth-first traversal helpers, visitors, and aliases.
 *
 * @defgroup nxpp_traversal Traversal
 * @brief BFS, DFS, visitor, and callback-based traversal helpers.
 */

#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/depth_first_search.hpp>

#include <iterator>
#include <memory>
#include <optional>
#include <queue>
#include <vector>

#include "graph.hpp"

namespace nxpp {

namespace detail {

enum class traversal_order {
    bfs,
    dfs
};

template <typename GraphWrapper, traversal_order Order>
class traversal_edges_view {
public:
    using NodeID = typename GraphWrapper::NodeType;
    using value_type = std::pair<NodeID, NodeID>;

    traversal_edges_view(const GraphWrapper& graph, const NodeID& start) : graph(&graph), start(start) {}

    class iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = traversal_edges_view::value_type;
        using difference_type = std::ptrdiff_t;
        using reference = const value_type&;

        iterator() = default;

        iterator(const GraphWrapper& graph, const NodeID& start)
            : state(std::make_shared<traversal_state>(graph, start)) {
            advance();
        }

        reference operator*() const {
            return *state->current;
        }

        const value_type* operator->() const {
            return &*state->current;
        }

        iterator& operator++() {
            advance();
            return *this;
        }

        void operator++(int) {
            ++(*this);
        }

        friend bool operator==(const iterator& it, std::default_sentinel_t) {
            return !it.state || it.state->done;
        }

        friend bool operator==(std::default_sentinel_t sentinel, const iterator& it) {
            return it == sentinel;
        }

    private:
        using GraphType = typename GraphWrapper::GraphType;
        using VertexDesc = typename GraphWrapper::VertexDesc;
        using OutEdgeIterator = typename boost::graph_traits<GraphType>::out_edge_iterator;

        struct edge_cursor {
            VertexDesc vertex;
            OutEdgeIterator current;
            OutEdgeIterator end;
        };

        struct traversal_state {
            traversal_state(const GraphWrapper& graph, const NodeID& start)
                : graph(&graph),
                  colors(boost::num_vertices(graph.get_impl()), boost::white_color),
                  current(std::nullopt),
                  done(false) {
                const auto start_vertex = graph.get_id_to_bgl_map().at(start);
                colors[graph.get_vertex_index(start_vertex)] = boost::gray_color;
                if constexpr (Order == traversal_order::bfs) {
                    bfs_queue.push(start_vertex);
                } else {
                    push_dfs_vertex(start_vertex);
                }
            }

            void push_dfs_vertex(VertexDesc vertex) {
                auto [edge_it, edge_end] = boost::out_edges(vertex, graph->get_impl());
                dfs_stack.push_back(edge_cursor{vertex, edge_it, edge_end});
            }

            const GraphWrapper* graph;
            std::vector<boost::default_color_type> colors;
            std::queue<VertexDesc> bfs_queue;
            std::optional<edge_cursor> bfs_cursor;
            std::vector<edge_cursor> dfs_stack;
            std::optional<value_type> current;
            bool done;
        };

        void advance() {
            if (!state || state->done) {
                return;
            }

            state->current.reset();
            if constexpr (Order == traversal_order::bfs) {
                advance_bfs();
            } else {
                advance_dfs();
            }
        }

        void advance_bfs() {
            while (!state->bfs_queue.empty()) {
                if (!state->bfs_cursor.has_value()) {
                    const auto vertex = state->bfs_queue.front();
                    auto [edge_it, edge_end] = boost::out_edges(vertex, state->graph->get_impl());
                    state->bfs_cursor = edge_cursor{vertex, edge_it, edge_end};
                }

                while (state->bfs_cursor->current != state->bfs_cursor->end) {
                    const auto edge = *state->bfs_cursor->current;
                    ++state->bfs_cursor->current;
                    const auto child = boost::target(edge, state->graph->get_impl());
                    const auto child_index = state->graph->get_vertex_index(child);
                    if (state->colors[child_index] == boost::white_color) {
                        state->colors[child_index] = boost::gray_color;
                        state->bfs_queue.push(child);
                        state->current = value_type{
                            state->graph->get_node_id(state->bfs_cursor->vertex),
                            state->graph->get_node_id(child)
                        };
                        return;
                    }
                }

                state->colors[state->graph->get_vertex_index(state->bfs_cursor->vertex)] = boost::black_color;
                state->bfs_queue.pop();
                state->bfs_cursor.reset();
            }

            state->done = true;
        }

        void advance_dfs() {
            while (!state->dfs_stack.empty()) {
                auto& cursor = state->dfs_stack.back();
                while (cursor.current != cursor.end) {
                    const auto edge = *cursor.current;
                    ++cursor.current;
                    const auto child = boost::target(edge, state->graph->get_impl());
                    const auto child_index = state->graph->get_vertex_index(child);
                    if (state->colors[child_index] == boost::white_color) {
                        state->colors[child_index] = boost::gray_color;
                        state->current = value_type{
                            state->graph->get_node_id(cursor.vertex),
                            state->graph->get_node_id(child)
                        };
                        state->push_dfs_vertex(child);
                        return;
                    }
                }

                state->colors[state->graph->get_vertex_index(cursor.vertex)] = boost::black_color;
                state->dfs_stack.pop_back();
            }

            state->done = true;
        }

        std::shared_ptr<traversal_state> state;
    };

    iterator begin() const {
        return iterator(*graph, start);
    }

    std::default_sentinel_t end() const {
        return {};
    }

private:
    const GraphWrapper* graph;
    NodeID start;
};

template <typename NodeID, typename EdgeWeight, bool Directed, typename EdgeRange>
Graph<NodeID, EdgeWeight, Directed> build_tree_from_edges(const NodeID& root, const EdgeRange& edges) {
    Graph<NodeID, EdgeWeight, Directed> tree;
    tree.add_node(root);
    for (const auto& [u, v] : edges) {
        tree.add_edge(u, v);
    }
    return tree;
}

} // namespace detail

/**
 * @brief Minimal visitor interface for wrapper-level traversal callbacks.
 *
 * Override the hooks you care about and pass the visitor to
 * breadth_first_search() or depth_first_search().
 *
 * @ingroup nxpp_traversal
 */
class visitor {
public:
    template <typename... Args>
    void examine_vertex(Args&&...) {}

    template <typename... Args>
    void tree_edge(Args&&...) {}

    template <typename... Args>
    void back_edge(Args&&...) {}
};


// Algorithms: Traversals

template <typename GraphWrapper, typename Edge>
class GenericBfsEdgeVisitor : public boost::default_bfs_visitor {
public:
    using NodeID = typename GraphWrapper::NodeType;

    GenericBfsEdgeVisitor(std::vector<std::pair<NodeID, NodeID>>& edges, const GraphWrapper& graph_wrapper)
        : tree_edges(edges), graph_wrapper(graph_wrapper) {}

    template <typename G>
    void tree_edge(Edge e, const G& g) {
        NodeID u = graph_wrapper.get_node_id(boost::source(e, g));
        NodeID v = graph_wrapper.get_node_id(boost::target(e, g));
        tree_edges.emplace_back(u, v);
    }
private:
    std::vector<std::pair<NodeID, NodeID>>& tree_edges;
    const GraphWrapper& graph_wrapper;
};

template <typename GraphWrapper>
/**
 * @brief Deprecated free-function alias for `G.bfs_edges(start)`.
 *
 * @param G Graph wrapper on which to run BFS.
 * @param start Start node ID.
 * @return The same BFS tree-edge list returned by `G.bfs_edges(start)`.
 */
[[deprecated("Use G.bfs_edges(start) instead.")]]
auto bfs_edges(const GraphWrapper& G, const typename GraphWrapper::NodeType& start) {
    return G.bfs_edges(start);
}

template <typename GraphWrapper>
/**
 * @brief Deprecated free-function alias for `G.bfs_tree(start)`.
 *
 * @param G Graph wrapper on which to build the BFS tree.
 * @param start Start node ID.
 * @return The same BFS tree graph returned by `G.bfs_tree(start)`.
 */
[[deprecated("Use G.bfs_tree(start) instead.")]]
auto bfs_tree(const GraphWrapper& G, const typename GraphWrapper::NodeType& start) {
    return G.bfs_tree(start);
}

template <typename GraphWrapper>
/**
 * @brief Deprecated free-function alias for `G.bfs_successors(start)`.
 *
 * @param G Graph wrapper on which to compute BFS successors.
 * @param start Start node ID.
 * @return The same sparse successor map returned by `G.bfs_successors(start)`.
 */
[[deprecated("Use G.bfs_successors(start) instead.")]]
auto bfs_successors(const GraphWrapper& G, const typename GraphWrapper::NodeType& start) {
    return G.bfs_successors(start);
}

template <typename GraphWrapper, typename Edge, typename OnVertex, typename OnTreeEdge>
class GenericBfsVisitVisitor : public boost::default_bfs_visitor {
public:
    using NodeID = typename GraphWrapper::NodeType;

    GenericBfsVisitVisitor(const GraphWrapper& graph_wrapper, OnVertex& on_vertex, OnTreeEdge& on_tree_edge)
        : graph_wrapper(graph_wrapper), on_vertex(on_vertex), on_tree_edge(on_tree_edge) {}

    template <typename G>
    void examine_vertex(typename boost::graph_traits<G>::vertex_descriptor u, const G&) const {
        on_vertex(graph_wrapper.get_node_id(u));
    }

    template <typename G>
    void tree_edge(Edge e, const G& g) const {
        on_tree_edge(graph_wrapper.get_node_id(boost::source(e, g)), graph_wrapper.get_node_id(boost::target(e, g)));
    }

private:
    const GraphWrapper& graph_wrapper;
    OnVertex& on_vertex;
    OnTreeEdge& on_tree_edge;
};

template <typename Visitor, typename NodeID, typename GraphWrapper>
void bfs_dispatch_examine_vertex(Visitor& visitor, const NodeID& u, const GraphWrapper& G) {
    if constexpr (requires { visitor.examine_vertex(u, G); }) {
        visitor.examine_vertex(u, G);
    }
}

template <typename Visitor, typename NodeID, typename GraphWrapper>
void bfs_dispatch_tree_edge(Visitor& visitor, const NodeID& u, const NodeID& v, const GraphWrapper& G) {
    if constexpr (requires { visitor.tree_edge(u, v, G); }) {
        visitor.tree_edge(u, v, G);
    }
}

template <typename NodeID, typename Edge, typename GraphWrapper, typename Visitor>
class GenericBfsObjectVisitor : public boost::default_bfs_visitor {
public:
    GenericBfsObjectVisitor(const GraphWrapper& graph_wrapper, Visitor& visitor)
        : graph_wrapper(graph_wrapper), visitor(visitor) {}

    template <typename G>
    void examine_vertex(typename boost::graph_traits<G>::vertex_descriptor u, const G&) const {
        bfs_dispatch_examine_vertex(visitor, graph_wrapper.get_node_id(u), graph_wrapper);
    }

    template <typename G>
    void tree_edge(Edge e, const G& g) const {
        bfs_dispatch_tree_edge(
            visitor,
            graph_wrapper.get_node_id(boost::source(e, g)),
            graph_wrapper.get_node_id(boost::target(e, g)),
            graph_wrapper
        );
    }

private:
    const GraphWrapper& graph_wrapper;
    Visitor& visitor;
};

template <typename GraphWrapper, typename Visitor>
/**
 * @brief Deprecated free-function alias for `G.breadth_first_search(start, visitor)`.
 *
 * @param G Graph wrapper on which to run BFS.
 * @param start Start node ID.
 * @param visitor Visitor object forwarded to the graph method.
 */
[[deprecated("Use G.breadth_first_search(start, visitor) instead.")]]
void breadth_first_search(const GraphWrapper& G, const typename GraphWrapper::NodeType& start, Visitor& visitor) {
    G.breadth_first_search(start, visitor);
}

template <typename GraphWrapper, typename OnVertex, typename OnTreeEdge>
/**
 * @brief Deprecated free-function alias for `G.bfs_visit(start, on_vertex, on_tree_edge)`.
 *
 * @param G Graph wrapper on which to run the callback-based BFS visit.
 * @param start Start node ID.
 * @param on_vertex Callback invoked when a vertex is examined.
 * @param on_tree_edge Callback invoked for each BFS tree edge.
 */
[[deprecated("Use G.bfs_visit(start, on_vertex, on_tree_edge) instead.")]]
void bfs_visit(const GraphWrapper& G, const typename GraphWrapper::NodeType& start, OnVertex&& on_vertex, OnTreeEdge&& on_tree_edge) {
    G.bfs_visit(start, std::forward<OnVertex>(on_vertex), std::forward<OnTreeEdge>(on_tree_edge));
}

template <typename GraphWrapper, typename Edge>
class GenericDfsEdgeVisitor : public boost::default_dfs_visitor {
public:
    using NodeID = typename GraphWrapper::NodeType;

    GenericDfsEdgeVisitor(std::vector<std::pair<NodeID, NodeID>>& edges, const GraphWrapper& graph_wrapper)
        : tree_edges(edges), graph_wrapper(graph_wrapper) {}

    template <typename G>
    void tree_edge(Edge e, const G& g) {
        NodeID u = graph_wrapper.get_node_id(boost::source(e, g));
        NodeID v = graph_wrapper.get_node_id(boost::target(e, g));
        tree_edges.emplace_back(u, v);
    }
private:
    std::vector<std::pair<NodeID, NodeID>>& tree_edges;
    const GraphWrapper& graph_wrapper;
};

template <typename GraphWrapper>
/**
 * @brief Deprecated free-function alias for `G.dfs_edges(start)`.
 *
 * @param G Graph wrapper on which to run DFS.
 * @param start Start node ID.
 * @return The same DFS tree-edge list returned by `G.dfs_edges(start)`.
 */
[[deprecated("Use G.dfs_edges(start) instead.")]]
auto dfs_edges(const GraphWrapper& G, const typename GraphWrapper::NodeType& start) {
    return G.dfs_edges(start);
}

template <typename GraphWrapper>
/**
 * @brief Deprecated free-function alias for `G.dfs_tree(start)`.
 *
 * @param G Graph wrapper on which to build the DFS tree.
 * @param start Start node ID.
 * @return The same DFS tree graph returned by `G.dfs_tree(start)`.
 */
[[deprecated("Use G.dfs_tree(start) instead.")]]
auto dfs_tree(const GraphWrapper& G, const typename GraphWrapper::NodeType& start) {
    return G.dfs_tree(start);
}

template <typename GraphWrapper>
/**
 * @brief Deprecated free-function alias for `G.dfs_predecessors(start)`.
 *
 * @param G Graph wrapper on which to compute DFS predecessors.
 * @param start Start node ID.
 * @return The same predecessor map returned by `G.dfs_predecessors(start)`.
 */
[[deprecated("Use G.dfs_predecessors(start) instead.")]]
auto dfs_predecessors(const GraphWrapper& G, const typename GraphWrapper::NodeType& start) {
    return G.dfs_predecessors(start);
}

template <typename GraphWrapper>
/**
 * @brief Deprecated free-function alias for `G.dfs_successors(start)`.
 *
 * @param G Graph wrapper on which to compute DFS successors.
 * @param start Start node ID.
 * @return The same sparse successor map returned by `G.dfs_successors(start)`.
 */
[[deprecated("Use G.dfs_successors(start) instead.")]]
auto dfs_successors(const GraphWrapper& G, const typename GraphWrapper::NodeType& start) {
    return G.dfs_successors(start);
}

template <typename GraphWrapper, typename Edge, typename OnTreeEdge, typename OnBackEdge>
class GenericDfsVisitVisitor : public boost::default_dfs_visitor {
public:
    GenericDfsVisitVisitor(const GraphWrapper& graph_wrapper, OnTreeEdge& on_tree_edge, OnBackEdge& on_back_edge)
        : graph_wrapper(graph_wrapper), on_tree_edge(on_tree_edge), on_back_edge(on_back_edge) {}

    template <typename G>
    void tree_edge(Edge e, const G& g) const {
        on_tree_edge(graph_wrapper.get_node_id(boost::source(e, g)), graph_wrapper.get_node_id(boost::target(e, g)));
    }

    template <typename G>
    void back_edge(Edge e, const G& g) const {
        on_back_edge(graph_wrapper.get_node_id(boost::source(e, g)), graph_wrapper.get_node_id(boost::target(e, g)));
    }

private:
    const GraphWrapper& graph_wrapper;
    OnTreeEdge& on_tree_edge;
    OnBackEdge& on_back_edge;
};

template <typename Visitor, typename NodeID, typename GraphWrapper>
void dfs_dispatch_tree_edge(Visitor& visitor, const NodeID& u, const NodeID& v, const GraphWrapper& G) {
    if constexpr (requires { visitor.tree_edge(u, v, G); }) {
        visitor.tree_edge(u, v, G);
    }
}

template <typename Visitor, typename NodeID, typename GraphWrapper>
void dfs_dispatch_back_edge(Visitor& visitor, const NodeID& u, const NodeID& v, const GraphWrapper& G) {
    if constexpr (requires { visitor.back_edge(u, v, G); }) {
        visitor.back_edge(u, v, G);
    }
}

template <typename NodeID, typename Edge, typename GraphWrapper, typename Visitor>
class GenericDfsObjectVisitor : public boost::default_dfs_visitor {
public:
    GenericDfsObjectVisitor(const GraphWrapper& graph_wrapper, Visitor& visitor)
        : graph_wrapper(graph_wrapper), visitor(visitor) {}

    template <typename G>
    void tree_edge(Edge e, const G& g) const {
        dfs_dispatch_tree_edge(
            visitor,
            graph_wrapper.get_node_id(boost::source(e, g)),
            graph_wrapper.get_node_id(boost::target(e, g)),
            graph_wrapper
        );
    }

    template <typename G>
    void back_edge(Edge e, const G& g) const {
        dfs_dispatch_back_edge(
            visitor,
            graph_wrapper.get_node_id(boost::source(e, g)),
            graph_wrapper.get_node_id(boost::target(e, g)),
            graph_wrapper
        );
    }

private:
    const GraphWrapper& graph_wrapper;
    Visitor& visitor;
};

template <typename GraphWrapper, typename Visitor>
/**
 * @brief Deprecated free-function alias for `G.depth_first_search(start, visitor)`.
 *
 * @param G Graph wrapper on which to run DFS.
 * @param start Start node ID.
 * @param visitor Visitor object forwarded to the graph method.
 */
[[deprecated("Use G.depth_first_search(start, visitor) instead.")]]
void depth_first_search(const GraphWrapper& G, const typename GraphWrapper::NodeType& start, Visitor& visitor) {
    G.depth_first_search(start, visitor);
}

template <typename GraphWrapper, typename OnTreeEdge, typename OnBackEdge>
/**
 * @brief Deprecated free-function alias for `G.dfs_visit(start, on_tree_edge, on_back_edge)`.
 *
 * @param G Graph wrapper on which to run the callback-based DFS visit.
 * @param start Start node ID.
 * @param on_tree_edge Callback invoked for DFS tree edges.
 * @param on_back_edge Callback invoked for DFS back edges.
 */
[[deprecated("Use G.dfs_visit(start, on_tree_edge, on_back_edge) instead.")]]
void dfs_visit(const GraphWrapper& G, const typename GraphWrapper::NodeType& start, OnTreeEdge&& on_tree_edge, OnBackEdge&& on_back_edge) {
    G.dfs_visit(start, std::forward<OnTreeEdge>(on_tree_edge), std::forward<OnBackEdge>(on_back_edge));
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::bfs_edges(const NodeID& start) const {
    std::vector<std::pair<NodeID, NodeID>> edges;
    using EdgeType = typename boost::graph_traits<GraphType>::edge_descriptor;

    if (!has_node(start)) {
        throw std::runtime_error("Traversal failed: start node not found.");
    }

    const auto start_bgl = get_id_to_bgl_map().at(start);
    GenericBfsEdgeVisitor<Graph, EdgeType> vis(edges, *this);
    boost::breadth_first_search(get_impl(), start_bgl, boost::visitor(vis));
    return edges;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::bfs_edges_view(const NodeID& start) const {
    if (!has_node(start)) {
        throw std::runtime_error("Traversal failed: start node not found.");
    }

    return detail::traversal_edges_view<Graph, detail::traversal_order::bfs>(*this, start);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::bfs_tree(const NodeID& start) const {
    if (!has_node(start)) {
        throw std::runtime_error("Traversal failed: start node not found.");
    }

    return detail::build_tree_from_edges<NodeID, EdgeWeight, Directed>(start, bfs_edges(start));
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::bfs_successors(const NodeID& start) const {
    if (!has_node(start)) throw std::runtime_error("Traversal failed: start node not found.");
    std::vector<std::optional<std::vector<NodeID>>> successors(boost::num_vertices(g));
    for (const auto& [u, v] : bfs_edges(start)) {
        const auto parent_index = get_vertex_index(get_id_to_bgl_map().at(u));
        if (!successors[parent_index].has_value()) {
            successors[parent_index] = std::vector<NodeID>{};
        }
        successors[parent_index]->push_back(v);
    }
    return build_sparse_node_indexed_result<std::vector<NodeID>>(successors);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <typename Visitor>
void Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::breadth_first_search(const NodeID& start, Visitor& visitor) const {
    using EdgeType = typename boost::graph_traits<GraphType>::edge_descriptor;
    if (!has_node(start)) throw std::runtime_error("Traversal failed: start node not found.");
    const auto start_bgl = get_id_to_bgl_map().at(start);
    std::vector<boost::default_color_type> color(boost::num_vertices(g));
    GenericBfsObjectVisitor<NodeID, EdgeType, Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>, Visitor> vis(*this, visitor);
    boost::breadth_first_search(
        get_impl(),
        start_bgl,
        boost::visitor(vis).color_map(boost::make_iterator_property_map(color.begin(), vertex_index_map))
    );
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <typename OnVertex, typename OnTreeEdge>
void Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::bfs_visit(const NodeID& start, OnVertex&& on_vertex, OnTreeEdge&& on_tree_edge) const {
    struct callback_visitor {
        OnVertex& on_vertex;
        OnTreeEdge& on_tree_edge;
        void examine_vertex(const NodeID& u, const Graph&) { on_vertex(u); }
        void tree_edge(const NodeID& u, const NodeID& v, const Graph&) { on_tree_edge(u, v); }
    };
    auto on_vertex_fn = std::forward<OnVertex>(on_vertex);
    auto on_tree_edge_fn = std::forward<OnTreeEdge>(on_tree_edge);
    callback_visitor visitor{on_vertex_fn, on_tree_edge_fn};
    breadth_first_search(start, visitor);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dfs_edges(const NodeID& start) const {
    std::vector<std::pair<NodeID, NodeID>> edges;
    using EdgeType = typename boost::graph_traits<GraphType>::edge_descriptor;

    if (!has_node(start)) {
        throw std::runtime_error("Traversal failed: start node not found.");
    }

    const auto start_bgl = get_id_to_bgl_map().at(start);
    GenericDfsEdgeVisitor<Graph, EdgeType> vis(edges, *this);
    std::vector<boost::default_color_type> color(boost::num_vertices(g));
    boost::depth_first_search(
        get_impl(),
        boost::root_vertex(start_bgl)
            .visitor(vis)
            .color_map(boost::make_iterator_property_map(color.begin(), vertex_index_map))
    );
    return edges;
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dfs_edges_view(const NodeID& start) const {
    if (!has_node(start)) {
        throw std::runtime_error("Traversal failed: start node not found.");
    }

    return detail::traversal_edges_view<Graph, detail::traversal_order::dfs>(*this, start);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dfs_tree(const NodeID& start) const {
    if (!has_node(start)) {
        throw std::runtime_error("Traversal failed: start node not found.");
    }

    return detail::build_tree_from_edges<NodeID, EdgeWeight, Directed>(start, dfs_edges(start));
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dfs_predecessors(const NodeID& start) const {
    if (!has_node(start)) throw std::runtime_error("Traversal failed: start node not found.");
    std::vector<std::optional<NodeID>> predecessors(boost::num_vertices(g));
    for (const auto& [u, v] : dfs_edges(start)) {
        predecessors[get_vertex_index(get_id_to_bgl_map().at(v))] = u;
    }
    return build_sparse_node_indexed_result<NodeID>(predecessors);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
auto Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dfs_successors(const NodeID& start) const {
    if (!has_node(start)) throw std::runtime_error("Traversal failed: start node not found.");
    std::vector<std::optional<std::vector<NodeID>>> successors(boost::num_vertices(g));
    for (const auto& [u, v] : dfs_edges(start)) {
        const auto parent_index = get_vertex_index(get_id_to_bgl_map().at(u));
        if (!successors[parent_index].has_value()) {
            successors[parent_index] = std::vector<NodeID>{};
        }
        successors[parent_index]->push_back(v);
    }
    return build_sparse_node_indexed_result<std::vector<NodeID>>(successors);
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <typename Visitor>
void Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::depth_first_search(const NodeID& start, Visitor& visitor) const {
    using EdgeType = typename boost::graph_traits<GraphType>::edge_descriptor;
    if (!has_node(start)) throw std::runtime_error("Traversal failed: start node not found.");
    const auto start_bgl = get_id_to_bgl_map().at(start);
    std::vector<boost::default_color_type> color(boost::num_vertices(g));
    GenericDfsObjectVisitor<NodeID, EdgeType, Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>, Visitor> vis(*this, visitor);
    boost::depth_first_search(
        get_impl(),
        boost::root_vertex(start_bgl)
            .visitor(vis)
            .color_map(boost::make_iterator_property_map(color.begin(), vertex_index_map))
    );
}

template <typename NodeID, typename EdgeWeight, bool Directed, bool Multi, bool Weighted, typename OutEdgeSelector, typename VertexSelector>
template <typename OnTreeEdge, typename OnBackEdge>
void Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>::dfs_visit(const NodeID& start, OnTreeEdge&& on_tree_edge, OnBackEdge&& on_back_edge) const {
    struct callback_visitor {
        OnTreeEdge& on_tree_edge;
        OnBackEdge& on_back_edge;
        void tree_edge(const NodeID& u, const NodeID& v, const Graph&) { on_tree_edge(u, v); }
        void back_edge(const NodeID& u, const NodeID& v, const Graph&) { on_back_edge(u, v); }
    };
    auto on_tree_edge_fn = std::forward<OnTreeEdge>(on_tree_edge);
    auto on_back_edge_fn = std::forward<OnBackEdge>(on_back_edge);
    callback_visitor visitor{on_tree_edge_fn, on_back_edge_fn};
    depth_first_search(start, visitor);
}

} // namespace nxpp
