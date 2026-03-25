import networkx as nx


def main() -> None:
    g = nx.Graph()
    g.add_nodes_from(range(5))
    edges = [
        (0, 2, 3), (1, 0, 2), (1, 2, 2),
        (1, 4, 5), (2, 3, 1), (3, 4, 5),
    ]
    g.add_weighted_edges_from(edges)

    mst = nx.minimum_spanning_tree(g, algorithm="prim")
    tree = nx.bfs_tree(mst, 0)

    parent = {0: 0}
    for u, v in tree.edges():
        parent[v] = u

    for i in range(5):
        print(f"Parent of {i}: {parent[i]}")


if __name__ == "__main__":
    main()
