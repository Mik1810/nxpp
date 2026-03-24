import networkx as nx

def test_dijkstra_scc():
    # 1. Inizializza un grafo diretto con chiavi testuali
    G = nx.DiGraph()

    # 2. Popolamento
    G.add_edge("Milano", "Roma", weight=5.0)
    G.add_edge("Roma", "Napoli", weight=2.5)
    G.add_edge("Venezia", "Milano", weight=2.0)
    G.add_edge("Napoli", "Roma", weight=3.0) 

    # 3. Stampa dei Nodi
    print("Nodi:")
    for n in G.nodes():
        print("-", n)
    print("\n")

    # Stampa di un peso salvato
    print("Peso Milano -> Roma:", G["Milano"]["Roma"]["weight"], "\n")

    # 4. Dijkstra Shortest Path
    print("Dijkstra (Venezia -> Napoli):")
    path = nx.dijkstra_path(G, "Venezia", "Napoli", weight="weight")
    for n in path:
        print("->", n)
    print("\n")

    # 5. Componenti Fortemente Connesse (SCC)
    print("Componenti Fortemente Connesse (SCC):")
    sccs = list(nx.strongly_connected_components(G))
    for i, scc in enumerate(sccs):
        print("SCC", i + 1, ":")
        for n in scc:
            print("  *", n)
    print("\n")

def test_integer_nodes():
    # 1. Inizializza un grafo NON orientato con chiavi Intere
    G = nx.Graph()

    # 2. Aggiunge più archi in blocco
    G.add_edges_from([
        (1, 2), (2, 3), (3, 4),
        (5, 6), (6, 7) # Un'isola disconnessa
    ])

    print("Nodi (Interi):")
    for n in G.nodes():
        print("-", n)
    print("\n")

    # 3. Esegue una visita BFS in ampiezza dal nodo 1
    print("Albero BFS (partendo dal nodo 1):")
    edges = list(nx.bfs_edges(G, 1))
    for u, v in edges:
        print(" (", u, "->", v, ")")
    print("\n\n")

    # 4. Trova le Componenti Connesse generiche (CC)
    print("Componenti Connesse (Undirected):")
    ccs = list(nx.connected_components(G))
    for i, cc in enumerate(ccs):
        print("CC", i + 1, ":")
        for n in cc:
            print("  *", n)
    print("\n")

def test_destruction():
    G = nx.DiGraph()

    G.add_edges_from([
        ("A", "B"), ("A", "C"), ("B", "D"), ("C", "D"), ("D", "E")
    ])

    print("Neighbors di A:")
    for n in G.neighbors("A"):
        print("-", n)
    print("\n")

    print("Grafo iniziale:")
    for n in G.nodes(): print(n)
    print("\n")

    G.remove_node("B")
    print("Dopo aver rimosso B:")
    for n in G.nodes(): print(n)
    print("\n")

    G.remove_edge("C", "D")

    print("Dopo aver rimosso C->D, l'albero BFS da A esplora:")
    edges = list(nx.bfs_edges(G, "A"))
    for u, v in edges:
        print(" (", u, "->", v, ")")
    print("\n\n")

    G.clear()
    print("Dopo clear(), quanti nodi rimangono?", len(list(G.nodes())), "\n")

def test_multigraph():
    print("=== NETWORKX (Python) - MultiGraph Test ===\n")
    MG = nx.MultiDiGraph()
    MG.add_edge("Milano", "Roma", weight=5.0)
    MG.add_edge("Milano", "Roma", weight=2.5)

    G = nx.DiGraph()
    G.add_edge("Milano", "Roma", weight=5.0)
    G.add_edge("Milano", "Roma", weight=2.5) # Sovrascrive

    print("Archi in MultiDiGraph:")
    for u, v, data in MG.edges(data=True):
        print(" (", u, "->", v, ") peso:", data["weight"])
    print("\n")

    print("Archi in DiGraph (Standard):")
    for u, v, data in G.edges(data=True):
        print(" (", u, "->", v, ") peso:", data["weight"])
    print("\n")

if __name__ == "__main__":
    test_dijkstra_scc()
    test_integer_nodes()
    test_destruction()
    test_multigraph()
