# Dead literals in retail pools: experiments

Question: retail `.rodata` keeps literals that no retail code references (evt_mobj: `MOBJ_Arrow`, `MOBJ_RideSwitchA/B`,
`MOBJ_trap`, `OFF_d_bom`, `MOBJ_BombRock`, `MOBJ_RedTornadoSwitch`), in codegen order between live functions' pools.
What source construct produces that?

Answer: **unused functions.** The original evt_mobj.c had event functions that no map calls. MWCC compiles them
normally, so their literals land in the pool at their codegen slot. mwld then deadstrips the functions from `.text`,
but it cannot strip the literals: under `#pragma pool_data on` the live functions address the pool through
`...rodata.0` (a size-0 label at the section start), and a reference to that label keeps the unit's entire pooled
`.rodata`. Unreferenced functions in our `.o` therefore do **not** block the link, and objdiff / agent_check ignore
them (they have no target symbol).

All tests: GC/2.6 with evt_mobj's cflags (`-O4,p -inline auto,deferred -rostr -sdata 48 -sdata2 6`, `pool_data on`
unless noted), compiled on the tower through wibo; scripts and test files live in the tower's
`~/ttyd-n4-dead/scratch/dead/` (`mw.sh` compile+dump, `lk.sh` mini mwld link with deadstrip, `secdiff.sh`
built-vs-target section bytes).

## Compiler: what pools a literal without .text (all negative)

Each test has a live function `g` and puts one dead literal `DEAD_A` in `f` between live literals.

| # | construct | DEAD_A pooled? | .text cost |
|---|---|---|---|
| t2 | unused `static const char* const tbl[] = {"A","B"}` | yes, at parse time (prefix) | none, but the 8-byte array object is emitted too |
| d1 | switch fallthrough dead stores `s="A"; s="B"; s="C"` | no | - |
| d2 | initialised local overwritten before use | no | - |
| d3 | literal after `return` | no | - |
| d8 | literal after `while (1)` | no | - |
| d4 | `if (x > 5 && x < 3) use("A")` | yes, in slot | branch kept in .text (not folded) |
| d5 | `switch (x & 1) { case 2: use("A"); }` | yes, in slot | case kept in .text |
| d6 | function-local `static const char s[] = "A"`, dead pointer | yes, but at parse time (first in pool) | none |
| d7 | unused local `char buf[16] = "A"` | yes, parse-time 16-byte template (prefix) | none |
| e1 | `static inline void dbg(const char*) {}` called with literal | no | - |
| e2 | `static inline const char* nm(void) { return "A"; }`, result discarded | no | - |
| e3 | `(void)"A";` | no | - |
| e4 | literal stored into a field of an unused local struct | no | - |
| e5 | variadic inline `dbgv("A", x)` (variadics are never inlined) | yes | out-of-line copy + call |
| e6 | inline wrapper `if (0) use(s)` | no | - |
| e7/e8 | conditional dead store / `?:` overwritten | no | - |
| h2/h4/h7 | `static inline` returning / using the literal, inlined and discarded (defined before or after the caller, nested) | no | - |
| h1/h3/h5/h6 | plain `static` function used only via inlinable call | yes | MWCC does not auto-inline it; the static function stays in .text |

Conclusion: MWCC drops every dead literal before pooling unless real code for it survives to codegen. The optimizer
runs before pooling and there is no "inlined then discarded, literal kept" path. Parse-time pooling (local static
arrays, char-array initialiser templates) lands in the prefix, not at a function's slot.

## Linker: what survives deadstrip (the actual mechanism)

| test | result |
|---|---|
| t5/t6 mini link (`lk.sh`, FORCEACTIVE `f` only) | unreferenced global `h` and static `name` are stripped from .text; their strings **stay**, because `f` references `...rodata.0` |
| t7 mini link, live function with only 2 strings (no pool base, direct `@NNN` relocs) | the dead function's string is **stripped**; its `.sdata2` float is stripped too |
| mot_jump (linked unit) + a dead global and a dead static function | `32 files OK`; agent_check 11/11 unchanged |
| evt_mobj with the six unused functions, unit flipped to Matching, linked with the six still-undefined `vec3_*` objects given their addresses in a copy of the LCF | the six functions are absent from the ELF; evt_mobj's linked `.rodata` (0x802C3DC8, 2576 bytes) is **byte-identical to retail main.dol**, all seven names in place |

So the retail strings survive only because the unit's pool is base-addressed; a dead float in `.sdata2` would not
survive (r2-relative, one symbol per constant). That fits retail: the dead entries in these units are `.rodata`
pool entries.

## Applied to evt_mobj

Started from main's evt_mobj.c plus codex-mobj's pool port (main's file is unchanged since codex branched), then
replaced codex's seven `pool_x()` helpers with the unused event functions the original evidently had. party_kuribo
and party_bomhei still test `animName` against `MOBJ_RideSwitchA/B`, `MOBJ_RedTornadoSwitch`, `MOBJ_BombRock` and
`MOBJ_Arrow`, so these objects existed:

- `evt_mobj_tornadoswitch_red` after `evt_mobj_floatswitch_red` (the blue set is switch/floatswitch/tornadoswitch;
  the red set lacked the third), body = `evt_mobj_tornadoswitch_blue` with `mobj_switch_red`.
- `evt_mobj_rideswitch_a`, `evt_mobj_rideswitch_b` before `evt_mobj_rideswitch_lightblue` (which uses
  `MOBJ_RideSwitchC`); reverse source order gives retail's C, B, A.
- `evt_mobj_arrow` after `evt_mobj_signboard`, signboard-style body.
- `evt_mobj_trap`, then `evt_mobj_bombrock` (`MOBJ_BombRock`, then `hitObjFlagOn("OFF_d_bom", 1)`) before
  `kpa_powerup`; text order gives retail's BombRock, OFF_d_bom, trap.

The bodies use no float or vector literals, so they add nothing to `.sdata2` or the parse-time prefix. They are
declared with the other `USER_FUNC`s in `include/evt/evt_mobj.h`. The bodies are reconstructions: only the
literal each one pools is constrained by retail.

Result: `.rodata` IDENTICAL (2576/2576), `.data`/`.sdata` IDENTICAL, `.sdata2` unchanged from codex-mobj (487 vs 288,
pre-existing), 81/95 functions at 100% with every score equal to codex-mobj's (79/95 on main), full build
`32 files OK`, non-MUST_MATCH compile clean.

## Same family elsewhere

- mot_roll (dead zero vec3s, (0,40,0), (0,-1,0)), evt_cam (unused format string, placeholder templates), evt_win:
  look for a missing sibling function (a debug print, the other half of a red/blue or on/off pair) and write it.
  Templates (`Vec v = {...}`, char-array initialisers) pool at parse time in source order, so a dead function's
  templates sit where its definition is in the source. Literals pool at its codegen slot (reverse source order).
- The unit must address its `.rodata` pool through `...rodata.0` (`pool_data on`, functions with several
  literals) for the dead entries to survive the link; check with `readelf -r` that some relocation targets `...rodata.0`.
- Keep float literals out of the reconstructed functions: each one adds an `.sdata2` entry to the `.o`, which
  objdiff counts as a data mismatch, even though the linker strips it. For the same reason, a dead `.sdata2`
  entry in retail cannot come from a dead function.
