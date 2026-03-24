import networkx as nx

def main():
    G = nx.DiGraph()
    G.add_nodes_from(range(5))
    edges = [(0, 2), (1, 0), (1, 4), (2, 1), (2, 3), (3, 2), (4, 3)]
    G.add_edges_from(edges)
    
    # In BGL, bfs generates tree edges. We mimic the custom_visitor output.
    bfs_edges = list(nx.bfs_edges(G, source=0))
    
    # BGL snippet esamina anche i vertici ma si focalizza su "Discovered tree edge"
    # (Per semplicità esatta stampiamo gli archi)
    for u, v in bfs_edges:
        print(f"Discovered tree edge from {u} to {v}")

if __name__ == "__main__":
    main()
