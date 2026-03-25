import networkx as nx


def main() -> None:
    g = nx.DiGraph()
    g.add_nodes_from(range(5))
    edges = [(0, 2), (1, 0), (1, 4), (2, 1), (2, 3), (3, 2), (4, 3)]
    g.add_edges_from(edges)

    color = {node: 0 for node in g.nodes()}

    def dfs(u: int) -> None:
        color[u] = 1
        for v in g.successors(u):
            if color[v] == 0:
                print(f"Discovered tree edge from {u} to {v}")
                dfs(v)
            elif color[v] == 1:
                print(f"Discovered back edge from {u} to {v}")
        color[u] = 2

    dfs(0)


if __name__ == "__main__":
    main()
