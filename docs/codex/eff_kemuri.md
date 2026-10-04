# eff_kemuri continuation report

The task is not complete. `configure.py` remains **NonMatching**, unchanged from the starting state. The retained source builds with all **6/6 functions at 100%**. The normal source branch also compiles without `MUST_MATCH`.

## Retained improvements

- The unit now defines its color templates, float constants, and both 60-byte scale tables. It no longer depends on its original object's data definitions when linked.
- `.rodata` is byte-identical (56 bytes): compiler-owned conversion doubles, the two zero vectors, and correctly padded strings.
- `.data` is byte-identical (120 bytes).
- `.sbss2` matches (8 bytes), using reversed scratch-color declarations and the small-data section pairing.
- Scalar FIFO writes reload named zero and one at the retail instruction locations. Grouping x/y through `GXParam1f32` in `effKemuriDisp` preserves the retail FPR assignments. Both Disp functions remain 100% despite named constants.
- Local half/20/6.4 values and `opt_loop_invariants off` retain the remaining Disp matches. Exact shapes and pragmas have `MUST_MATCH` guards with ordinary GX calls in the natural branches.
- The signed conversion helper flushes deferred code before the four-byte `.sdata2` tail padding. Its emitted body is stripped by the linker.

## Remaining blocker

`.sdata2` has the correct 192-byte size and all required values, but four anonymous entries are at its end instead of among the 37 named floats:

| Value | Required offset |
| --- | --- |
| 20 | `0x24` |
| 0.5 | `0x5c` |
| 6.4 | `0x64` |
| 1/128 | `0x88` |

The rest of the pool is consequently displaced. Replacing those literals with named objects changes the hoisted FPR allocation or load timing. Defining data before the functions produces the right pool but propagates initializers into code, changing the Disp instructions. Immediate helpers between definitions after the functions flush every deferred function at the first helper, so they cannot interleave the four entries individually.

The pool issue remains a compiler/code-shape problem: all six functions pass an explicit relocation-value comparison, with zero findings. No wrong constant values are hidden by the 100% objdiff scores.

## Validation

- Final source: all six functions 100%; `.text`, `.rodata`, `.data`, `.sbss2` 100%; `.sdata2` fuzzy score 91.48937% (the data bytes are not identical).
- Temporarily set Matching and ran `ninja build/G8MJ01/ok`: **31 files OK; MarioSt.dol FAILED**.
- `link_diag`: 116 changed text bytes from relocation offsets and 102 changed `.sdata2` bytes, **218 total DOL bytes differ**. Other data sections are identical.
- Restored NonMatching and ran `ninja build/G8MJ01/ok`: **32 files OK**. This success uses the original kemuri object and does not establish that the source links identically.
- `python3 tools/reloc_value_check.py eff_kemuri --all`: clean. Also called `check_unit` directly and verified all six functions were compared: zero findings.
- Compiled the current source without `-DMUST_MATCH`: success.
- `git diff --check`: clean. No changes to `tools/`, no push, no commit.

## Experiment artifacts

Every experiment is logged in `CODEX_ATTEMPTS.md`, with percentages and checksum status. Sources, per-unit reports, sweep scripts, and final link logs are under `tmp/kemuri/`.

Useful checkpoints:

- `six-matched-hybrid-before-pool.c`: all six functions match with named constants and literal dimensions; duplicate data definitions remain.
- `hybrid-remove-unused-dimension-defs.c`: all six match, only the four anonymous entries need rearranging; `.sdata2` is 188 bytes before padding.
- `literal-except-zero-one-scalar-fifo.c`: all six match with only zero and one kept named. A potentially simpler alternative for further pool experiments.
- `two-named-early-const-volatile-cast.c`: exact float pool, but regresses code; illustrates the initializer-propagation boundary.
- `guarded-partial-tail-padding-signed.c`: the final retained source.

All-named experiments got close but did not pass: the best Disp getter/FIFO variants reached 99.85%; the best secondary Disp named-6.4 variants reached 99.85265%. They are rejected, not retained.

Some early build-failure entries say “compiler hung” because `agent_check.py` matches the timeout marker in Ninja's printed command even when the actual failure is a compiler diagnostic. Follow-up direct builds identified ordinary errors, including C90 declaration ordering and nonliteral aggregate initializers; those entries are not evidence of a compiler hang.

---


## literal-locals-entry-pragma

All float uses become exact literals. Local one and 12.8 retain runtime negation/product; current Entry locals under opt_propagation off. Colors are function-local static const templates.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## disp-order-one-half-twenty

Reorder the three literal locals to give half f31, twenty f30, one f28.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## disp-order-one-twenty-half

Reorder the three literal locals to give half f31, twenty f30, one f28.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## disp-order-half-one-twenty

Reorder the three literal locals to give half f31, twenty f30, one f28.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## disp-order-half-twenty-one

Reorder the three literal locals to give half f31, twenty f30, one f28.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## disp-order-twenty-one-half

Reorder the three literal locals to give half f31, twenty f30, one f28.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## disp-order-twenty-half-one

Reorder the three literal locals to give half f31, twenty f30, one f28.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## disp2-early

Adjust the 12.8 temporary live range or runtime half product operands.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## disp2-reuse

Adjust the 12.8 temporary live range or runtime half product operands.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## disp2-block

Adjust the 12.8 temporary live range or runtime half product operands.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## disp2-constneg

Adjust the 12.8 temporary live range or runtime half product operands.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## disp2-swap

Adjust the 12.8 temporary live range or runtime half product operands.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## disp2-halflocal

Adjust the 12.8 temporary live range or runtime half product operands.

BUILD FAILED: THE COMPILER HUNG and was killed (a normal compile takes about a second). Your last edit makes mwcceppc loop forever: undo it (lastgood.c is the last version that built) and write that part differently.
timeout -k 10 300 build/tools/wibo build/tools/sjiswrap.exe build/compilers/GC/2.6/mwcceppc.exe -nodefaults -proc gekko -align powerpc -enum int -multibyte -char unsigned -fp hardware -Cpp_exceptions off -pragma "cats off" -O4,p -inline auto -pragma "warn_notinlined off" -pragma "warn_padding off" -once -maxerrors 1 -nosyspath -i include -i libs/dolsdk2004/include -i libs/dolsdk2004/include/libc -i libs/musyx/include -i build/G8MJ01/include -DBUILD_VERSION=0 -DVERSION_G8MJ01 -DSDK_REVISION=1 -DMUST_MATCH -requireprotos -sym on -DNDEBUG=1 -once -fp_contract on -inline auto,deferred -use_lmw_stmw on -rostr -sdata 48 -sdata2 6 -DMUSY_VERSION_MAJOR=2 -DMUSY_VERSION_MINOR=0 -DMUSY_VERSION_PATCH=3 -lang=c -MMD -c src/effect/eff_kemuri.c -o build/G8MJ01/src/effect || { r=$?; if [ $r -eq 124 ] || [ $r -eq 137 ]; then echo "COMPILER TIMEOUT: src/effect/eff_kemuri.c"; fi; exit $r; } && "/Library/Frameworks/Python.framework/Versions/3.12/bin/python3" tools/transform_dep.py build/G8MJ01/src/effect/eff_kemuri.d build/G8MJ01/src/effect/eff_kemuri.d


Checksum: no (exploratory object build; not linked).

## effKemuriDisp-opt_propagation

Function-scoped pragma on the literal/local source, measured separately.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.32750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 98.96185%

Checksum: no (exploratory object build; not linked).

## effKemuriDisp-opt_lifetimes

Function-scoped pragma on the literal/local source, measured separately.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## effKemuriDisp-opt_loop_invariants

Function-scoped pragma on the literal/local source, measured separately.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## effKemuriDisp-opt_common_subs

Function-scoped pragma on the literal/local source, measured separately.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## effKemuri2Disp-opt_propagation

Function-scoped pragma on the literal/local source, measured separately.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 94.32416%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 91.48937%, .text 97.98774%

Checksum: no (exploratory object build; not linked).

## effKemuri2Disp-opt_lifetimes

Function-scoped pragma on the literal/local source, measured separately.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## effKemuri2Disp-opt_loop_invariants

Function-scoped pragma on the literal/local source, measured separately.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.94106%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.93529%

Checksum: no (exploratory object build; not linked).

## effKemuri2Disp-opt_common_subs

Function-scoped pragma on the literal/local source, measured separately.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 96.83694%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 98.85899%

Checksum: no (exploratory object build; not linked).

## disp-one-quad-only

Alter which uses retain one or change initialization lifetime.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.20000%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.47207%

Checksum: no (exploratory object build; not linked).

## disp-one-assigned-late

Alter which uses retain one or change initialization lifetime.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## disp-half-assigned-late

Alter which uses retain one or change initialization lifetime.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## disp-twenty-assigned-late

Alter which uses retain one or change initialization lifetime.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## disp-all-assigned-late

Alter which uses retain one or change initialization lifetime.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## disp2-p12p8-after

Change positive-width and half locals or move runtime negation into product.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## disp2-n-local

Change positive-width and half locals or move runtime negation into product.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 98.65422%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.48910%

Checksum: no (exploratory object build; not linked).

## disp2-neg-size-assign

Change positive-width and half locals or move runtime negation into product.

BUILD FAILED: THE COMPILER HUNG and was killed (a normal compile takes about a second). Your last edit makes mwcceppc loop forever: undo it (lastgood.c is the last version that built) and write that part differently.
timeout -k 10 300 build/tools/wibo build/tools/sjiswrap.exe build/compilers/GC/2.6/mwcceppc.exe -nodefaults -proc gekko -align powerpc -enum int -multibyte -char unsigned -fp hardware -Cpp_exceptions off -pragma "cats off" -O4,p -inline auto -pragma "warn_notinlined off" -pragma "warn_padding off" -once -maxerrors 1 -nosyspath -i include -i libs/dolsdk2004/include -i libs/dolsdk2004/include/libc -i libs/musyx/include -i build/G8MJ01/include -DBUILD_VERSION=0 -DVERSION_G8MJ01 -DSDK_REVISION=1 -DMUST_MATCH -requireprotos -sym on -DNDEBUG=1 -once -fp_contract on -inline auto,deferred -use_lmw_stmw on -rostr -sdata 48 -sdata2 6 -DMUSY_VERSION_MAJOR=2 -DMUSY_VERSION_MINOR=0 -DMUSY_VERSION_PATCH=3 -lang=c -MMD -c src/effect/eff_kemuri.c -o build/G8MJ01/src/effect || { r=$?; if [ $r -eq 124 ] || [ $r -eq 137 ]; then echo "COMPILER TIMEOUT: src/effect/eff_kemuri.c"; fi; exit $r; } && "/Library/Frameworks/Python.framework/Versions/3.12/bin/python3" tools/transform_dep.py build/G8MJ01/src/effect/eff_kemuri.d build/G8MJ01/src/effect/eff_kemuri.d


Checksum: no (exploratory object build; not linked).

## disp2-half-before-loop

Change positive-width and half locals or move runtime negation into product.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.93124%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.93188%

Checksum: no (exploratory object build; not linked).

## disp2-half-in-quad

Change positive-width and half locals or move runtime negation into product.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.93124%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.93188%

Checksum: no (exploratory object build; not linked).

## disp2-n12p8-block-result

Change positive-width and half locals or move runtime negation into product.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## named-prefix-new-1

Keep a named positive-width/angle prefix while Entry retains its anonymous literal-local pragma.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## named-prefix-new-2

Keep a named positive-width/angle prefix while Entry retains its anonymous literal-local pragma.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## named-prefix-new-3

Keep a named positive-width/angle prefix while Entry retains its anonymous literal-local pragma.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 98.36739%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.38965%

Checksum: no (exploratory object build; not linked).

## named-prefix-new-4

Keep a named positive-width/angle prefix while Entry retains its anonymous literal-local pragma.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.73500%, effKemuri2Disp 98.36739%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.08923%

Checksum: no (exploratory object build; not linked).

## named-prefix-new-5

Keep a named positive-width/angle prefix while Entry retains its anonymous literal-local pragma.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.73500%, effKemuri2Disp 98.36739%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.08923%

