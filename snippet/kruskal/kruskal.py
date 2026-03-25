import networkx as nx


def main() -> None:
    g = nx.Graph()
    g.add_nodes_from(range(5))
    edges = [
        (0, 2, 3), (1, 0, 2), (1, 2, 2),
        (1, 4, 5), (2, 3, 1), (3, 4, 5),
    ]
    g.add_weighted_edges_from(edges)

    mst = nx.minimum_spanning_tree(g, algorithm="kruskal")
    mst_pairs = {frozenset((u, v)) for u, v in mst.edges()}
    for u, v, w in sorted(edges, key=lambda item: (item[2], -max(item[0], item[1]), min(item[0], item[1]))):
        if frozenset((u, v)) in mst_pairs:
            print(f"Edge from {u} to {v}")


if __name__ == "__main__":
    main()
