import sys
import networkx as nx


def to_vertex_id(i: int) -> int:
    return (2 * i - 2) if i > 0 else (-2 * i - 1)


def solve() -> bool:
    data = sys.stdin.read().strip().split()
    if not data:
        return True

    it = iter(data)
    n = int(next(it))
    m = int(next(it))

    g = nx.DiGraph()
    g.add_nodes_from(range(2 * n))

    for _ in range(m):
        x = int(next(it))
        y = int(next(it))
        g.add_edge(to_vertex_id(-x), to_vertex_id(y))
        g.add_edge(to_vertex_id(-y), to_vertex_id(x))

    comp_map = {}
    for comp_id, comp in enumerate(nx.strongly_connected_components(g)):
        for node in comp:
            comp_map[node] = comp_id

    for i in range(1, n + 1):
        if comp_map[to_vertex_id(i)] == comp_map[to_vertex_id(-i)]:
            return False
    return True


if __name__ == "__main__":
    print("Satisfiable" if solve() else "Not satisfiable")