Checksum: no (exploratory object build; not linked).

## named-prefix-new-6

Keep a named positive-width/angle prefix while Entry retains its anonymous literal-local pragma.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.73500%, effKemuri2Disp 98.36739%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.08923%

Checksum: no (exploratory object build; not linked).

## named-prefix-new-7

Keep a named positive-width/angle prefix while Entry retains its anonymous literal-local pragma.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.73500%, effKemuri2Disp 98.36739%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.08923%

Checksum: no (exploratory object build; not linked).

## named-prefix-new-8

Keep a named positive-width/angle prefix while Entry retains its anonymous literal-local pragma.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.73500%, effKemuri2Disp 98.36739%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.08923%

Checksum: no (exploratory object build; not linked).

## named-prefix-new-9

Keep a named positive-width/angle prefix while Entry retains its anonymous literal-local pragma.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.73500%, effKemuri2Disp 98.36739%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.08923%

Checksum: no (exploratory object build; not linked).

## named-prefix-new-10

Keep a named positive-width/angle prefix while Entry retains its anonymous literal-local pragma.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.73500%, effKemuri2Disp 98.36739%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.08923%

Checksum: no (exploratory object build; not linked).

## disp-shape-early-half

Change half/one expression webs and loop scope to reproduce saved FPR allocation.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.18750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.46867%

Checksum: no (exploratory object build; not linked).

## disp-shape-scoped-half

Change half/one expression webs and loop scope to reproduce saved FPR allocation.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## disp-shape-all-quad-local

Change half/one expression webs and loop scope to reproduce saved FPR allocation.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## disp-shape-one-loop

Change half/one expression webs and loop scope to reproduce saved FPR allocation.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## disp-shape-negative-local

Change half/one expression webs and loop scope to reproduce saved FPR allocation.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.25000%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.21322%

Checksum: no (exploratory object build; not linked).

## disp-shape-half-expr

Change half/one expression webs and loop scope to reproduce saved FPR allocation.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91826%

Checksum: no (exploratory object build; not linked).

## disp-shape-twenty-expr

Change half/one expression webs and loop scope to reproduce saved FPR allocation.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.81250%, effKemuri2Disp 99.89195%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 95.74468%, .text 99.91145%

Checksum: no (exploratory object build; not linked).

## disp2-noinv-half-top

Keep no-invariants pragma, introduce runtime half locals or reverse multiply spelling.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp2-noinv-half-loop

Keep no-invariants pragma, introduce runtime half locals or reverse multiply spelling.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp2-noinv-half-quad

Keep no-invariants pragma, introduce runtime half locals or reverse multiply spelling.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.98035%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.94891%

Checksum: no (exploratory object build; not linked).

## disp2-noinv-half-both

Keep no-invariants pragma, introduce runtime half locals or reverse multiply spelling.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp2-noinv-reverse

Keep no-invariants pragma, introduce runtime half locals or reverse multiply spelling.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.94106%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.93529%

Checksum: no (exploratory object build; not linked).

## disp2-noinv-reverse-local

Keep no-invariants pragma, introduce runtime half locals or reverse multiply spelling.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.94106%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.93529%

Checksum: no (exploratory object build; not linked).

## disp2-noinv-all-half-top

Keep no-invariants pragma, introduce runtime half locals or reverse multiply spelling.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-propagation-decl

Pragma preserves local webs; initialize after calls or in inner-loop scope.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.67750%, effKemuri2Disp 99.94106%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.07425%

Checksum: no (exploratory object build; not linked).

## disp-propagation-assign

Pragma preserves local webs; initialize after calls or in inner-loop scope.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.58000%, effKemuri2Disp 99.94106%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.59264%

Checksum: no (exploratory object build; not linked).

## disp-propagation-inner

Pragma preserves local webs; initialize after calls or in inner-loop scope.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.06750%, effKemuri2Disp 99.94106%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.72548%

Checksum: no (exploratory object build; not linked).

## disp-morepragma-opt_common_subs

Check another optimizer affecting the literal half/twenty webs.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.94106%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.93529%

Checksum: no (exploratory object build; not linked).

## disp-morepragma-opt_lifetimes

Check another optimizer affecting the literal half/twenty webs.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.94106%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.93529%

Checksum: no (exploratory object build; not linked).

## disp-morepragma-opt_strength_reduction

Check another optimizer affecting the literal half/twenty webs.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.94106%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.93529%

Checksum: no (exploratory object build; not linked).

## disp-morepragma-opt_algebraic

Check another optimizer affecting the literal half/twenty webs.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 99.94106%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.93529%

Checksum: no (exploratory object build; not linked).

## disp-inline-one

Inline scalar helper changes the optimizer temporary ordering without extra runtime code.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-inline-half

Inline scalar helper changes the optimizer temporary ordering without extra runtime code.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-inline-twenty

Inline scalar helper changes the optimizer temporary ordering without extra runtime code.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-inline-half-one

Inline scalar helper changes the optimizer temporary ordering without extra runtime code.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-inline-all

Inline scalar helper changes the optimizer temporary ordering without extra runtime code.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-inline-mul

Inline scalar helper changes the optimizer temporary ordering without extra runtime code.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-inline-mul-propagation

Inline scalar helper changes the optimizer temporary ordering without extra runtime code.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-inline-half-propagation

Inline scalar helper changes the optimizer temporary ordering without extra runtime code.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-inline-one-propagation

Inline scalar helper changes the optimizer temporary ordering without extra runtime code.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-inline-all-propagation

Inline scalar helper changes the optimizer temporary ordering without extra runtime code.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-expr-negone

Try equivalent unary/product or assignment shapes to alter the half web.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.78750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 99.39714%

Checksum: no (exploratory object build; not linked).

## disp-expr-half-late-product

Try equivalent unary/product or assignment shapes to alter the half web.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.66250%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.90803%

Checksum: no (exploratory object build; not linked).

## disp-expr-half-positive-first

Try equivalent unary/product or assignment shapes to alter the half web.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.77500%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.93869%

Checksum: no (exploratory object build; not linked).

## disp-expr-one-as-double

Try equivalent unary/product or assignment shapes to alter the half web.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.36250%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 92.43697%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.28133%

Checksum: no (exploratory object build; not linked).

## disp-expr-twenty-double

Try equivalent unary/product or assignment shapes to alter the half web.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.14750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 92.43697%, .sbss2 0.00000%, .sdata2 93.61702%, .text 98.95027%

Checksum: no (exploratory object build; not linked).

## disp-expr-half-double

Try equivalent unary/product or assignment shapes to alter the half web.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.48750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 92.43697%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.58787%

Checksum: no (exploratory object build; not linked).

## disp-expr-half-assign-arg

Try equivalent unary/product or assignment shapes to alter the half web.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-expr-twenty-assign-arg

Try equivalent unary/product or assignment shapes to alter the half web.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.25000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.79564%

Checksum: no (exploratory object build; not linked).

## disp-internal-opt_propagation-decl

Test whether locally applied pragmas alter frontend propagation separately from backend flags.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-internal-opt_propagation-preheader

Test whether locally applied pragmas alter frontend propagation separately from backend flags.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-internal-opt_propagation-quad

Test whether locally applied pragmas alter frontend propagation separately from backend flags.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-internal-opt_propagation-loop

Test whether locally applied pragmas alter frontend propagation separately from backend flags.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.32750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 98.99932%

Checksum: no (exploratory object build; not linked).

## disp-internal-opt_loop_invariants-decl

Test whether locally applied pragmas alter frontend propagation separately from backend flags.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-internal-opt_loop_invariants-preheader

Test whether locally applied pragmas alter frontend propagation separately from backend flags.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-internal-opt_loop_invariants-quad

Test whether locally applied pragmas alter frontend propagation separately from backend flags.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-internal-opt_loop_invariants-loop

Test whether locally applied pragmas alter frontend propagation separately from backend flags.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-internal-opt_common_subs-decl

Test whether locally applied pragmas alter frontend propagation separately from backend flags.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-internal-opt_common_subs-preheader

Test whether locally applied pragmas alter frontend propagation separately from backend flags.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-internal-opt_common_subs-quad

Test whether locally applied pragmas alter frontend propagation separately from backend flags.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-internal-opt_common_subs-loop

Test whether locally applied pragmas alter frontend propagation separately from backend flags.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-qualifier-const-one

Try const/register qualifiers on literal-local widths.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.85000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 99.14169%

Checksum: no (exploratory object build; not linked).

## disp-qualifier-const-half

Try const/register qualifiers on literal-local widths.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.18750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.50613%

Checksum: no (exploratory object build; not linked).

## disp-qualifier-const-twenty

Try const/register qualifiers on literal-local widths.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-qualifier-register-one

Try const/register qualifiers on literal-local widths.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-qualifier-register-half

Try const/register qualifiers on literal-local widths.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-qualifier-register-twenty

Try const/register qualifiers on literal-local widths.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-qualifier-mutable-one

Try const/register qualifiers on literal-local widths.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-qualifier-mutable-half

Try const/register qualifiers on literal-local widths.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-qualifier-mutable-twenty

Try const/register qualifiers on literal-local widths.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-neghalf-top-negfirst

Materialize the negative quad x coordinate at different scopes; let the compiler hoist it.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.25000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.25068%

Checksum: no (exploratory object build; not linked).

## disp-neghalf-top-productfirst

Materialize the negative quad x coordinate at different scopes; let the compiler hoist it.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.76250%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.39033%

Checksum: no (exploratory object build; not linked).

## disp-neghalf-preheader-negfirst

Materialize the negative quad x coordinate at different scopes; let the compiler hoist it.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-neghalf-preheader-productfirst

Materialize the negative quad x coordinate at different scopes; let the compiler hoist it.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.66250%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.90803%

Checksum: no (exploratory object build; not linked).

## disp-neghalf-outer-negfirst

Materialize the negative quad x coordinate at different scopes; let the compiler hoist it.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-neghalf-outer-productfirst

Materialize the negative quad x coordinate at different scopes; let the compiler hoist it.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.66250%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.90803%

Checksum: no (exploratory object build; not linked).

## disp-neghalf-inner-negfirst

Materialize the negative quad x coordinate at different scopes; let the compiler hoist it.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-neghalf-inner-productfirst

Materialize the negative quad x coordinate at different scopes; let the compiler hoist it.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.66250%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.90803%

Checksum: no (exploratory object build; not linked).

## disp-neghalf-quad-negfirst

Materialize the negative quad x coordinate at different scopes; let the compiler hoist it.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-neghalf-quad-productfirst

Materialize the negative quad x coordinate at different scopes; let the compiler hoist it.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.66250%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.90803%

Checksum: no (exploratory object build; not linked).

## disp-temp-sxsy

Introduce scalar temps for scale or quad dimensions to alter CSE and register web ordering.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-temp-sysx

Introduce scalar temps for scale or quad dimensions to alter CSE and register web ordering.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-temp-sxonly

Introduce scalar temps for scale or quad dimensions to alter CSE and register web ordering.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-temp-scale-arg-assign

Introduce scalar temps for scale or quad dimensions to alter CSE and register web ordering.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-temp-half-quad-local

Introduce scalar temps for scale or quad dimensions to alter CSE and register web ordering.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-temp-half-array-scalar

Introduce scalar temps for scale or quad dimensions to alter CSE and register web ordering.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.87250%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 99.69278%

Checksum: no (exploratory object build; not linked).

## disp-temp-one-y-local

Introduce scalar temps for scale or quad dimensions to alter CSE and register web ordering.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-temp-negative-x-local

Introduce scalar temps for scale or quad dimensions to alter CSE and register web ordering.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-direct-colors

Pass color templates directly to avoid the extra copies when propagation is off.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-prop-quals-local-local-local

With direct color arguments, keep selected scalar locals under propagation off and fold the rest explicitly.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.79905%

Checksum: no (exploratory object build; not linked).

## disp-prop-quals-local-local-const

With direct color arguments, keep selected scalar locals under propagation off and fold the rest explicitly.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.80000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.94550%

Checksum: no (exploratory object build; not linked).

