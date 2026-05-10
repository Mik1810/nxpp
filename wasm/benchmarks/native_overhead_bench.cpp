#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "include/nxpp.hpp"

namespace {

struct Config {
    int nodes = 300;
    int edges = 900;
    int floyd_nodes = 35;
    int attr_ops = 1000;
    int multigraph_edges = 600;
    int iterations = 5;
};

template <typename T>
void consume(const T& value) {
#if defined(__GNUC__) || defined(__clang__)
    asm volatile("" : : "g"(&value) : "memory");
#else
    (void)value;
#endif
}

int read_int_arg(int argc, char** argv, const std::string& name, int fallback) {
    for (int i = 1; i + 1 < argc; ++i) {
        if (argv[i] == name) {
            return std::atoi(argv[i + 1]);
        }
    }
    return fallback;
}

Config read_config(int argc, char** argv) {
    Config config;
    config.nodes = read_int_arg(argc, argv, "--nodes", config.nodes);
    config.edges = read_int_arg(argc, argv, "--edges", config.edges);
    config.floyd_nodes = read_int_arg(argc, argv, "--floyd-nodes", config.floyd_nodes);
    config.attr_ops = read_int_arg(argc, argv, "--attr-ops", config.attr_ops);
    config.multigraph_edges = read_int_arg(argc, argv, "--multigraph-edges", config.multigraph_edges);
    config.iterations = read_int_arg(argc, argv, "--iterations", config.iterations);
    return config;
}

nxpp::DiGraphInt make_digraph(int nodes, int edges) {
    nxpp::DiGraphInt graph;
    for (int node = 0; node < nodes; ++node) {
        graph.add_node(node);
    }
    for (int edge = 0; edge < edges; ++edge) {
        const int source = edge % nodes;
        int target = (edge * 37 + 11) % nodes;
        if (target == source) {
            target = (target + 1) % nodes;
        }
        graph.add_edge(source, target, static_cast<double>((edge % 17) + 1));
    }
    return graph;
}

nxpp::DiGraphInt make_floyd_graph(int nodes) {
    nxpp::DiGraphInt graph;
    for (int node = 0; node < nodes; ++node) {
        graph.add_node(node);
    }
    for (int node = 0; node + 1 < nodes; ++node) {
        graph.add_edge(node, node + 1, 1.0);
        if (node + 3 < nodes) {
            graph.add_edge(node, node + 3, 4.0);
        }
    }
    return graph;
}

template <typename Fn>
double measure_ms(int iterations, Fn&& fn) {
    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; ++i) {
        fn();
    }
    const auto stop = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(stop - start).count();
}

void print_row(const std::string& workload, int iterations, double total_ms, const std::string& notes) {
    const double ops_per_second = total_ms > 0.0 ? (static_cast<double>(iterations) * 1000.0 / total_ms) : 0.0;
    std::cout << "native_cpp," << workload << "," << iterations << ","
              << std::fixed << std::setprecision(3) << total_ms << ","
              << std::fixed << std::setprecision(3) << ops_per_second << ","
              << notes << "\n";
}

} // namespace

int main(int argc, char** argv) {
    const Config config = read_config(argc, argv);
    std::cout << "layer,workload,iterations,total_ms,ops_per_second,notes\n";

    const double construction_ms = measure_ms(config.iterations, [&] {
        auto graph = make_digraph(config.nodes, config.edges);
        consume(graph.num_edges());
    });
    print_row("construct_digraph", config.iterations, construction_ms, "nodes=" + std::to_string(config.nodes) + ";edges=" + std::to_string(config.edges));

    auto graph = make_digraph(config.nodes, config.edges);
    const double bfs_ms = measure_ms(config.iterations, [&] {
        const auto edges = graph.bfs_edges(0);
        consume(edges.size());
    });
    print_row("bfs_edges", config.iterations, bfs_ms, "source=0");

    const double dijkstra_ms = measure_ms(config.iterations, [&] {
        const auto result = graph.dijkstra_shortest_paths(0);
        consume(result.distance.size());
    });
    print_row("dijkstra_shortest_paths", config.iterations, dijkstra_ms, "source=0");

    auto floyd_graph = make_floyd_graph(config.floyd_nodes);
    const double floyd_ms = measure_ms(config.iterations, [&] {
        const auto matrix = floyd_graph.floyd_warshall_all_pairs_shortest_paths();
        consume(matrix.size());
    });
    print_row("floyd_warshall_all_pairs", config.iterations, floyd_ms, "nodes=" + std::to_string(config.floyd_nodes));

    nxpp::DiGraphInt attr_graph;
    const double attr_ms = measure_ms(config.iterations, [&] {
        for (int i = 0; i < config.attr_ops; ++i) {
            const int node = i % config.nodes;
            attr_graph.node(node)["label"] = std::string("node-") + std::to_string(node);
            const auto value = attr_graph.get_node_attr<std::string>(node, "label");
            consume(value);
        }
    });
    print_row("attribute_roundtrip", config.iterations, attr_ms, "ops_per_iteration=" + std::to_string(config.attr_ops));

    const double multigraph_ms = measure_ms(config.iterations, [&] {
        nxpp::MultiDiGraphInt multigraph;
        for (int i = 0; i < config.multigraph_edges; ++i) {
            const auto id = multigraph.add_edge_with_id(i % config.nodes, (i + 1) % config.nodes, 1.0);
            multigraph.set_edge_attr(id, "capacity", i + 1);
        }
        const auto ids = multigraph.edge_ids(0, 1);
        consume(ids.size());
    });
    print_row("multigraph_edge_ids", config.iterations, multigraph_ms, "edges=" + std::to_string(config.multigraph_edges));

    return 0;
}
