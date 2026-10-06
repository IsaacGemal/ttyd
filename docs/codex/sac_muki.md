# sac_muki pooling experiments (2026-10-06, Codex GPT-6)

Limit: 35 source-build experiments. Baseline: 11/15 functions at 100%; main_point 95.12%, disp_3D 99.99%, disp_3D_alpha 98.39%, disp_2D 99.95%. main_cursor starts at 100%. `.sdata2` CONTENT: 18 DUP_OURS, no missing entries; `.rodata` CONTENT: 1 DUP_OURS (conversion double). Function order matches. Every experiment is compared against the full-precision baseline scores, and regressing candidates are discarded before delivery. Source snapshots and detailed results are under `build/codex_muki/`.

## Experiment 01: Native pooled strings, const vectors, no RO/base pointers or named conversion double

```text
MarioSt/battle/sac/sac_muki: 8/15 functions at 100%
   96.34%    648 B  main_icon
   87.27%    684 B  main_point
   99.98%    760 B  disp_3D
   88.39%    800 B  main_star
   98.39%    908 B  disp_3D_alpha
   98.39%   1592 B  main_cursor
   99.95%   1772 B  disp_2D
  100.00%     12 B  get_ptr
  100.00%     44 B  start_game
  100.00%     44 B  star_stone_appear
  100.00%    108 B  end_muki
  100.00%    132 B  wait_game_end
  100.00%    188 B  set_weapon
  100.00%    296 B  main_muki
  100.00%   1156 B  main_base
Regressions: main_cursor 100.00000000 -> 98.38693000, main_point 95.12281000 -> 87.27486000, main_icon 100.00000000 -> 96.33951000, main_star 100.00000000 -> 88.39500000, disp_3D 99.98947000 -> 99.98421000
.sdata2: CONTENT {'DUP_OURS': 18}
.rodata: ORDER {}
```

## Experiment 02: Replace 18 early duplicated floats by literals; volatile reads use const compound scalar literals

```text
MarioSt/battle/sac/sac_muki: 8/15 functions at 100%
   94.53%    296 B  main_muki
   87.40%    648 B  main_icon
   90.32%    684 B  main_point
   87.81%    760 B  disp_3D
   95.86%    800 B  main_star
   92.24%    908 B  disp_3D_alpha
   99.95%   1772 B  disp_2D
  100.00%     12 B  get_ptr
  100.00%     44 B  start_game
  100.00%     44 B  star_stone_appear
  100.00%    108 B  end_muki
  100.00%    132 B  wait_game_end
  100.00%    188 B  set_weapon
  100.00%   1156 B  main_base
  100.00%   1592 B  main_cursor
Regressions: main_muki 100.00000000 -> 94.52702000, main_point 95.12281000 -> 90.32164000, main_icon 100.00000000 -> 87.40124000, main_star 100.00000000 -> 95.85500000, disp_3D 99.98947000 -> 87.80526000, disp_3D_alpha 98.38766500 -> 92.23788500
.sdata2: CONTENT {'DUP_OURS': 1}
.rodata: CONTENT {'DUP_OURS': 1}
```

## Experiment 03: Literal duplicated floats except the named zero and one required by volatile MATCH_F32 reads

```text
MarioSt/battle/sac/sac_muki: 11/15 functions at 100%
   95.12%    684 B  main_point
   99.99%    760 B  disp_3D
   98.65%    908 B  disp_3D_alpha
   99.95%   1772 B  disp_2D
  100.00%     12 B  get_ptr
  100.00%     44 B  start_game
  100.00%     44 B  star_stone_appear
  100.00%    108 B  end_muki
  100.00%    132 B  wait_game_end
  100.00%    188 B  set_weapon
  100.00%    296 B  main_muki
  100.00%    648 B  main_icon
  100.00%    800 B  main_star
  100.00%   1156 B  main_base
  100.00%   1592 B  main_cursor
Regressions: none
.sdata2: CONTENT {'DUP_OURS': 2}
.rodata: CONTENT {'DUP_OURS': 1}
```

## Experiment 04: Combine nonregressing float cleanup with compound Vec templates and pooled strings/double

```text
MarioSt/battle/sac/sac_muki: 10/15 functions at 100%
   90.11%    684 B  main_point
   99.96%    760 B  disp_3D
   98.65%    908 B  disp_3D_alpha
   98.63%   1592 B  main_cursor
   99.95%   1772 B  disp_2D
  100.00%     12 B  get_ptr
  100.00%     44 B  start_game
  100.00%     44 B  star_stone_appear
  100.00%    108 B  end_muki
  100.00%    132 B  wait_game_end
  100.00%    188 B  set_weapon
  100.00%    296 B  main_muki
  100.00%    648 B  main_icon
  100.00%    800 B  main_star
  100.00%   1156 B  main_base
Regressions: main_cursor 100.00000000 -> 98.62562600, main_point 95.12281000 -> 90.10526000, disp_3D 99.98947000 -> 99.95789000
.sdata2: CONTENT {'DUP_OURS': 2}
.rodata: ORDER {}
```

