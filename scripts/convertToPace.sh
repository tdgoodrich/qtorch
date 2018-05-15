#!/bin/bash

for graph in $1/*; do
    python ~/qtorch/scripts/dgf-to-gr.py $graph $2/$(basename $graph .dgf).gr
done
