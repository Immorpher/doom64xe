#!/bin/sh

export N64_LIBGCCDIR="/opt/crashsdk/lib/gcc/mips64-elf/12.2.0"
export PATH=$PATH:/opt/crashsdk/bin

make clean

rm ./Data/DOOM64.WAD.o
rm ./Data/DOOM64.WMD.o
rm ./Data/DOOM64.WSD.o
rm ./Data/DOOM64.WDD.o

mips-n64-ld -r -b binary ./Data/DOOM64.WAD -o ./Data/DOOM64.WAD.o
mips-n64-ld -r -b binary ./Data/DOOM64.WMD -o ./Data/DOOM64.WMD.o
mips-n64-ld -r -b binary ./Data/DOOM64.WSD -o ./Data/DOOM64.WSD.o
mips-n64-ld -r -b binary ./Data/DOOM64.WDD -o ./Data/DOOM64.WDD.o

mkdir build
mips-n64-gcc -c -G0 -Os -finline-functions-called-once -ffast-math -falign-functions=32 -g -mabi=32 -ffreestanding -mfix4300 -I/usr/include/n64 -I/usr/include/n64/PR -Iinclude -Ibuild -Ibuild/include -Isrc \
-D_FINALROM=1 -DNDEBUG=1 -DF3DEX_GBI_2=1 -MMD -MF build/cfb.d  -o build/cfb.o cfb.c

mips-n64-gcc -c -G0 -Os -finline-functions-called-once -ffast-math -falign-functions=32 -g -mabi=32 -ffreestanding -mfix4300 -I/usr/include/n64 -I/usr/include/n64/PR -Iinclude -Ibuild -Ibuild/include -Isrc \
-D_FINALROM=1 -DNDEBUG=1 -DF3DEX_GBI_2=1 -MMD -MF build/audio_heap.d  -o build/audio_heap.o audio_heap.c

make
