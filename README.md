Paper Mario: The Thousand-Year Door
===================================

A complete matching decompilation of *Paper Mario: The Thousand-Year Door* (GameCube, Japanese release `G8MJ01`).

**Status: 100% matched.** All 9,770 functions in the main executable and its REL modules match, and the build
links into a DOL and RELs that are byte-for-byte identical to the original game. Seven of them stay in
assembly because the original was hand-written assembly or uses instructions the compiler can't produce from C;
each is listed with its evidence in `tools/asm_exceptions.json`.

> [!NOTE]
> This is an AI-assisted fork of [doldecomp/ttyd](https://github.com/doldecomp/ttyd). Everything up to the fork
> point is theirs: NWPlayer123 and the other doldecomp contributors built the project, its tooling and the first
> stretch of matched code, and released it under the MIT license.
>
> The work added here (matching the rest of the game, then cleaning up the code) was done by AI agents, directed by
> Isaac Gemal. A function was accepted only when the build checker confirmed it compiled to the exact bytes of the
> original. Nothing from this fork is submitted upstream.

This repository does **not** contain any game assets. You need your own copy of the game to build it.

Supported versions:

- `G8MJ01`: Rev 0 (JP)

Readable build
==============

Matching a 2004 compiler byte for byte sometimes needs code shaped for the compiler rather than for people: a
temporary that steers register allocation, a type pun, a `#pragma`. Every such trick sits under `#ifdef MUST_MATCH`,
with the plain C version under `#else`:

```c
#ifdef MUST_MATCH
    // The copy through t places the template on the stack where the original has it.
    { Vec t = tmpl; t.x = px; t.y = py; t.z = pz; v = t; }
#else
    v = tmpl;
    v.x = px; v.y = py; v.z = pz;
#endif
```

- `python configure.py` builds the matching game (`MUST_MATCH` defined).
- `python configure.py --non-matching` compiles the readable version of every file instead. It is not byte-identical
  and doesn't link, but it's the code to read, port or mod.

Cleanup is tracked with `tools/cleanup_index.py`, which counts the same categories as
[Melee's cleanup index](https://doldecomp.github.io/melee/cleanup/): raw offsets where a field should be, pointer
casts covering unknown types, `MUST_MATCH` regions, pragmas, `goto`s, and so on. `python tools/cleanup_index.py --top 20`
lists the files with the most left to clean.

Cleanup
=======

Matching proves the code is right, not that it's readable. After the game reached 100% on 2026-10-18, the work
switched to cleanup, under one constraint: every change keeps the build byte-identical.

What that covered:
- **Naming and typing.** Raw byte offsets became struct fields, `void*` work pointers got their real types, and
  data tables stored as raw words became structs with pointer members. Names only where there's evidence; otherwise
  `field_0xNN`.
- **Hacks fenced off.** Compiler-steering tricks moved under `MUST_MATCH` with a plain `#else`, and many were
  replaced outright by natural code that happens to compile to the same bytes. Whole-file `#pragma`s became per-file
  compiler flags, as Wind Waker, Twilight Princess and Animal Crossing do.
- **Scripts and data.** Every event script is written with the EVT macros instead of hex words, and named pool
  constants that existed only to pin data order became plain literals.

The cleanup index (Melee's categories, weighted per thousand lines of code) since it was adopted on 2026-10-22:

| Category | 2026-10-22 | Now |
|---|---|---|
| Index (weighted, per kLOC) | 28.9 | 17.0 |
| Pointer-to-int casts | 2,694 | 32 |
| Struct pointer casts | 2,708 | 1,245 |
| Pointer type puns (`*(T*)&x`) | 1,180 | 489 |
| Address-named symbols | 2,632 | 1,077 |
| `MUST_MATCH` regions | 834 | 703 |
| Optimisation pragmas | 606 | 325 |
| `goto` | 106 | 102 |

What's left is mostly naming (4,624 `field_0xNN` members whose meaning nobody has proven yet) and casts that the
game's own APIs require, such as event-script arguments, which are always integers. The full history, with what
each pass did and why, is in [`docs/CONTEXT.md`](docs/CONTEXT.md).

Documentation
=============

- [`docs/CONTEXT-matching.md`](docs/CONTEXT-matching.md): the ledger of the matching phase, from the fork to 100%:
  what each wave of agents did, what worked and what didn't.
- [`docs/CONTEXT.md`](docs/CONTEXT.md): the cleanup phase, plus the standing rules (what counts as a hack, how things
  get named, what may change).
- [`docs/CLEANUP_GUIDE.md`](docs/CLEANUP_GUIDE.md) and [`docs/AGENT_BRIEF.md`](docs/AGENT_BRIEF.md): the working
  rules for cleanup and matching.

Names follow outside evidence only: the symbol map inherited from upstream, strings in the binary, SDK names, or the
[Super Paper Mario decomp](https://github.com/SeekyCt/spm-decomp), which shares much of TTYD's engine. Struct
members with unknown meaning stay `field_0xNN` rather than getting a guessed name.

Checks
======

`python tools/ci.py` runs what every change must pass: the full build must recreate `build/G8MJ01/ok` (byte
identity), every changed file must compile without `MUST_MATCH`, initialised data must keep its exact bytes
(`tools/data_check.py`), and lints for new pragmas, unguarded function-pointer casts and prototypes in `.c` files.
It also prints the cleanup index and warns if a change raises it.

Dependencies
============

Windows
--------

On Windows, it's **highly recommended** to use native tooling. WSL or msys2 are **not** required.  
When running under WSL, [objdiff](#diffing) is unable to get filesystem notifications for automatic rebuilds.

- Install [Python](https://www.python.org/downloads/) and add it to `%PATH%`.
  - Also available from the [Windows Store](https://apps.microsoft.com/store/detail/python-311/9NRWMJP3717K).
- Download [ninja](https://github.com/ninja-build/ninja/releases) and add it to `%PATH%`.
  - Quick install via pip: `pip install ninja`

macOS
------

- Install [ninja](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages):

  ```sh
  brew install ninja
  ```

- Install [wine-crossover](https://github.com/Gcenx/homebrew-wine):

  ```sh
  brew install --cask --no-quarantine gcenx/wine/wine-crossover
  ```

After OS upgrades, if macOS complains about `Wine Crossover.app` being unverified, you can unquarantine it using:

```sh
sudo xattr -rd com.apple.quarantine '/Applications/Wine Crossover.app'
```

Linux
------

- Install [ninja](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages).
- For non-x86(_64) platforms: Install wine from your package manager.
  - For x86(_64), [wibo](https://github.com/decompals/wibo), a minimal 32-bit Windows binary wrapper, will be automatically downloaded and used.

Building
========

- Clone the repository:

  ```sh
  git clone https://github.com/IsaacGemal/ttyd.git
  ```

- Copy your game's disc image to `orig/G8MJ01`.
  - Supported formats: ISO (GCM), RVZ, WIA, WBFS, CISO, NFS, GCZ, TGC
  - After the initial build, the disc image can be deleted to save space.

- Configure:

  ```sh
  python configure.py
  ```

  To use a version other than `G8MJ01` (JP Rev 0), specify it with `--version`.

- Build:

  ```sh
  ninja
  ```

Diffing
=======

Once the initial build succeeds, an `objdiff.json` should exist in the project root.

Download the latest release from [encounter/objdiff](https://github.com/encounter/objdiff). Under project settings, set `Project directory`. The configuration should be loaded automatically.

Select an object from the left sidebar to begin diffing. Changes to the project will rebuild automatically: changes to source files, headers, `configure.py`, `splits.txt` or `symbols.txt`.

![](assets/objdiff.png)

License
=======

MIT, as released by the upstream project; see [`LICENSE`](LICENSE). Paper Mario: The Thousand-Year Door is © Nintendo
and Intelligent Systems. This project contains no game data.
