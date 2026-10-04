# eff_fire linking attempts

All work stays in this checkout; tools/ is unchanged. Experimental regressions are restored before finishing. Section percentages are objdiff's fuzzy scores; pool comparisons and the linked checksums decide byte identity.

## Initial investigation

Ran pool_plan.py before changing the source. It found no named prefix that fixes the pool: the all-literal probe folds -0.5, -16, and -12 and loses 32 and 24. Probe function scores: SmokeDisp 96.44%, Disp2 96.86%, Disp3 97.81%, SmokeMain 99.48%, Main 99.76%, Disp 99.98%, Entry 100%. Baseline has all seven functions and .text/.rodata/.data/.sdata/.sbss2 at 100%; .sdata2 is 98.99666%, 208 bytes versus 184. The duplicated values are reciprocal 128, zero, one, third, sixteen, and half. Checksum: no (baseline Matching flip fails; see next entry). No changes from the probe retained.

## All floats literal (named color prefix preserved)

Functions: effFireEntry 100.00000%, effFireMain 99.76063%, effFireDisp 99.98202%, effFireDisp2 96.86242%, effFireDisp3 97.81317%, effFireSmokeMain 99.48113%, effFireSmokeDisp 96.43541%.

Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 100.00000%, .sdata 100.00000%, .sdata2 91.59892%, .text 98.38510%.

Checksum: no (not run for this exploratory variant).

```text
== .sdata2: CONTENT
   EXTRA (3):
     -12                      @628                         effFireDisp2   [neg]
     -0.5                     @420                         effFireSmokeDisp   [neg]
     -16                      @558                         effFireDisp3, effFireDisp2   [neg]
   MISSING (2):
     32                       float_32_8041e118            effFireDisp3, effFireDisp2   [prod]
     24                       float_24_8041e134            effFireDisp2   [prod]
== .rodata: IDENTICAL
```

## SmokeDisp local one and half prevent folded negation and repeated half loads

Functions: effFireEntry 100.00000%, effFireMain 99.76063%, effFireDisp 99.98202%, effFireDisp2 96.86242%, effFireDisp3 97.81317%, effFireSmokeMain 99.48113%, effFireSmokeDisp 100.00000%.

Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 100.00000%, .sdata 100.00000%, .sdata2 93.69863%, .text 98.64687%.

Checksum: no (not run for this exploratory variant).

```text
== .sdata2: CONTENT
   EXTRA (2):
     -16                      @557                         effFireDisp3, effFireDisp2   [neg]
     -12                      @627                         effFireDisp2   [neg]
   MISSING (2):
     32                       float_32_8041e118            effFireDisp3, effFireDisp2   [prod]
     24                       float_24_8041e134            effFireDisp2   [prod]
== .rodata: IDENTICAL
```

## Disp2 and Disp3 local widths, half and twelve retain runtime products

Functions: effFireEntry 100.00000%, effFireMain 99.76063%, effFireDisp 99.98202%, effFireDisp2 97.94463%, effFireDisp3 100.00000%, effFireSmokeMain 99.48113%, effFireSmokeDisp 100.00000%.

Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 100.00000%, .sdata 100.00000%, .sdata2 95.93496%, .text 99.51511%.

Checksum: no (not run for this exploratory variant).

```text
== .sdata2: CONTENT
   EXTRA (1):
     -16                      @633                         effFireDisp2   [neg]
== .rodata: IDENTICAL
```

## SmokeMain fade local, Disp scale division, Disp2 second width local

Functions: effFireEntry 100.00000%, effFireMain 99.76063%, effFireDisp 100.00000%, effFireDisp2 97.94463%, effFireDisp3 100.00000%, effFireSmokeMain 100.00000%, effFireSmokeDisp 100.00000%.

Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 100.00000%, .sdata 100.00000%, .sdata2 95.93496%, .text 99.53795%.

Checksum: no (not run for this exploratory variant).

```text
== .sdata2: CONTENT
   EXTRA (1):
     -16                      @633                         effFireDisp2   [neg]
== .rodata: IDENTICAL
```

## Disp2 cached height and sixteen plus second runtime product; Main stretch locals

