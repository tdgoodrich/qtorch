#!./env/bin/python

import networkx as nx
import sys

regularity = int(sys.argv[1])
vertices = int(sys.argv[2])
seed = int(sys.argv[3])
number = int(sys.argv[4])
output_dir = sys.argv[5]

for n in range(number):
    G = nx.random_regular_graph( regularity, vertices, seed + n )
    filename = '{}regRand{}Node{}.dgf'.format(regularity, vertices, seed + n)
    f = open(output_dir + filename, 'w+')
    f.write('c {}-regular random {}-node graph generated by NetworkX with seed {}\n'.format(regularity, vertices, seed + n))

    for edge in nx.generate_edgelist(G, data=False):
        f.write('e {}\n'.format(edge))

    f.close()

