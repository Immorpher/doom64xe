#!/bin/bash

IMAGE="doom64ce/n64sdkmod:2"

if [[ "$(docker images -q $IMAGE 2> /dev/null)" == "" ]]; then
  docker build -t $IMAGE .
fi

docker run -it --rm --name doom64ce -v "$(pwd)":/usr/src -w /usr/src/doom64 $IMAGE ./make-rom.sh
