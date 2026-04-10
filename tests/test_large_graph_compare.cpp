#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <optional>
#include <queue>
#include <random>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include <boost/graph/dag_shortest_paths.hpp>
#include <boost/graph/strong_components.hpp>
#include <boost/graph/edmonds_karp_max_flow.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>

#ifdef NXPP_HEADER_UNDER_TEST
#include NXPP_HEADER_UNDER_TEST
#else
#include "include/nxpp/graph.hpp"
#include "include/nxpp/attributes.hpp"
#include "include/nxpp/multigraph.hpp"
#include "include/nxpp/traversal.hpp"
#include "include/nxpp/shortest_paths.hpp"
#include "include/nxpp/components.hpp"
#include "include/nxpp/flow.hpp"
#endif

constexpr const char* green = "\033[32m";
constexpr const char* red = "\033[31m";
constexpr const char* reset = "\033[0m";

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

using RawUndirectedGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
using RawDirectedGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS>;
using RawDirectedWeightedGraph = boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::bidirectionalS,
    boost::no_property,
    boost::property<boost::edge_weight_t, int>>;
using RawNamedDirectedWeightedGraph = boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::bidirectionalS,
    boost::property<boost::vertex_name_t, int>,
    boost::property<boost::edge_weight_t, int>>;
using RawNamedMultiDiGraph = boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::bidirectionalS,
    boost::property<boost::vertex_name_t, int>,
    boost::property<boost::edge_weight_t, int, boost::property<boost::edge_index_t, std::size_t>>>;
using RawFlowTraits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
using RawCapacityFlowGraph = boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::directedS,
    boost::no_property,
    boost::property<
        boost::edge_capacity_t,
        long,
        boost::property<
            boost::edge_residual_capacity_t,
            long,
            boost::property<boost::edge_reverse_t, RawFlowTraits::edge_descriptor>>>>;
using RawCostFlowGraph = boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::directedS,
    boost::no_property,
    boost::property<
        boost::edge_weight_t,
        long,
        boost::property<
            boost::edge_capacity_t,
            long,
            boost::property<
                boost::edge_residual_capacity_t,
                long,
                boost::property<boost::edge_reverse_t, RawFlowTraits::edge_descriptor>>>>>;

using NxppWeightedDiGraphIntListSelectors =
    nxpp::Graph<int, int, true, false, true, boost::listS, boost::listS>;
using NxppWeightedDiGraphIntListOutVecVertexSelectors =
    nxpp::Graph<int, int, true, false, true, boost::listS, boost::vecS>;

struct LargeUndirectedGraphInput {
    int num_nodes;
    std::vector<std::pair<int, int>> edges;
};

struct LargeDirectedWeightedGraphInput {
    int num_nodes;
    std::vector<std::tuple<int, int, int>> edges;
};

struct LargeDirectedGraphInput {
    int num_nodes;
    std::vector<std::pair<int, int>> edges;
};

struct CapacityEdgeInput {
    int source;
    int target;
    long capacity;
};

struct CostFlowEdgeInput {
    int source;
    int target;
    long capacity;
    long weight;
};

struct LargeCapacityGraphInput {
    int num_nodes;
    int source;
    int sink;
    std::vector<CapacityEdgeInput> edges;
};

struct LargeCostFlowGraphInput {
    int num_nodes;
    int source;
    int sink;
    std::vector<CostFlowEdgeInput> edges;
};

std::uint64_t make_directed_edge_key(int source, int target) {
    return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(source)) << 32) |
           static_cast<std::uint32_t>(target);
}

std::uint64_t make_undirected_edge_key(int a, int b) {
    if (a > b) {
        std::swap(a, b);
    }
    return make_directed_edge_key(a, b);
}

LargeUndirectedGraphInput generate_large_partitioned_undirected_graph(
    const std::vector<int>& component_sizes,
    const std::vector<int>& extra_edges_per_component,
    std::uint32_t seed
) {
    expect(component_sizes.size() == extra_edges_per_component.size(),
           "component-size and extra-edge vectors should match");

    LargeUndirectedGraphInput input{0, {}};
    for (const int size : component_sizes) {
        input.num_nodes += size;
    }

    std::mt19937 rng(seed);
    std::unordered_set<std::uint64_t> seen_edges;
    seen_edges.reserve(static_cast<std::size_t>(input.num_nodes) * 4);

    auto add_edge = [&](int u, int v) {
        if (u == v) {
            return false;
        }
        const auto key = make_undirected_edge_key(u, v);
        if (!seen_edges.insert(key).second) {
            return false;
        }
        input.edges.emplace_back(u, v);
        return true;
    };

    int offset = 0;
    for (std::size_t component = 0; component < component_sizes.size(); ++component) {
        const int size = component_sizes[component];
        const int end = offset + size;

        for (int node = offset; node + 1 < end; ++node) {
            add_edge(node, node + 1);
        }

        std::uniform_int_distribution<int> node_dist(offset, end - 1);
        int added = 0;
        while (added < extra_edges_per_component[component]) {
            const int u = node_dist(rng);
            const int v = node_dist(rng);
            if (add_edge(u, v)) {
                ++added;
            }
        }

        offset = end;
    }

    return input;
}

LargeDirectedWeightedGraphInput generate_large_weighted_digraph(
    int num_nodes,
    int extra_edges,
    std::uint32_t seed
) {
    LargeDirectedWeightedGraphInput input{num_nodes, {}};

    std::mt19937 rng(seed);
    std::unordered_set<std::uint64_t> seen_edges;
    seen_edges.reserve(static_cast<std::size_t>(num_nodes) * 6);

    auto add_edge = [&](int source, int target, int weight) {
        if (source == target) {
            return false;
        }
        const auto key = make_directed_edge_key(source, target);
        if (!seen_edges.insert(key).second) {
            return false;
        }
        input.edges.emplace_back(source, target, weight);
        return true;
    };

    for (int node = 0; node + 1 < num_nodes; ++node) {
        add_edge(node, node + 1, 1 + (node % 13));
    }

    std::uniform_int_distribution<int> node_dist(0, num_nodes - 1);
    std::uniform_int_distribution<int> weight_dist(1, 20);
    int added = 0;
    while (added < extra_edges) {
        const int source = node_dist(rng);
        const int target = node_dist(rng);
        const int weight = weight_dist(rng);
        if (add_edge(source, target, weight)) {
            ++added;
        }
    }

    return input;
}

LargeDirectedWeightedGraphInput generate_component_weighted_digraph(
    const std::vector<int>& component_sizes,
    const std::vector<int>& extra_edges_per_component,
    std::uint32_t seed,
    int min_weight,
    int max_weight,
    bool acyclic
) {
    expect(component_sizes.size() == extra_edges_per_component.size(),
           "component-size and extra-edge vectors should match");
    expect(min_weight <= max_weight, "weight range should be valid");

    LargeDirectedWeightedGraphInput input{0, {}};
    for (const int size : component_sizes) {
        input.num_nodes += size;
    }

    std::mt19937 rng(seed);
    std::unordered_set<std::uint64_t> seen_edges;
    seen_edges.reserve(static_cast<std::size_t>(input.num_nodes) * 6);

    auto add_edge = [&](int source, int target, int weight) {
        if (source == target) {
            return false;
        }
        const auto key = make_directed_edge_key(source, target);
        if (!seen_edges.insert(key).second) {
            return false;
        }
        input.edges.emplace_back(source, target, weight);
        return true;
    };

    std::uniform_int_distribution<int> weight_dist(min_weight, max_weight);

    int offset = 0;
    for (std::size_t component = 0; component < component_sizes.size(); ++component) {
        const int size = component_sizes[component];
        const int end = offset + size;

        for (int node = offset; node + 1 < end; ++node) {
            add_edge(node, node + 1, weight_dist(rng));
        }

        if (size > 1) {
            std::uniform_int_distribution<int> node_dist(offset, end - 1);
            int added = 0;
            while (added < extra_edges_per_component[component]) {
                int source = node_dist(rng);
                int target = node_dist(rng);
                if (acyclic) {
                    if (source == target) {
                        continue;
                    }
                    if (source > target) {
                        std::swap(source, target);
                    }
                    if (source == target) {
                        continue;
                    }
                }
                if (add_edge(source, target, weight_dist(rng))) {
                    ++added;
                }
            }
        }

        offset = end;
    }

    return input;
}

