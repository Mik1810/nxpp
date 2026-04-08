#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/betweenness_centrality.hpp>
#include <boost/graph/page_rank.hpp>

using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS>;
using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

int main() {
    Graph G(4);

    boost::add_edge(0, 1, G);
    boost::add_edge(0, 2, G);
    boost::add_edge(1, 2, G);
    boost::add_edge(2, 0, G);
    boost::add_edge(2, 3, G);
    boost::add_edge(3, 2, G);

    std::vector<double> degree(boost::num_vertices(G), 0.0);
    std::vector<double> rank(boost::num_vertices(G), 0.0);
    std::vector<double> betweenness(boost::num_vertices(G), 0.0);

    const auto index_map = boost::get(boost::vertex_index, G);
    const auto rank_map = boost::make_iterator_property_map(rank.begin(), index_map);
    const auto betweenness_map = boost::make_iterator_property_map(betweenness.begin(), index_map);

    const double scale = boost::num_vertices(G) > 1 ? 1.0 / static_cast<double>(boost::num_vertices(G) - 1) : 0.0;
    for (auto [vit, vend] = boost::vertices(G); vit != vend; ++vit) {
        degree[*vit] = static_cast<double>(boost::in_degree(*vit, G) + boost::out_degree(*vit, G)) * scale;
    }

    boost::graph::page_rank(G, rank_map);
    boost::brandes_betweenness_centrality(G, betweenness_map);

    double rank_sum = 0.0;
    for (const auto score : rank) {
        rank_sum += score;
    }
    if (rank_sum != 0.0) {
        for (auto& score : rank) {
            score /= rank_sum;
        }
    }

    if (boost::num_vertices(G) > 2) {
        const double factor = 1.0 / static_cast<double>((boost::num_vertices(G) - 1) * (boost::num_vertices(G) - 2));
        for (auto& score : betweenness) {
            score *= factor;
        }
    }

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "Degree centrality\n";
    for (Vertex v = 0; v < boost::num_vertices(G); ++v) {
        std::cout << v << ": " << degree[v] << "\n";
    }

    std::cout << "\nPageRank\n";
    for (Vertex v = 0; v < boost::num_vertices(G); ++v) {
        std::cout << v << ": " << rank[v] << "\n";
    }

    std::cout << "\nBetweenness centrality\n";
    for (Vertex v = 0; v < boost::num_vertices(G); ++v) {
        std::cout << v << ": " << betweenness[v] << "\n";
    }

    return EXIT_SUCCESS;
}
