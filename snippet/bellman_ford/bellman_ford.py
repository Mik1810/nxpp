import networkx as nx


def main() -> None:
    g = nx.DiGraph()
    g.add_nodes_from(range(5))
    edges = [
        (0, 2, 3), (1, 0, 2), (1, 4, 5),
        (2, 1, 2), (2, 3, 1), (3, 2, 4), (3, 4, 5),
    ]
    g.add_weighted_edges_from(edges)

    lengths, paths = nx.single_source_bellman_ford(g, 0)
    for i in range(5):
        dist = int(lengths[i])
        pred = 0 if i == 0 else paths[i][-2]
        print(f"Distance to {i}: {dist} Parent: {pred}")


if __name__ == "__main__":
    main()