LargeDirectedGraphInput generate_large_scc_digraph(
    const std::vector<int>& block_sizes,
    const std::vector<int>& extra_internal_edges,
    int extra_forward_edges_per_block,
    std::uint32_t seed
) {
    expect(block_sizes.size() == extra_internal_edges.size(),
           "block-size and extra-internal-edge vectors should match");

    LargeDirectedGraphInput input{0, {}};
    for (const int size : block_sizes) {
        input.num_nodes += size;
    }

    std::mt19937 rng(seed);
    std::unordered_set<std::uint64_t> seen_edges;
    seen_edges.reserve(static_cast<std::size_t>(input.num_nodes) * 6);

    auto add_edge = [&](int source, int target) {
        if (source == target) {
            return false;
        }
        const auto key = make_directed_edge_key(source, target);
        if (!seen_edges.insert(key).second) {
            return false;
        }
        input.edges.emplace_back(source, target);
        return true;
    };

    std::vector<std::pair<int, int>> blocks;
    int offset = 0;
    for (std::size_t block = 0; block < block_sizes.size(); ++block) {
        const int size = block_sizes[block];
        const int end = offset + size;
        blocks.emplace_back(offset, end);

        if (size > 1) {
            for (int node = offset; node + 1 < end; ++node) {
                add_edge(node, node + 1);
            }
            add_edge(end - 1, offset);
        }

        if (size > 1) {
            std::uniform_int_distribution<int> node_dist(offset, end - 1);
            int added = 0;
            while (added < extra_internal_edges[block]) {
                if (add_edge(node_dist(rng), node_dist(rng))) {
                    ++added;
                }
            }
        }

        offset = end;
    }

    for (std::size_t block = 0; block + 1 < blocks.size(); ++block) {
        const auto [source_begin, source_end] = blocks[block];
        const auto [target_begin, target_end] = blocks[block + 1];
        if (source_begin == source_end || target_begin == target_end) {
            continue;
        }

        std::uniform_int_distribution<int> source_dist(source_begin, source_end - 1);
        std::uniform_int_distribution<int> target_dist(target_begin, target_end - 1);
        int added = 0;
        while (added < extra_forward_edges_per_block) {
            if (add_edge(source_dist(rng), target_dist(rng))) {
                ++added;
            }
        }
    }

    return input;
}

LargeDirectedWeightedGraphInput generate_reachable_negative_cycle_graph(int num_nodes) {
    expect(num_nodes >= 64, "negative-cycle graph should be large enough for the fixed cycle layout");

    LargeDirectedWeightedGraphInput input{num_nodes, {}};
    std::unordered_set<std::uint64_t> seen_edges;
    seen_edges.reserve(static_cast<std::size_t>(num_nodes) * 3);

    auto add_edge = [&](int source, int target, int weight) {
        if (source == target) {
            return false;
        }
        const auto key = make_directed_edge_key(source, target);
        if (!seen_edges.insert(key).second) {
            return false;
        }
        input.edges.emplace_back(source, target, weight);
        return true;
    };

    for (int node = 0; node + 1 < num_nodes; ++node) {
        add_edge(node, node + 1, 2);
    }

    add_edge(40, 80, 1);
    add_edge(80, 120, -4);
    add_edge(120, 40, 1);

    return input;
}

LargeCapacityGraphInput generate_large_capacity_graph(
    int layers,
    int width,
    std::uint32_t seed
) {
    expect(layers >= 2, "capacity graph should have at least two internal layers");
    expect(width >= 2, "capacity graph width should be at least two");

    LargeCapacityGraphInput input;
    input.source = 0;
    input.sink = 1 + layers * width;
    input.num_nodes = input.sink + 1;

    std::mt19937 rng(seed);
    std::uniform_int_distribution<long> cap_dist(2, 9);

    auto node_id = [&](int layer, int index) {
        return 1 + layer * width + index;
    };

    for (int index = 0; index < width; ++index) {
        input.edges.push_back({input.source, node_id(0, index), cap_dist(rng)});
    }

    for (int layer = 0; layer + 1 < layers; ++layer) {
        for (int index = 0; index < width; ++index) {
            const int from = node_id(layer, index);
            for (int delta = 0; delta < 3; ++delta) {
                const int to_index = (index + delta) % width;
                input.edges.push_back({from, node_id(layer + 1, to_index), cap_dist(rng)});
            }
        }
    }

    for (int index = 0; index < width; ++index) {
        input.edges.push_back({node_id(layers - 1, index), input.sink, cap_dist(rng)});
    }

    return input;
}

LargeCostFlowGraphInput generate_large_cost_flow_graph(
    int layers,
    int width,
    std::uint32_t seed
) {
    expect(layers >= 2, "cost-flow graph should have at least two internal layers");
    expect(width >= 2, "cost-flow graph width should be at least two");

    LargeCostFlowGraphInput input;
    input.source = 0;
    input.sink = 1 + layers * width;
    input.num_nodes = input.sink + 1;

    std::mt19937 rng(seed);
    std::uniform_int_distribution<long> cap_dist(1, 5);
    std::uniform_int_distribution<long> weight_dist(1, 9);

    auto node_id = [&](int layer, int index) {
        return 1 + layer * width + index;
    };

    for (int index = 0; index < width; ++index) {
        input.edges.push_back({input.source, node_id(0, index), cap_dist(rng), weight_dist(rng)});
    }

    for (int layer = 0; layer + 1 < layers; ++layer) {
        for (int index = 0; index < width; ++index) {
            const int from = node_id(layer, index);
            for (int delta = 0; delta < 3; ++delta) {
                const int to_index = (index + delta) % width;
                input.edges.push_back({from, node_id(layer + 1, to_index), cap_dist(rng), weight_dist(rng)});
            }
        }
    }

    for (int index = 0; index < width; ++index) {
        input.edges.push_back({node_id(layers - 1, index), input.sink, cap_dist(rng), weight_dist(rng)});
    }

    return input;
}

RawUndirectedGraph build_raw_undirected_graph(const LargeUndirectedGraphInput& input) {
    RawUndirectedGraph graph(static_cast<std::size_t>(input.num_nodes));
    for (const auto& [u, v] : input.edges) {
        boost::add_edge(u, v, graph);
    }
    return graph;
}

RawDirectedWeightedGraph build_raw_directed_weighted_graph(const LargeDirectedWeightedGraphInput& input) {
    RawDirectedWeightedGraph graph(static_cast<std::size_t>(input.num_nodes));
    auto weight_map = boost::get(boost::edge_weight, graph);
    for (const auto& [u, v, weight] : input.edges) {
        const auto [edge, inserted] = boost::add_edge(u, v, graph);
        (void)inserted;
        weight_map[edge] = weight;
    }
    return graph;
}

RawDirectedGraph build_raw_directed_graph(const LargeDirectedGraphInput& input) {
    RawDirectedGraph graph(static_cast<std::size_t>(input.num_nodes));
    for (const auto& [u, v] : input.edges) {
        boost::add_edge(u, v, graph);
    }
    return graph;
}

RawNamedDirectedWeightedGraph build_raw_named_directed_weighted_graph(const LargeDirectedWeightedGraphInput& input) {
    RawNamedDirectedWeightedGraph graph(static_cast<std::size_t>(input.num_nodes));
    auto name_map = boost::get(boost::vertex_name, graph);
    for (int node = 0; node < input.num_nodes; ++node) {
        boost::put(name_map, boost::vertex(node, graph), node);
    }

    auto weight_map = boost::get(boost::edge_weight, graph);
    for (const auto& [u, v, weight] : input.edges) {
        const auto [edge, inserted] = boost::add_edge(u, v, graph);
        (void)inserted;
        weight_map[edge] = weight;
    }
    return graph;
}

nxpp::UnweightedGraphInt build_nxpp_undirected_graph(const LargeUndirectedGraphInput& input) {
    nxpp::UnweightedGraphInt graph;
    for (int node = 0; node < input.num_nodes; ++node) {
        graph.add_node(node);
    }
    for (const auto& [u, v] : input.edges) {
        graph.add_edge(u, v);
    }
    return graph;
}

template <typename GraphType>
GraphType build_nxpp_undirected_graph_typed(const LargeUndirectedGraphInput& input) {
    GraphType graph;
    for (int node = 0; node < input.num_nodes; ++node) {
        graph.add_node(node);
    }
    for (const auto& [u, v] : input.edges) {
        graph.add_edge(u, v);
    }
    return graph;
}

nxpp::WeightedDiGraphInt build_nxpp_directed_weighted_graph(const LargeDirectedWeightedGraphInput& input) {
    nxpp::WeightedDiGraphInt graph;
    for (int node = 0; node < input.num_nodes; ++node) {
        graph.add_node(node);
    }
    for (const auto& [u, v, weight] : input.edges) {
        graph.add_edge(u, v, weight);
    }
    return graph;
}

template <typename GraphType>
GraphType build_nxpp_directed_weighted_graph_typed(const LargeDirectedWeightedGraphInput& input) {
    GraphType graph;
    for (int node = 0; node < input.num_nodes; ++node) {
        graph.add_node(node);
    }
    for (const auto& [u, v, weight] : input.edges) {
        graph.add_edge(u, v, weight);
    }
    return graph;
}

