<div align="center">
    <img src="./images/punch-logo.png" width="200" height="200">
    <h1>PUNCH</h1>
</div>

> Texas Hold'em where the pot is your health. Win the hand, punch someone.
> Lose it, guess whether to block.

## Run

Needs [CMake](https://cmake.org/download/) 3.20+ and a C++23 compiler
(GCC 13+, Clang 17+, MSVC 19.29+ / VS 2022).

```sh
cmake -S . -B build -DBUILD_TESTING=OFF
cmake --build build --config Release
```

Then:

| OS | command |
|---|---|
| Windows | `build\Release\punch.exe --start` |
| macOS / Linux | `./build/punch --start` |

Other commands: `--help`, `--version`, `--random-card`.

## Tests

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --config Release
ctest --test-dir build --build-config Release --output-on-failure
```

## Rules

Everyone starts at **100 HP** with **4 blocks**. Blinds are 5/10 HP.

Play a hand of Hold'em, except betting commits **health**, not chips.
Betting does not spend it, it puts it at risk: whatever you committed is
exactly what a punch costs you. Fold early and you are barely exposed;
ride a big raise and you are on the hook for all of it. Minimum punch is
10 HP, so anyone still standing is worth hitting.

The winner picks **one** target. At the same moment, everyone else
secretly picks block or no block.

| | blocked | did not block |
|---|---|---|
| **punched** | 0 damage, block kept | full damage |
| **not punched** | block burned | nothing |

Blocks are only lost by guessing wrong. Run out and you are wide open.
Last one standing wins.

[Implementation design](./docs/IMPLEMENTATION_DESIGN.md) ·
[Building](./docs/BUILDING.md)
