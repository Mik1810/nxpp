import networkx as nx

def main():
    G = nx.DiGraph()
    G.add_nodes_from(range(5))
    edges = [(0, 2), (1, 0), (1, 4), (2, 1), (2, 3), (3, 2), (4, 3)]
    G.add_edges_from(edges)
    
    # DFS tree edges
    dfs_edges = list(nx.dfs_edges(G, source=0))
    for u, v in dfs_edges:
        print(f"Discovered tree edge from {u} to {v}")
        
    # BGL snippet printed back_edges too, ma networkx ha un iteratore separato se serve, 
    # o si può calcolare dalla differenza. Qui stampiamo i tree_edges base.

if __name__ == "__main__":
    main()
