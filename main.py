import networkx as nx


def format_number(value: float) -> str:
    return f"{value:g}"


def showcase_string_node_ids_and_attributes() -> None:
    print("\nString Node IDs And Attributes")

    # This is the closest mental model to the nxpp demo: named nodes and
    # ad-hoc attributes are the default starting point instead of an add-on.
    graph = nx.DiGraph()
    graph.add_edge("Milan", "Rome", weight=5.0, capacity=8, company="Trenitalia")
    graph.add_edge("Rome", "Naples", weight=2.5)
    graph.add_edge("Milan", "Florence", weight=2.0)
    graph.add_edge("Florence", "Naples", weight=4.0)

    graph.add_node("Rome", population=2800000)
    graph.add_node("Milan", region="Lombardy")

    print("Nodes:", *graph.nodes())
    print("Edges:")
    edge_order = [
        ("Milan", "Rome"),
        ("Rome", "Naples"),
        ("Milan", "Florence"),
        ("Florence", "Naples"),
    ]
    for u, v in edge_order:
        print(f"  {u} -> {v} (weight={format_number(graph[u][v]['weight'])})")
    print("Rome population:", graph.nodes["Rome"]["population"])
    print("Milan -> Rome operator:", graph["Milan"]["Rome"]["company"])


def showcase_materialized_shortest_path_results() -> None:
    print("\nMaterialized Shortest Path Results")

    # This is the Python-side companion to main_nxpp.cpp: we ask for concrete
    # path data directly, without the descriptor/index bookkeeping seen in Boost.
    graph = nx.DiGraph()
    graph.add_edge("Milan", "Rome", weight=5.0)
    graph.add_edge("Rome", "Naples", weight=2.5)
    graph.add_edge("Milan", "Florence", weight=2.0)
    graph.add_edge("Florence", "Naples", weight=4.0)
    graph.add_edge("Naples", "Bari", weight=3.0)

    distances = nx.single_source_dijkstra_path_length(graph, "Milan", weight="weight")
    paths = nx.single_source_dijkstra_path(graph, "Milan", weight="weight")
    predecessor_map, _ = nx.dijkstra_predecessor_and_distance(graph, "Milan", weight="weight")
    predecessors = {node: values[0] if values else node for node, values in predecessor_map.items()}

    print("Distance Milan -> Naples:", format_number(distances["Naples"]))
    print("Path Milan -> Naples:", *paths["Naples"])
    print("Predecessor of Bari:", predecessors["Bari"])


def showcase_traversal_and_generators() -> None:
    print("\nTraversal And Generators")

    # This mirrors the same generated-path and BFS story as the two C++ files,
    # but from the high-level NetworkX point of view.
    path = nx.path_graph(5)
    tree_edges = list(nx.bfs_edges(path, 0))
    centrality = nx.degree_centrality(path)

    print("BFS tree edges from 0:")
    for u, v in tree_edges:
        print(f"  {u} -> {v}")

    print("Degree centrality on path_graph(5):")
    for node in range(5):
        print(f"  node {node}: {format_number(centrality[node])}")


def showcase_flow_and_cut_helpers() -> None:
    print("\nFlow And Cut Helpers")

    # This is the other useful reference point for the C++ demos: concise like
    # nxpp, but a reminder of how much setup raw Boost has to carry by hand.
    graph = nx.DiGraph()
    graph.add_edge(0, 1, capacity=3)
    graph.add_edge(0, 2, capacity=2)
    graph.add_edge(1, 3, capacity=2)
    graph.add_edge(2, 3, capacity=2)
    graph.add_edge(1, 2, capacity=1)

    flow_value, _ = nx.maximum_flow(graph, 0, 3)
    cut_value, partitions = nx.minimum_cut(graph, 0, 3)
    left, right = partitions
    cut_edges = [(u, v) for u, v in graph.edges() if u in left and v in right]

    print("Maximum flow value:", flow_value)
    print("Minimum cut value:", cut_value)
    print("Cut edges:")
    for u, v in cut_edges:
        print(f"  {u} -> {v}")


def showcase_precise_multigraph_edges() -> None:
    print("\nPrecise Multigraph Edge Identity")

    # NetworkX uses edge keys for this role, so this section acts as the Python
    # companion to nxpp's edge_id-based multigraph API.
    graph = nx.MultiDiGraph()

    fast_train = graph.add_edge("Milan", "Rome", weight=5.0, service="fast")
    graph.add_edge("Milan", "Rome", weight=8.0, service="night")

    parallel_edges = list(graph.edges(keys=True, data=True))
    print("Parallel edges Milan -> Rome:", len(parallel_edges))
    for u, v, key, data in parallel_edges:
        print(f"  edge_key={key} {u} -> {v} weight={format_number(data['weight'])} service={data['service']}")

    graph.remove_edge("Milan", "Rome", fast_train)
    print("After removing one key, remaining Milan -> Rome edges:", graph.number_of_edges("Milan", "Rome"))


if __name__ == "__main__":
    showcase_string_node_ids_and_attributes()
    showcase_materialized_shortest_path_results()
    showcase_traversal_and_generators()
    showcase_flow_and_cut_helpers()
    showcase_precise_multigraph_edges()
