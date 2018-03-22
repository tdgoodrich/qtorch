#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
docker build --rm -t maxcut:latest -f $DIR/Dockerfile $DIR/../
