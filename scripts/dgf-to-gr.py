#!/bin/python

import sys

dgfFile = sys.argv[1]
grFile = sys.argv[2]
dgf = []
nodes = 0
gr = []
comments = []

try:

    with open(dgfFile, "r") as f:
        dgf = list(f)

finally:
    f.close()

for i in range(len(dgf)):
    if dgf[i][0] == "e":
        gr.append( dgf[i][1:].rstrip().lstrip() )
    elif dgf[i][0] == "c":
        comments.append(dgf[i])

for i in range(len(gr)):
    j = max(list(map(int, gr[i].split(" "))))
    if j+1 > nodes:
        nodes = j + 1

    gr[i] = ( [x+1 for x in list(map(int, gr[i].split(" ")))] )

comments.append("p tw " + str(nodes) + " " + str(len(gr)) + "\n")

try:
    with open(grFile, "w") as f:
        f.write("".join(comments))
        for edge in gr:
            f.write(" ".join(list(map(str, edge))) + "\n")

finally:
    f.close()
