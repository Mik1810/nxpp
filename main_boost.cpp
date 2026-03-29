#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/edmonds_karp_max_flow.hpp>
#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

struct CityVertex {
    string name;
    int population = 0;
    string region;
};

struct CityEdge {
    double weight = 1.0;
    int capacity = 0;
    string company;
};

using CityGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, CityVertex, CityEdge>;
using CityVertexDesc = boost::graph_traits<CityGraph>::vertex_descriptor;
using IntGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
using FlowTraits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
using FlowGraph = boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::directedS,
    boost::no_property,
    boost::property<boost::edge_capacity_t, long,
    boost::property<boost::edge_residual_capacity_t, long,
    boost::property<boost::edge_reverse_t, FlowTraits::edge_descriptor>>>>;
using FlowVertexDesc = boost::graph_traits<FlowGraph>::vertex_descriptor;
using RawMultiGraph = boost::adjacency_list<boost::listS, boost::vecS, boost::bidirectionalS, CityVertex, CityEdge>;
using RawMultiEdgeDesc = boost::graph_traits<RawMultiGraph>::edge_descriptor;
using RawMultiVertexDesc = boost::graph_traits<RawMultiGraph>::vertex_descriptor;

CityVertexDesc ensure_city(CityGraph& graph,
                           unordered_map<string, CityVertexDesc>& ids,
                           const string& name) {
    if (const auto it = ids.find(name); it != ids.end()) {
        return it->second;
    }
    const auto vertex = add_vertex(graph);
    graph[vertex].name = name;
    ids.emplace(name, vertex);
    return vertex;
}

void add_named_edge(CityGraph& graph,
                    unordered_map<string, CityVertexDesc>& ids,
                    const string& from,
                    const string& to,
                    double weight,
                    int capacity = 0,
                    string company = {}) {
    const auto u = ensure_city(graph, ids, from);
    const auto v = ensure_city(graph, ids, to);
    const auto [edge, inserted] = add_edge(u, v, graph);
    if (!inserted) {
        return;
    }
    graph[edge].weight = weight;
    graph[edge].capacity = capacity;
    graph[edge].company = std::move(company);
}

void print_named_edges(const CityGraph& graph) {
    for (const auto edge : boost::make_iterator_range(edges(graph))) {
        const auto u = source(edge, graph);
        const auto v = target(edge, graph);
        std::cout << "  " << graph[u].name << " -> " << graph[v].name
                  << " (weight=" << graph[edge].weight << ")\n";
    }
}

void showcase_string_node_ids_and_attributes() {
    cout << "\nString Node IDs And Attributes\n";

    // In raw Boost we build the descriptor map ourselves before we can treat
    // city names as the public surface of the graph.
    CityGraph graph;
    unordered_map<string, CityVertexDesc> ids;

    add_named_edge(graph, ids, "Milan", "Rome", 5.0, 8, "Trenitalia");
    add_named_edge(graph, ids, "Rome", "Naples", 2.5);
    add_named_edge(graph, ids, "Milan", "Florence", 2.0);
    add_named_edge(graph, ids, "Florence", "Naples", 4.0);

    graph[ids.at("Rome")].population = 2800000;
    graph[ids.at("Milan")].region = "Lombardy";

    cout << "Nodes:";
    for (const auto vertex : boost::make_iterator_range(vertices(graph))) {
        cout << " " << graph[vertex].name;
    }
    cout << "\nEdges:\n";
    print_named_edges(graph);

    const auto [milan_rome, exists] = edge(ids.at("Milan"), ids.at("Rome"), graph);
    if (!exists) {
        return;
    }

    cout << "Rome population: " << graph[ids.at("Rome")].population << '\n';
    cout << "Milan -> Rome operator: " << graph[milan_rome].company << '\n';
}

