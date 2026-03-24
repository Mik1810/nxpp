import networkx as nx

def main():
    G = nx.DiGraph()
    G.add_nodes_from(range(6))
    edges = [
        (0, 1, 1), (0, 3, 2), (1, 2, 3), (1, 4, 4),
        (2, 5, 2), (3, 2, 2), (4, 5, 2)
    ]
    for u, v, c in edges:
        G.add_edge(u, v, capacity=c)
        
    flow_value, flow_dict = nx.maximum_flow(G, _s=0, _t=5)
    print(f"The maximum flow from 0 to 5 is {flow_value}")
    
    # Check flow across (1, 4)
    print(f"The flow across edge (1, 4) is: {flow_dict[1][4]}")

if __name__ == "__main__":
    main()