## Experiment 05: Emit weapon strings before const vectors/tables; volatile vector reads preserve source copy order

```text
MarioSt/battle/sac/sac_muki: 9/15 functions at 100%
   96.34%    648 B  main_icon
   88.16%    684 B  main_point
   89.03%    800 B  main_star
   98.65%    908 B  disp_3D_alpha
   98.25%   1592 B  main_cursor
   99.34%   1772 B  disp_2D
  100.00%     12 B  get_ptr
  100.00%     44 B  start_game
  100.00%     44 B  star_stone_appear
  100.00%    108 B  end_muki
  100.00%    132 B  wait_game_end
  100.00%    188 B  set_weapon
  100.00%    296 B  main_muki
  100.00%    760 B  disp_3D
  100.00%   1156 B  main_base
Regressions: main_cursor 100.00000000 -> 98.24874000, main_point 95.12281000 -> 88.15790000, main_icon 100.00000000 -> 96.33951000, main_star 100.00000000 -> 89.02500000, disp_2D 99.95486000 -> 99.34086000
.sdata2: CONTENT {'DUP_OURS': 2}
.rodata: IDENTICAL {}
```

## Experiment 06: Correct pooled prefix with original four writable vector objects; direct const reads

```text
MarioSt/battle/sac/sac_muki: 11/15 functions at 100%
   87.30%    684 B  main_point
   98.65%    908 B  disp_3D_alpha
   98.39%   1592 B  main_cursor
   99.95%   1772 B  disp_2D
  100.00%     12 B  get_ptr
  100.00%     44 B  start_game
  100.00%     44 B  star_stone_appear
  100.00%    108 B  end_muki
  100.00%    132 B  wait_game_end
  100.00%    188 B  set_weapon
  100.00%    296 B  main_muki
  100.00%    648 B  main_icon
  100.00%    760 B  disp_3D
  100.00%    800 B  main_star
  100.00%   1156 B  main_base
Regressions: main_cursor 100.00000000 -> 98.38693000, main_point 95.12281000 -> 87.29825000
.sdata2: CONTENT {'DUP_OURS': 2}
.rodata: IDENTICAL {}
```

## Experiment 07: Diagnostic: original vector addressing using compiler-pooled first-string base (no RO macro)

```text
MarioSt/battle/sac/sac_muki: 12/15 functions at 100%
   95.12%    684 B  main_point
   98.65%    908 B  disp_3D_alpha
   99.95%   1772 B  disp_2D
  100.00%     12 B  get_ptr
  100.00%     44 B  start_game
  100.00%     44 B  star_stone_appear
  100.00%    108 B  end_muki
  100.00%    132 B  wait_game_end
  100.00%    188 B  set_weapon
  100.00%    296 B  main_muki
  100.00%    648 B  main_icon
  100.00%    760 B  disp_3D
  100.00%    800 B  main_star
  100.00%   1156 B  main_base
  100.00%   1592 B  main_cursor
Regressions: none
.sdata2: CONTENT {'DUP_OURS': 2}
.rodata: CONTENT {'MISSING': 4}
```

## Experiment 08: Literal BODY strings share the compiler pool with the alias-preserving vector base

```text
MarioSt/battle/sac/sac_muki: 11/15 functions at 100%
   95.12%    684 B  main_point
   98.65%    908 B  disp_3D_alpha
   99.55%   1592 B  main_cursor
   99.95%   1772 B  disp_2D
  100.00%     12 B  get_ptr
  100.00%     44 B  start_game
  100.00%     44 B  star_stone_appear
  100.00%    108 B  end_muki
  100.00%    132 B  wait_game_end
  100.00%    188 B  set_weapon
  100.00%    296 B  main_muki
  100.00%    648 B  main_icon
  100.00%    760 B  disp_3D
  100.00%    800 B  main_star
  100.00%   1156 B  main_base
Regressions: main_cursor 100.00000000 -> 99.54774000
.sdata2: CONTENT {'DUP_OURS': 2}
.rodata: IDENTICAL {}
```

## Experiment 09: Native vector source with explicit volatile word-copy shape in main_cursor

