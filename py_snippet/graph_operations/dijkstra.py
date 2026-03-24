import networkx as nx
import sys

def main():
    G = nx.DiGraph()
    G.add_nodes_from(range(5))
    edges = [
        (0, 2, 3), (1, 0, 2), (1, 4, 5),
        (2, 1, 2), (2, 3, 1), (3, 2, 4), (3, 4, 5)
    ]
    G.add_weighted_edges_from(edges)
    
    # networkx returns length dict and path dict
    lengths, paths = nx.single_source_dijkstra(G, 0)
    
    for i in range(5):
        if i in lengths:
            if i == 0:
                pred = 0
            else:
                pred = paths[i][-2] # The predecessor is the second to last node in path
            print(f"Distance to {i}: {lengths[i]} Parent: {pred}")
        else:
            print(f"Distance to {i}: 2147483647 Parent: {i}")

if __name__ == "__main__":
    main()