## disp-prop-quals-local-local-literal

With direct color arguments, keep selected scalar locals under propagation off and fold the rest explicitly.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.80000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.94550%

Checksum: no (exploratory object build; not linked).

## disp-prop-quals-local-const-local

With direct color arguments, keep selected scalar locals under propagation off and fold the rest explicitly.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.70000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.37330%

Checksum: no (exploratory object build; not linked).

## disp-prop-quals-local-const-const

With direct color arguments, keep selected scalar locals under propagation off and fold the rest explicitly.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.23750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.51975%

Checksum: no (exploratory object build; not linked).

## disp-prop-quals-local-const-literal

With direct color arguments, keep selected scalar locals under propagation off and fold the rest explicitly.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.23750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.51975%

Checksum: no (exploratory object build; not linked).

## disp-prop-quals-local-literal-local

With direct color arguments, keep selected scalar locals under propagation off and fold the rest explicitly.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.70000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.37330%

Checksum: no (exploratory object build; not linked).

## disp-prop-quals-local-literal-const

With direct color arguments, keep selected scalar locals under propagation off and fold the rest explicitly.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.23750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.51975%

Checksum: no (exploratory object build; not linked).

## disp-prop-quals-local-literal-literal

With direct color arguments, keep selected scalar locals under propagation off and fold the rest explicitly.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.23750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.51975%

Checksum: no (exploratory object build; not linked).

## disp-prop-quals-const-local-local

With direct color arguments, keep selected scalar locals under propagation off and fold the rest explicitly.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.00000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.18256%

Checksum: no (exploratory object build; not linked).

## disp-prop-quals-const-local-const

With direct color arguments, keep selected scalar locals under propagation off and fold the rest explicitly.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.01250%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.18597%

Checksum: no (exploratory object build; not linked).

## disp-prop-quals-const-local-literal

With direct color arguments, keep selected scalar locals under propagation off and fold the rest explicitly.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.01250%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.18597%

Checksum: no (exploratory object build; not linked).

## disp-prop-quals-const-const-local

With direct color arguments, keep selected scalar locals under propagation off and fold the rest explicitly.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 93.69500%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 98.28201%

Checksum: no (exploratory object build; not linked).

## disp-prop-quals-const-const-const

With direct color arguments, keep selected scalar locals under propagation off and fold the rest explicitly.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 93.67000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 98.27521%

Checksum: no (exploratory object build; not linked).

## disp-prop-quals-const-const-literal

With direct color arguments, keep selected scalar locals under propagation off and fold the rest explicitly.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 93.67000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 98.27521%

Checksum: no (exploratory object build; not linked).

## disp-prop-quals-const-literal-local

With direct color arguments, keep selected scalar locals under propagation off and fold the rest explicitly.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 93.69500%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 98.28201%

Checksum: no (exploratory object build; not linked).

## disp-prop-quals-const-literal-const

With direct color arguments, keep selected scalar locals under propagation off and fold the rest explicitly.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 93.67000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 98.27521%

Checksum: no (exploratory object build; not linked).

## disp-prop-quals-const-literal-literal

With direct color arguments, keep selected scalar locals under propagation off and fold the rest explicitly.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 93.67000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 98.27521%

Checksum: no (exploratory object build; not linked).

## disp-quad-helper-half-one-begin-in

Inline quad helper reshapes argument and constant webs without changing quad operations.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-quad-helper-half-one-begin-out

Inline quad helper reshapes argument and constant webs without changing quad operations.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-quad-helper-half-one-negative-arg

Inline quad helper reshapes argument and constant webs without changing quad operations.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-quad-helper-one-half-begin-in

Inline quad helper reshapes argument and constant webs without changing quad operations.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-quad-helper-one-half-begin-out

Inline quad helper reshapes argument and constant webs without changing quad operations.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-quad-helper-one-half-negative-arg

Inline quad helper reshapes argument and constant webs without changing quad operations.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-named-negative-on-one-half-negHalf

Keep the negative product in a named local, reordering its declaration with one and half.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-named-negative-on-one-negHalf-half

Keep the negative product in a named local, reordering its declaration with one and half.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-named-negative-on-half-one-negHalf

Keep the negative product in a named local, reordering its declaration with one and half.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-named-negative-on-half-negHalf-one

Keep the negative product in a named local, reordering its declaration with one and half.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-named-negative-on-negHalf-one-half

Keep the negative product in a named local, reordering its declaration with one and half.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-named-negative-on-negHalf-half-one

Keep the negative product in a named local, reordering its declaration with one and half.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-named-negative-off-one-half-negHalf

Keep the negative product in a named local, reordering its declaration with one and half.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.78750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.94210%

Checksum: no (exploratory object build; not linked).

## disp-named-negative-off-one-negHalf-half

Keep the negative product in a named local, reordering its declaration with one and half.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.93188%

Checksum: no (exploratory object build; not linked).

## disp-named-negative-off-half-one-negHalf

Keep the negative product in a named local, reordering its declaration with one and half.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-named-negative-off-half-negHalf-one

Keep the negative product in a named local, reordering its declaration with one and half.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.93188%

Checksum: no (exploratory object build; not linked).

## disp-named-negative-off-negHalf-one-half

Keep the negative product in a named local, reordering its declaration with one and half.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.80000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.94550%

Checksum: no (exploratory object build; not linked).

## disp-named-negative-off-negHalf-half-one

Keep the negative product in a named local, reordering its declaration with one and half.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.93188%

Checksum: no (exploratory object build; not linked).

## disp-one-type-s32-implicit

Use an integer local for unity, relying on constant propagation to remove conversion instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.05000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 99.46867%

Checksum: no (exploratory object build; not linked).

## disp-one-type-s32-cast

Use an integer local for unity, relying on constant propagation to remove conversion instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-one-type-s32-negcast

Use an integer local for unity, relying on constant propagation to remove conversion instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.05000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 99.46867%

Checksum: no (exploratory object build; not linked).

## disp-one-type-u32-implicit

Use an integer local for unity, relying on constant propagation to remove conversion instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.05000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 99.46867%

Checksum: no (exploratory object build; not linked).

## disp-one-type-u32-cast

Use an integer local for unity, relying on constant propagation to remove conversion instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-one-type-u32-negcast

Use an integer local for unity, relying on constant propagation to remove conversion instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.05000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 99.46867%

Checksum: no (exploratory object build; not linked).

## disp-one-type-s16-implicit

Use an integer local for unity, relying on constant propagation to remove conversion instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.05000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 99.46867%

Checksum: no (exploratory object build; not linked).

## disp-one-type-s16-cast

Use an integer local for unity, relying on constant propagation to remove conversion instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-one-type-s16-negcast

Use an integer local for unity, relying on constant propagation to remove conversion instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.05000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 99.46867%

Checksum: no (exploratory object build; not linked).

## disp-one-type-u16-implicit

Use an integer local for unity, relying on constant propagation to remove conversion instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.05000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 99.46867%

Checksum: no (exploratory object build; not linked).

## disp-one-type-u16-cast

Use an integer local for unity, relying on constant propagation to remove conversion instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-one-type-u16-negcast

Use an integer local for unity, relying on constant propagation to remove conversion instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.05000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 99.46867%

Checksum: no (exploratory object build; not linked).

## disp-one-type-s8-implicit

Use an integer local for unity, relying on constant propagation to remove conversion instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.05000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 99.46867%

Checksum: no (exploratory object build; not linked).

## disp-one-type-s8-cast

Use an integer local for unity, relying on constant propagation to remove conversion instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-one-type-s8-negcast

Use an integer local for unity, relying on constant propagation to remove conversion instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.05000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 99.46867%

Checksum: no (exploratory object build; not linked).

## disp-one-type-u8-implicit

Use an integer local for unity, relying on constant propagation to remove conversion instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.05000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 99.46867%

Checksum: no (exploratory object build; not linked).

## disp-one-type-u8-cast

Use an integer local for unity, relying on constant propagation to remove conversion instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-one-type-u8-negcast

Use an integer local for unity, relying on constant propagation to remove conversion instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.05000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 99.46867%

Checksum: no (exploratory object build; not linked).

## disp-one-scope-top-plain

Change one declaration scope, rather than merely its assignment scope.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-one-scope-top-const

Change one declaration scope, rather than merely its assignment scope.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.85000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 99.14169%

Checksum: no (exploratory object build; not linked).

## disp-one-scope-preheader-plain

Change one declaration scope, rather than merely its assignment scope.

BUILD FAILED: THE COMPILER HUNG and was killed (a normal compile takes about a second). Your last edit makes mwcceppc loop forever: undo it (lastgood.c is the last version that built) and write that part differently.
timeout -k 10 300 build/tools/wibo build/tools/sjiswrap.exe build/compilers/GC/2.6/mwcceppc.exe -nodefaults -proc gekko -align powerpc -enum int -multibyte -char unsigned -fp hardware -Cpp_exceptions off -pragma "cats off" -O4,p -inline auto -pragma "warn_notinlined off" -pragma "warn_padding off" -once -maxerrors 1 -nosyspath -i include -i libs/dolsdk2004/include -i libs/dolsdk2004/include/libc -i libs/musyx/include -i build/G8MJ01/include -DBUILD_VERSION=0 -DVERSION_G8MJ01 -DSDK_REVISION=1 -DMUST_MATCH -requireprotos -sym on -DNDEBUG=1 -once -fp_contract on -inline auto,deferred -use_lmw_stmw on -rostr -sdata 48 -sdata2 6 -DMUSY_VERSION_MAJOR=2 -DMUSY_VERSION_MINOR=0 -DMUSY_VERSION_PATCH=3 -lang=c -MMD -c src/effect/eff_kemuri.c -o build/G8MJ01/src/effect || { r=$?; if [ $r -eq 124 ] || [ $r -eq 137 ]; then echo "COMPILER TIMEOUT: src/effect/eff_kemuri.c"; fi; exit $r; } && "/Library/Frameworks/Python.framework/Versions/3.12/bin/python3" tools/transform_dep.py build/G8MJ01/src/effect/eff_kemuri.d build/G8MJ01/src/effect/eff_kemuri.d


Checksum: no (exploratory object build; not linked).

## disp-one-scope-preheader-const

Change one declaration scope, rather than merely its assignment scope.

BUILD FAILED: THE COMPILER HUNG and was killed (a normal compile takes about a second). Your last edit makes mwcceppc loop forever: undo it (lastgood.c is the last version that built) and write that part differently.
timeout -k 10 300 build/tools/wibo build/tools/sjiswrap.exe build/compilers/GC/2.6/mwcceppc.exe -nodefaults -proc gekko -align powerpc -enum int -multibyte -char unsigned -fp hardware -Cpp_exceptions off -pragma "cats off" -O4,p -inline auto -pragma "warn_notinlined off" -pragma "warn_padding off" -once -maxerrors 1 -nosyspath -i include -i libs/dolsdk2004/include -i libs/dolsdk2004/include/libc -i libs/musyx/include -i build/G8MJ01/include -DBUILD_VERSION=0 -DVERSION_G8MJ01 -DSDK_REVISION=1 -DMUST_MATCH -requireprotos -sym on -DNDEBUG=1 -once -fp_contract on -inline auto,deferred -use_lmw_stmw on -rostr -sdata 48 -sdata2 6 -DMUSY_VERSION_MAJOR=2 -DMUSY_VERSION_MINOR=0 -DMUSY_VERSION_PATCH=3 -lang=c -MMD -c src/effect/eff_kemuri.c -o build/G8MJ01/src/effect || { r=$?; if [ $r -eq 124 ] || [ $r -eq 137 ]; then echo "COMPILER TIMEOUT: src/effect/eff_kemuri.c"; fi; exit $r; } && "/Library/Frameworks/Python.framework/Versions/3.12/bin/python3" tools/transform_dep.py build/G8MJ01/src/effect/eff_kemuri.d build/G8MJ01/src/effect/eff_kemuri.d


Checksum: no (exploratory object build; not linked).

## disp-one-scope-outer-plain

