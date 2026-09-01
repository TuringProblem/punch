# Building PUNCH

## Requirements

C++23, specifically `std::format`. That is the tightest constraint:

| toolchain | minimum |
|---|---|
| GCC       | 13 |
| Clang     | 17 (or 14+ paired with libstdc++ 13) |
| Apple Clang | 15 (Xcode 15) |
| MSVC      | 19.29 (VS 2019 16.10) |

CMake 3.20+.

## Any platform

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Binary lands at `build/punch`, or `build/Release/punch.exe` on Windows.

## Tests

```sh
cd build && ctest --output-on-failure
```

Six suites: deck integrity, hand evaluator (checked against published
Hold'em frequencies), tie redraws, punch resolution, the betting engine,
and full games played to a knockout.

## Makefile

The Makefile still works on macOS and Linux for a quick loop:

```sh
make        # build to ./build/main
make run
```

It is not portable to Windows. Use CMake there.

## Platform notes

Windows needs two things set at startup, both handled by `initConsole()`
in `include/platform.hpp`, called from `main`:

- `SetConsoleOutputCP(CP_UTF8)` so the card glyphs are not mangled
- `ENABLE_VIRTUAL_TERMINAL_PROCESSING` so the ANSI screen clear works

MSVC also needs `/utf-8` at compile time, since the source files contain
UTF-8 card glyphs and MSVC otherwise assumes the local codepage. CMake
sets this.

Even with the codepage correct, the playing card glyphs (U+1F0A1 and up)
need a font that actually has them. Windows Terminal with Cascadia Mono
does not, and will show boxes. That is a font problem, not an encoding
one, and it is the reason to keep an ASCII card fallback.
