#!/bin/bash

set -x
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

for graph in $DIR/../data/graphs/*; do
    $DIR/dockerCut.sh -f $graph -o $DIR/../data/solutions/$(basename $graph).cut -a 1 -n $DIR/../data/angles/$(basename $graph).angle
done
