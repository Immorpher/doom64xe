#!/bin/sh
# Builds libultra_rom.a from ModernN64SDK/libultra_modern with LTO bytecode
# embedded (-flto -ffat-lto-objects), so the Doom 64 link step can optimize
# across the game/libultra boundary.
#
# Fat objects keep the normal machine code next to the LTO bytecode, so the
# resulting archive still links fine without LTO (make LTO=0).
#
# Usage:
#   ./build-libultra-lto.sh                 install over /usr/lib/n64/libultra_rom.a
#                                           (needs root; the original is kept as
#                                           libultra_rom.nolto.a)
#   ./build-libultra-lto.sh -o /some/dir    just write libultra_rom.a into that dir
#
# Environment:
#   LIBULTRA_REF   branch/tag/commit of libultra_modern to build (default: its default branch)
#   N64_SDK_PATH   crashoveride95 SDK location (default: /opt/crashsdk)
set -eu

LIBULTRA_REF="${LIBULTRA_REF:-}"
N64_SDK_PATH="${N64_SDK_PATH:-/opt/crashsdk}"
LIBDIR=/usr/lib/n64
OUT=""

while getopts "o:" opt; do
    case "$opt" in
        o) OUT="$OPTARG" ;;
        *) echo "usage: $0 [-o outdir]" >&2; exit 1 ;;
    esac
done

# mips-n64-gcc-ar / the gcc LTO plugin live in the SDK bin directory
PATH="$PATH:$N64_SDK_PATH/bin"
export PATH

for tool in git make python3 mips-n64-gcc mips-n64-gcc-ar; do
    command -v "$tool" >/dev/null 2>&1 || { echo "error: '$tool' not found in PATH" >&2; exit 1; }
done

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

git clone -q https://github.com/ModernN64SDK/libultra_modern "$WORK/libultra_modern"
if [ -n "$LIBULTRA_REF" ]; then
    git -C "$WORK/libultra_modern" checkout -q "$LIBULTRA_REF"
fi
echo "Building libultra_modern $(git -C "$WORK/libultra_modern" rev-parse --short HEAD) with LTO"

# Command line variables override the Makefile's own OPT_FLAGS/AR assignments.
# The flags below are the Makefile's libultra_rom flags plus the LTO ones.
make -C "$WORK/libultra_modern" -j"$(nproc)" VERSION=libultra_rom \
    OPT_FLAGS="-Os -g -ggdb -flto=auto -ffat-lto-objects" \
    AR=mips-n64-gcc-ar

LIB="$WORK/libultra_modern/build/libultra_rom/libultra_rom.a"
[ -f "$LIB" ] || { echo "error: $LIB was not produced" >&2; exit 1; }

# Sanity check: the archive must really contain LTO bytecode
if ! mips-n64-objdump -h "$LIB" 2>/dev/null | grep -q "gnu.lto"; then
    echo "error: built libultra_rom.a has no LTO sections; the flags did not take effect" >&2
    exit 1
fi

if [ -n "$OUT" ]; then
    mkdir -p "$OUT"
    cp "$LIB" "$OUT/libultra_rom.a"
    echo "Wrote $OUT/libultra_rom.a"
else
    if [ ! -f "$LIBDIR/libultra_rom.nolto.a" ]; then
        cp "$LIBDIR/libultra_rom.a" "$LIBDIR/libultra_rom.nolto.a"
    fi
    cp "$LIB" "$LIBDIR/libultra_rom.a"
    echo "Installed $LIBDIR/libultra_rom.a (original saved as libultra_rom.nolto.a)"
fi