Functions: effFireEntry 100.00000%, effFireMain 100.00000%, effFireDisp 100.00000%, effFireDisp2 100.00000%, effFireDisp3 100.00000%, effFireSmokeMain 100.00000%, effFireSmokeDisp 100.00000%.

Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 100.00000%, .sdata 100.00000%, .sdata2 96.98630%, .text 100.00000%.

Checksum: no (not run for this exploratory variant).

```text
== .sdata2: ORDER
   first displaced entry 23: target -8 (effFireMain) vs ours 32 (effFireDisp3, effFireDisp2); 21 entries out of place
== .rodata: IDENTICAL
```

## Deferred smoke-speed helper between SmokeMain and Disp3 pools

Functions: effFireEntry 100.00000%, effFireMain 100.00000%, effFireDisp 100.00000%, effFireDisp2 100.00000%, effFireDisp3 100.00000%, effFireSmokeMain 100.00000%, effFireSmokeDisp 100.00000%.

Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 100.00000%, .sdata 100.00000%, .sdata2 99.17808%, .text 100.00000%.

Checksum: no; Matching ninja reported 31 files OK, DOL failed. link_diag narrowed it to four bytes in scratch-word loads; .sbss2 symbols were reversed.

```text
== .sdata2: IDENTICAL
== .rodata: IDENTICAL
```

## Reverse zero-initialized scratch declarations to match sbss2 addresses

Functions: effFireEntry 100.00000%, effFireMain 100.00000%, effFireDisp 100.00000%, effFireDisp2 100.00000%, effFireDisp3 100.00000%, effFireSmokeMain 100.00000%, effFireSmokeDisp 100.00000%.

Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 100.00000%, .sdata 100.00000%, .sdata2 99.17808%, .text 100.00000%.

Checksum: yes; Matching ninja passed all 32 checksums.

```text
== .sdata2: IDENTICAL
== .rodata: IDENTICAL
```

## Guard matching local mutability and division, document pool and scratch order

Functions: effFireEntry 100.00000%, effFireMain 100.00000%, effFireDisp 100.00000%, effFireDisp2 100.00000%, effFireDisp3 100.00000%, effFireSmokeMain 100.00000%, effFireSmokeDisp 100.00000%.

Sections: .data 100.00000%, .rodata 100.00000%, .sbss2 100.00000%, .sdata 100.00000%, .sdata2 99.17808%, .text 100.00000%.

Checksum: yes; Matching ninja passed all 32 checksums; link_diag reported zero differing DOL bytes.

```text
== .sdata2: IDENTICAL
== .rodata: IDENTICAL
```

## Final verification and findings

- configure.py retains `Object(Matching, "effect/eff_fire.c")`. All seven functions remain 100%.
- The float definitions and named Fire string were replaced by anonymous literals. Non-const local factors/dimensions reproduce the original runtime negations and register allocation; the natural build uses const locals. Division by 32 reproduces Disp's multiply operand order after strength reduction.
- The deferred MUST_MATCH smoke-speed helper pools -8 after SmokeMain and before Disp3; its unused body is stripped, as with the existing conversion/string pool helpers. No opt_propagation or opt_loop_invariants pragmas were needed.
- Reversing the scratch declarations fixes the original .sbss2 addresses, eliminating the final four DOL differences.
- Sections: .text/.data/.rodata/.sdata/.sbss2 all 100%; objdiff .sdata2 99.17808% because the compiler's five-byte Fire string ends the unlinked section three zero padding bytes earlier. Pool entries are IDENTICAL, and linker padding produces identical final bytes.
- Final checksum: yes, all 32 files OK; link_diag: `dol: 0 bytes differ`. Direct reloc_value_check.check_unit on this checkout's original/built eff_fire objects: zero findings. The requested reloc_value_check.py eff_fire command also reports zero findings (Matching units are skipped by its CLI).
- Full tools/ci.py: PASS, including natural compilation without MUST_MATCH, data validation, and no ratchet regressions. git diff --check: clean.
- pool_plan's single-named-constant probe restored Disp with the reciprocal 32 constant and SmokeMain with 0.99, but could not produce a compatible named prefix. These probe edits were restored; the final solution uses locals and anonymous pooling.
- No tools/ edits, fetched code, commits, or pushes.
