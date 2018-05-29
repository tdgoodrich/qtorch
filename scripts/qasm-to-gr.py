#!/bin/python

import sys

qasmFile = sys.argv[1]
grFile = sys.argv[2]
qasm = []
nodes = 0
print(qasmFile)

try:
    q = open(qasmFile, "r")

    for line in q.readlines():
        qasm.append(line)

finally:
    q.close()

qubits = int(qasm[0])
edgeList = []
last = []

for i in range(qubits):
    last.append(i)
    nodes += 1

for i in range(1, len(qasm)):
    (gate, rest) = qasm[i].split(" ", 1)

    if gate == "H":
        edgeList.append((int(rest), nodes))
        last[int(rest)] = nodes
        nodes += 1

    if gate == "CNOT":
        (a, b) = rest.split(" ", 1)
        aprime = last[int(a)]
        bprime = last[int(b)]
        edgeList.append((aprime, nodes))
        edgeList.append((bprime, nodes))
        last[int(a)] = nodes
        last[int(b)] = nodes
        nodes += 1

    if gate == "Rz":
        (a, b) = rest.split(" ", 1)
        bprime = last[int(b)]
        edgeList.append((bprime, nodes))
        last[int(b)] = nodes
        nodes += 1

    if gate == "Rx":
        (a, b) = rest.split(" ", 1)
        bprime = last[int(b)]
        edgeList.append((bprime, nodes))
        last[int(b)] = nodes
        nodes += 1

for i in range(qubits):
    edgeList.append((last[i], nodes))
    nodes += 1

edgeList = set(edgeList)

try:
    out = open(grFile, 'w')

    out.write("c Created from " + qasmFile + "\n")
    out.write("p tw " + str(nodes) + " " + str(len(edgeList)) + "\n")
    for edge in edgeList:
        out.write(str(edge[0] + 1) + " " + str(edge[1] + 1) + "\n")


finally:
    out.close()