nxpp::UnweightedDiGraphInt build_nxpp_directed_graph(const LargeDirectedGraphInput& input) {
    nxpp::UnweightedDiGraphInt graph;
    for (int node = 0; node < input.num_nodes; ++node) {
        graph.add_node(node);
    }
    for (const auto& [u, v] : input.edges) {
        graph.add_edge(u, v);
    }
    return graph;
}

template <typename GraphType>
GraphType build_nxpp_directed_graph_typed(const LargeDirectedGraphInput& input) {
    GraphType graph;
    for (int node = 0; node < input.num_nodes; ++node) {
        graph.add_node(node);
    }
    for (const auto& [u, v] : input.edges) {
        graph.add_edge(u, v);
    }
    return graph;
}

template <typename GraphType>
std::unordered_map<int, typename boost::graph_traits<GraphType>::vertex_descriptor> raw_named_id_map(const GraphType& graph) {
    std::unordered_map<int, typename boost::graph_traits<GraphType>::vertex_descriptor> result;
    const auto name_map = boost::get(boost::vertex_name, graph);
    for (auto [vertex_it, vertex_end] = boost::vertices(graph); vertex_it != vertex_end; ++vertex_it) {
        result.emplace(boost::get(name_map, *vertex_it), *vertex_it);
    }
    return result;
}

template <typename GraphType>
std::vector<int> raw_named_nodes_sorted(const GraphType& graph) {
    std::vector<int> nodes;
    const auto name_map = boost::get(boost::vertex_name, graph);
    nodes.reserve(static_cast<std::size_t>(boost::num_vertices(graph)));
    for (auto [vertex_it, vertex_end] = boost::vertices(graph); vertex_it != vertex_end; ++vertex_it) {
        nodes.push_back(boost::get(name_map, *vertex_it));
    }
    std::sort(nodes.begin(), nodes.end());
    return nodes;
}

template <typename GraphType>
void raw_remove_node_named(GraphType& graph, int node_id) {
    const auto id_map = raw_named_id_map(graph);
    const auto it = id_map.find(node_id);
    if (it == id_map.end()) {
        throw std::runtime_error("Raw named graph node not found");
    }
    boost::clear_vertex(it->second, graph);
    boost::remove_vertex(it->second, graph);
}

std::map<int, int> raw_named_dijkstra_distances(
    const RawNamedDirectedWeightedGraph& graph,
    int source_id
) {
    const auto id_map = raw_named_id_map(graph);
    const auto source_it = id_map.find(source_id);
    if (source_it == id_map.end()) {
        throw std::runtime_error("Raw named graph source not found");
    }

    std::vector<int> distances(
        static_cast<std::size_t>(boost::num_vertices(graph)),
        std::numeric_limits<int>::max()
    );
    boost::dijkstra_shortest_paths(
        graph,
        source_it->second,
        boost::distance_map(
            boost::make_iterator_property_map(distances.begin(), boost::get(boost::vertex_index, graph))
        )
    );

    std::map<int, int> result;
    const auto name_map = boost::get(boost::vertex_name, graph);
    for (auto [vertex_it, vertex_end] = boost::vertices(graph); vertex_it != vertex_end; ++vertex_it) {
        result.emplace(boost::get(name_map, *vertex_it), distances[static_cast<std::size_t>(*vertex_it)]);
    }
    return result;
}

std::vector<std::tuple<int, int, int>> raw_named_weighted_edges_sorted(
    const RawNamedDirectedWeightedGraph& graph
) {
    std::vector<std::tuple<int, int, int>> edges;
    const auto name_map = boost::get(boost::vertex_name, graph);
    const auto weight_map = boost::get(boost::edge_weight, graph);
    edges.reserve(static_cast<std::size_t>(boost::num_edges(graph)));
    for (auto [edge_it, edge_end] = boost::edges(graph); edge_it != edge_end; ++edge_it) {
        edges.emplace_back(
            boost::get(name_map, boost::source(*edge_it, graph)),
            boost::get(name_map, boost::target(*edge_it, graph)),
            boost::get(weight_map, *edge_it)
        );
    }
    std::sort(edges.begin(), edges.end());
    return edges;
}

void raw_named_add_or_assign_weighted_edge(
    RawNamedDirectedWeightedGraph& graph,
    int source_id,
    int target_id,
    int weight
) {
    const auto id_map = raw_named_id_map(graph);
    const auto source_it = id_map.find(source_id);
    const auto target_it = id_map.find(target_id);
    if (source_it == id_map.end() || target_it == id_map.end()) {
        throw std::runtime_error("Raw named graph endpoint node not found");
    }

    const auto existing = boost::edge(source_it->second, target_it->second, graph);
    auto weight_map = boost::get(boost::edge_weight, graph);
    if (existing.second) {
        boost::put(weight_map, existing.first, weight);
        return;
    }

    const auto [edge_desc, inserted] = boost::add_edge(source_it->second, target_it->second, graph);
    (void)inserted;
    boost::put(weight_map, edge_desc, weight);
}

void raw_named_remove_weighted_edges_between(
    RawNamedDirectedWeightedGraph& graph,
    int source_id,
    int target_id
) {
    const auto id_map = raw_named_id_map(graph);
    const auto source_it = id_map.find(source_id);
    const auto target_it = id_map.find(target_id);
    if (source_it == id_map.end() || target_it == id_map.end()) {
        throw std::runtime_error("Raw named graph endpoint node not found");
    }

    std::vector<RawNamedDirectedWeightedGraph::edge_descriptor> to_remove;
    for (auto [edge_it, edge_end] = boost::out_edges(source_it->second, graph); edge_it != edge_end; ++edge_it) {
        if (boost::target(*edge_it, graph) == target_it->second) {
            to_remove.push_back(*edge_it);
        }
    }

    for (const auto edge_desc : to_remove) {
        boost::remove_edge(edge_desc, graph);
    }
}

std::map<int, std::map<int, int>> raw_named_all_pairs_dijkstra_map(
    const RawNamedDirectedWeightedGraph& graph
) {
    std::map<int, std::map<int, int>> result;
    const auto nodes = raw_named_nodes_sorted(graph);
    for (const int source_id : nodes) {
        result.emplace(source_id, raw_named_dijkstra_distances(graph, source_id));
    }
    return result;
}

std::vector<std::vector<int>> raw_named_all_pairs_dijkstra_matrix(
    const RawNamedDirectedWeightedGraph& graph
) {
    const auto nodes = raw_named_nodes_sorted(graph);
    const auto all_pairs = raw_named_all_pairs_dijkstra_map(graph);

    std::vector<std::vector<int>> matrix(
        nodes.size(),
        std::vector<int>(nodes.size(), std::numeric_limits<int>::max())
    );

    for (std::size_t row = 0; row < nodes.size(); ++row) {
        const auto& distance_map = all_pairs.at(nodes[row]);
        for (std::size_t col = 0; col < nodes.size(); ++col) {
            matrix[row][col] = distance_map.at(nodes[col]);
        }
    }

    return matrix;
}

std::vector<std::tuple<std::size_t, int, int, int>> raw_named_multigraph_edges_sorted(
    const RawNamedMultiDiGraph& graph
) {
    std::vector<std::tuple<std::size_t, int, int, int>> edges;
    const auto name_map = boost::get(boost::vertex_name, graph);
    const auto weight_map = boost::get(boost::edge_weight, graph);
    const auto edge_index_map = boost::get(boost::edge_index, graph);
    edges.reserve(static_cast<std::size_t>(boost::num_edges(graph)));
    for (auto [edge_it, edge_end] = boost::edges(graph); edge_it != edge_end; ++edge_it) {
        edges.emplace_back(
            boost::get(edge_index_map, *edge_it),
            boost::get(name_map, boost::source(*edge_it, graph)),
            boost::get(name_map, boost::target(*edge_it, graph)),
            boost::get(weight_map, *edge_it)
        );
    }
    std::sort(edges.begin(), edges.end());
    return edges;
}

template <typename GraphType>
std::vector<int> nxpp_nodes_sorted(const GraphType& graph) {
    auto nodes = graph.nodes();
    std::sort(nodes.begin(), nodes.end());
    return nodes;
}

template <typename GraphType>
std::vector<std::tuple<int, int, int>> nxpp_weighted_edges_sorted(const GraphType& graph) {
    auto edges = graph.edges();
    std::sort(edges.begin(), edges.end());
    return edges;
}

