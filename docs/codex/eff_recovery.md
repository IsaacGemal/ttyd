# eff_recovery linking attempts

Budget: at most 40 build experiments. Work stays in this checkout; tools/ is unchanged. Rejected function regressions will be restored. Object experiments do not test the linked checksum; this is distinguished from failed checksum builds.

## 01: pool_plan --no-search probe

The all-literal probe failed because pool_plan replaces identifiers in existing string #define lines, producing invalid #define string string. It restored the source. No probe scores available; checksum no (not tested). The static table confirms the target uses one shared half and positive 108, with named colours before the anonymous pool. Its BASE-REGISTER diagnosis is spurious here: the existing .rodata already compares IDENTICAL. No tools edits made.

## 02: Baseline after source restoration; confirm seven functions and duplicated half.

Functions: effRecoveryEntry 100.00000%; effRecoveryMain 100.00000%; effRecoveryMain2 100.00000%; effRecoveryDisp 100.00000%; effRecoveryDisp2 100.00000%; effRecoveryDisp3 100.00000%; effRecoveryDisp4 100.00000%.

Sections: .data 92.90188%; .rodata 100.00000%; .sbss2 100.00000%; .sdata 100.00000%; .sdata2 96.20254%; .text 100.00000%.

Checksum: no (object experiment; not tested).

```text
== .sdata2: CONTENT
   DUP_OURS (1):
     0.5                      @476                         effRecoveryDisp4, effRecoveryDisp2
== .rodata: IDENTICAL
```

Relocation-value findings: 0.

## 03: Disp3 half literal local; remove its named definition to merge the half pool.

Functions: effRecoveryEntry 100.00000%; effRecoveryMain 100.00000%; effRecoveryMain2 100.00000%; effRecoveryDisp 100.00000%; effRecoveryDisp2 100.00000%; effRecoveryDisp3 100.00000%; effRecoveryDisp4 100.00000%.

Sections: .data 92.90188%; .rodata 100.00000%; .sbss2 100.00000%; .sdata 100.00000%; .sdata2 97.43590%; .text 100.00000%.

Checksum: no (object experiment; not tested).

```text
== .sdata2: ORDER
   first displaced entry 4: target 0.0174533 (effRecoveryDisp4, effRecoveryDisp3, effRecoveryDisp2, effRecoveryDisp) vs ours 108 (effRecoveryDisp3); 12 entries out of place
== .rodata: IDENTICAL
```

Relocation-value findings: 0.

## 04: Replace positive 108 with a block local, keeping half and runtime negation.

Functions: effRecoveryEntry 100.00000%; effRecoveryMain 100.00000%; effRecoveryMain2 100.00000%; effRecoveryDisp 100.00000%; effRecoveryDisp2 100.00000%; effRecoveryDisp3 99.91290%; effRecoveryDisp4 100.00000%.

Sections: .data 92.90188%; .rodata 100.00000%; .sbss2 100.00000%; .sdata 100.00000%; .sdata2 100.00000%; .text 99.98738%.

Checksum: no (object experiment; not tested).

```text
== .sdata2: IDENTICAL
== .rodata: IDENTICAL
```

Relocation-value findings: 0.

## 05: Use division by two for the half factor, testing reciprocal register allocation.

Functions: effRecoveryEntry 100.00000%; effRecoveryMain 100.00000%; effRecoveryMain2 100.00000%; effRecoveryDisp 100.00000%; effRecoveryDisp2 100.00000%; effRecoveryDisp3 99.91290%; effRecoveryDisp4 100.00000%.

Sections: .data 92.90188%; .rodata 100.00000%; .sbss2 100.00000%; .sdata 100.00000%; .sdata2 100.00000%; .text 99.98738%.

Checksum: no (object experiment; not tested).

```text
== .sdata2: IDENTICAL
== .rodata: IDENTICAL
```

Relocation-value findings: 0.

## 06: Inline width helper with scalar parameter and output locals; change FPR web ranks.

Functions: effRecoveryEntry 100.00000%; effRecoveryMain 100.00000%; effRecoveryMain2 100.00000%; effRecoveryDisp 100.00000%; effRecoveryDisp2 100.00000%; effRecoveryDisp3 97.89198%; effRecoveryDisp4 100.00000%.

