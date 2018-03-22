#!/bin/bash

set -x
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

for graph in $DIR/../data/graphs/*; do
    $DIR/dockerCut.sh -f $graph -o $DIR/../data/angles/$(basename $graph).angle -a 0
done