template <typename GraphType>
std::vector<std::tuple<std::size_t, int, int, int>> nxpp_multigraph_edges_sorted(const GraphType& graph) {
    std::vector<std::tuple<std::size_t, int, int, int>> edges;
    const auto ids = graph.edge_ids();
    edges.reserve(ids.size());
    for (const auto edge_id : ids) {
        const auto [u, v] = graph.get_edge_endpoints(edge_id);
        edges.emplace_back(edge_id, u, v, graph.get_edge_weight(edge_id));
    }
    std::sort(edges.begin(), edges.end());
    return edges;
}

std::optional<RawNamedMultiDiGraph::edge_descriptor> raw_named_multigraph_find_edge_by_id(
    const RawNamedMultiDiGraph& graph,
    std::size_t edge_id
) {
    const auto edge_index_map = boost::get(boost::edge_index, graph);
    for (auto [edge_it, edge_end] = boost::edges(graph); edge_it != edge_end; ++edge_it) {
        if (boost::get(edge_index_map, *edge_it) == edge_id) {
            return *edge_it;
        }
    }
    return std::nullopt;
}

void raw_named_multigraph_remove_edge_by_id(RawNamedMultiDiGraph& graph, std::size_t edge_id) {
    const auto edge_desc = raw_named_multigraph_find_edge_by_id(graph, edge_id);
    if (!edge_desc.has_value()) {
        throw std::runtime_error("Raw named multigraph edge id not found");
    }
    boost::remove_edge(*edge_desc, graph);
}

void raw_named_multigraph_remove_edges_between(RawNamedMultiDiGraph& graph, int source_id, int target_id) {
    const auto id_map = raw_named_id_map(graph);
    const auto source_it = id_map.find(source_id);
    const auto target_it = id_map.find(target_id);
    if (source_it == id_map.end() || target_it == id_map.end()) {
        throw std::runtime_error("Raw named multigraph endpoint node not found");
    }

    std::vector<RawNamedMultiDiGraph::edge_descriptor> to_remove;
    for (auto [edge_it, edge_end] = boost::out_edges(source_it->second, graph); edge_it != edge_end; ++edge_it) {
        if (boost::target(*edge_it, graph) == target_it->second) {
            to_remove.push_back(*edge_it);
        }
    }

    for (const auto edge_desc : to_remove) {
        boost::remove_edge(edge_desc, graph);
    }
}

struct BuiltRawCapacityFlowGraph {
    RawCapacityFlowGraph graph;
    typename boost::property_map<RawCapacityFlowGraph, boost::edge_capacity_t>::type capacity;
    typename boost::property_map<RawCapacityFlowGraph, boost::edge_residual_capacity_t>::type residual;
    typename boost::property_map<RawCapacityFlowGraph, boost::edge_reverse_t>::type reverse;

    BuiltRawCapacityFlowGraph()
        : graph(),
          capacity(boost::get(boost::edge_capacity, graph)),
          residual(boost::get(boost::edge_residual_capacity, graph)),
          reverse(boost::get(boost::edge_reverse, graph)) {}
};

struct BuiltRawCostFlowGraph {
    RawCostFlowGraph graph;
    typename boost::property_map<RawCostFlowGraph, boost::edge_weight_t>::type weight;
    typename boost::property_map<RawCostFlowGraph, boost::edge_capacity_t>::type capacity;
    typename boost::property_map<RawCostFlowGraph, boost::edge_residual_capacity_t>::type residual;
    typename boost::property_map<RawCostFlowGraph, boost::edge_reverse_t>::type reverse;

    BuiltRawCostFlowGraph()
        : graph(),
          weight(boost::get(boost::edge_weight, graph)),
          capacity(boost::get(boost::edge_capacity, graph)),
          residual(boost::get(boost::edge_residual_capacity, graph)),
          reverse(boost::get(boost::edge_reverse, graph)) {}
};

BuiltRawCapacityFlowGraph build_raw_capacity_flow_graph(const LargeCapacityGraphInput& input) {
    BuiltRawCapacityFlowGraph built;
    for (int node = 0; node < input.num_nodes; ++node) {
        boost::add_vertex(built.graph);
    }

    for (const auto& edge : input.edges) {
        const auto [forward, added] = boost::add_edge(edge.source, edge.target, built.graph);
        const auto [backward, rev_added] = boost::add_edge(edge.target, edge.source, built.graph);
        (void)added;
        (void)rev_added;
        built.capacity[forward] = edge.capacity;
        built.capacity[backward] = 0;
        built.reverse[forward] = backward;
        built.reverse[backward] = forward;
    }

    return built;
}

BuiltRawCostFlowGraph build_raw_cost_flow_graph(const LargeCostFlowGraphInput& input) {
    BuiltRawCostFlowGraph built;
    for (int node = 0; node < input.num_nodes; ++node) {
        boost::add_vertex(built.graph);
    }

    for (const auto& edge : input.edges) {
        const auto [forward, added] = boost::add_edge(edge.source, edge.target, built.graph);
        const auto [backward, rev_added] = boost::add_edge(edge.target, edge.source, built.graph);
        (void)added;
        (void)rev_added;
        built.weight[forward] = edge.weight;
        built.weight[backward] = -edge.weight;
        built.capacity[forward] = edge.capacity;
        built.capacity[backward] = 0;
        built.reverse[forward] = backward;
        built.reverse[backward] = forward;
    }

    return built;
}

nxpp::UnweightedDiGraphInt build_nxpp_capacity_graph(const LargeCapacityGraphInput& input) {
    nxpp::UnweightedDiGraphInt graph;
    for (int node = 0; node < input.num_nodes; ++node) {
        graph.add_node(node);
    }
    for (const auto& edge : input.edges) {
        graph.add_edge(edge.source, edge.target, {{"capacity", edge.capacity}});
    }
    return graph;
}

nxpp::WeightedDiGraphInt build_nxpp_cost_flow_graph(const LargeCostFlowGraphInput& input) {
    nxpp::WeightedDiGraphInt graph;
    for (int node = 0; node < input.num_nodes; ++node) {
        graph.add_node(node);
    }
    for (const auto& edge : input.edges) {
        graph.add_edge(edge.source, edge.target, static_cast<int>(edge.weight), {{"capacity", edge.capacity}});
    }
    return graph;
}

std::vector<int> raw_flow_reachable_nodes_after_max_flow(
    const BuiltRawCapacityFlowGraph& built,
    int source
) {
    std::vector<int> reachable;
    std::vector<bool> visited(static_cast<std::size_t>(boost::num_vertices(built.graph)), false);
    std::queue<int> pending;
    visited[static_cast<std::size_t>(source)] = true;
    pending.push(source);

    while (!pending.empty()) {
        const int current = pending.front();
        pending.pop();
        reachable.push_back(current);
        for (auto [edge_it, edge_end] = boost::out_edges(current, built.graph); edge_it != edge_end; ++edge_it) {
            const int next = static_cast<int>(boost::target(*edge_it, built.graph));
            if (!visited[static_cast<std::size_t>(next)] && built.residual[*edge_it] > 0) {
                visited[static_cast<std::size_t>(next)] = true;
                pending.push(next);
            }
        }
    }

    std::sort(reachable.begin(), reachable.end());
    return reachable;
}

struct RawBfsTreeEdgeCollector : boost::default_bfs_visitor {
    const RawUndirectedGraph* graph = nullptr;
    std::vector<std::pair<int, int>>* tree_edges = nullptr;

    RawBfsTreeEdgeCollector(
        const RawUndirectedGraph& graph_ref,
        std::vector<std::pair<int, int>>& tree_edge_ref
    )
        : graph(&graph_ref), tree_edges(&tree_edge_ref) {}

    template <typename Edge, typename Graph>
    void tree_edge(Edge edge, const Graph&) const {
        tree_edges->emplace_back(
            static_cast<int>(boost::source(edge, *graph)),
            static_cast<int>(boost::target(edge, *graph))
        );
    }
};

struct RawDfsTreeEdgeCollector : boost::default_dfs_visitor {
    const RawUndirectedGraph* graph = nullptr;
    std::vector<std::pair<int, int>>* tree_edges = nullptr;

    RawDfsTreeEdgeCollector(
        const RawUndirectedGraph& graph_ref,
        std::vector<std::pair<int, int>>& tree_edge_ref
    )
        : graph(&graph_ref), tree_edges(&tree_edge_ref) {}

    template <typename Edge, typename Graph>
    void tree_edge(Edge edge, const Graph&) const {
        tree_edges->emplace_back(
            static_cast<int>(boost::source(edge, *graph)),
            static_cast<int>(boost::target(edge, *graph))
        );
    }
};

std::vector<int> bfs_tree_edges_to_depths(
    int num_nodes,
    int start,
    const std::vector<std::pair<int, int>>& tree_edges
) {
    std::vector<int> depth(static_cast<std::size_t>(num_nodes), -1);
    depth[static_cast<std::size_t>(start)] = 0;

    for (const auto& [u, v] : tree_edges) {
        expect(depth[static_cast<std::size_t>(u)] >= 0,
               "BFS tree edge encountered before its parent was discovered");
        depth[static_cast<std::size_t>(v)] = depth[static_cast<std::size_t>(u)] + 1;
    }

    return depth;
}