```text
MarioSt/battle/sac/sac_muki: 11/15 functions at 100%
   87.30%    684 B  main_point
   98.65%    908 B  disp_3D_alpha
   98.51%   1592 B  main_cursor
   99.95%   1772 B  disp_2D
  100.00%     12 B  get_ptr
  100.00%     44 B  start_game
  100.00%     44 B  star_stone_appear
  100.00%    108 B  end_muki
  100.00%    132 B  wait_game_end
  100.00%    188 B  set_weapon
  100.00%    296 B  main_muki
  100.00%    648 B  main_icon
  100.00%    760 B  disp_3D
  100.00%    800 B  main_star
  100.00%   1156 B  main_base
Regressions: main_cursor 100.00000000 -> 98.51005000, main_point 95.12281000 -> 87.29825000
.sdata2: CONTENT {'DUP_OURS': 2}
.rodata: IDENTICAL {}
```

## Experiment 10: No explicit base variable: inline pooled first-string vector offsets as diagnostic

```text
MarioSt/battle/sac/sac_muki: 11/15 functions at 100%
   85.76%    684 B  main_point
   98.65%    908 B  disp_3D_alpha
   99.56%   1592 B  main_cursor
   99.95%   1772 B  disp_2D
  100.00%     12 B  get_ptr
  100.00%     44 B  start_game
  100.00%     44 B  star_stone_appear
  100.00%    108 B  end_muki
  100.00%    132 B  wait_game_end
  100.00%    188 B  set_weapon
  100.00%    296 B  main_muki
  100.00%    648 B  main_icon
  100.00%    760 B  disp_3D
  100.00%    800 B  main_star
  100.00%   1156 B  main_base
Regressions: main_cursor 100.00000000 -> 99.56030000, main_point 95.12281000 -> 85.76023000
.sdata2: CONTENT {'DUP_OURS': 2}
.rodata: IDENTICAL {}
```

## Experiment 11: Move named zero/one definitions after code to prevent folded duplicate literals

```text
MarioSt/battle/sac/sac_muki: 10/15 functions at 100%
   95.12%    684 B  main_point
   99.99%    760 B  disp_3D
   98.65%    908 B  disp_3D_alpha
   99.76%   1156 B  main_base
   99.95%   1772 B  disp_2D
  100.00%     12 B  get_ptr
  100.00%     44 B  start_game
  100.00%     44 B  star_stone_appear
  100.00%    108 B  end_muki
  100.00%    132 B  wait_game_end
  100.00%    188 B  set_weapon
  100.00%    296 B  main_muki
  100.00%    648 B  main_icon
  100.00%    800 B  main_star
  100.00%   1592 B  main_cursor
Regressions: main_base 100.00000000 -> 99.75778000
.sdata2: ORDER {}
.rodata: CONTENT {'DUP_OURS': 1}
```

## Experiment 12: Reload zero only for the two main_base difference comparisons

```text
MarioSt/battle/sac/sac_muki: 10/15 functions at 100%
   95.12%    684 B  main_point
   99.99%    760 B  disp_3D
   98.65%    908 B  disp_3D_alpha
   99.31%   1156 B  main_base
   99.95%   1772 B  disp_2D
  100.00%     12 B  get_ptr
  100.00%     44 B  start_game
  100.00%     44 B  star_stone_appear
  100.00%    108 B  end_muki
  100.00%    132 B  wait_game_end
  100.00%    188 B  set_weapon
  100.00%    296 B  main_muki
  100.00%    648 B  main_icon
  100.00%    800 B  main_star
  100.00%   1592 B  main_cursor
Regressions: main_base 100.00000000 -> 99.30796000
.sdata2: ORDER {}
.rodata: CONTENT {'DUP_OURS': 1}
```

## Experiment 13: Keep zero early for main_base allocation; move only one after code

```text
MarioSt/battle/sac/sac_muki: 11/15 functions at 100%
   95.12%    684 B  main_point
   99.99%    760 B  disp_3D
   98.65%    908 B  disp_3D_alpha
   99.95%   1772 B  disp_2D
  100.00%     12 B  get_ptr
  100.00%     44 B  start_game
  100.00%     44 B  star_stone_appear
  100.00%    108 B  end_muki
  100.00%    132 B  wait_game_end
  100.00%    188 B  set_weapon
  100.00%    296 B  main_muki
  100.00%    648 B  main_icon
  100.00%    800 B  main_star
  100.00%   1156 B  main_base
  100.00%   1592 B  main_cursor
Regressions: none
.sdata2: CONTENT {'DUP_OURS': 1}
.rodata: CONTENT {'DUP_OURS': 1}
```

## Experiment 14: Typed load expression for late zero in main_base comparisons

