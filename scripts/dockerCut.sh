#!/bin/sh
set -x
FILENAME=""
OUTPUT=""
TIMEOUT="10m"
ACTION=0
ANGLE=""

while [ ! $# -eq 0 ]
do
  case "$1" in
    -f|--filename)
      FILENAME=$2
      shift
    ;;
    -t|--timeout)
      TIMEOUT=$2
      shift
    ;;
    -o|--output)
      OUTPUT=$2
      shift
    ;;
    -a|--action)
      ACTION=$2
      shift
    ;;
    -n|--angle)
      ANGLE=$2
      shift
    ;;
  esac
  shift
done

if [[ ! -e $FILENAME ]]
  then
  echo "Input file does not exist"
  exit 1
fi

if [[ -z $OUTPUT ]]
  then
  echo "No output file specified"
  exit 1
fi

GRAPH=$(basename $FILENAME)

if [[ ! -z $ANGLE ]]; then
  ANGLE_MOUNT="-v=$(dirname $ANGLE):/angles/"
  ANGLE_BASE=$(basename $ANGLE)
else
  ANGLE_MOUNT=""
  ANGLE_BASE=""
fi

CID=$(docker run -d -v=$(dirname $FILENAME):/graphs/ \
  $ANGLE_MOUNT \
  maxcut /run.sh $GRAPH $TIMEOUT $ACTION $ANGLE_BASE )

docker wait $CID

cp $(dirname $FILENAME)/output:$CID.angle $OUTPUT

rm -f $(dirname $FILENAME)/output:$CID.angle