std::vector<int> normalized_component_labels(const std::vector<int>& labels) {
    std::unordered_map<int, std::vector<int>> groups;
    groups.reserve(labels.size());

    for (std::size_t node = 0; node < labels.size(); ++node) {
        groups[labels[node]].push_back(static_cast<int>(node));
    }

    std::vector<std::vector<int>> ordered_groups;
    ordered_groups.reserve(groups.size());
    for (auto& [label, group] : groups) {
        (void)label;
        std::sort(group.begin(), group.end());
        ordered_groups.push_back(group);
    }

    std::sort(ordered_groups.begin(), ordered_groups.end());

    std::vector<int> normalized(labels.size(), -1);
    for (std::size_t component_index = 0; component_index < ordered_groups.size(); ++component_index) {
        for (const int node : ordered_groups[component_index]) {
            normalized[static_cast<std::size_t>(node)] = static_cast<int>(component_index);
        }
    }

    return normalized;
}

template <typename DistanceMap>
std::vector<int> extract_distance_vector(
    const DistanceMap& distance_map,
    int num_nodes
) {
    std::vector<int> distances(static_cast<std::size_t>(num_nodes));
    for (int node = 0; node < num_nodes; ++node) {
        distances[static_cast<std::size_t>(node)] = distance_map.at(node);
    }
    return distances;
}

std::vector<int> raw_dag_shortest_path_distances(
    const RawDirectedWeightedGraph& graph,
    int source_node,
    int num_nodes
) {
    std::vector<int> distances(static_cast<std::size_t>(num_nodes), std::numeric_limits<int>::max());
    std::vector<RawDirectedWeightedGraph::vertex_descriptor> predecessors(static_cast<std::size_t>(num_nodes));

    for (auto [vertex_it, vertex_end] = boost::vertices(graph); vertex_it != vertex_end; ++vertex_it) {
        predecessors[static_cast<std::size_t>(*vertex_it)] = *vertex_it;
    }

    boost::dag_shortest_paths(
        graph,
        boost::vertex(source_node, graph),
        boost::distance_map(
            boost::make_iterator_property_map(distances.begin(), boost::get(boost::vertex_index, graph))
        ).predecessor_map(
            boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index, graph))
        ).distance_inf(std::numeric_limits<int>::max()).distance_zero(0)
    );

    return distances;
}

const std::vector<std::uint32_t>& large_graph_regression_seeds() {
    static const std::vector<std::uint32_t> seeds = {1337u, 2026u, 42424u};
    return seeds;
}

void test_large_bfs_matches_raw_boost() {
    constexpr int start_node = 0;
    for (const auto seed : large_graph_regression_seeds()) {
        const auto input = generate_large_partitioned_undirected_graph(
            {1500, 1200, 900},
            {4500, 3600, 2700},
            seed
        );

        const auto nxpp_graph = build_nxpp_undirected_graph(input);
        const auto raw_graph = build_raw_undirected_graph(input);

        const auto nxpp_tree_edges = nxpp_graph.bfs_edges(start_node);
        const auto nxpp_depth = bfs_tree_edges_to_depths(input.num_nodes, start_node, nxpp_tree_edges);

        std::vector<std::pair<int, int>> raw_tree_edges;
        std::vector<boost::default_color_type> color(static_cast<std::size_t>(input.num_nodes));
        RawBfsTreeEdgeCollector visitor(raw_graph, raw_tree_edges);
        boost::breadth_first_search(
            raw_graph,
            boost::vertex(start_node, raw_graph),
            boost::visitor(visitor).color_map(
                boost::make_iterator_property_map(color.begin(), boost::get(boost::vertex_index, raw_graph))
            )
        );
        const auto raw_depth = bfs_tree_edges_to_depths(input.num_nodes, start_node, raw_tree_edges);

        expect(nxpp_depth == raw_depth, "large-graph BFS depth map should match raw Boost across regression seeds");
    }
}

void test_large_connected_components_match_raw_boost() {
    for (const auto seed : large_graph_regression_seeds()) {
        const auto input = generate_large_partitioned_undirected_graph(
            {1500, 1200, 900, 1},
            {4500, 3600, 2700, 0},
            seed
        );

        const auto nxpp_graph = build_nxpp_undirected_graph(input);
        const auto raw_graph = build_raw_undirected_graph(input);

        const auto nxpp_component_map = nxpp_graph.connected_components();
        std::vector<int> nxpp_labels(static_cast<std::size_t>(input.num_nodes));
        for (int node = 0; node < input.num_nodes; ++node) {
            nxpp_labels[static_cast<std::size_t>(node)] = nxpp_component_map.at(node);
        }

        std::vector<int> raw_labels(static_cast<std::size_t>(input.num_nodes), -1);
        boost::connected_components(
            raw_graph,
            boost::make_iterator_property_map(raw_labels.begin(), boost::get(boost::vertex_index, raw_graph))
        );

        expect(
            normalized_component_labels(nxpp_labels) == normalized_component_labels(raw_labels),
            "large-graph connected-component partition should match raw Boost across regression seeds"
        );
    }
}

void test_large_dfs_tree_matches_raw_boost() {
    constexpr int start_node = 0;
    const auto input = generate_large_partitioned_undirected_graph(
        {2600},
        {9000},
        4242u
    );

    const auto nxpp_graph = build_nxpp_undirected_graph(input);
    const auto raw_graph = build_raw_undirected_graph(input);

    const auto nxpp_tree_edges = nxpp_graph.dfs_edges(start_node);

    std::vector<std::pair<int, int>> raw_tree_edges;
    std::vector<boost::default_color_type> color(static_cast<std::size_t>(input.num_nodes));
    RawDfsTreeEdgeCollector visitor(raw_graph, raw_tree_edges);
    boost::depth_first_search(
        raw_graph,
        boost::root_vertex(boost::vertex(start_node, raw_graph))
            .visitor(visitor)
            .color_map(boost::make_iterator_property_map(color.begin(), boost::get(boost::vertex_index, raw_graph)))
    );

    expect(nxpp_tree_edges == raw_tree_edges, "large-graph DFS tree edges should match raw Boost");
}

void test_large_strongly_connected_components_match_raw_boost() {
    for (const auto seed : large_graph_regression_seeds()) {
        const auto input = generate_large_scc_digraph(
            {900, 700, 500, 1},
            {3500, 2500, 1800, 0},
            200,
            seed
        );

        const auto nxpp_graph = build_nxpp_directed_graph(input);
        const auto raw_graph = build_raw_directed_graph(input);

        const auto nxpp_component_map = nxpp_graph.strongly_connected_component_map();
        std::vector<int> nxpp_labels(static_cast<std::size_t>(input.num_nodes));
        for (int node = 0; node < input.num_nodes; ++node) {
            nxpp_labels[static_cast<std::size_t>(node)] = nxpp_component_map.at(node);
        }

        std::vector<int> raw_labels(static_cast<std::size_t>(input.num_nodes), -1);
        boost::strong_components(
            raw_graph,
            boost::make_iterator_property_map(raw_labels.begin(), boost::get(boost::vertex_index, raw_graph))
        );

        expect(
            normalized_component_labels(nxpp_labels) == normalized_component_labels(raw_labels),
            "large-graph strongly-connected-component partition should match raw Boost across regression seeds"
        );
    }
}

void test_large_dijkstra_distances_match_raw_boost() {
    constexpr int source_node = 0;
    for (const auto seed : large_graph_regression_seeds()) {
        const auto input = generate_component_weighted_digraph(
            {2800, 399, 1},
            {12000, 1200, 0},
            seed,
            1,
            20,
            false
        );

        const auto nxpp_graph = build_nxpp_directed_weighted_graph(input);
        const auto raw_graph = build_raw_directed_weighted_graph(input);

        const auto nxpp_result = nxpp_graph.dijkstra_shortest_paths(source_node);
        std::vector<int> nxpp_distances(static_cast<std::size_t>(input.num_nodes));
        for (int node = 0; node < input.num_nodes; ++node) {
            nxpp_distances[static_cast<std::size_t>(node)] = nxpp_result.distance.at(node);
        }

        std::vector<int> raw_distances(
            static_cast<std::size_t>(input.num_nodes),
            std::numeric_limits<int>::max()
        );
        boost::dijkstra_shortest_paths(
            raw_graph,
            boost::vertex(source_node, raw_graph),
            boost::distance_map(
                boost::make_iterator_property_map(raw_distances.begin(), boost::get(boost::vertex_index, raw_graph))
            )
        );

        expect(nxpp_distances == raw_distances,
               "large-graph Dijkstra distances should match raw Boost across regression seeds");
    }
}