void showcase_materialized_shortest_path_results() {
    cout << "\nMaterialized Shortest Path Results\n";

    // Here the bookkeeping becomes visible: descriptor-indexed predecessor and
    // distance buffers have to be assembled before calling the algorithm.
    CityGraph graph;
    unordered_map<string, CityVertexDesc> ids;

    add_named_edge(graph, ids, "Milan", "Rome", 5.0);
    add_named_edge(graph, ids, "Rome", "Naples", 2.5);
    add_named_edge(graph, ids, "Milan", "Florence", 2.0);
    add_named_edge(graph, ids, "Florence", "Naples", 4.0);
    add_named_edge(graph, ids, "Naples", "Bari", 3.0);

    vector<CityVertexDesc> predecessor(num_vertices(graph));
    vector<double> distance(num_vertices(graph), numeric_limits<double>::infinity());

    boost::dijkstra_shortest_paths(
        graph,
        ids.at("Milan"),
        boost::predecessor_map(predecessor.data())
            .distance_map(distance.data())
            .weight_map(get(&CityEdge::weight, graph)));

    vector<string> path;
    for (auto current = ids.at("Naples");; current = predecessor[current]) {
        path.push_back(graph[current].name);
        if (current == predecessor[current]) {
            break;
        }
    }
    reverse(path.begin(), path.end());

    cout << "Distance Milan -> Naples: " << distance[ids.at("Naples")] << '\n';
    cout << "Path Milan -> Naples:";
    for (const auto& node : path) {
        cout << " " << node;
    }
    cout << '\n';
    cout << "Predecessor of Bari: " << graph[predecessor[ids.at("Bari")]].name << '\n';
}

struct BfsRecorder : boost::default_bfs_visitor {
    explicit BfsRecorder(vector<pair<int, int>>& out) : out_edges(out) {}

    template <typename Edge, typename Graph>
    void tree_edge(Edge edge, const Graph& graph) const {
        out_edges.emplace_back(static_cast<int>(source(edge, graph)), static_cast<int>(target(edge, graph)));
    }

    vector<pair<int, int>>& out_edges;
};

void showcase_traversal_and_generators() {
    cout << "\nTraversal And Generators\n";

    // Even a path graph is assembled edge by edge here, which makes the raw
    // Boost version a good baseline for the nxpp generator helpers.
    IntGraph graph(5);
    add_edge(0, 1, graph);
    add_edge(1, 2, graph);
    add_edge(2, 3, graph);
    add_edge(3, 4, graph);

    vector<pair<int, int>> tree_edges;
    boost::breadth_first_search(graph, vertex(0, graph), boost::visitor(BfsRecorder(tree_edges)));

    cout << "BFS tree edges from 0:\n";
    for (const auto& [u, v] : tree_edges) {
        cout << "  " << u << " -> " << v << '\n';
    }

    cout << "Degree centrality on path_graph(5):\n";
    for (int node = 0; node < 5; ++node) {
        const auto centrality = static_cast<double>(boost::degree(node, graph)) / 4.0;
        cout << "  node " << node << ": " << centrality << '\n';
    }
}

void add_flow_edge(FlowGraph& graph, FlowVertexDesc from, FlowVertexDesc to, long capacity_value) {
    const auto [forward, inserted_forward] = add_edge(from, to, graph);
    const auto [reverse, inserted_reverse] = add_edge(to, from, graph);
    if (!inserted_forward || !inserted_reverse) {
        return;
    }

    auto capacity = get(boost::edge_capacity, graph);
    auto reverse_edge = get(boost::edge_reverse, graph);
    auto residual = get(boost::edge_residual_capacity, graph);

    capacity[forward] = capacity_value;
    capacity[reverse] = 0;
    reverse_edge[forward] = reverse;
    reverse_edge[reverse] = forward;
    residual[forward] = capacity_value;
    residual[reverse] = 0;
}

