import networkx as nx

def main():
    G = nx.Graph()  # Undirected Graph per components
    G.add_nodes_from(range(5))
    edges = [(0, 2), (1, 0), (1, 2), (3, 4)]
    G.add_edges_from(edges)
    
    components = list(nx.connected_components(G))
    # Mappiamo i nodi all'indice della componente per emulare BGL component array
    comp_map = {}
    for i, comp in enumerate(components):
        for node in comp:
            comp_map[node] = i
            
    for i in range(5):
        print(f"Vertex {i} is in component {comp_map[i]}")

if __name__ == "__main__":
    main()
