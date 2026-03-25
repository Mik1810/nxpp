import networkx as nx


def main() -> None:
    g = nx.DiGraph()
    g.add_nodes_from(range(5))
    g.add_edges_from([(0, 2), (1, 0), (1, 2), (1, 4), (2, 3), (3, 4)])

    order = list(nx.topological_sort(g))
    print(" ".join(str(node) for node in order))


if __name__ == "__main__":
    main()
