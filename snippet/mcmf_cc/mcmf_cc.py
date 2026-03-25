import networkx as nx


def main() -> None:
    g = nx.DiGraph()
    g.add_nodes_from(range(6))
    edges = [
        (0, 1, 1, 2), (0, 3, 2, 3), (1, 2, 3, 5), (1, 4, 4, 1),
        (2, 5, 2, 3), (3, 1, 1, 1), (3, 2, 2, 6), (4, 5, 2, 1),
    ]
    for u, v, c, w in edges:
        g.add_edge(u, v, capacity=c, weight=w)

    flow_dict = nx.max_flow_min_cost(g, 0, 5)
    flow_value = sum(flow_dict[0][v] for v in flow_dict[0])
    cost_value = nx.cost_of_flow(g, flow_dict)

    print(f"The maximum flow from 0 to 5 is {flow_value}")
    print(f"The minimum cost of a max flow from 0 to 5 is {cost_value}")


if __name__ == "__main__":
    main()
