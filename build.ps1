$image="doom64ce/n64sdkmod:2"

if (!(docker images -q $image 2> $null)) { 
  docker build --no-cache -t $image .
}

docker run -it --rm --name doom64ce -v .:/usr/src -w /usr/src/doom64 $image ./make-rom.sh
