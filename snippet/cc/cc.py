import networkx as nx


def main() -> None:
    g = nx.Graph()
    g.add_nodes_from(range(5))
    g.add_edges_from([(0, 2), (1, 0), (1, 2), (3, 4)])

    comp_map = {}
    for comp_id, comp in enumerate(nx.connected_components(g)):
        for node in comp:
            comp_map[node] = comp_id

    for i in range(5):
        print(f"Vertex {i} is in component {comp_map[i]}")


if __name__ == "__main__":
    main()
