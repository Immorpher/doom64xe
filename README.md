# DOOM 64 XE

Doom 64 XE (eXpanded Edition) is an interation of the Doom 64 Reverse Engineering source built upon four repositories:

Erick194's complete reverse engineering of Doom 64: [DOOM 64 RE](https://github.com/Erick194/DOOM64-RE/)\
Molecicco's enhancements, additions, and bug fixes: [DOOM 64 CE](https://github.com/azamorapl/DOOM64-RE/tree/romhack)\
JNMartin's update to modern SDK and optimizations: [DOOM 64 Merciless Edition](https://github.com/jnmartin84/Doom-64-Merciless-Edition/tree/modern)\
Nova's fork of Merciless Edition additional enhancements: [DOOM 64 Ultra](https://github.com/thekovic/doom64ultra)

The focus of this project is to combine the features of the aforementioned repositories to create a demo-compatible, modding-friendly, port with quality of life features.

## Building

### Preparation

Before compiling, the data of the original Doom 64 is required. The Tools folder contains an extractor utility that can be run against the [Doom 64 USA 1.0 ROM](https://datomatic.no-intro.org/index.php?page=show_record&s=24&n=0179). Version 1.1 is not supported. The extractor will produce four files: `DOOM64.WAD`, `DOOM64.WMD`, `DOOM64.WSD` and `DOOM64.WDD` that have to be placed into the doom64/data folder.

**This rom hack requires resources that are only present in Nightdive's 2020 release.** For this reason, a BPS patch is included in the Data folder (`DOOM64.BPS`). The Tools folder includes Floating BPS, a tool that can be used to open it. With it, you must patch the `DOOM64.WAD` found in your 2020 release installation directory (MD5 `0AABA212339C72250F8A53A0A2B6189E`). Then, copy the newly created `DOOM64.WAD` into the Data folder, replacing the one in the previous step.

Alternatively, you can build the rom disabling the new options that require the new resources and use the extracted wad instead. See [Configuration](#configuration) for information on how to disable the `ENABLE_EXTRA_EPISODES` and `ENABLE_REMASTER_SPRITES` preprocessor definitions.

### Modern SDK

If you are running on a Linux distribution supported by the [Modern N64 SDK](https://crashoveride95.github.io/modernsdk/index.html) ([source](https://github.com/ModernN64SDK/n64sdkmod)) and have it installed, run `make-rom.sh` inside the `doom64` directory.

If you prefer to run the build inside a container, a Dockerfile is included that builds an image with only the files required to build this project (~460MB). You can then mount the `doom64` directory and run `make-rom.sh`. Alternatively, you can run `build.sh` or `build.ps1` (for Windows users) to do it automatically.

### Link time optimization (LTO)

The build uses LTO (`-flto=auto`) by default. Two things are needed for it to work:

- **Game code**: nothing to do, `doom64/Makefile` handles it and links through the gcc driver so the LTO plugin is used. `make LTO=0` restores the old non-LTO build.
- **libultra**: for the optimizer to see across the game/libultra boundary, `libultra_rom.a` has to be rebuilt with `-flto -ffat-lto-objects`. The Dockerfile does this automatically (the image tag is `:2` so an old non-LTO image is not reused). Outside Docker, run `./build-libultra-lto.sh` once as root; it installs the LTO build over `/usr/lib/n64/libultra_rom.a` and keeps the original as `libultra_rom.nolto.a`. With a stock libultra the game still builds, only libultra stays opaque to the optimizer.

If the build was previously done in a different directory, run `make clean` first (`make-rom.sh` already does).

If using gdb to debug `doom64.elf`, change the `DISCARD` section inside `doom64.ld` from `*(*);` to `*(.MIPS.abiflags);` before building the rom.

## Configuration

You can add the following preprocessor definitions to the `DEFINES` section of the Makefile:

- `ENABLE_EXTRA_EPISODES=0`, disables the episode selection UI. Useful if you are building a custom rom and have deleted the Lost Levels from the wad.
- `ENABLE_REMASTER_SPRITES=0`, disables the option to toggle green blood and use alternate medikit sprites. Useful if building the rom without the extra sprites from the 2020 rerelease.
- `ENABLE_NIGHTMARE=0`, disables the fifth difficulty.

## Notes

Special thanks to Erick194, Molecicco, JNMartin, Nova and Kovic for making all of this possible.