Sections: .data 92.90188%; .rodata 100.00000%; .sbss2 100.00000%; .sdata 100.00000%; .sdata2 97.46835%; .text 99.69460%.

Checksum: no (object experiment; not tested).

```text
== .sdata2: CONTENT
   EXTRA (2):
     -54                      @514                         effRecoveryDisp3
     54                       @516                         effRecoveryDisp3
   MISSING (1):
     108                      float_108_8041cafc           effRecoveryDisp3   [prod]
== .rodata: IDENTICAL
```

Relocation-value findings: 0.

## 07: Pass non-const width local through the inline helper, preserving runtime arithmetic.

Functions: effRecoveryEntry 100.00000%; effRecoveryMain 100.00000%; effRecoveryMain2 100.00000%; effRecoveryDisp 100.00000%; effRecoveryDisp2 100.00000%; effRecoveryDisp3 99.91290%; effRecoveryDisp4 100.00000%.

Sections: .data 92.90188%; .rodata 100.00000%; .sbss2 100.00000%; .sdata 100.00000%; .sdata2 100.00000%; .text 99.98738%.

Checksum: no (object experiment; not tested).

```text
== .sdata2: IDENTICAL
== .rodata: IDENTICAL
```

Relocation-value findings: 0.

## 08: Disable propagation for Disp3 with explicit positive width and half locals.

Functions: effRecoveryEntry 100.00000%; effRecoveryMain 100.00000%; effRecoveryMain2 100.00000%; effRecoveryDisp 100.00000%; effRecoveryDisp2 100.00000%; effRecoveryDisp3 99.25087%; effRecoveryDisp4 100.00000%.

Sections: .data 92.90188%; .rodata 100.00000%; .sbss2 100.00000%; .sdata 100.00000%; .sdata2 100.00000%; .text 99.89147%.

Checksum: no (object experiment; not tested).

```text
== .sdata2: IDENTICAL
== .rodata: IDENTICAL
```

Relocation-value findings: 0.

## 09: Reverse multiply operands to give the literal width the target f1 register.

Functions: effRecoveryEntry 100.00000%; effRecoveryMain 100.00000%; effRecoveryMain2 100.00000%; effRecoveryDisp 100.00000%; effRecoveryDisp2 100.00000%; effRecoveryDisp3 99.96516%; effRecoveryDisp4 100.00000%.

Sections: .data 92.90188%; .rodata 100.00000%; .sbss2 100.00000%; .sdata 100.00000%; .sdata2 100.00000%; .text 99.99495%.

Checksum: no (object experiment; not tested).

```text
== .sdata2: IDENTICAL
== .rodata: IDENTICAL
```

Relocation-value findings: 0.

## 10: Extend the half local through the preheader with the existing overwritten f27 assignment.

Functions: effRecoveryEntry 100.00000%; effRecoveryMain 100.00000%; effRecoveryMain2 100.00000%; effRecoveryDisp 100.00000%; effRecoveryDisp2 100.00000%; effRecoveryDisp3 99.96516%; effRecoveryDisp4 100.00000%.

Sections: .data 92.90188%; .rodata 100.00000%; .sbss2 100.00000%; .sdata 100.00000%; .sdata2 100.00000%; .text 99.99495%.

Checksum: no (object experiment; not tested).

```text
== .sdata2: IDENTICAL
== .rodata: IDENTICAL
```

Relocation-value findings: 0.

## 11: Widen only the negated intermediate and cast it back before the single-precision product.

Functions: effRecoveryEntry 100.00000%; effRecoveryMain 100.00000%; effRecoveryMain2 100.00000%; effRecoveryDisp 100.00000%; effRecoveryDisp2 100.00000%; effRecoveryDisp3 99.61672%; effRecoveryDisp4 100.00000%.

Sections: .data 92.90188%; .rodata 100.00000%; .sbss2 100.00000%; .sdata 100.00000%; .sdata2 100.00000%; .text 99.94447%.

Checksum: no (object experiment; not tested).

