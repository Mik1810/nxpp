import networkx as nx


def main() -> None:
    g = nx.DiGraph()
    g.add_nodes_from(range(5))
    g.add_edge(0, 1, weight=5)

    for u, v, data in g.edges(data=True):
        print(f"Edge from {u} to {v} with weight {data['weight']}")


if __name__ == "__main__":
    main()