void test_large_bellman_ford_distances_match_raw_boost() {
    constexpr int source_node = 0;
    const auto input = generate_component_weighted_digraph(
        {1800, 299, 1},
        {7500, 900, 0},
        5050u,
        -5,
        15,
        true
    );

    const auto nxpp_graph = build_nxpp_directed_weighted_graph(input);
    const auto raw_graph = build_raw_directed_weighted_graph(input);

    const auto nxpp_result = nxpp_graph.bellman_ford_shortest_paths(source_node);
    const auto nxpp_distances = extract_distance_vector(nxpp_result.distance, input.num_nodes);

    std::vector<int> raw_distances(static_cast<std::size_t>(input.num_nodes), std::numeric_limits<int>::max());
    std::vector<RawDirectedWeightedGraph::vertex_descriptor> raw_predecessors(static_cast<std::size_t>(input.num_nodes));
    for (auto [vertex_it, vertex_end] = boost::vertices(raw_graph); vertex_it != vertex_end; ++vertex_it) {
        raw_predecessors[static_cast<std::size_t>(*vertex_it)] = *vertex_it;
    }
    raw_distances[static_cast<std::size_t>(source_node)] = 0;

    const bool ok = boost::bellman_ford_shortest_paths(
        raw_graph,
        input.num_nodes,
        boost::weight_map(boost::get(boost::edge_weight, raw_graph))
            .distance_map(boost::make_iterator_property_map(raw_distances.begin(), boost::get(boost::vertex_index, raw_graph)))
            .predecessor_map(boost::make_iterator_property_map(raw_predecessors.begin(), boost::get(boost::vertex_index, raw_graph)))
            .root_vertex(boost::vertex(source_node, raw_graph))
    );

    expect(ok, "raw Boost Bellman-Ford should succeed on the acyclic signed graph");
    expect(nxpp_distances == raw_distances,
           "large-graph Bellman-Ford distances should match raw Boost");
}

void test_large_dag_shortest_paths_match_raw_boost() {
    constexpr int source_node = 0;
    const auto input = generate_component_weighted_digraph(
        {1800, 299, 1},
        {7500, 900, 0},
        5050u,
        -5,
        15,
        true
    );

    const auto nxpp_graph = build_nxpp_directed_weighted_graph(input);
    const auto raw_graph = build_raw_directed_weighted_graph(input);

    const auto nxpp_result = nxpp_graph.dag_shortest_paths(source_node);
    const auto nxpp_distances = extract_distance_vector(nxpp_result.distance, input.num_nodes);
    const auto raw_distances = raw_dag_shortest_path_distances(raw_graph, source_node, input.num_nodes);

    expect(nxpp_distances == raw_distances,
           "large-graph DAG shortest-path distances should match raw Boost");
}

void test_negative_cycle_detection_matches_raw_boost() {
    constexpr int source_node = 0;
    const auto input = generate_reachable_negative_cycle_graph(1200);

    const auto nxpp_graph = build_nxpp_directed_weighted_graph(input);
    const auto raw_graph = build_raw_directed_weighted_graph(input);

    std::vector<int> raw_distances(static_cast<std::size_t>(input.num_nodes), std::numeric_limits<int>::max());
    std::vector<RawDirectedWeightedGraph::vertex_descriptor> raw_predecessors(static_cast<std::size_t>(input.num_nodes));
    for (auto [vertex_it, vertex_end] = boost::vertices(raw_graph); vertex_it != vertex_end; ++vertex_it) {
        raw_predecessors[static_cast<std::size_t>(*vertex_it)] = *vertex_it;
    }
    raw_distances[static_cast<std::size_t>(source_node)] = 0;

    const bool raw_ok = boost::bellman_ford_shortest_paths(
        raw_graph,
        input.num_nodes,
        boost::weight_map(boost::get(boost::edge_weight, raw_graph))
            .distance_map(boost::make_iterator_property_map(raw_distances.begin(), boost::get(boost::vertex_index, raw_graph)))
            .predecessor_map(boost::make_iterator_property_map(raw_predecessors.begin(), boost::get(boost::vertex_index, raw_graph)))
            .root_vertex(boost::vertex(source_node, raw_graph))
    );

    bool nxpp_threw = false;
    try {
        (void)nxpp_graph.bellman_ford_shortest_paths(source_node);
    } catch (const std::runtime_error&) {
        nxpp_threw = true;
    }

    expect(!raw_ok, "raw Boost Bellman-Ford should report the reachable negative cycle");
    expect(nxpp_threw, "nxpp Bellman-Ford should throw on the reachable negative cycle");
}

void test_large_remove_node_state_stays_aligned_with_raw_boost() {
    for (const auto seed : large_graph_regression_seeds()) {
        const auto input = generate_component_weighted_digraph(
            {2000, 400, 1},
            {8000, 1200, 0},
            seed,
            1,
            20,
            false
        );

        auto nxpp_graph = build_nxpp_directed_weighted_graph(input);
        auto raw_graph = build_raw_named_directed_weighted_graph(input);

        nxpp_graph.node(400)["role"] = "remove-me";
        nxpp_graph.node(2400)["role"] = "isolated";
        nxpp_graph[399][400]["kind"] = "incident";
        nxpp_graph[1200][1201]["kind"] = "survivor";

        for (const int node_id : {400, 2399, 2400}) {
            nxpp_graph.remove_node(node_id);
            raw_remove_node_named(raw_graph, node_id);
        }

        expect(nxpp_nodes_sorted(nxpp_graph) == raw_named_nodes_sorted(raw_graph),
               "remaining node ids after large remove_node should match raw Boost across regression seeds");
        expect(nxpp_weighted_edges_sorted(nxpp_graph) == raw_named_weighted_edges_sorted(raw_graph),
               "remaining weighted edge set after large remove_node should match raw Boost across regression seeds");
        expect(nxpp_graph.dijkstra_shortest_paths(0).distance == raw_named_dijkstra_distances(raw_graph, 0),
               "Dijkstra distances after large remove_node should match raw Boost across regression seeds");

        expect(!nxpp_graph.has_node_attr(400, "role"),
               "removed large-graph node should lose its stored attributes");
        expect(!nxpp_graph.has_node(2400),
               "removed isolated node should disappear from the graph");
        expect(!nxpp_graph.has_edge_attr(399, 400, "kind"),
               "incident edge attributes should be cleaned after large remove_node");
        expect(nxpp_graph.has_edge_attr(1200, 1201, "kind"),
               "non-incident edge attributes should survive large remove_node");
    }
}

void test_large_custom_selector_dijkstra_and_remove_node_match_raw_boost() {
    const auto input = generate_component_weighted_digraph(
        {1200, 250, 1},
        {4800, 700, 0},
        8080u,
        1,
        20,
        false
    );

    auto nxpp_graph = build_nxpp_directed_weighted_graph_typed<NxppWeightedDiGraphIntListSelectors>(input);
    auto raw_graph = build_raw_named_directed_weighted_graph(input);

    nxpp_graph.node(200)["role"] = "remove-me";
    nxpp_graph[199][200]["kind"] = "incident";
    nxpp_graph[700][701]["kind"] = "survivor";

    for (const int node_id : {200, 1449, 1450}) {
        nxpp_graph.remove_node(node_id);
        raw_remove_node_named(raw_graph, node_id);
    }

    expect(nxpp_nodes_sorted(nxpp_graph) == raw_named_nodes_sorted(raw_graph),
           "custom-selector graph nodes should stay aligned with raw Boost after remove_node");
    expect(nxpp_weighted_edges_sorted(nxpp_graph) == raw_named_weighted_edges_sorted(raw_graph),
           "custom-selector graph edges should stay aligned with raw Boost after remove_node");
    expect(nxpp_graph.dijkstra_shortest_paths(0).distance == raw_named_dijkstra_distances(raw_graph, 0),
           "custom-selector graph Dijkstra distances should match raw Boost after remove_node");
    expect(!nxpp_graph.has_node_attr(200, "role"),
           "custom-selector remove_node should clear removed node attributes");
    expect(!nxpp_graph.has_edge_attr(199, 200, "kind"),
           "custom-selector remove_node should clear incident edge attributes");
    expect(nxpp_graph.has_edge_attr(700, 701, "kind"),
           "custom-selector remove_node should preserve non-incident edge attributes");
}

