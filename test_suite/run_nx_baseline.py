import networkx as nx
import json
import os

def main():
    # Definiamo la directory della test suite
    test_dir = os.path.dirname(os.path.abspath(__file__))
    
    # Generiamo un grafo connesso per avere risultati consistenti
    G = nx.erdos_renyi_graph(50, 0.2, seed=42)
    # Assicuriamoci che sia connesso per i path
    while not nx.is_connected(G):
        G = nx.erdos_renyi_graph(50, 0.2)
        
    # Salviamo il grafo in edgelist
    edgelist_path = os.path.join(test_dir, "graph.edgelist")
    nx.write_edgelist(G, edgelist_path, data=False)
    print(f"Graph saved to {edgelist_path}")
    
    # 1. BFS Edges da nodo 0
    bfs_edges = list(nx.bfs_edges(G, 0))
    with open(os.path.join(test_dir, "bfs_expected.json"), "w") as f:
        json.dump(bfs_edges, f)
        
    # 2. Dijkstra Path da 0 a 45
    dijkstra_path = nx.dijkstra_path(G, 0, 45)
    with open(os.path.join(test_dir, "dijkstra_expected.json"), "w") as f:
        json.dump(dijkstra_path, f)
        
    # 3. Connected Components
    cc = list(nx.connected_components(G))
    # Convertiamo set a list e ordiniamo per avere un risultato deterministico
    cc_sorted = sorted([sorted(list(c)) for c in cc])
    with open(os.path.join(test_dir, "cc_expected.json"), "w") as f:
        json.dump(cc_sorted, f)
        
    print("Baseline generated successfully.")

if __name__ == "__main__":
    main()