Change one declaration scope, rather than merely its assignment scope.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-one-scope-outer-const

Change one declaration scope, rather than merely its assignment scope.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.85000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 99.14169%

Checksum: no (exploratory object build; not linked).

## disp-one-scope-inner-plain

Change one declaration scope, rather than merely its assignment scope.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-one-scope-inner-const

Change one declaration scope, rather than merely its assignment scope.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.85000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.63158%, .text 99.14169%

Checksum: no (exploratory object build; not linked).

## disp-one-scope-quad-plain

Change one declaration scope, rather than merely its assignment scope.

BUILD FAILED: THE COMPILER HUNG and was killed (a normal compile takes about a second). Your last edit makes mwcceppc loop forever: undo it (lastgood.c is the last version that built) and write that part differently.
timeout -k 10 300 build/tools/wibo build/tools/sjiswrap.exe build/compilers/GC/2.6/mwcceppc.exe -nodefaults -proc gekko -align powerpc -enum int -multibyte -char unsigned -fp hardware -Cpp_exceptions off -pragma "cats off" -O4,p -inline auto -pragma "warn_notinlined off" -pragma "warn_padding off" -once -maxerrors 1 -nosyspath -i include -i libs/dolsdk2004/include -i libs/dolsdk2004/include/libc -i libs/musyx/include -i build/G8MJ01/include -DBUILD_VERSION=0 -DVERSION_G8MJ01 -DSDK_REVISION=1 -DMUST_MATCH -requireprotos -sym on -DNDEBUG=1 -once -fp_contract on -inline auto,deferred -use_lmw_stmw on -rostr -sdata 48 -sdata2 6 -DMUSY_VERSION_MAJOR=2 -DMUSY_VERSION_MINOR=0 -DMUSY_VERSION_PATCH=3 -lang=c -MMD -c src/effect/eff_kemuri.c -o build/G8MJ01/src/effect || { r=$?; if [ $r -eq 124 ] || [ $r -eq 137 ]; then echo "COMPILER TIMEOUT: src/effect/eff_kemuri.c"; fi; exit $r; } && "/Library/Frameworks/Python.framework/Versions/3.12/bin/python3" tools/transform_dep.py build/G8MJ01/src/effect/eff_kemuri.d build/G8MJ01/src/effect/eff_kemuri.d


Checksum: no (exploratory object build; not linked).

## disp-one-scope-quad-const

Change one declaration scope, rather than merely its assignment scope.

BUILD FAILED: THE COMPILER HUNG and was killed (a normal compile takes about a second). Your last edit makes mwcceppc loop forever: undo it (lastgood.c is the last version that built) and write that part differently.
timeout -k 10 300 build/tools/wibo build/tools/sjiswrap.exe build/compilers/GC/2.6/mwcceppc.exe -nodefaults -proc gekko -align powerpc -enum int -multibyte -char unsigned -fp hardware -Cpp_exceptions off -pragma "cats off" -O4,p -inline auto -pragma "warn_notinlined off" -pragma "warn_padding off" -once -maxerrors 1 -nosyspath -i include -i libs/dolsdk2004/include -i libs/dolsdk2004/include/libc -i libs/musyx/include -i build/G8MJ01/include -DBUILD_VERSION=0 -DVERSION_G8MJ01 -DSDK_REVISION=1 -DMUST_MATCH -requireprotos -sym on -DNDEBUG=1 -once -fp_contract on -inline auto,deferred -use_lmw_stmw on -rostr -sdata 48 -sdata2 6 -DMUSY_VERSION_MAJOR=2 -DMUSY_VERSION_MINOR=0 -DMUSY_VERSION_PATCH=3 -lang=c -MMD -c src/effect/eff_kemuri.c -o build/G8MJ01/src/effect || { r=$?; if [ $r -eq 124 ] || [ $r -eq 137 ]; then echo "COMPILER TIMEOUT: src/effect/eff_kemuri.c"; fi; exit $r; } && "/Library/Frameworks/Python.framework/Versions/3.12/bin/python3" tools/transform_dep.py build/G8MJ01/src/effect/eff_kemuri.d build/G8MJ01/src/effect/eff_kemuri.d


Checksum: no (exploratory object build; not linked).

## disp-double-one-cast

Use a double local but explicitly round each use to float; examine whether the optimizer web changes without extra instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.64750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 92.43697%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.35899%

Checksum: no (exploratory object build; not linked).

## disp-double-one-cast-assignment

Use a double local but explicitly round each use to float; examine whether the optimizer web changes without extra instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.58500%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 92.43697%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.34197%

Checksum: no (exploratory object build; not linked).

## disp-double-one-reuse-round

Use a double local but explicitly round each use to float; examine whether the optimizer web changes without extra instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.58500%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 92.43697%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.34197%

Checksum: no (exploratory object build; not linked).

## disp-double-half-cast

Use a double local but explicitly round each use to float; examine whether the optimizer web changes without extra instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.80000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 92.43697%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.40054%

Checksum: no (exploratory object build; not linked).

## disp-double-half-cast-assignment

Use a double local but explicitly round each use to float; examine whether the optimizer web changes without extra instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.75000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 92.43697%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.38693%

Checksum: no (exploratory object build; not linked).

## disp-double-half-reuse-round

Use a double local but explicitly round each use to float; examine whether the optimizer web changes without extra instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.75000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 92.43697%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.38693%

Checksum: no (exploratory object build; not linked).

## disp-double-twenty-cast

Use a double local but explicitly round each use to float; examine whether the optimizer web changes without extra instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 95.79750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 92.43697%, .sbss2 0.00000%, .sdata2 93.61702%, .text 98.85490%

Checksum: no (exploratory object build; not linked).

## disp-double-twenty-cast-assignment

Use a double local but explicitly round each use to float; examine whether the optimizer web changes without extra instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.56250%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 92.43697%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.33583%

Checksum: no (exploratory object build; not linked).

## disp-double-twenty-reuse-round

Use a double local but explicitly round each use to float; examine whether the optimizer web changes without extra instructions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.56250%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 92.43697%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.33583%

Checksum: no (exploratory object build; not linked).

## disp-literal-pragma-opt_dead_assignments

Test optimizer switches on the new all-literal matched-loop source.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.33750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.81948%

Checksum: no (exploratory object build; not linked).

## disp-literal-pragma-opt_dead_code

Test optimizer switches on the new all-literal matched-loop source.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-literal-pragma-opt_strength_reduction

Test optimizer switches on the new all-literal matched-loop source.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-literal-pragma-peephole

Test optimizer switches on the new all-literal matched-loop source.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-literal-pragma-global_optimizer

Test optimizer switches on the new all-literal matched-loop source.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-literal-pragma-opt_algebraic

Test optimizer switches on the new all-literal matched-loop source.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-literal-pragma-opt_pointer_aliasing

Test optimizer switches on the new all-literal matched-loop source.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-literal-pragma-optimize_for_size

Test optimizer switches on the new all-literal matched-loop source.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-split-two-ones

Split unity/dimension uses or assign a scalar in its argument.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-split-two-halves

Split unity/dimension uses or assign a scalar in its argument.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-split-two-twenties

Split unity/dimension uses or assign a scalar in its argument.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-split-one-argument-assign

Split unity/dimension uses or assign a scalar in its argument.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-split-half-use-assign

Split unity/dimension uses or assign a scalar in its argument.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.69000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.09809%

Checksum: no (exploratory object build; not linked).

## disp-restore-named-float_deg2rad_8041c6a4

Restore one original external constant to locate the source of the saved-FPR ordering change; pool layout handled later.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-restore-named-float_1_8041c6b0

Restore one original external constant to locate the source of the saved-FPR ordering change; pool layout handled later.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 94.11500%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 98.39646%

Checksum: no (exploratory object build; not linked).

## disp-restore-named-float_0_8041c6bc

Restore one original external constant to locate the source of the saved-FPR ordering change; pool layout handled later.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 95.42750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 98.75409%

Checksum: no (exploratory object build; not linked).

## disp-restore-named-float_0p0625_8041c6d8

Restore one original external constant to locate the source of the saved-FPR ordering change; pool layout handled later.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.47312%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## disp-restore-named-float_neg0p2_8041c6dc

Restore one original external constant to locate the source of the saved-FPR ordering change; pool layout handled later.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 92.47312%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## all-named-repro

Define all owned data, use all floats by name. Reproduce the prior blocker with no literal duplicates.

BUILD FAILED: THE COMPILER HUNG and was killed (a normal compile takes about a second). Your last edit makes mwcceppc loop forever: undo it (lastgood.c is the last version that built) and write that part differently.
timeout -k 10 300 build/tools/wibo build/tools/sjiswrap.exe build/compilers/GC/2.6/mwcceppc.exe -nodefaults -proc gekko -align powerpc -enum int -multibyte -char unsigned -fp hardware -Cpp_exceptions off -pragma "cats off" -O4,p -inline auto -pragma "warn_notinlined off" -pragma "warn_padding off" -once -maxerrors 1 -nosyspath -i include -i libs/dolsdk2004/include -i libs/dolsdk2004/include/libc -i libs/musyx/include -i build/G8MJ01/include -DBUILD_VERSION=0 -DVERSION_G8MJ01 -DSDK_REVISION=1 -DMUST_MATCH -requireprotos -sym on -DNDEBUG=1 -once -fp_contract on -inline auto,deferred -use_lmw_stmw on -rostr -sdata 48 -sdata2 6 -DMUSY_VERSION_MAJOR=2 -DMUSY_VERSION_MINOR=0 -DMUSY_VERSION_PATCH=3 -lang=c -MMD -c src/effect/eff_kemuri.c -o build/G8MJ01/src/effect || { r=$?; if [ $r -eq 124 ] || [ $r -eq 137 ]; then echo "COMPILER TIMEOUT: src/effect/eff_kemuri.c"; fi; exit $r; } && "/Library/Frameworks/Python.framework/Versions/3.12/bin/python3" tools/transform_dep.py build/G8MJ01/src/effect/eff_kemuri.d build/G8MJ01/src/effect/eff_kemuri.d


Checksum: no (exploratory object build; not linked).

## named-fifo-effKemuriDisp-direct

Reload named zero/unity inside individual FIFO stores, avoiding early evaluation of volatile arguments in three/two-component wrappers.

BUILD FAILED: THE COMPILER HUNG and was killed (a normal compile takes about a second). Your last edit makes mwcceppc loop forever: undo it (lastgood.c is the last version that built) and write that part differently.
timeout -k 10 300 build/tools/wibo build/tools/sjiswrap.exe build/compilers/GC/2.6/mwcceppc.exe -nodefaults -proc gekko -align powerpc -enum int -multibyte -char unsigned -fp hardware -Cpp_exceptions off -pragma "cats off" -O4,p -inline auto -pragma "warn_notinlined off" -pragma "warn_padding off" -once -maxerrors 1 -nosyspath -i include -i libs/dolsdk2004/include -i libs/dolsdk2004/include/libc -i libs/musyx/include -i build/G8MJ01/include -DBUILD_VERSION=0 -DVERSION_G8MJ01 -DSDK_REVISION=1 -DMUST_MATCH -requireprotos -sym on -DNDEBUG=1 -once -fp_contract on -inline auto,deferred -use_lmw_stmw on -rostr -sdata 48 -sdata2 6 -DMUSY_VERSION_MAJOR=2 -DMUSY_VERSION_MINOR=0 -DMUSY_VERSION_PATCH=3 -lang=c -MMD -c src/effect/eff_kemuri.c -o build/G8MJ01/src/effect || { r=$?; if [ $r -eq 124 ] || [ $r -eq 137 ]; then echo "COMPILER TIMEOUT: src/effect/eff_kemuri.c"; fi; exit $r; } && "/Library/Frameworks/Python.framework/Versions/3.12/bin/python3" tools/transform_dep.py build/G8MJ01/src/effect/eff_kemuri.d build/G8MJ01/src/effect/eff_kemuri.d


Checksum: no (exploratory object build; not linked).

## named-fifo-effKemuri2Disp-direct

