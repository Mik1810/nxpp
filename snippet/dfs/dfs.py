import networkx as nx


def main() -> None:
    g = nx.DiGraph()
    g.add_nodes_from(range(5))
    edges = [(0, 2), (1, 0), (1, 4), (2, 1), (2, 3), (3, 2), (4, 3)]
    g.add_edges_from(edges)

    tree_edges = set(nx.dfs_edges(g, source=0))
    for u, v in nx.edge_dfs(g, source=0):
        if (u, v) in tree_edges:
            print(f"Discovered tree edge from {u} to {v}")
        else:
            print(f"Discovered back edge from {u} to {v}")


if __name__ == "__main__":
    main()
