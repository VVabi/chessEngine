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

## Testing changes with self-play

`tools/selfplay/selfplay.py` plays a match between two engine binaries and reports W/L/D, Elo difference with 95% error bars, likelihood of superiority (LOS) and optionally an SPRT. Engine 1 is the candidate, engine 2 the baseline.

Set up the Python environment once (needs conda):

```sh
conda env create -f tools/selfplay/environment.yml
conda activate chessengine
```

Keep a copy of the baseline binary before making changes, then for example:

```sh
cp Cpp/Release/Vabi ~/Vabi-baseline
# ... change code, rebuild ...
python tools/selfplay/selfplay.py --engine1 Cpp/Release/Vabi --engine2 ~/Vabi-baseline \
    --tc 10+0.1 --games 1000 --concurrency 6 --sprt 0 5 --pgn games.pgn
```

- Time control: `--tc BASE+INC` (seconds), `--movetime SEC`, `--depth N` or `--nodes N`
- `--concurrency/-j N` games in parallel (keep it at or below the number of physical cores)
- `--openings FILE` FEN/EPD start positions (default `ChessHelper/src/Tools/openingPositions.txt`); each opening is played with both colors
- `--sprt ELO0 ELO1` stops as soon as the result is statistically clear; see `--help` for all options
