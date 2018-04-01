#!/bin/python3

import networkx as nx
import sys

qasmFile = sys.argv[1]
qasm = []
propTable = []
nodes = 0

try:
  q = open(qasmFile, "r")

  for line in q.readlines():
    qasm.append(line)

finally:
  q.close() 

qubits = int(qasm[0])
G = nx.Graph()

for i in range(qubits):
  G.add_node(nodes)
  propTable.append((nodes,nodes)) 
  nodes = nodes + 1


for i in range(1, len(qasm)):
  (gate, rest) = qasm[i].split(maxsplit=1)

  if gate == "H":
    G.add_node(nodes)
    G.add_edge(int(rest), nodes)