```text
MarioSt/battle/sac/sac_muki: 10/15 functions at 100%
   95.12%    684 B  main_point
   99.99%    760 B  disp_3D
   98.65%    908 B  disp_3D_alpha
   99.76%   1156 B  main_base
   99.95%   1772 B  disp_2D
  100.00%     12 B  get_ptr
  100.00%     44 B  start_game
  100.00%     44 B  star_stone_appear
  100.00%    108 B  end_muki
  100.00%    132 B  wait_game_end
  100.00%    188 B  set_weapon
  100.00%    296 B  main_muki
  100.00%    648 B  main_icon
  100.00%    800 B  main_star
  100.00%   1592 B  main_cursor
Regressions: main_base 100.00000000 -> 99.75778000
.sdata2: ORDER {}
.rodata: CONTENT {'DUP_OURS': 1}
```

## Experiment 15: Verify whether pool_data pragma is needed for the retained float cleanup

```text
MarioSt/battle/sac/sac_muki: 11/15 functions at 100%
   95.12%    684 B  main_point
   99.99%    760 B  disp_3D
   98.65%    908 B  disp_3D_alpha
   99.95%   1772 B  disp_2D
  100.00%     12 B  get_ptr
  100.00%     44 B  start_game
  100.00%     44 B  star_stone_appear
  100.00%    108 B  end_muki
  100.00%    132 B  wait_game_end
  100.00%    188 B  set_weapon
  100.00%    296 B  main_muki
  100.00%    648 B  main_icon
  100.00%    800 B  main_star
  100.00%   1156 B  main_base
  100.00%   1592 B  main_cursor
Regressions: none
.sdata2: CONTENT {'DUP_OURS': 1}
.rodata: CONTENT {'DUP_OURS': 1}
```

## Experiment 16: Final cleanup: document remaining allocation constraints and group the late one definition with sdata2

```text
MarioSt/battle/sac/sac_muki: 11/15 functions at 100%
   95.12%    684 B  main_point
   99.99%    760 B  disp_3D
   98.65%    908 B  disp_3D_alpha
   99.95%   1772 B  disp_2D
  100.00%     12 B  get_ptr
  100.00%     44 B  start_game
  100.00%     44 B  star_stone_appear
  100.00%    108 B  end_muki
  100.00%    132 B  wait_game_end
  100.00%    188 B  set_weapon
  100.00%    296 B  main_muki
  100.00%    648 B  main_icon
  100.00%    800 B  main_star
  100.00%   1156 B  main_base
  100.00%   1592 B  main_cursor
Regressions: none
.sdata2: CONTENT {'DUP_OURS': 1}
.rodata: CONTENT {'DUP_OURS': 1}
```

## Validation build 17: natural compilation without MUST_MATCH

PASS; same compiler and flags, with MUST_MATCH and dependency emission removed.

## Final verification and retained result

Retained experiment 16 (experiment 15's implementation, with comments and the late `1.0f` definition grouped with `.sdata2`). There were 16 source experiments plus one natural validation compile, within the 35-build limit. All regressing candidates were discarded. No pool helper, new section declaration, new pragma, tools/configure edit, commit, or push was retained.

- `ninja -j2`: PASS; report and progress regenerated.
- Natural compilation with the matching compiler/flags but without `MUST_MATCH`: PASS (validation build 17).
- `python3 tools/sdata2_diff.py battle/sac/sac_muki`: `.sdata2` CONTENT with **1 DUP_OURS** (zero), down from **18**; no MISSING/EXTRA and no LAYOUT. `.rodata` remains CONTENT with its original **1 DUP_OURS** (conversion double).
- `python3 tools/reloc_value_check.py --partial sac_muki`: **0 findings**.
- `python3 tools/fn_order_check.py battle/sac/sac_muki`: **order matches**.
- `git diff --check`: PASS.
- Full-precision one-unit report comparison: all 15 functions remain at or above baseline. `disp_3D_alpha` improves **98.387665 -> 98.651985**; all other function scores are unchanged, including `main_cursor` **100** and `disp_3D` **99.989470**.

The data-section fuzzy score is not a content comparison: `.sdata2` fuzzy changes 98.24177 -> 73.62638 as the literal entries move and named duplicates disappear; its content checker improves by 17 duplicate entries. `.rodata` fuzzy is unchanged. The unit is not fully matched or linked.

Remaining constraints: zero must stay visible before the function definitions to preserve `main_base`'s FP register allocation. Moving zero after the code removes the final duplicate but lowers `main_base` to 99.757780 (experiment 11). Moving only one after the code removes its duplicate without changing any function. Replacing the existing `.rodata` prefix and BODY strings can make the pool IDENTICAL and `disp_3D` 100%, but direct const-vector reads hoist loads across destination stores in `main_cursor`/`main_point`; explicit first-string bases preserve alias scheduling but add a second base-address web when BODY strings become literals (99.547740 cursor in experiment 8). Const or volatile vector copies and compound literals did not satisfy the score gate in the tested forms. This is a bounded result, not a proof that another C shape cannot solve the remaining pools.