void test_large_custom_outedge_selector_dijkstra_and_remove_node_match_raw_boost() {
    const auto input = generate_component_weighted_digraph(
        {1300, 280, 1},
        {5200, 780, 0},
        9090u,
        1,
        20,
        false
    );

    auto nxpp_graph =
        build_nxpp_directed_weighted_graph_typed<NxppWeightedDiGraphIntListOutVecVertexSelectors>(input);
    auto raw_graph = build_raw_named_directed_weighted_graph(input);

    nxpp_graph.node(260)["role"] = "remove-me";
    nxpp_graph[259][260]["kind"] = "incident";
    nxpp_graph[900][901]["kind"] = "survivor";

    for (const int node_id : {260, 1579, 1580}) {
        nxpp_graph.remove_node(node_id);
        raw_remove_node_named(raw_graph, node_id);
    }

    expect(nxpp_nodes_sorted(nxpp_graph) == raw_named_nodes_sorted(raw_graph),
           "custom outedge-selector graph nodes should stay aligned with raw Boost after remove_node");
    expect(nxpp_weighted_edges_sorted(nxpp_graph) == raw_named_weighted_edges_sorted(raw_graph),
           "custom outedge-selector graph edges should stay aligned with raw Boost after remove_node");
    expect(nxpp_graph.dijkstra_shortest_paths(0).distance == raw_named_dijkstra_distances(raw_graph, 0),
           "custom outedge-selector graph Dijkstra distances should match raw Boost after remove_node");
    expect(!nxpp_graph.has_node_attr(260, "role"),
           "custom outedge-selector remove_node should clear removed node attributes");
    expect(!nxpp_graph.has_edge_attr(259, 260, "kind"),
           "custom outedge-selector remove_node should clear incident edge attributes");
    expect(nxpp_graph.has_edge_attr(900, 901, "kind"),
           "custom outedge-selector remove_node should preserve non-incident edge attributes");
}

void test_large_multigraph_mutations_stay_aligned_with_raw_boost() {
    using NxMultiDiGraphInt = nxpp::Graph<int, int, true, true, true>;

    constexpr int num_nodes = 180;
    NxMultiDiGraphInt nxpp_graph;
    RawNamedMultiDiGraph raw_graph(static_cast<std::size_t>(num_nodes));
    auto raw_name_map = boost::get(boost::vertex_name, raw_graph);
    auto raw_weight_map = boost::get(boost::edge_weight, raw_graph);
    auto raw_edge_index_map = boost::get(boost::edge_index, raw_graph);

    for (int node = 0; node < num_nodes; ++node) {
        boost::put(raw_name_map, boost::vertex(node, raw_graph), node);
        nxpp_graph.add_node(node);
    }

    auto add_parallel_bundle = [&](int source, int target, int base_weight) {
        std::vector<std::size_t> ids;
        ids.reserve(4);
        for (int offset = 0; offset < 4; ++offset) {
            const int weight = base_weight + offset;
            const std::size_t edge_id = nxpp_graph.add_edge_with_id(source, target, weight);
            const auto [edge_desc, inserted] = boost::add_edge(source, target, raw_graph);
            (void)inserted;
            boost::put(raw_weight_map, edge_desc, weight);
            boost::put(raw_edge_index_map, edge_desc, edge_id);
            ids.push_back(edge_id);
        }
        return ids;
    };

    std::vector<std::size_t> removed_by_id;
    std::size_t survivor_with_attr = 0;
    for (int node = 0; node + 1 < num_nodes; ++node) {
        const auto ids = add_parallel_bundle(node, node + 1, 10 + node * 5);
        if (node % 11 == 0) {
            nxpp_graph.set_edge_attr(ids.front(), "label", "tracked");
        }
        if (node == 30) {
            removed_by_id.push_back(ids[1]);
        }
        if (node == 75) {
            removed_by_id.push_back(ids[2]);
        }
        if (node == 110) {
            survivor_with_attr = ids[0];
            nxpp_graph.set_edge_attr(survivor_with_attr, "label", "survivor");
        }
    }

    for (const auto edge_id : removed_by_id) {
        nxpp_graph.remove_edge(edge_id);
        raw_named_multigraph_remove_edge_by_id(raw_graph, edge_id);
    }

    nxpp_graph.remove_edge(40, 41);
    raw_named_multigraph_remove_edges_between(raw_graph, 40, 41);

    nxpp_graph.remove_node(90);
    raw_remove_node_named(raw_graph, 90);

    expect(nxpp_nodes_sorted(nxpp_graph) == raw_named_nodes_sorted(raw_graph),
           "remaining multigraph node ids should match raw Boost after mutations");
    expect(nxpp_multigraph_edges_sorted(nxpp_graph) == raw_named_multigraph_edges_sorted(raw_graph),
           "remaining multigraph edge summary should match raw Boost after mutations");

    for (const auto edge_id : removed_by_id) {
        expect(!nxpp_graph.has_edge_id(edge_id), "removed edge_id should disappear from the large multigraph");
        expect(!nxpp_graph.has_edge_attr(edge_id, "label"),
               "removed edge_id attributes should be cleaned in the large multigraph");
    }

    expect(nxpp_graph.edge_ids(40, 41).empty(),
           "remove_edge(u, v) should clear the whole parallel bundle in the large multigraph");
    expect(!nxpp_graph.has_edge(89, 90) && !nxpp_graph.has_edge(90, 91),
           "remove_node() should clear incident parallel bundles in the large multigraph");
    expect(nxpp_graph.has_edge_id(survivor_with_attr),
           "a non-incident tracked edge should survive the large multigraph mutations");
    expect(nxpp_graph.get_edge_attr<std::string>(survivor_with_attr, "label") == "survivor",
           "surviving large multigraph edge should keep its tracked attributes");
}

void test_large_attribute_state_survives_repeated_mutations() {
    auto graph = build_nxpp_directed_weighted_graph(
        generate_component_weighted_digraph(
            {1400, 300, 1},
            {6000, 800, 0},
            6060u,
            1,
            12,
            false
        )
    );

    for (int node = 0; node < 1701; ++node) {
        if (node % 17 == 0) {
            graph.node(node)["group"] = "tracked";
        }
    }

    for (int node = 0; node + 1 < 1701; ++node) {
        if (node % 23 == 0 && graph.has_edge(node, node + 1)) {
            graph[node][node + 1]["kind"] = "tracked-edge";
        }
    }

    for (const int node_id : {17, 34, 68, 250, 900, 1699, 1700}) {
        if (graph.has_node(node_id)) {
            graph.remove_node(node_id);
        }
    }

    expect(graph.has_node_attr(0, "group"), "surviving tracked node attribute should remain available");
    expect(graph.get_node_attr<std::string>(0, "group") == "tracked",
           "surviving tracked node attribute should keep its value");
    expect(!graph.has_node_attr(34, "group"), "removed tracked node should lose its attribute state");
    expect(!graph.has_node(1700), "removed isolated node should be absent after repeated mutations");

    expect(graph.has_edge_attr(23, 24, "kind"),
           "surviving tracked edge attribute should remain available");
    expect(graph.get_edge_attr<std::string>(23, 24, "kind") == "tracked-edge",
           "surviving tracked edge attribute should keep its value");
    expect(!graph.has_edge_attr(68, 69, "kind"),
           "edge attributes incident to a removed node should be cleaned");

    const auto dijkstra = graph.dijkstra_shortest_paths(0);
    expect(dijkstra.distance.at(0) == 0, "distance to the source should remain zero after repeated mutations");
    expect(dijkstra.distance.count(1700) == 0,
           "removed node should not appear in shortest-path results after repeated mutations");
}