Reload named zero/unity inside individual FIFO stores, avoiding early evaluation of volatile arguments in three/two-component wrappers.

BUILD FAILED: THE COMPILER HUNG and was killed (a normal compile takes about a second). Your last edit makes mwcceppc loop forever: undo it (lastgood.c is the last version that built) and write that part differently.
timeout -k 10 300 build/tools/wibo build/tools/sjiswrap.exe build/compilers/GC/2.6/mwcceppc.exe -nodefaults -proc gekko -align powerpc -enum int -multibyte -char unsigned -fp hardware -Cpp_exceptions off -pragma "cats off" -O4,p -inline auto -pragma "warn_notinlined off" -pragma "warn_padding off" -once -maxerrors 1 -nosyspath -i include -i libs/dolsdk2004/include -i libs/dolsdk2004/include/libc -i libs/musyx/include -i build/G8MJ01/include -DBUILD_VERSION=0 -DVERSION_G8MJ01 -DSDK_REVISION=1 -DMUST_MATCH -requireprotos -sym on -DNDEBUG=1 -once -fp_contract on -inline auto,deferred -use_lmw_stmw on -rostr -sdata 48 -sdata2 6 -DMUSY_VERSION_MAJOR=2 -DMUSY_VERSION_MINOR=0 -DMUSY_VERSION_PATCH=3 -lang=c -MMD -c src/effect/eff_kemuri.c -o build/G8MJ01/src/effect || { r=$?; if [ $r -eq 124 ] || [ $r -eq 137 ]; then echo "COMPILER TIMEOUT: src/effect/eff_kemuri.c"; fi; exit $r; } && "/Library/Frameworks/Python.framework/Versions/3.12/bin/python3" tools/transform_dep.py build/G8MJ01/src/effect/eff_kemuri.d build/G8MJ01/src/effect/eff_kemuri.d


Checksum: no (exploratory object build; not linked).

## named-fifo-effKemuriDisp-param

Reload named zero/unity inside individual FIFO stores, avoiding early evaluation of volatile arguments in three/two-component wrappers.

BUILD FAILED: THE COMPILER HUNG and was killed (a normal compile takes about a second). Your last edit makes mwcceppc loop forever: undo it (lastgood.c is the last version that built) and write that part differently.
timeout -k 10 300 build/tools/wibo build/tools/sjiswrap.exe build/compilers/GC/2.6/mwcceppc.exe -nodefaults -proc gekko -align powerpc -enum int -multibyte -char unsigned -fp hardware -Cpp_exceptions off -pragma "cats off" -O4,p -inline auto -pragma "warn_notinlined off" -pragma "warn_padding off" -once -maxerrors 1 -nosyspath -i include -i libs/dolsdk2004/include -i libs/dolsdk2004/include/libc -i libs/musyx/include -i build/G8MJ01/include -DBUILD_VERSION=0 -DVERSION_G8MJ01 -DSDK_REVISION=1 -DMUST_MATCH -requireprotos -sym on -DNDEBUG=1 -once -fp_contract on -inline auto,deferred -use_lmw_stmw on -rostr -sdata 48 -sdata2 6 -DMUSY_VERSION_MAJOR=2 -DMUSY_VERSION_MINOR=0 -DMUSY_VERSION_PATCH=3 -lang=c -MMD -c src/effect/eff_kemuri.c -o build/G8MJ01/src/effect || { r=$?; if [ $r -eq 124 ] || [ $r -eq 137 ]; then echo "COMPILER TIMEOUT: src/effect/eff_kemuri.c"; fi; exit $r; } && "/Library/Frameworks/Python.framework/Versions/3.12/bin/python3" tools/transform_dep.py build/G8MJ01/src/effect/eff_kemuri.d build/G8MJ01/src/effect/eff_kemuri.d


Checksum: no (exploratory object build; not linked).

## named-fifo-effKemuri2Disp-param

Reload named zero/unity inside individual FIFO stores, avoiding early evaluation of volatile arguments in three/two-component wrappers.

BUILD FAILED: THE COMPILER HUNG and was killed (a normal compile takes about a second). Your last edit makes mwcceppc loop forever: undo it (lastgood.c is the last version that built) and write that part differently.
timeout -k 10 300 build/tools/wibo build/tools/sjiswrap.exe build/compilers/GC/2.6/mwcceppc.exe -nodefaults -proc gekko -align powerpc -enum int -multibyte -char unsigned -fp hardware -Cpp_exceptions off -pragma "cats off" -O4,p -inline auto -pragma "warn_notinlined off" -pragma "warn_padding off" -once -maxerrors 1 -nosyspath -i include -i libs/dolsdk2004/include -i libs/dolsdk2004/include/libc -i libs/musyx/include -i build/G8MJ01/include -DBUILD_VERSION=0 -DVERSION_G8MJ01 -DSDK_REVISION=1 -DMUST_MATCH -requireprotos -sym on -DNDEBUG=1 -once -fp_contract on -inline auto,deferred -use_lmw_stmw on -rostr -sdata 48 -sdata2 6 -DMUSY_VERSION_MAJOR=2 -DMUSY_VERSION_MINOR=0 -DMUSY_VERSION_PATCH=3 -lang=c -MMD -c src/effect/eff_kemuri.c -o build/G8MJ01/src/effect || { r=$?; if [ $r -eq 124 ] || [ $r -eq 137 ]; then echo "COMPILER TIMEOUT: src/effect/eff_kemuri.c"; fi; exit $r; } && "/Library/Frameworks/Python.framework/Versions/3.12/bin/python3" tools/transform_dep.py build/G8MJ01/src/effect/eff_kemuri.d build/G8MJ01/src/effect/eff_kemuri.d


Checksum: no (exploratory object build; not linked).

## named-fifo-effKemuriDisp-position2-param

Reload named zero/unity inside individual FIFO stores, avoiding early evaluation of volatile arguments in three/two-component wrappers.

BUILD FAILED: THE COMPILER HUNG and was killed (a normal compile takes about a second). Your last edit makes mwcceppc loop forever: undo it (lastgood.c is the last version that built) and write that part differently.
timeout -k 10 300 build/tools/wibo build/tools/sjiswrap.exe build/compilers/GC/2.6/mwcceppc.exe -nodefaults -proc gekko -align powerpc -enum int -multibyte -char unsigned -fp hardware -Cpp_exceptions off -pragma "cats off" -O4,p -inline auto -pragma "warn_notinlined off" -pragma "warn_padding off" -once -maxerrors 1 -nosyspath -i include -i libs/dolsdk2004/include -i libs/dolsdk2004/include/libc -i libs/musyx/include -i build/G8MJ01/include -DBUILD_VERSION=0 -DVERSION_G8MJ01 -DSDK_REVISION=1 -DMUST_MATCH -requireprotos -sym on -DNDEBUG=1 -once -fp_contract on -inline auto,deferred -use_lmw_stmw on -rostr -sdata 48 -sdata2 6 -DMUSY_VERSION_MAJOR=2 -DMUSY_VERSION_MINOR=0 -DMUSY_VERSION_PATCH=3 -lang=c -MMD -c src/effect/eff_kemuri.c -o build/G8MJ01/src/effect || { r=$?; if [ $r -eq 124 ] || [ $r -eq 137 ]; then echo "COMPILER TIMEOUT: src/effect/eff_kemuri.c"; fi; exit $r; } && "/Library/Frameworks/Python.framework/Versions/3.12/bin/python3" tools/transform_dep.py build/G8MJ01/src/effect/eff_kemuri.d build/G8MJ01/src/effect/eff_kemuri.d


Checksum: no (exploratory object build; not linked).

## named-fifo-effKemuri2Disp-position2-param

Reload named zero/unity inside individual FIFO stores, avoiding early evaluation of volatile arguments in three/two-component wrappers.

BUILD FAILED: THE COMPILER HUNG and was killed (a normal compile takes about a second). Your last edit makes mwcceppc loop forever: undo it (lastgood.c is the last version that built) and write that part differently.
timeout -k 10 300 build/tools/wibo build/tools/sjiswrap.exe build/compilers/GC/2.6/mwcceppc.exe -nodefaults -proc gekko -align powerpc -enum int -multibyte -char unsigned -fp hardware -Cpp_exceptions off -pragma "cats off" -O4,p -inline auto -pragma "warn_notinlined off" -pragma "warn_padding off" -once -maxerrors 1 -nosyspath -i include -i libs/dolsdk2004/include -i libs/dolsdk2004/include/libc -i libs/musyx/include -i build/G8MJ01/include -DBUILD_VERSION=0 -DVERSION_G8MJ01 -DSDK_REVISION=1 -DMUST_MATCH -requireprotos -sym on -DNDEBUG=1 -once -fp_contract on -inline auto,deferred -use_lmw_stmw on -rostr -sdata 48 -sdata2 6 -DMUSY_VERSION_MAJOR=2 -DMUSY_VERSION_MINOR=0 -DMUSY_VERSION_PATCH=3 -lang=c -MMD -c src/effect/eff_kemuri.c -o build/G8MJ01/src/effect || { r=$?; if [ $r -eq 124 ] || [ $r -eq 137 ]; then echo "COMPILER TIMEOUT: src/effect/eff_kemuri.c"; fi; exit $r; } && "/Library/Frameworks/Python.framework/Versions/3.12/bin/python3" tools/transform_dep.py build/G8MJ01/src/effect/eff_kemuri.d build/G8MJ01/src/effect/eff_kemuri.d


Checksum: no (exploratory object build; not linked).

## all-named-repro

Define all owned data, use all floats by name. Reproduce the prior blocker with no literal duplicates.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 92.07072%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 95.24523%

Checksum: no (exploratory object build; not linked).

## named-fifo-effKemuriDisp-direct

Reload named zero/unity inside individual FIFO stores, avoiding early evaluation of volatile arguments in three/two-component wrappers.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.30000%, effKemuri2Disp 92.07072%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 96.24251%

Checksum: no (exploratory object build; not linked).

## named-fifo-effKemuri2Disp-direct

Reload named zero/unity inside individual FIFO stores, avoiding early evaluation of volatile arguments in three/two-component wrappers.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.31238%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.75613%

Checksum: no (exploratory object build; not linked).

## named-fifo-effKemuriDisp-param

Reload named zero/unity inside individual FIFO stores, avoiding early evaluation of volatile arguments in three/two-component wrappers.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.30000%, effKemuri2Disp 92.07072%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 96.24251%

Checksum: no (exploratory object build; not linked).

## named-fifo-effKemuri2Disp-param

Reload named zero/unity inside individual FIFO stores, avoiding early evaluation of volatile arguments in three/two-component wrappers.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.26326%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.73910%

Checksum: no (exploratory object build; not linked).

## named-fifo-effKemuriDisp-position2-param

Reload named zero/unity inside individual FIFO stores, avoiding early evaluation of volatile arguments in three/two-component wrappers.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 95.59000%, effKemuri2Disp 92.07072%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 96.04905%

Checksum: no (exploratory object build; not linked).

## named-fifo-effKemuri2Disp-position2-param

Reload named zero/unity inside individual FIFO stores, avoiding early evaluation of volatile arguments in three/two-component wrappers.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.26326%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.73910%

Checksum: no (exploratory object build; not linked).

## named-disp2-sixp4-before-p8

Move the named 6.4 assignment past the setup calls so it has the retail live range.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.44991%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.80382%

Checksum: no (exploratory object build; not linked).

## named-disp2-sixp4-after-p8

Move the named 6.4 assignment past the setup calls so it has the retail live range.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.44991%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.80382%

Checksum: no (exploratory object build; not linked).

## named-disp2-sixp4-after-p8-noinv

Combine the late named 6.4 assignment with loop-invariant optimization disabled.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.49902%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.82085%

Checksum: no (exploratory object build; not linked).

## named-disp2-sixp4-loop

Move the named 6.4 assignment past the setup calls so it has the retail live range.

BUILD FAILED
#    File: src\effect\eff_kemuri.c
# --------------------------------
#     541: const f32 negSizeX = -p12;
#   Error: ^^^^^
#   expression syntax error
#   Too many errors printed, aborting program

