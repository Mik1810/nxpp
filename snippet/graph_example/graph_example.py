import networkx as nx


def main() -> None:
    g = nx.DiGraph()
    g.add_nodes_from(range(5))
    edges = [(0, 2), (1, 0), (1, 4), (2, 1), (2, 3), (3, 2), (3, 4)]
    g.add_edges_from(edges)

    for u, v in edges:
        print(f"Edge from {u} to {v}")

    for _, v in g.out_edges(2):
        print(f"Out edge from 2 to {v}")


if __name__ == "__main__":
    main()