void test_large_combined_weighted_mutation_sequence_matches_raw_boost() {
    const auto input = generate_component_weighted_digraph(
        {950, 250, 1},
        {4200, 900, 0},
        9191u,
        1,
        18,
        false
    );

    auto nxpp_graph = build_nxpp_directed_weighted_graph(input);
    auto raw_graph = build_raw_named_directed_weighted_graph(input);

    nxpp_graph.node(300)["role"] = "remove-me";
    nxpp_graph.node(1200)["role"] = "isolated";
    nxpp_graph[50][51]["tag"] = "old";
    nxpp_graph[299][300]["incident"] = "drop";
    nxpp_graph[20][21]["stable"] = "keep";

    nxpp_graph.remove_edge(50, 51);
    raw_named_remove_weighted_edges_between(raw_graph, 50, 51);
    expect(!nxpp_graph.has_edge_attr(50, 51, "tag"),
           "remove_edge(u, v) should clear tracked edge attributes before the edge is re-added");

    nxpp_graph.add_edge(50, 51, 7);
    raw_named_add_or_assign_weighted_edge(raw_graph, 50, 51, 7);
    nxpp_graph[50][51]["tag"] = "new";

    nxpp_graph.add_edge(20, 700, 1);
    raw_named_add_or_assign_weighted_edge(raw_graph, 20, 700, 1);
    nxpp_graph[20][700]["bridge"] = "fresh";

    nxpp_graph.add_edge(700, 850, 2);
    raw_named_add_or_assign_weighted_edge(raw_graph, 700, 850, 2);

    nxpp_graph.remove_node(300);
    raw_remove_node_named(raw_graph, 300);

    nxpp_graph.remove_node(1200);
    raw_remove_node_named(raw_graph, 1200);

    nxpp_graph.remove_edge(20, 700);
    raw_named_remove_weighted_edges_between(raw_graph, 20, 700);
    expect(!nxpp_graph.has_edge_attr(20, 700, "bridge"),
           "removing a newly added bridge edge should clear its tracked attributes");

    nxpp_graph.add_edge(20, 700, 4);
    raw_named_add_or_assign_weighted_edge(raw_graph, 20, 700, 4);
    nxpp_graph[20][700]["bridge"] = "readded";

    expect(nxpp_nodes_sorted(nxpp_graph) == raw_named_nodes_sorted(raw_graph),
           "combined weighted mutation sequence should preserve the same node ids as raw Boost");
    expect(nxpp_weighted_edges_sorted(nxpp_graph) == raw_named_weighted_edges_sorted(raw_graph),
           "combined weighted mutation sequence should preserve the same edge set as raw Boost");
    expect(nxpp_graph.dijkstra_shortest_paths(0).distance == raw_named_dijkstra_distances(raw_graph, 0),
           "combined weighted mutation sequence should preserve Dijkstra results against raw Boost");

    expect(!nxpp_graph.has_node_attr(300, "role"),
           "removed node should lose its tracked attributes in the combined mutation sequence");
    expect(!nxpp_graph.has_node(1200),
           "removed isolated node should stay absent in the combined mutation sequence");
    expect(!nxpp_graph.has_edge_attr(299, 300, "incident"),
           "incident edge attributes should be cleaned when a touched node is removed");
    expect(nxpp_graph.has_edge_attr(20, 21, "stable"),
           "unrelated surviving edge attributes should remain after the combined mutation sequence");
    expect(nxpp_graph.get_edge_attr<std::string>(50, 51, "tag") == "new",
           "re-added edge should expose only the new tracked attribute value");
    expect(nxpp_graph.get_edge_attr<std::string>(20, 700, "bridge") == "readded",
           "re-added bridge edge should keep the latest tracked attribute value");
}

void test_large_floyd_warshall_all_pairs_matches_raw_boost() {
    const auto input = generate_component_weighted_digraph(
        {180, 70, 1},
        {1400, 300, 0},
        314159u,
        1,
        16,
        false
    );

    const auto nxpp_graph = build_nxpp_directed_weighted_graph(input);
    const auto raw_graph = build_raw_named_directed_weighted_graph(input);

    const auto nxpp_matrix = nxpp_graph.floyd_warshall_all_pairs_shortest_paths();
    const auto nxpp_map = nxpp_graph.floyd_warshall_all_pairs_shortest_paths_map();
    const auto raw_matrix = raw_named_all_pairs_dijkstra_matrix(raw_graph);
    const auto raw_map = raw_named_all_pairs_dijkstra_map(raw_graph);

    expect(nxpp_matrix == raw_matrix,
           "large Floyd-Warshall all-pairs matrix should match the raw Boost all-pairs baseline");
    expect(nxpp_map == raw_map,
           "large Floyd-Warshall all-pairs map should match the raw Boost all-pairs baseline");

    expect(nxpp_matrix.front().front() == 0,
           "Floyd-Warshall should keep zero distance on the diagonal");
    expect(nxpp_map.at(0).at(input.num_nodes - 1) == std::numeric_limits<int>::max(),
           "Floyd-Warshall should keep unreachable pairs at numeric infinity");
}

void test_large_max_flow_and_min_cut_match_raw_boost() {
    const auto input = generate_large_capacity_graph(5, 12, 777u);

    const auto nxpp_graph = build_nxpp_capacity_graph(input);
    auto raw_built = build_raw_capacity_flow_graph(input);

    const auto nxpp_flow = nxpp_graph.maximum_flow(input.source, input.sink);
    const auto nxpp_cut = nxpp_graph.minimum_cut(input.source, input.sink);

    const long raw_flow = boost::edmonds_karp_max_flow(raw_built.graph, input.source, input.sink);
    const auto raw_reachable = raw_flow_reachable_nodes_after_max_flow(raw_built, input.source);

    auto nxpp_reachable = nxpp_cut.reachable;
    std::sort(nxpp_reachable.begin(), nxpp_reachable.end());

    expect(nxpp_flow.value == raw_flow, "large maximum_flow value should match raw Boost");
    expect(nxpp_cut.value == raw_flow, "large minimum_cut value should match raw Boost max-flow");
    expect(nxpp_reachable == raw_reachable,
           "large minimum_cut reachable partition should match raw Boost residual reachability");
}

void test_large_successive_shortest_path_matches_raw_boost() {
    const auto input = generate_large_cost_flow_graph(4, 10, 42424u);

    const auto nxpp_graph = build_nxpp_cost_flow_graph(input);
    auto raw_built = build_raw_cost_flow_graph(input);

    const auto nxpp_result = nxpp_graph.successive_shortest_path_nonnegative_weights(input.source, input.sink);

    boost::successive_shortest_path_nonnegative_weights(raw_built.graph, input.source, input.sink);
    long raw_flow = 0;
    for (auto [edge_it, edge_end] = boost::out_edges(input.source, raw_built.graph); edge_it != edge_end; ++edge_it) {
        raw_flow += raw_built.capacity[*edge_it] - raw_built.residual[*edge_it];
    }
    const long raw_cost = boost::find_flow_cost(raw_built.graph);

    expect(nxpp_result.flow == raw_flow,
           "large SSP flow value should match raw Boost");
    expect(nxpp_result.cost == raw_cost,
           "large SSP cost should match raw Boost");
}

bool run_test(const std::string& name, const std::function<void()>& fn) {
    try {
        fn();
        std::cout << "[TEST] " << name << " | " << green << "PASS" << reset << "\n";
        return true;
    } catch (const std::exception& ex) {
        std::cout << "[TEST] " << name << " | " << red << "FAIL" << reset
                  << " (" << ex.what() << ")\n";
        return false;
    }
}

int main() {
    int passed = 0;
    constexpr int total = 17;

    passed += run_test("large BFS matches raw Boost", test_large_bfs_matches_raw_boost) ? 1 : 0;
    passed += run_test("large DFS tree matches raw Boost", test_large_dfs_tree_matches_raw_boost) ? 1 : 0;
    passed += run_test("large connected components match raw Boost", test_large_connected_components_match_raw_boost) ? 1 : 0;
    passed += run_test("large strongly connected components match raw Boost", test_large_strongly_connected_components_match_raw_boost) ? 1 : 0;
    passed += run_test("large Dijkstra distances match raw Boost", test_large_dijkstra_distances_match_raw_boost) ? 1 : 0;
    passed += run_test("large Bellman-Ford distances match raw Boost", test_large_bellman_ford_distances_match_raw_boost) ? 1 : 0;
    passed += run_test("large DAG shortest paths match raw Boost", test_large_dag_shortest_paths_match_raw_boost) ? 1 : 0;
    passed += run_test("negative cycle detection matches raw Boost", test_negative_cycle_detection_matches_raw_boost) ? 1 : 0;
    passed += run_test("large remove_node state stays aligned with raw Boost", test_large_remove_node_state_stays_aligned_with_raw_boost) ? 1 : 0;
    passed += run_test("large custom-selector Dijkstra and remove_node match raw Boost", test_large_custom_selector_dijkstra_and_remove_node_match_raw_boost) ? 1 : 0;
    passed += run_test("large custom outedge-selector Dijkstra and remove_node match raw Boost", test_large_custom_outedge_selector_dijkstra_and_remove_node_match_raw_boost) ? 1 : 0;
    passed += run_test("large multigraph mutations stay aligned with raw Boost", test_large_multigraph_mutations_stay_aligned_with_raw_boost) ? 1 : 0;
    passed += run_test("large attribute state survives repeated mutations", test_large_attribute_state_survives_repeated_mutations) ? 1 : 0;
    passed += run_test("large combined weighted mutation sequence matches raw Boost", test_large_combined_weighted_mutation_sequence_matches_raw_boost) ? 1 : 0;
    passed += run_test("large Floyd-Warshall all-pairs matches raw Boost", test_large_floyd_warshall_all_pairs_matches_raw_boost) ? 1 : 0;
    passed += run_test("large max flow and min cut match raw Boost", test_large_max_flow_and_min_cut_match_raw_boost) ? 1 : 0;
    passed += run_test("large successive shortest path matches raw Boost", test_large_successive_shortest_path_matches_raw_boost) ? 1 : 0;

    return passed == total ? 0 : 1;
}