User break, cancelled...
ninja: build stopped: subcommand failed.

Checksum: no (exploratory object build; not linked).

## named-disp2-sixp4-loop-noinv

Combine the late named 6.4 assignment with loop-invariant optimization disabled.

BUILD FAILED
#    File: src\effect\eff_kemuri.c
# --------------------------------
#     542: const f32 negSizeX = -p12;
#   Error: ^^^^^
#   expression syntax error
#   Too many errors printed, aborting program

User break, cancelled...
ninja: build stopped: subcommand failed.

Checksum: no (exploratory object build; not linked).

## named-disp2-sixp4-quad

Move the named 6.4 assignment past the setup calls so it has the retail live range.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.30255%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.75272%

Checksum: no (exploratory object build; not linked).

## named-disp2-sixp4-quad-noinv

Combine the late named 6.4 assignment with loop-invariant optimization disabled.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.34185%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.76635%

Checksum: no (exploratory object build; not linked).

## named-disp2-sixp4-declare-loop

Move the named 6.4 assignment past the setup calls so it has the retail live range.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.44991%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.80382%

Checksum: no (exploratory object build; not linked).

## named-disp-late-widths-preheader

Move named half/twenty assignments to where retail first loads them.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.07500%, effKemuri2Disp 92.07072%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 96.45368%

Checksum: no (exploratory object build; not linked).

## named-disp-late-widths-preheader-noinv

Disable invariant motion with late named width assignments and explicit FIFO reloads.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.07500%, effKemuri2Disp 92.07072%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 96.45368%

Checksum: no (exploratory object build; not linked).

## named-disp-late-widths-outer

Move named half/twenty assignments to where retail first loads them.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.56250%, effKemuri2Disp 92.07072%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 96.58651%

Checksum: no (exploratory object build; not linked).

## named-disp-late-widths-outer-noinv

Disable invariant motion with late named width assignments and explicit FIFO reloads.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.56250%, effKemuri2Disp 92.07072%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 96.58651%

Checksum: no (exploratory object build; not linked).

## named-disp-late-widths-inner

Move named half/twenty assignments to where retail first loads them.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.07500%, effKemuri2Disp 92.07072%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 96.45368%

Checksum: no (exploratory object build; not linked).

## named-disp-late-widths-inner-noinv

Disable invariant motion with late named width assignments and explicit FIFO reloads.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.07500%, effKemuri2Disp 92.07072%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 96.45368%

Checksum: no (exploratory object build; not linked).

## named-disp-late-widths-quad

Move named half/twenty assignments to where retail first loads them.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.08750%, effKemuri2Disp 92.07072%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 96.45709%

Checksum: no (exploratory object build; not linked).

## named-disp-late-widths-quad-noinv

Disable invariant motion with late named width assignments and explicit FIFO reloads.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.08750%, effKemuri2Disp 92.07072%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 96.45709%

Checksum: no (exploratory object build; not linked).

## named-disp2-direct

Use named 6.4 directly at the quad, or read it through MATCH_F32 only at the preheader.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.85265%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.94346%

Checksum: no (exploratory object build; not linked).

## named-disp2-late-volatile

Use named 6.4 directly at the quad, or read it through MATCH_F32 only at the preheader.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.49902%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.82085%

Checksum: no (exploratory object build; not linked).

## named-disp2-quad-volatile

Use named 6.4 directly at the quad, or read it through MATCH_F32 only at the preheader.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.34185%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.76635%

Checksum: no (exploratory object build; not linked).

## named-disp2-direct-no-pragma

Use named 6.4 directly at the quad, or read it through MATCH_F32 only at the preheader.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.81336%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.92984%

Checksum: no (exploratory object build; not linked).

## named-disp2-late-volatile-propagation

Use named 6.4 directly at the quad, or read it through MATCH_F32 only at the preheader.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 96.05501%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 96.62670%

Checksum: no (exploratory object build; not linked).

## named-disp-selective-unity

Keep named unity for quad heights, reload it once for each texture scale call and each FIFO texcoord.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 92.07072%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.04973%

Checksum: no (exploratory object build; not linked).

## named-disp-volatile-widths

With selective unity reloads, change width reads to direct named uses or local volatile preheader reads.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 92.07072%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.04973%

Checksum: no (exploratory object build; not linked).

## named-disp-direct-widths

With selective unity reloads, change width reads to direct named uses or local volatile preheader reads.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.80000%, effKemuri2Disp 92.07072%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.19618%

Checksum: no (exploratory object build; not linked).

## named-disp-half-only

With selective unity reloads, change width reads to direct named uses or local volatile preheader reads.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 92.07072%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.04973%

Checksum: no (exploratory object build; not linked).

## named-disp-twenty-only

With selective unity reloads, change width reads to direct named uses or local volatile preheader reads.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 92.07072%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.04973%

Checksum: no (exploratory object build; not linked).

## named-disp-widths-propagation

With selective unity reloads, change width reads to direct named uses or local volatile preheader reads.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.58000%, effKemuri2Disp 92.07072%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 96.86376%

Checksum: no (exploratory object build; not linked).

## named-with-literal-dimensions

Combine exact named FIFO reloads with the original literal half/twenty/sixp4 locals; duplicates are addressed separately.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.93188%

Checksum: no (exploratory object build; not linked).

## hybrid-dimensions-no-pragma

Use half/twenty/sixp4 anonymously throughout the unit, while retaining named constants and explicit zero/unity FIFO reloads.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 99.89195%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.89442%

Checksum: no (exploratory object build; not linked).

## hybrid-dimensions-noinv

Use half/twenty/sixp4 anonymously throughout the unit, while retaining named constants and explicit zero/unity FIFO reloads.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 99.94106%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.91145%

Checksum: no (exploratory object build; not linked).

## hybrid-dimensions-local-half-noinv

Use half/twenty/sixp4 anonymously throughout the unit, while retaining named constants and explicit zero/unity FIFO reloads.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.93188%

Checksum: no (exploratory object build; not linked).

## hybrid-dimensions-local-half

Use half/twenty/sixp4 anonymously throughout the unit, while retaining named constants and explicit zero/unity FIFO reloads.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 99.94106%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.91145%

Checksum: no (exploratory object build; not linked).

## fifo-helpers-disp-direct-colors-literal-direct

Use inline pointer-valued coordinate helpers so named constant reloads happen at the FIFO writes but argument-temporary structure resembles GX.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.93188%

Checksum: no (exploratory object build; not linked).

## fifo-helpers-disp-direct-colors-literal-param

Use inline pointer-valued coordinate helpers so named constant reloads happen at the FIFO writes but argument-temporary structure resembles GX.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 100.00000%
Sections: .data 0.00000%, .rodata 99.09910%, .sbss2 0.00000%, .sdata2 93.61702%, .text 99.95572%

Checksum: no (exploratory object build; not linked).

## fifo-helpers-named-with-literal-dimensions-positions

Use inline pointer-valued coordinate helpers so named constant reloads happen at the FIFO writes but argument-temporary structure resembles GX.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.97500%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.72071%

Checksum: no (exploratory object build; not linked).

## fifo-helpers-named-with-literal-dimensions-texcoords

Use inline pointer-valued coordinate helpers so named constant reloads happen at the FIFO writes but argument-temporary structure resembles GX.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.93188%

Checksum: no (exploratory object build; not linked).

## fifo-helpers-named-with-literal-dimensions-both

Use inline pointer-valued coordinate helpers so named constant reloads happen at the FIFO writes but argument-temporary structure resembles GX.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.97500%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.72071%

Checksum: no (exploratory object build; not linked).

## fifo-helpers-named-disp-direct-widths-positions

Use inline pointer-valued coordinate helpers so named constant reloads happen at the FIFO writes but argument-temporary structure resembles GX.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.82500%, effKemuri2Disp 92.07072%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 96.93052%

Checksum: no (exploratory object build; not linked).

## fifo-helpers-named-disp-direct-widths-texcoords

Use inline pointer-valued coordinate helpers so named constant reloads happen at the FIFO writes but argument-temporary structure resembles GX.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.80000%, effKemuri2Disp 92.07072%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.19618%

Checksum: no (exploratory object build; not linked).

## fifo-helpers-named-disp-direct-widths-both

Use inline pointer-valued coordinate helpers so named constant reloads happen at the FIFO writes but argument-temporary structure resembles GX.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.82500%, effKemuri2Disp 92.07072%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 96.93052%

Checksum: no (exploratory object build; not linked).

## named-disp-fifo-shape-param-all-param

Group the non-reloaded x/y writes through GX helpers while loading z and texcoords immediately at their FIFO writes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.93188%

Checksum: no (exploratory object build; not linked).

## named-disp-fifo-shape-param-all-direct

Group the non-reloaded x/y writes through GX helpers while loading z and texcoords immediately at their FIFO writes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.93188%

Checksum: no (exploratory object build; not linked).

## named-disp-fifo-shape-param-all-helper

Group the non-reloaded x/y writes through GX helpers while loading z and texcoords immediately at their FIFO writes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.93188%

Checksum: no (exploratory object build; not linked).

## named-disp-fifo-shape-position2-param-param

Group the non-reloaded x/y writes through GX helpers while loading z and texcoords immediately at their FIFO writes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.75000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.65940%

Checksum: no (exploratory object build; not linked).

## named-disp-fifo-shape-position2-param-direct

Group the non-reloaded x/y writes through GX helpers while loading z and texcoords immediately at their FIFO writes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.75000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.65940%

Checksum: no (exploratory object build; not linked).

## named-disp-fifo-shape-position2-param-helper

Group the non-reloaded x/y writes through GX helpers while loading z and texcoords immediately at their FIFO writes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.75000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.65940%

Checksum: no (exploratory object build; not linked).

## named-disp-fifo-shape-position2-direct-param

Group the non-reloaded x/y writes through GX helpers while loading z and texcoords immediately at their FIFO writes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.75000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.65940%

Checksum: no (exploratory object build; not linked).

## named-disp-fifo-shape-position2-direct-direct

Group the non-reloaded x/y writes through GX helpers while loading z and texcoords immediately at their FIFO writes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.97500%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.72071%

Checksum: no (exploratory object build; not linked).

## named-disp-fifo-shape-position2-direct-helper

Group the non-reloaded x/y writes through GX helpers while loading z and texcoords immediately at their FIFO writes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.97500%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.72071%

Checksum: no (exploratory object build; not linked).

## named-disp-fifo-shape-param-xy-direct-z-param

Group the non-reloaded x/y writes through GX helpers while loading z and texcoords immediately at their FIFO writes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.93188%

Checksum: no (exploratory object build; not linked).

## named-disp-fifo-shape-param-xy-direct-z-direct

Group the non-reloaded x/y writes through GX helpers while loading z and texcoords immediately at their FIFO writes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 100.00000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 100.00000%

Checksum: no (exploratory object build; not linked).

## named-disp-fifo-shape-param-xy-direct-z-helper

Group the non-reloaded x/y writes through GX helpers while loading z and texcoords immediately at their FIFO writes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 100.00000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 100.00000%

Checksum: no (exploratory object build; not linked).

## named-disp-fifo-shape-direct-xy-param-z-param

Group the non-reloaded x/y writes through GX helpers while loading z and texcoords immediately at their FIFO writes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.93188%

Checksum: no (exploratory object build; not linked).

## named-disp-fifo-shape-direct-xy-param-z-direct

Group the non-reloaded x/y writes through GX helpers while loading z and texcoords immediately at their FIFO writes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.93188%

Checksum: no (exploratory object build; not linked).

## named-disp-fifo-shape-direct-xy-param-z-helper

Group the non-reloaded x/y writes through GX helpers while loading z and texcoords immediately at their FIFO writes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.93188%

Checksum: no (exploratory object build; not linked).

## named-disp-fifo-shape-direct-x-param-yz-param

Group the non-reloaded x/y writes through GX helpers while loading z and texcoords immediately at their FIFO writes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.93188%

Checksum: no (exploratory object build; not linked).

## named-disp-fifo-shape-direct-x-param-yz-direct

Group the non-reloaded x/y writes through GX helpers while loading z and texcoords immediately at their FIFO writes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.93188%

