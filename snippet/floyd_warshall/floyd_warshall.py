import math
import networkx as nx


def main() -> None:
    g = nx.DiGraph()
    g.add_nodes_from(range(5))
    edges = [
        (0, 2, 3), (1, 0, 2), (1, 4, 5),
        (2, 1, 2), (2, 3, 1), (3, 2, 4), (3, 4, 5),
    ]
    g.add_weighted_edges_from(edges)

    dist = dict(nx.floyd_warshall(g, weight="weight"))
    for i in range(5):
        for j in range(5):
            value = dist[i][j]
            print(2147483647 if math.isinf(value) else int(value), end=" ")
        print()


if __name__ == "__main__":
    main()
