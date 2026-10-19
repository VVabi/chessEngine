# chessEngine

An alpha/beta-pruning based chessengine. Playing strength should be ~2500 Elo.

## Building

The engine is built with CMake (>= 3.21) and a C++14 compiler. Only tested on Linux with gcc; cross-compiling for Windows with MinGW-w64 is supported via a preset.

```sh
cd Cpp
cmake --preset Release
cmake --build --preset Release -j
./Release/Vabi
```

Available presets (each builds into `Cpp/<preset>/Vabi`):

| Preset                | Description                                      |
|-----------------------|--------------------------------------------------|
| `Debug`               | `-O0 -g3`, `DEBUG` consistency checks, pedantic  |
| `Release`             | `-O3`                                            |
| `TestRelease`         | `Release` + `-funroll-loops`                     |
| `uciChallengeRelease` | `TestRelease` + `RANDOMEVAL` + `EXPERIMENTAL`    |
| `uciDefenderRelease`  | `TestRelease` + `RANDOMEVAL`                     |
| `Windows`             | `Release` cross-compiled with `x86_64-w64-mingw32-g++` |

Without presets, the usual CMake workflow works too; the `VABI_*` options in `Cpp/CMakeLists.txt` toggle the individual defines and flags:

```sh
cmake -S Cpp -B build -DCMAKE_BUILD_TYPE=Release -DVABI_RANDOMEVAL=ON
cmake --build build -j
```
