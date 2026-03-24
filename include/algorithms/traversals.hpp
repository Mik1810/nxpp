#ifndef NXPP_TRAVERSALS_HPP
#define NXPP_TRAVERSALS_HPP

#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <vector>
#include <utility>

namespace nxpp {

// Visitor per raccogliere gli archi dell'albero BFS
template <typename Edge>
class BfsEdgeVisitor : public boost::default_bfs_visitor {
public:
    BfsEdgeVisitor(std::vector<std::pair<int, int>>& edges) : tree_edges(edges) {}

    template <typename Graph>
    void tree_edge(Edge e, const Graph& g) {
        tree_edges.emplace_back(boost::source(e, g), boost::target(e, g));
    }
private:
    std::vector<std::pair<int, int>>& tree_edges;
};

template <typename GraphWrapper>
std::vector<std::pair<int, int>> bfs_edges(const GraphWrapper& G, int start) {
    std::vector<std::pair<int, int>> edges;
    using GraphType = typename GraphWrapper::GraphType;
    using EdgeType = typename boost::graph_traits<GraphType>::edge_descriptor;
    
    BfsEdgeVisitor<EdgeType> vis(edges);
    boost::breadth_first_search(G.get_impl(), start, boost::visitor(vis));
    return edges;
}

// Visitor per raccogliere gli archi dell'albero DFS
template <typename Edge>
class DfsEdgeVisitor : public boost::default_dfs_visitor {
public:
    DfsEdgeVisitor(std::vector<std::pair<int, int>>& edges) : tree_edges(edges) {}

    template <typename Graph>
    void tree_edge(Edge e, const Graph& g) {
        tree_edges.emplace_back(boost::source(e, g), boost::target(e, g));
    }
private:
    std::vector<std::pair<int, int>>& tree_edges;
};

template <typename GraphWrapper>
std::vector<std::pair<int, int>> dfs_edges(const GraphWrapper& G, int start) {
    std::vector<std::pair<int, int>> edges;
    using GraphType = typename GraphWrapper::GraphType;
    using EdgeType = typename boost::graph_traits<GraphType>::edge_descriptor;
    
    DfsEdgeVisitor<EdgeType> vis(edges);
    boost::depth_first_search(G.get_impl(), boost::root_vertex(start).visitor(vis));
    return edges;
}

} // namespace nxpp

#endif // NXPP_TRAVERSALS_HPP
