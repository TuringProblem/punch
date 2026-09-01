# Include Paths

## The change

`Makefile` gained:

```make
INCLUDES := -I./include -I./src
```

...added to every `clang++` invocation.

## Why

Headers live in two directories:

```
include/    cards.hpp, cardpair.hpp, player.hpp, plays.hpp, engine.hpp, screen.hpp
src/        main.cpp, menu.hpp
```

Before this change the compile line had no `-I` flag:

```
clang++ ./src/main.cpp -std=c++23 -o ./build/main
```

So `#include "cards.hpp"` from inside `src/menu.hpp` failed to resolve.

### How `#include "..."` actually resolves

For quoted includes, the preprocessor searches, in order:

1. The directory of the file doing the including — **not** the current working directory, and **not** the directory of the top-level `.cpp`.
2. Each `-I` directory, left to right.
3. System include paths.

`src/menu.hpp` including `"cards.hpp"` therefore looked for `src/cards.hpp`, which does not
exist. `include/` was never searched, because nothing told the compiler it existed.

This also explains why `include/cardpair.hpp` could already do `#include "cards.hpp"` without
any flags: step 1 found `include/cards.hpp` as a sibling in the same directory. The bug only
showed up when crossing the `src/` ↔ `include/` boundary.

With `-I./include` present, step 2 finds it from anywhere in the project.

## This is not a linker flag

Easy to conflate, since one `clang++` command runs preprocess → compile → link and all flags
sit on the same line. They target different stages:

| Flag | Stage | Purpose |
| --- | --- | --- |
| `-I<dir>` | Preprocessor | Where to find header files for `#include` |
| `-L<dir>` | Linker | Where to find library files (`.a`, `.dylib`) |
| `-l<name>` | Linker | Which library to link against |

The linker was never involved in this failure. The build died at preprocessing, before any
object code existed. And because these headers are header-only (everything is `inline`,
`constexpr`, a lambda, or an `enum class`), there are no out-of-line definitions in separate
translation units — nothing to link. `-I` is the whole fix.

## Consequence for how we write includes

Use project-root-relative paths from the include roots, not `../`:

```cpp
#include "cards.hpp"        // good — resolved via -I./include
#include "../include/cards.hpp"  // avoid — breaks if the file moves
```

## Editors / clangd

`compile_commands.json` must carry the same `-I` flags, or clangd reports
`'cards.hpp' file not found` in the editor even when `make` succeeds. If you regenerate that
file, make sure the flags survive.