void showcase_flow_and_cut_helpers() {
    cout << "\nFlow And Cut Helpers\n";

    // Flow is where the raw model gets noticeably lower-level: we wire reverse
    // edges and residual capacity maps ourselves before asking for max flow.
    FlowGraph graph(4);
    add_flow_edge(graph, 0, 1, 3);
    add_flow_edge(graph, 0, 2, 2);
    add_flow_edge(graph, 1, 3, 2);
    add_flow_edge(graph, 2, 3, 2);
    add_flow_edge(graph, 1, 2, 1);

    const long max_flow = boost::edmonds_karp_max_flow(graph, 0, 3);
    auto capacity = get(boost::edge_capacity, graph);
    auto residual = get(boost::edge_residual_capacity, graph);

    vector<bool> reachable(num_vertices(graph), false);
    queue<FlowVertexDesc> queue;
    reachable[0] = true;
    queue.push(0);
    while (!queue.empty()) {
        const auto u = queue.front();
        queue.pop();
        for (const auto edge : boost::make_iterator_range(out_edges(u, graph))) {
            const auto v = target(edge, graph);
            if (residual[edge] > 0 && !reachable[v]) {
                reachable[v] = true;
                queue.push(v);
            }
        }
    }

    vector<pair<int, int>> cut_edges;
    long cut_value = 0;
    for (const auto edge : boost::make_iterator_range(edges(graph))) {
        const auto u = source(edge, graph);
        const auto v = target(edge, graph);
        if (capacity[edge] == 0) {
            continue;
        }
        if (reachable[u] && !reachable[v]) {
            cut_edges.emplace_back(static_cast<int>(u), static_cast<int>(v));
            cut_value += capacity[edge];
        }
    }

    cout << "Maximum flow value: " << max_flow << '\n';
    cout << "Minimum cut value: " << cut_value << '\n';
    cout << "Cut edges:\n";
    for (const auto& [u, v] : cut_edges) {
        cout << "  " << u << " -> " << v << '\n';
    }
}

RawMultiVertexDesc ensure_city(RawMultiGraph& graph,
                               unordered_map<string, RawMultiVertexDesc>& ids,
                               const string& name) {
    if (const auto it = ids.find(name); it != ids.end()) {
        return it->second;
    }
    const auto vertex = add_vertex(graph);
    graph[vertex].name = name;
    ids.emplace(name, vertex);
    return vertex;
}

void showcase_precise_multigraph_edges() {
    cout << "\nPrecise Multigraph Edge Identity\n";

    // Raw Boost can represent the parallel edges just fine, but the example has
    // to carry edge descriptors around explicitly to talk about one exact edge.
    RawMultiGraph graph;
    unordered_map<string, RawMultiVertexDesc> ids;
    const auto milan = ensure_city(graph, ids, "Milan");
    const auto rome = ensure_city(graph, ids, "Rome");

    const auto [fast_train, inserted_fast] = add_edge(milan, rome, graph);
    const auto [night_train, inserted_night] = add_edge(milan, rome, graph);
    if (!inserted_fast || !inserted_night) {
        return;
    }

    graph[fast_train].weight = 5.0;
    graph[fast_train].company = "fast";
    graph[night_train].weight = 8.0;
    graph[night_train].company = "night";

    vector<RawMultiEdgeDesc> parallel_edges;
    for (const auto edge : boost::make_iterator_range(out_edges(milan, graph))) {
        if (target(edge, graph) == rome) {
            parallel_edges.push_back(edge);
        }
    }

    cout << "Parallel edges Milan -> Rome: " << parallel_edges.size() << '\n';
    size_t edge_index = 0;
    for (const auto edge : parallel_edges) {
        cout << "  edge_descriptor[" << edge_index++ << "] Milan -> Rome"
             << " weight=" << graph[edge].weight
             << " service=" << graph[edge].company << '\n';
    }

    remove_edge(fast_train, graph);

    size_t remaining = 0;
    for (const auto edge : boost::make_iterator_range(out_edges(milan, graph))) {
        if (target(edge, graph) == rome) {
            ++remaining;
        }
    }
    cout << "After removing one descriptor, remaining Milan -> Rome edges: "
         << remaining << '\n';
}

int main() {
    showcase_string_node_ids_and_attributes();
    showcase_materialized_shortest_path_results();
    showcase_traversal_and_generators();
    showcase_flow_and_cut_helpers();
    showcase_precise_multigraph_edges();
    return 0;
}
