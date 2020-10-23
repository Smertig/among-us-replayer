<p align="center">
  <a href="https://store.steampowered.com/app/945360/Among_Us/"><img src="https://cdn.cloudflare.steamstatic.com/steam/apps/945360/header.jpg"></a>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/License-MIT-blue.svg" alt="License: MIT">
  <a href="https://github.com/Smertig/among-us-replayer/releases/latest"><img src="https://img.shields.io/github/v/release/Smertig/among-us-replayer.svg" alt="Latest Release"></a>
  <a href="https://github.com/Smertig/among-us-replayer/actions"><img src="https://github.com/Smertig/among-us-replayer/workflows/Build%20On%20Push/badge.svg" alt="GitHub Actions"></a>
</p>

# Among Us Replayer</b>

This repository contains source code of Replayer for [Among Us](https://store.steampowered.com/app/945360/Among_Us/) PC version. That's a tool for viewing `*.aurp` files that were captured by [Replay Mod](https://github.com/Smertig/among-us-replay-mod).

## Usage

### Recording game

- Check out [Among Us Replay Mod](https://github.com/Smertig/among-us-replay-mod) for detailed instruction

### Replaying game

- Download `among-us-replayer.zip` from [latest release](https://github.com/Smertig/among-us-replayer/releases/latest) page
- Unpack it
- Run `among-us-replayer.exe`

All the replays can be found at `/Steam/steamapps/common/Among Us/replay` folder.

## Replay Demo

[Youtube](https://youtu.be/WmfwYmQp_js)

## How To Build

Requirements:
- Windows
- C++17 compiler (tested on MSVC 16.7.5)
- CMake 3.17
- [Conan](https://conan.io/downloads.html) (C++ package manager)

```shell script
## in repo directory

# 1. Create build dir
mkdir build && cd build

# 2. Configure CMake project
cmake .. -DCMAKE_BUILD_TYPE=Release -A Win32

# 3. Build project
cmake --build . --config Release --target among-us-replayer

# 4. Success! Your replayer is at `bin/among-us-replayer.exe`
```

## License

- MIT
