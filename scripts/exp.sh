#!/bin/bash


for reg in 3 4 5; do
    for nodes in 10 12 14 16 18 20 22; do
        for num in 0 1 2 3 4; do
            for alg in quickbb meiji-e freetdi; do
                graph=$reg
                graph+=regRand$nodes
                graph+=Node$num
                echo $graph
                ~/tensor-networks/contraction-suite/code/dab/single.sh -v -f ~/qtorch/data/paceGraphs/$graph.gr -t 10m -o ~/qtorch/data/tds/$graph.td -a $alg --validate 
            done
        done
    done
done