```text
== .sdata2: IDENTICAL
== .rodata: IDENTICAL
```

Relocation-value findings: 0.

## 12: Use scalarized structure fields for width, half, and negative width; alter temporary ranks.

Functions: effRecoveryEntry 100.00000%; effRecoveryMain 100.00000%; effRecoveryMain2 100.00000%; effRecoveryDisp 100.00000%; effRecoveryDisp2 100.00000%; effRecoveryDisp3 99.96516%; effRecoveryDisp4 100.00000%.

Sections: .data 92.90188%; .rodata 100.00000%; .sbss2 100.00000%; .sdata 100.00000%; .sdata2 100.00000%; .text 99.99495%.

Checksum: no (object experiment; not tested).

```text
== .sdata2: IDENTICAL
== .rodata: IDENTICAL
```

Relocation-value findings: 0.

## 13: Inline negation helper makes the negative width an expression temporary.

Functions: effRecoveryEntry 100.00000%; effRecoveryMain 100.00000%; effRecoveryMain2 100.00000%; effRecoveryDisp 100.00000%; effRecoveryDisp2 100.00000%; effRecoveryDisp3 99.96516%; effRecoveryDisp4 100.00000%.

Sections: .data 92.90188%; .rodata 100.00000%; .sbss2 100.00000%; .sdata 100.00000%; .sdata2 100.00000%; .text 99.99495%.

Checksum: no (object experiment; not tested).

```text
== .sdata2: IDENTICAL
== .rodata: IDENTICAL
```

Relocation-value findings: 0.

## 14: Initialize the width local through a static constant so storage can fold while retaining its temporary shape.

Functions: effRecoveryEntry 100.00000%; effRecoveryMain 100.00000%; effRecoveryMain2 100.00000%; effRecoveryDisp 100.00000%; effRecoveryDisp2 100.00000%; effRecoveryDisp3 99.96516%; effRecoveryDisp4 100.00000%.

Sections: .data 92.90188%; .rodata 100.00000%; .sbss2 100.00000%; .sdata 100.00000%; .sdata2 100.00000%; .text 99.99495%.

Checksum: no (object experiment; not tested).

```text
== .sdata2: IDENTICAL
== .rodata: IDENTICAL
```

Relocation-value findings: 0.

## 15: Seed the anonymous prefix with an immediately generated unused helper, keeping opaque 108 after code; place colour definitions first.

Functions: effRecoveryEntry 100.00000%; effRecoveryMain 100.00000%; effRecoveryMain2 100.00000%; effRecoveryDisp 100.00000%; effRecoveryDisp2 100.00000%; effRecoveryDisp3 100.00000%; effRecoveryDisp4 100.00000%.

Sections: .data 92.90188%; .rodata 100.00000%; .sbss2 100.00000%; .sdata 100.00000%; .sdata2 100.00000%; .text 100.00000%.

Checksum: no (object experiment; not tested).

```text
== .sdata2: IDENTICAL
== .rodata: IDENTICAL
```

Relocation-value findings: 0.

## 16: Matching link attempt with default tool discovery

Functions: all seven 100%; sections: .text/.rodata/.sbss2/.sdata/.sdata2 100%, .data 92.90188% (raw .data bytes identical). Checksum: no (build blocked before linking). Default configure regenerated a download dependency for the already installed objdiff-cli; the download failed because network DNS is unavailable. Reconfigured with explicit paths to the tools already inside this checkout; no tools/ changes.

The object has 364/368 .rodata bytes and 156/160 .sdata2 bytes: only the final zero padding is absent, to be supplied by linker alignment. .data is exactly 1920 bytes; .sdata is exactly 120 bytes. The currently present DOL predates this link and is not evidence for the Matching variant.

The optional mwcc debugger was unavailable (missing installed debugger script); it did not compile or consume a build experiment.

## 17: Matching full link with installed tools

Functions: all seven 100%. Sections: .text/.rodata/.sbss2/.sdata/.sdata2 100%; .data 92.90188%. Checksum: no; 31 files OK, DOL failed. link_diag: 57 DOL bytes differ, only four jump-table address loads and the jump-table block. Pool addresses, scratch addresses, and all float loads are correct. recoveryPoolPrefix is absent from the linked ELF.

