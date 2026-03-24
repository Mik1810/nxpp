import networkx as nx

def main():
    G = nx.DiGraph()
    G.add_nodes_from(range(6))
    edges = [(5, 2), (5, 0), (4, 0), (4, 1), (2, 3), (3, 1)]
    G.add_edges_from(edges)
    
    try:
        ts = list(nx.topological_sort(G))
        print("Topological Sort:")
        for node in ts:
            print(node)
    except nx.NetworkXUnfeasible:
        print("Graph is not a DAG!")

if __name__ == "__main__":
    main()
