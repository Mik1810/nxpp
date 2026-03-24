import networkx as nx

def main():
    G = nx.DiGraph()
    G.add_nodes_from(range(5))
    edges = [(0, 2), (1, 0), (1, 4), (2, 1), (2, 3), (3, 2), (3, 4)]
    G.add_edges_from(edges)
    
    components = list(nx.strongly_connected_components(G))
    # NetworkX returns components in a topologically sorted order (or reversed)
    # Map node to a component ID to emulate the BGL strong_components property map
    comp_map = {}
    for i, comp in enumerate(components):
        for node in comp:
            comp_map[node] = i
            
    for i in range(5):
        print(f"Vertex {i} is in component {comp_map.get(i, -1)}")

if __name__ == "__main__":
    main()
