#!bin/bash

set -x

TIMEOUT=$2
ACTION=0
ANGLE=""

ACTION=$3
ANGLE=$4

function timeout_monitor(){
  sleep "$TIMEOUT"
  kill $1
}

CID=$(awk -F/ '{ print $NF }' /proc/1/cpuset)

IN="/graphs/$1"
OUT="/graphs/output:$CID.angle"

if [ ! -z $ANGLE ]; then
  ANGLE=/angles/$ANGLE
fi

./maxcutQAOA $IN 1 $ACTION $ANGLE $OUT & pid=$!

( timeout_monitor $pid ) & tmpid=$!

wait $pid

kill $tmpid


