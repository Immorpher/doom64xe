FROM debian:bookworm-slim AS sdk

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

# Rebuild libultra_rom.a with LTO bytecode. This happens in a throwaway stage so
# git/python3 and the libultra sources do not end up in the final image.
FROM sdk AS libultra-lto

RUN apt-get update && \
    apt-get install -y --no-install-recommends git python3 && \
    rm -rf /var/lib/apt/lists/*

# Empty = default branch. Pin to a commit hash for reproducible builds
ARG LIBULTRA_REF=
ENV LIBULTRA_REF=${LIBULTRA_REF}

COPY build-libultra-lto.sh /usr/local/bin/build-libultra-lto.sh
RUN /usr/local/bin/build-libultra-lto.sh -o /out

FROM sdk

COPY --from=libultra-lto /out/libultra_rom.a /usr/lib/n64/libultra_rom.a

WORKDIR ${N64_SDK_PATH}
