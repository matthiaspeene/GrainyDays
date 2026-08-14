# Rain Linux Build Instructions

This directory contains the Projucer-generated Makefile for building the Rain
VST3 plug-in and standalone application on Linux.

## Prerequisites

- A C++20-compatible compiler (`g++` or `clang++`)
- GNU Make
- `pkg-config`
- JUCE 9.0.1 modules
- The `melatonin_perfetto` user module
- The Perfetto SDK

On Fedora, install the required build and development packages with:

```sh
sudo dnf install \
  gcc-c++ \
  make \
  pkgconf-pkg-config \
  alsa-lib-devel \
  freetype-devel \
  fontconfig-devel \
  libglvnd-devel \
  libcurl-devel \
  gtk3-devel \
  webkit2gtk4.1-devel
```

On Debian or Ubuntu, install the required build and development packages with:

```sh
sudo apt update
sudo apt install \
  build-essential \
  pkg-config \
  libasound2-dev \
  libfreetype6-dev \
  libfontconfig1-dev \
  libgl1-mesa-dev \
  libegl1-mesa-dev \
  libcurl4-openssl-dev \
  libgtk-3-dev \
  libwebkit2gtk-4.1-dev
```

If `libwebkit2gtk-4.1-dev` is unavailable on your distribution, install
`libwebkit2gtk-4.0-dev` instead. The Makefile detects either version.

## Source layout

The generated Makefile currently expects this layout relative to the JUCE
directory:

```text
JUCE/
├── modules/
├── third_party/perfetto/sdk/
├── usermodules/melatonin_perfetto/
└── projects/GrainyDays/Rain/
```

In particular, the following files and directories must be available:

- `JUCE/modules/`
- `JUCE/modules/juce_audio_processors_headless/format_types/VST3_SDK/`
- `JUCE/third_party/perfetto/sdk/perfetto.cc`
- `JUCE/third_party/perfetto/sdk/perfetto.h`
- `JUCE/usermodules/melatonin_perfetto/`

The Makefile contains absolute JUCE module paths generated on the original
machine. If your JUCE checkout is elsewhere, open `Rain.jucer` in Projucer,
update the Linux exporter module paths, and save the project to regenerate this
directory. Do not edit the generated Makefile directly because Projucer will
overwrite it.

## Build

From the repository root:

```sh
cd Rain/Builds/LinuxMakefile
make CONFIG=Debug -j"$(nproc)"
```

For an optimized release build:

```sh
make CONFIG=Release -j"$(nproc)"
```

Build only one plug-in format by naming its target:

```sh
make CONFIG=Debug VST3 -j"$(nproc)"
make CONFIG=Debug Standalone -j"$(nproc)"
```

Add `V=1` to show full compiler commands when diagnosing a build problem:

```sh
make CONFIG=Debug V=1
```

## Build outputs

Artifacts are written to `Rain/Builds/LinuxMakefile/build/`:

- `build/Rain` — standalone application
- `build/Rain.vst3/` — VST3 plug-in bundle

The generated Makefile also copies a successful VST3 build to `~/.vst3/`,
where most Linux plug-in hosts can discover it. Restart or rescan your host
after rebuilding. If that copy fails because the destination is not writable,
the build artifact is still available locally and can be installed manually:

```sh
mkdir -p ~/.vst3
cp -R build/Rain.vst3 ~/.vst3/
```

## Clean

```sh
make CONFIG=Release clean
```

Use `CONFIG=Debug` instead to clean the Debug intermediates. A clean rebuild is
required after moving or deleting a header because generated `.d` dependency
files can otherwise retain the old path:

```sh
make CONFIG=Release clean
make CONFIG=Release -j"$(nproc)"
```

## Troubleshooting

- **A package is missing:** run the failing `pkg-config` command shown by
  `make V=1`, then install the corresponding development package.
- **`No rule to make target` names a moved or deleted header:** remove the
  stale dependency files with the clean-and-rebuild commands above.
- **A JUCE or user-module header is missing:** regenerate the Makefile from
  `Rain/Rain.jucer` after correcting the exporter module paths in Projucer.
- **Perfetto files are missing:** make sure the Perfetto SDK is present at
  `JUCE/third_party/perfetto/sdk/`, or update its path in Projucer and
  regenerate the project.
- **The VST3 does not appear in a host:** confirm that `~/.vst3/Rain.vst3`
  exists, then force the host to rescan its plug-ins.