Checksum: no (exploratory object build; not linked).

## named-disp-fifo-shape-direct-x-param-yz-helper

Group the non-reloaded x/y writes through GX helpers while loading z and texcoords immediately at their FIFO writes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 99.93188%

Checksum: no (exploratory object build; not linked).

## six-matched-hybrid-before-pool

All six functions at 100%; dimension literals still duplicate their named definitions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 100.00000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 100.00000%

Checksum: no (exploratory object build; not linked).

## hybrid-remove-unused-dimension-defs

Remove now-unreferenced named dimension/reciprocal objects; the anonymous entries still need their original positions.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 100.00000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 91.48937%, .text 100.00000%

Checksum: no (exploratory object build; not linked).

## pool-interleave-defer-helper

Emit unused scalar helpers between the named constant definitions to pool the four anonymous values in their original slots.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 100.00000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 91.48937%, .text 100.00000%

Checksum: no (exploratory object build; not linked).

## pool-interleave-immediate-helper

Emit unused scalar helpers between the named constant definitions to pool the four anonymous values in their original slots.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 100.00000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 43.33333%, .text 100.00000%

Checksum: no (exploratory object build; not linked).

## pool-interleave-force-active-helper

Emit unused scalar helpers between the named constant definitions to pool the four anonymous values in their original slots.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 100.00000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 43.33333%, .text 100.00000%

Checksum: no (exploratory object build; not linked).

## pool-helper-flush-explicit-section

Test explicit read-only sections and delayed pragma reset to interleave helpers with named objects before deferred code flushes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 100.00000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 93.61702%, .text 100.00000%

Checksum: no (exploratory object build; not linked).

## pool-helper-flush-no-reset

Test explicit read-only sections and delayed pragma reset to interleave helpers with named objects before deferred code flushes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 100.00000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 43.33333%, .text 100.00000%

Checksum: no (exploratory object build; not linked).

## pool-helper-flush-no-reset-explicit

Test explicit read-only sections and delayed pragma reset to interleave helpers with named objects before deferred code flushes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 100.00000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 93.61702%, .text 100.00000%

Checksum: no (exploratory object build; not linked).

## pool-helper-flush-defer-reset-at-end

Test explicit read-only sections and delayed pragma reset to interleave helpers with named objects before deferred code flushes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 100.00000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 43.33333%, .text 100.00000%

Checksum: no (exploratory object build; not linked).

## pool-helper-flush-push-pop

Test explicit read-only sections and delayed pragma reset to interleave helpers with named objects before deferred code flushes.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 100.00000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 43.33333%, .text 100.00000%

Checksum: no (exploratory object build; not linked).

## pool-before-code-plain

Emit named data and individual literal helpers before any actual function parses; test whether constant propagation can be held off.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 79.80147%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.85000%, effKemuri2Disp 98.67387%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 91.48937%, .text 96.81062%

Checksum: no (exploratory object build; not linked).

## pool-before-code-prop-off

Emit named data and individual literal helpers before any actual function parses; test whether constant propagation can be held off.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 99.31624%, effKemuriDisp 95.64000%, effKemuri2Disp 93.50491%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 91.48937%, .text 96.50545%

Checksum: no (exploratory object build; not linked).

## pool-before-code-fold-off

Emit named data and individual literal helpers before any actual function parses; test whether constant propagation can be held off.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 79.80147%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.85000%, effKemuri2Disp 98.67387%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 91.48937%, .text 96.81062%

Checksum: no (exploratory object build; not linked).

## pool-before-code-prop-fold-off

Emit named data and individual literal helpers before any actual function parses; test whether constant propagation can be held off.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 99.31624%, effKemuriDisp 95.64000%, effKemuri2Disp 93.50491%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 91.48937%, .text 96.50545%

Checksum: no (exploratory object build; not linked).

## pool-before-code-force-active

Keep the unused literal helpers active while each is compiled before the real code.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 79.80147%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.85000%, effKemuri2Disp 98.67387%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 91.48937%, .text 96.81062%

Checksum: no (exploratory object build; not linked).

## mutable-pool-before-code

Use match-only mutable-typed objects in the read-only small-data section, preventing MWCC from propagating their initializers. Emit the four literal helpers before actual functions.

Functions: effKemuriEntry 84.50000%, effKemuri2Entry 92.02206%, effKemuriMain 91.63917%, effKemuri2Main 94.48718%, effKemuriDisp 95.62250%, effKemuri2Disp 96.11788%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 66.66667%, .sdata2 86.02151%, .text 93.99523%

Checksum: no (exploratory object build; not linked).

## literal-except-zero-one-scalar-fifo

Retain only zero/unity named objects for precise FIFO reloads; use the successful scalar-write shape with all other floats literal.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 100.00000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 89.89362%, .text 100.00000%

Checksum: no (exploratory object build; not linked).

## scalar-fifo-all-literal

Use all literals with the matching mixed FIFO-write structure.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.85000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 91.23711%, .text 99.14169%

Checksum: no (exploratory object build; not linked).

## scalar-fifo-all-literal-noinv

Use all literals with the matching mixed FIFO-write structure.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.85000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 91.23711%, .text 99.14169%

Checksum: no (exploratory object build; not linked).

## scalar-fifo-all-literal-disp-prop

Use all literals with the matching mixed FIFO-write structure.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 95.63500%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 92.18750%, .text 98.81062%

Checksum: no (exploratory object build; not linked).

## two-named-early-const-volatile-cast

Interleave named zero/unity with literal helpers before actual code; const volatile storage with const-qualified views may prevent initializer propagation without extra loads.

Functions: effKemuriEntry 86.45536%, effKemuri2Entry 91.91177%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 94.51000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 96.72139%

Checksum: no (exploratory object build; not linked).

## two-named-early-const-volatile-view

Interleave named zero/unity with literal helpers before actual code; const volatile storage with const-qualified views may prevent initializer propagation without extra loads.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.85000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 96.90722%, .text 99.14169%

Checksum: no (exploratory object build; not linked).

## two-named-early-const-plain

Interleave named zero/unity with literal helpers before actual code; const volatile storage with const-qualified views may prevent initializer propagation without extra loads.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.85000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 96.90722%, .text 99.14169%

Checksum: no (exploratory object build; not linked).

## two-named-early-extern-before-def

Interleave named zero/unity with literal helpers before actual code; const volatile storage with const-qualified views may prevent initializer propagation without extra loads.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.85000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 96.90722%, .text 99.14169%

Checksum: no (exploratory object build; not linked).

## early-two-views-plain-pointer-helper

Use literals outside Disp and alternate const views of the early zero/unity storage, preserving volatile FIFO reloads.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 94.51000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 97.91667%, .text 98.50409%

Checksum: no (exploratory object build; not linked).

## early-two-views-const-pointer-helper

Use literals outside Disp and alternate const views of the early zero/unity storage, preserving volatile FIFO reloads.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 94.51000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 97.91667%, .text 98.50409%

Checksum: no (exploratory object build; not linked).

## early-two-views-volatile-pointer-helper

Use literals outside Disp and alternate const views of the early zero/unity storage, preserving volatile FIFO reloads.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 94.51000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 97.91667%, .text 98.50409%

Checksum: no (exploratory object build; not linked).

## early-two-views-prop-off

Use literals outside Disp and alternate const views of the early zero/unity storage, preserving volatile FIFO reloads.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.31000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 97.91667%, .text 97.90463%

Checksum: no (exploratory object build; not linked).

## early-two-views-local-zero-unity

Use literals outside Disp and alternate const views of the early zero/unity storage, preserving volatile FIFO reloads.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 94.06500%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 97.91667%, .text 98.38284%

Checksum: no (exploratory object build; not linked).

## six-fifo-all-named-dimensions

Apply all named dimensions to the exact mixed FIFO write form to isolate remaining register-order differences.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.48750%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.41894%

Checksum: no (exploratory object build; not linked).

## named-disp-late-after-setup-half-first

Place named width assignments at successive scopes using the exact mixed FIFO form to reproduce retail preheader scheduling.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.63011%

Checksum: no (exploratory object build; not linked).

## named-disp-late-after-setup-twenty-first

Place named width assignments at successive scopes using the exact mixed FIFO form to reproduce retail preheader scheduling.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.63011%

Checksum: no (exploratory object build; not linked).

## named-disp-late-before-outer-half-first

Place named width assignments at successive scopes using the exact mixed FIFO form to reproduce retail preheader scheduling.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.63011%

Checksum: no (exploratory object build; not linked).

## named-disp-late-before-outer-twenty-first

Place named width assignments at successive scopes using the exact mixed FIFO form to reproduce retail preheader scheduling.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.63011%

Checksum: no (exploratory object build; not linked).

## named-disp-late-outer-top-half-first

Place named width assignments at successive scopes using the exact mixed FIFO form to reproduce retail preheader scheduling.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.76294%

Checksum: no (exploratory object build; not linked).

## named-disp-late-outer-top-twenty-first

Place named width assignments at successive scopes using the exact mixed FIFO form to reproduce retail preheader scheduling.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.76294%

Checksum: no (exploratory object build; not linked).

## named-disp-late-inner-top-half-first

Place named width assignments at successive scopes using the exact mixed FIFO form to reproduce retail preheader scheduling.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.63011%

Checksum: no (exploratory object build; not linked).

## named-disp-late-inner-top-twenty-first

Place named width assignments at successive scopes using the exact mixed FIFO form to reproduce retail preheader scheduling.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.63011%

Checksum: no (exploratory object build; not linked).

## named-disp-late-before-scale-half-first

Place named width assignments at successive scopes using the exact mixed FIFO form to reproduce retail preheader scheduling.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.76294%

Checksum: no (exploratory object build; not linked).

## named-disp-late-before-scale-twenty-first

Place named width assignments at successive scopes using the exact mixed FIFO form to reproduce retail preheader scheduling.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.76294%

Checksum: no (exploratory object build; not linked).

## named-disp-late-before-vertices-half-first

Place named width assignments at successive scopes using the exact mixed FIFO form to reproduce retail preheader scheduling.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.63011%

Checksum: no (exploratory object build; not linked).

## named-disp-late-before-vertices-twenty-first

Place named width assignments at successive scopes using the exact mixed FIFO form to reproduce retail preheader scheduling.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.63011%

Checksum: no (exploratory object build; not linked).

## named-disp2-late-after-p8-invariants-on

Use the all-named half/twenty form and put the 6.4 read after setup or within the outer loop.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.44991%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.80382%

Checksum: no (exploratory object build; not linked).

## named-disp2-late-after-p8-invariants-off

Use the all-named half/twenty form and put the 6.4 read after setup or within the outer loop.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.49902%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.82085%

Checksum: no (exploratory object build; not linked).

## named-disp2-late-before-p8-invariants-on

Use the all-named half/twenty form and put the 6.4 read after setup or within the outer loop.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.44991%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.80382%

Checksum: no (exploratory object build; not linked).

## named-disp2-late-before-p8-invariants-off

Use the all-named half/twenty form and put the 6.4 read after setup or within the outer loop.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.49902%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.82085%

Checksum: no (exploratory object build; not linked).

## named-disp2-late-before-n12-invariants-on

Use the all-named half/twenty form and put the 6.4 read after setup or within the outer loop.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.44991%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.80382%

Checksum: no (exploratory object build; not linked).

## named-disp2-late-before-n12-invariants-off

Use the all-named half/twenty form and put the 6.4 read after setup or within the outer loop.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.49902%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.82085%

Checksum: no (exploratory object build; not linked).

## named-disp2-late-outer-top-invariants-on

Use the all-named half/twenty form and put the 6.4 read after setup or within the outer loop.

BUILD FAILED
#    File: src\effect\eff_kemuri.c
# --------------------------------
#     541: const f32 negSizeX = -p12;
#   Error: ^^^^^
#   expression syntax error
#   Too many errors printed, aborting program

User break, cancelled...
ninja: build stopped: subcommand failed.

Checksum: no (exploratory object build; not linked).

## named-disp2-late-outer-top-invariants-off

