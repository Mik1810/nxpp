import networkx as nx


def main() -> None:
    g = nx.DiGraph()
    g.add_nodes_from(range(5))
    edges = [(0, 2), (1, 0), (1, 4), (2, 1), (2, 3), (3, 2), (4, 3)]
    g.add_edges_from(edges)

    print("Visiting vertex 0")
    discovered = {0}
    queue = [0]
    while queue:
        u = queue.pop(0)
        for v in g.successors(u):
            if v not in discovered:
                print(f"Discovered tree edge from {u} to {v}")
                discovered.add(v)
                queue.append(v)
        if queue:
            print(f"Visiting vertex {queue[0]}")


if __name__ == "__main__":
    main()