The inherited gap_77C_8036FD18_data is actually emitted at 0x8036FD1C, immediately before the first jump table. It shifts all four tables by four bytes. Object .data bytes looked identical because the unlinked jump-table entries are relocation placeholders, and tail padding hides the size change. Remove this gap per AGENT_BRIEF's exception for .data ending in jump tables; linker alignment supplies the true trailing zero word.

## 18: Remove inherited gap before compiler-generated jump tables; document pool prefix and guard matching loop pragma.

Functions: effRecoveryEntry 100.00000%; effRecoveryMain 100.00000%; effRecoveryMain2 100.00000%; effRecoveryDisp 100.00000%; effRecoveryDisp2 100.00000%; effRecoveryDisp3 100.00000%; effRecoveryDisp4 100.00000%.

Sections: .data 100.00000%; .rodata 100.00000%; .sbss2 100.00000%; .sdata 100.00000%; .sdata2 100.00000%; .text 100.00000%.

Checksum: yes; Matching ninja passed, all 32 files OK. link_diag: dol: 0 bytes differ. cmp confirms the complete DOL equals the original.

```text
== .sdata2: IDENTICAL
== .rodata: IDENTICAL
```

Relocation-value findings: 0.

## 19: Complete CI invocation with installed-tool paths

Functions: all seven 100%. Sections: .text/.data/.rodata/.sdata/.sdata2/.sbss2 all 100%. Checksum: yes (experiment 18 retained; no source changes).

The complete tool test suite ran 200 tests and encountered one environment error: test_mwccdbg.LockTest.test_second_holder_waits_for_the_first cannot bind a localhost socket in this sandbox (PermissionError: Operation not permitted). CI stops at this test gate. No tool tests or tools/ files were changed. build/recovery-task/19-ci.log records the full failure.

## 20: Remaining standard CI stages, run separately after the test gate

Functions: all seven 100%. Sections: .text/.data/.rodata/.sdata/.sdata2/.sbss2 all 100%. Checksum: yes; Matching ninja remains clean, all 32 files OK.

Lint, prototype checks, extend_data uniqueness, compiler hang guard, build, natural compilation without MUST_MATCH, changed data validation, and the accepted-state ratchet all pass. No regressions; six measures improve. A workspace-local wrapper adds configure.py's existing explicit tool-path flags so CI uses the installed tools instead of downloading them; the checks themselves are unchanged. See build/recovery-task/20-ci-remaining.log. The full CI gate is still accurately reported as blocked by the single socket-binding test.

## Final state and what fixed what

- configure.py retains Object(Matching, "effect/eff_recovery.c"). Seven of seven original functions and all six compared sections score 100%.
- A MUST_MATCH half local in Disp3 removes the duplicate 0.5 object and shares Disp4/Disp2's anonymous half. The opaque named positive 108 preserves Disp3's original floating-point register allocation.
- The guarded immediately generated recoveryPoolPrefix helper seeds the eleven literals preceding 108, with the colour words defined first. The deferred real functions reuse these entries, and 108 now lands at its original +0x3c position. Both pool comparisons are IDENTICAL. The helper exists in the object but is absent from MarioSt.elf.
- Removing the inherited source-level .data gap restores the four compiler-generated switch tables to their original addresses. The original section's trailing zero word is linker padding. The linker likewise supplies .rodata/.sdata2 tail padding.
- Matching ninja passes all 32 checksums; link_diag reports dol: 0 bytes differ; cmp confirms the complete MarioSt.dol equals orig/G8MJ01/sys/main.dol. The requested reloc_value_check.py eff_recovery --all and explicit check_unit both report zero findings. Natural compilation passes; no whole-project regression is reported.
- Twenty logged attempt/validation entries are within the 40-experiment budget (the failed CI test gate and unavailable debugger did not compile a variant). No remaining link blocker. Full CI's only limitation is the sandbox-denied localhost socket test.
- No tools/ changes, downloaded/reference code, commits, or pushes. Existing CODEX_TASK.md and PRIOR_REPORT.md were preserved.