Use the all-named half/twenty form and put the 6.4 read after setup or within the outer loop.

BUILD FAILED
#    File: src\effect\eff_kemuri.c
# --------------------------------
#     542: const f32 negSizeX = -p12;
#   Error: ^^^^^
#   expression syntax error
#   Too many errors printed, aborting program

User break, cancelled...
ninja: build stopped: subcommand failed.

Checksum: no (exploratory object build; not linked).

## named-disp2-late-before-last-begin-invariants-on

Use the all-named half/twenty form and put the 6.4 read after setup or within the outer loop.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.81336%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.92984%

Checksum: no (exploratory object build; not linked).

## named-disp2-late-before-last-begin-invariants-off

Use the all-named half/twenty form and put the 6.4 read after setup or within the outer loop.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.85265%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.94346%

Checksum: no (exploratory object build; not linked).

## named-disp2-late-before-last-transform-invariants-on

Use the all-named half/twenty form and put the 6.4 read after setup or within the outer loop.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.16503%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.70504%

Checksum: no (exploratory object build; not linked).

## named-disp2-late-before-last-transform-invariants-off

Use the all-named half/twenty form and put the 6.4 read after setup or within the outer loop.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.20432%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.71867%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-outer-f32-normal

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.76294%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-outer-f32-prop-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.06750%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.57697%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-outer-f32-cse-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.81250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.77998%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-outer-const-f32-normal

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.76294%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-outer-const-f32-prop-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.06750%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.57697%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-outer-const-f32-cse-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.81250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.77998%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-outer-volatile-f32-normal

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.08500%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.76430%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-outer-volatile-f32-prop-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 95.58000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.62670%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-outer-volatile-f32-cse-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.08500%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.76430%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-inner-f32-normal

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.63011%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-inner-f32-prop-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.58000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.44415%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-inner-f32-cse-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.32500%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.64714%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-inner-const-f32-normal

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.63011%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-inner-const-f32-prop-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.58000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.44415%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-inner-const-f32-cse-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.32500%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.64714%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-inner-volatile-f32-normal

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 95.59750%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.63147%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-inner-volatile-f32-prop-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 95.09250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.49387%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-inner-volatile-f32-cse-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 95.61000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.63488%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-function-f32-normal

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.48750%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.41894%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-function-f32-prop-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.80500%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.23297%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-function-f32-cse-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.53750%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.43256%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-function-const-f32-normal

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.48750%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.41894%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-function-const-f32-prop-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.80500%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.23297%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-function-const-f32-cse-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.53750%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.43256%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-function-volatile-f32-normal

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.79250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 96.86716%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-function-volatile-f32-prop-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.28750%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 96.72956%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-function-volatile-f32-cse-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.84250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 96.88079%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-outer-block-f32-normal

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.76294%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-outer-block-f32-prop-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.06750%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.57697%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-outer-block-f32-cse-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.81250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.77998%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-outer-block-const-f32-normal

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.76294%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-outer-block-const-f32-prop-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.06750%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.57697%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-outer-block-const-f32-cse-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.81250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.77998%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-outer-block-volatile-f32-normal

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.08500%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.76430%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-outer-block-volatile-f32-prop-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 95.58000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.62670%

Checksum: no (exploratory object build; not linked).

## named-disp-scoped-outer-block-volatile-f32-cse-off

Test width initialization in distinct scopes and optimization passes with the correct scalar FIFO write grouping.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 96.08500%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.76430%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-half-volatile-before-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.63011%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-half-volatile-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.70250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.20504%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-half-pointer-helper-before-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.63011%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-half-pointer-helper-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.76294%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-half-value-helper-before-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.63011%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-half-value-helper-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.76294%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-half-double-cast-before-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.63011%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-half-double-cast-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.76294%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-half-const-view-before-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.63011%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-half-const-view-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.76294%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-twenty-volatile-before-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.63011%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-twenty-volatile-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.25000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.62670%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-twenty-pointer-helper-before-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.30000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.64033%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-twenty-pointer-helper-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.80000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.77657%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-twenty-value-helper-before-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.30000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.64033%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-twenty-value-helper-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.80000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.77657%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-twenty-double-cast-before-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.63011%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-twenty-double-cast-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.76294%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-twenty-const-view-before-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.63011%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-twenty-const-view-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.76294%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-both-volatile-before-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.63011%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-both-volatile-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.07750%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.03474%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-both-pointer-helper-before-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.35000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.65395%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-both-pointer-helper-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.78678%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-both-value-helper-before-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.35000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.65395%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-both-value-helper-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.78678%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-both-double-cast-before-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.63011%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-both-double-cast-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.76294%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-both-const-view-before-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.26250%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.63011%

Checksum: no (exploratory object build; not linked).

## named-disp-width-view-both-const-view-outer

Alter the read expression and its evaluation scope to change the hoisted width register webs without changing arithmetic.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.76294%

Checksum: no (exploratory object build; not linked).

## named-width-identity-fifo-direct-direct

Combine named width identity getters with alternate FIFO expression groups to adjust register affinity.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.76294%

Checksum: no (exploratory object build; not linked).

## named-width-identity-fifo-direct-param

Combine named width identity getters with alternate FIFO expression groups to adjust register affinity.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.85000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.79019%

Checksum: no (exploratory object build; not linked).

## named-width-identity-fifo-direct-helper

Combine named width identity getters with alternate FIFO expression groups to adjust register affinity.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.75000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.76294%

Checksum: no (exploratory object build; not linked).

## named-width-identity-fifo-param-direct

Combine named width identity getters with alternate FIFO expression groups to adjust register affinity.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.78678%

Checksum: no (exploratory object build; not linked).

## named-width-identity-fifo-param-param

Combine named width identity getters with alternate FIFO expression groups to adjust register affinity.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.85000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.79019%

Checksum: no (exploratory object build; not linked).

## named-width-identity-fifo-param-helper

Combine named width identity getters with alternate FIFO expression groups to adjust register affinity.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 99.83750%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.78678%

Checksum: no (exploratory object build; not linked).

## named-width-identity-fifo-position2-direct

Combine named width identity getters with alternate FIFO expression groups to adjust register affinity.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.82500%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.51090%

Checksum: no (exploratory object build; not linked).

## named-width-identity-fifo-position2-param

Combine named width identity getters with alternate FIFO expression groups to adjust register affinity.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.82500%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.51090%

Checksum: no (exploratory object build; not linked).

## named-width-identity-fifo-position2-helper

Combine named width identity getters with alternate FIFO expression groups to adjust register affinity.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 98.82500%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.51090%

Checksum: no (exploratory object build; not linked).

## named-width-identity-fifo-group-position-direct

Combine named width identity getters with alternate FIFO expression groups to adjust register affinity.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.60000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.17711%

Checksum: no (exploratory object build; not linked).

## named-width-identity-fifo-group-position-param

Combine named width identity getters with alternate FIFO expression groups to adjust register affinity.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.62500%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.18392%

Checksum: no (exploratory object build; not linked).

## named-width-identity-fifo-group-position-helper

Combine named width identity getters with alternate FIFO expression groups to adjust register affinity.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 97.60000%, effKemuri2Disp 96.62869%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 98.17711%

Checksum: no (exploratory object build; not linked).

## named-sixp4-getter-value-before-quad

Use inline scalar getters to give the named 6.4 read a fresh temporary web.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.85265%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.94346%

Checksum: no (exploratory object build; not linked).

## named-sixp4-getter-value-before-setup

Use inline scalar getters to give the named 6.4 read a fresh temporary web.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.46955%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.81062%

Checksum: no (exploratory object build; not linked).

## named-sixp4-getter-value-after-setup

Use inline scalar getters to give the named 6.4 read a fresh temporary web.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.49902%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.82085%

Checksum: no (exploratory object build; not linked).

## named-sixp4-getter-pointer-before-quad

Use inline scalar getters to give the named 6.4 read a fresh temporary web.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.85265%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.94346%

Checksum: no (exploratory object build; not linked).

## named-sixp4-getter-pointer-before-setup

Use inline scalar getters to give the named 6.4 read a fresh temporary web.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.46955%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.81062%

Checksum: no (exploratory object build; not linked).

## named-sixp4-getter-pointer-after-setup

Use inline scalar getters to give the named 6.4 read a fresh temporary web.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.49902%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.82085%

Checksum: no (exploratory object build; not linked).

## named-sixp4-getter-local-return-before-quad

Use inline scalar getters to give the named 6.4 read a fresh temporary web.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.85265%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.94346%

Checksum: no (exploratory object build; not linked).

## named-sixp4-getter-local-return-before-setup

Use inline scalar getters to give the named 6.4 read a fresh temporary web.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.46955%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.81062%

Checksum: no (exploratory object build; not linked).

## named-sixp4-getter-local-return-after-setup

Use inline scalar getters to give the named 6.4 read a fresh temporary web.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.49902%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.82085%

Checksum: no (exploratory object build; not linked).

## named-sixp4-getter-double-return-before-quad

Use inline scalar getters to give the named 6.4 read a fresh temporary web.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.85265%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.94346%

Checksum: no (exploratory object build; not linked).

## named-sixp4-getter-double-return-before-setup

Use inline scalar getters to give the named 6.4 read a fresh temporary web.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.46955%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.81062%

Checksum: no (exploratory object build; not linked).

## named-sixp4-getter-double-return-after-setup

Use inline scalar getters to give the named 6.4 read a fresh temporary web.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 92.64000%, effKemuri2Disp 99.49902%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 0.00000%, .sdata2 100.00000%, .text 97.82085%

Checksum: no (exploratory object build; not linked).

## guarded-partial-sbss2

Retain the six exact functions, own the data tables and colors, restore scratch objects to sbss2, and guard match-only FIFO/pragmas with natural GX calls. Four anonymous floats still follow the named block.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 100.00000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 100.00000%, .sdata2 91.48937%, .text 100.00000%

Checksum: no (exploratory object build; not linked).

## guarded-partial-tail-padding

Flush the deferred code using a conversion helper that reuses the unsigned conversion double, then append the exact four-byte sdata2 padding. The four dimension constants remain displaced.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 100.00000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 85.71429%, .sbss2 100.00000%, .sdata2 91.48937%, .text 100.00000%

Checksum: no (exploratory object build; not linked).

## guarded-partial-tail-padding-signed

Use the signed conversion helper to preserve the retail order of the two conversion doubles while appending the pool padding.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 100.00000%, effKemuri2Disp 100.00000%
Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 100.00000%, .sdata2 91.48937%, .text 100.00000%

Checksum: no (exploratory object build; not linked).

## final-matching-link-and-restoration

Retained `guarded-partial-tail-padding-signed.c` as the final source. Temporarily flipped configure.py to Matching and linked; restored NonMatching after the checksum failure. Added CODEX_RESULT.md with the remaining pool problem and continuation checkpoints.

Functions: effKemuriEntry 100.00000%, effKemuri2Entry 100.00000%, effKemuriMain 100.00000%, effKemuri2Main 100.00000%, effKemuriDisp 100.00000%, effKemuri2Disp 100.00000%.
Sections: .data 100.00000% (120 bytes), .rodata 100.00000% (56 bytes), .sbss2 100.00000% (8 bytes), .sdata2 91.48937% (192 bytes), .text 100.00000% (5872 target bytes; unused pool helper stripped at link).

Checksum: NO with Matching (31 files OK; MarioSt.dol failed; 218 DOL bytes differ). YES after restoration to NonMatching (32 files OK, using the original kemuri object).

Verification: explicit relocation-value comparison covered all six functions with zero findings; standard reloc_value_check clean; natural branch compiled without MUST_MATCH; git diff --check clean. configure.py is unchanged; tools/ untouched.

Actual anonymous slots: half at 0xac, 20 at 0xb0, 6.4 at 0xb4, reciprocal at 0xb8; target slots respectively 0x5c, 0x24, 0x64, 0x88. Padding at 0xbc matches.

Correction: earlier entries reporting a compiler hang reflect agent_check's timeout-marker misclassification of the printed Ninja command. Subsequent direct Ninja output identified ordinary compiler errors; see CODEX_RESULT.md.
