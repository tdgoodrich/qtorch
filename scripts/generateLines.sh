#!/bin/bash

for graph in ~/qtorch/data/paceGraphs/*; do
    python ~/tensor-networks/contraction-suite/code/line_constructor.py ~/qtorch/data/paceGraphs/$(basename $graph) ~/qtorch/data/lines/$(basename $graph)
done 
