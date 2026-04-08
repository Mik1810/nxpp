import networkx as nx


def main() -> None:
    g = nx.DiGraph()
    g.add_edges_from(
        [
            (0, 1),
            (0, 2),
            (1, 2),
            (2, 0),
            (2, 3),
            (3, 2),
        ]
    )

    degree = nx.degree_centrality(g)
    rank = nx.pagerank(g)
    between = nx.betweenness_centrality(g, normalized=True)

    print("Degree centrality")
    for node, score in degree.items():
        print(f"{node}: {score:.2f}")

    print("\nPageRank")
    for node, score in rank.items():
        print(f"{node}: {score:.2f}")

    print("\nBetweenness centrality")
    for node, score in between.items():
        print(f"{node}: {score:.2f}")


if __name__ == "__main__":
    main()
