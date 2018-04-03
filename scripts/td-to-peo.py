"""
This file provides a converter for transforming a tree
decomposition into a perfect elimination ordering.
"""

import networkx as nx
import sys

def read_td(td_filename):
    """
    Read in a .td file and extract the following:
    bags (dict): A lookup table that maps a tree decomp node to a
                 set of graph vertices.
    tree (NetworkX Graph): The tree decomp tree.
    """
    # The bags lookup will map bag_number -> {list of vertices}
    bags = {}

    # Additionally, we'll store the decomposition tree in a
    # NetworkX graph
    tree = nx.Graph()

    with open(td_filename, 'r') as infile:
        # Ignore comments
        line = infile.readline()
        while line[0] == 'c':
            line = infile.readline()

        # The next line will look like "s td 28 25 95"
        # Currently unused
        num_nodes, max_bag, num_vertices = map(int,
                                               line.split()[2:])

        line = infile.readline()
        while line[0] == 'b':
            # A bag line will look like:
            # "b 1 1 11 16 41 42 43 44 45"
            node = int(line.split()[1])
            vertices = set(map(int, line.split()[2:]))
            bags[node] = vertices
            line = infile.readline()

        # Add a node for each bag
        tree.add_nodes_from(bags)

        # Add the first edge
        tree.add_edge(*map(int, line.split()))

        # The remainder of the file is edges
        for line in infile.readlines():
            tree.add_edge(*map(int, line.split()))

    return bags, tree


def increment_peo(bags, tree, peo):
    """
    Given a tree decomp and a partial perfect elimination ordering
    (peo), add one more vertex to the peo or recognize that we're
    already done.
    """

    # Base case: If one node left, add its vertices to the peo
    if tree.order() == 1:
        only_vertex = list(tree.nodes())[0]
        peo += list(bags[only_vertex])
        return peo

    # Otherwise we can identify a leaf and its parent
    leaf = list(filter(lambda node: tree.degree[node] == 1,
                tree.nodes()))[0]
    parent = list(tree.neighbors(leaf))[0]

    # See if there are any vertices in leaf's bag that are not in
    # parent's bag
    vertex_diff = bags[leaf] - bags[parent]

    # If there's a vertex in the leaf and not in the parent,
    # then remove it from the graph and add it to the peo.
    if vertex_diff:
        next_vertex = vertex_diff.pop()
        peo.append(next_vertex)
        for key in bags:
            bags[key].discard(next_vertex)

    # Else remove the leaf from the graph
    else:
        tree.remove_node(leaf)
        bags.pop(leaf)

    # Recurse until we hit the base case
    return increment_peo(bags, tree, peo)


def generate_peo(td_filename):
    """
    Generates a perfect elimination ordering from a tree decomposition. The
    algorithm is taken from Markov and Shi Proof of Prop 4.2
    (https://arxiv.org/pdf/quant-ph/0511069.pdf).
    """

    # Inflate bags and tree with the tree decomp
    bags, tree = read_td(td_filename)

    # Recursively construct the peo
    peo = increment_peo(bags, tree, [])
    return peo


if __name__ == "__main__":
    inp = sys.argv[1]
    gr = sys.argv[2]
    bags, tree = read_td(inp)
    peo = generate_peo(inp)
    graph = []
    try:
        q = open(gr, "r")

        for line in q.readlines():
            graph.append(line)
    finally:
        q.close()

    del graph[:1]
    print(graph)

    print(peo)
    print(len(peo))
