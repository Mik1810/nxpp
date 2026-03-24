import networkx as nx

def main():
    G = nx.Graph()
    G.add_nodes_from(range(5))
    edges = [
        (0, 1, 2), (0, 3, 6), (1, 2, 3), 
        (1, 3, 8), (1, 4, 5), (2, 4, 7), (3, 4, 9)
    ]
    G.add_weighted_edges_from(edges)
    
    # Kruskal algorithm for Minimum Spanning Tree
    mst = list(nx.minimum_spanning_edges(G, algorithm="kruskal", data=False))
    
    print("Minimum Spanning Tree Edges:")
    for u, v in mst:
        print(f"{u} - {v}")

if __name__ == "__main__":
    main()
