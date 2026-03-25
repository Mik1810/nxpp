import networkx as nx


def main() -> None:
    g = nx.DiGraph()
    g.add_nodes_from(range(5))
    g.add_edges_from([(0, 2), (1, 0), (1, 4), (2, 1), (2, 3), (3, 2), (3, 4)])

    root_map = {}
    for comp in nx.strongly_connected_components(g):
        root = min(comp)
        for node in comp:
            root_map[node] = root

    for i in range(5):
        print(f"The representative of the SCC of vertex {i} is {root_map[i]}")


if __name__ == "__main__":
    main()
