FROM debian:bookworm-slim

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        ca-certificates \
        gcc \
        make && \
    echo "deb [trusted=yes] https://crashoveride95.github.io/apt ./" > /etc/apt/sources.list.d/crashoveride95.list && \
    dpkg --add-architecture i386 && \
    apt-get update && \
    apt-get install -y \
        binutils-mips-n64 \
        gcc-mips-n64 \
        makemask \
        n64sdk \
        newlib-mips-n64 \
        root-compatibility-environment && \
    rm -rf /var/lib/apt/lists/*

ENV ROOT=/etc/n64
ENV PATH=${ROOT}/usr/sbin:${PATH}
ENV N64_SDK_PATH=/opt/crashsdk
ENV PATH=${PATH}:${N64_SDK_PATH}/bin
ENV N64_LIBGCCDIR=${N64_SDK_PATH}/lib/gcc/mips64-elf/12.2.0

WORKDIR ${N64_SDK_PATH}
