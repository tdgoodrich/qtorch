#!/bin/bash

set -x

INPUT=$1
OUTPUT=$2
EDGE_PRE="e "
COMM_PRE="p "
EDGES=0
NODES=0

IFS=$'\n'

for line in $(cat < $1) ; do
  if [[ $line == $COMM_PRE* ]]; then
    IFS=$' '
    for num in $line; do
        if (($num + 1 > $NODES )); then
        $NODES=$((num + 1))
      fi
    done
    echo ${line#$COMM_PRE} >> "c $OUTPUT"
  elif [[ $line == $EDGE_PRE* ]]; then
    IFS=$' '
    for num in $line; do
        if (($num + 1 > $NODES )); then
        NODES=$((num + 1))
      fi
    done
    EDGES=$((EDGES+1 ))
    echo ${line#$EDGE_PRE} >> $OUTPUT
  fi
done

echo "p tw $NODES $EDGES" | cat - $OUTPUT > temp && mv temp $OUTPUT
