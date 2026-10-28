# eff_ultra_hammer attempts

Budget: at most 30 build experiments. Existing MSR asm is unchanged. No tools/ files are edited.

## Setup and baseline
- Read CODEX_TASK.md, docs/AGENT_BRIEF.md, relevant CONTEXT and Codex pool recipes, original handler assembly, and the build/check scripts.
- Existing source object has byte-identical .rodata (48 bytes) and .sdata2 (32 bytes). Error strings and unsigned conversion double are in the separate `error_handler.c` unit.
- Pool-plan probe and baseline checks failed before compilation: absent Ninja log makes installed tools look unbuilt and triggers unavailable network downloads. Configured explicit paths to the installed local tools. These pre-compilation failures are setup checks, not source build experiments.

## Experiment 1: unsigned font conversion + pool_data
- Cast both font dimensions to u32; enable pool_data under MUST_MATCH.
- Result: handler 97.05%; .rodata grows 48 -> 56 bytes with unsigned double at +0x30. External string base cannot address that compiler-owned pool. Existing register and frame mismatches remain. No link attempted.

## Experiment 2: explicit C conversion using existing external mask
- Tested two initialized double/word unions and subtraction of the mask through the existing string base.
- Result: 96.51%; correct frame size but wrong explicit stack slots, double fsub/frsp rather than native fsubs, and register differences. Rejected; restore native conversions.

## Experiment 3: literal error strings and native unsigned conversions
- Enabled pool_data only around handler group; replaced UH_STR base+offset expressions with the original string literals.
- Result: 98.21%; all four other functions stay 100%. Correct native conversion and frame, but pool base includes the preceding 48 effect bytes, so handler offsets are all +0x30 and register colouring still differs. New .rodata contains the exact 48+408 original bytes.

## Experiment 4: separate emission groups
- Move handler/setup group after effect source and flush deferred effect functions with a signed conversion helper.
- Result: still 98.21%; helper emits its own signed double ahead of effect constants and shifts pool layout. A codegen flush does not create a distinct .rodata base. Rejected.

## Experiment 5: alternate const section during parsing
- Tested a const_type section selection around the deferred handler.
- Result: unchanged 98.21%, still one .rodata section (456 bytes). Const section state applies at emission rather than retaining the surrounding parse-time selection. Rejected.

## Experiment 6: distinct emitted handler constant section
- Flush effect functions, then emit handler immediately with a distinct const section and pool_data.
- Result: .rodata stays exact 48 bytes; handler section is exactly 408 bytes and mask offset +0x190. Handler 98.24%; remaining register and two local stack-copy differences. Custom section linking is not yet verified.

## Experiment 7: nameable local register order
- Order handler locals for the original register assignments.
- Result: 98.95%; error/context/dsisr/dar, font, loop pointers/counters and dimensions now match. Remaining pool/base/retrace-register rotation, clear-colour store order, and address-add shape.

## Experiment 8: common-subexpression optimization on
- Result: identical 98.95%. The pragma no longer matters for this pooled shape; revert for now and remove when final shape is known.

## Experiment 9: link-layout probe (in progress)
- Temporarily mark effect Matching and remove force_active from the data unit's now-unreferenced rodata (retain it on sdata2).
- Ninja rebuilds the existing checkout's source objects because the imported build has no dependency log; no source compiler changes outside this task.

## Experiment 10: named first-string pointer
- Result: 98.47%. Creates a second base web and extra addi; rejected.
- Experiment 9 result: link fails on four pre-existing globally defined compiler-pair aliases colliding with ac_air_gauge. They are local in original assembly; make them static. Custom .rodata$uh section is unsupported by the linker command file (it maps to .ctors), so it cannot be retained in a final solution without a supported layout.

## Experiment 11: signed-width roundtrip on font pointer
- Tested documented pointer roundtrip to retain address-copy shape. Result: unchanged 98.95%; rejected. Also corrected four compiler-pair aliases to static to remove verified duplicate-symbol link errors.

## Experiment 12: base/count declaration swap and GXColor local
- Result: identical 98.95%, including reversed colour stack copy. Neither change affects these remaining webs.

## Experiment 13: early buffer-base assignment
- Result: 98.03%; moves buffer-base addi to function entry and still uses r29. Rejected.

## Experiment 14: full font-address and retrace-value roundtrips
- Result: 98.19%; retrace roundtrip adds a dead srawi; font address remains folded. Rejected.

## Experiment 15: alternate per-handler section naming
- `.rodata@uh` also maps to .ctors. After fixing local alias scope the ELF links, but this layout cannot match retail. Rejected custom-section approach.

## Experiment 16: register order with original external strings/signed pool
- Result: still 96.75%; cannot retain the custom-pool register improvements with the existing external-string shape.

## Experiment 17: direct buffer indexing
- Remove explicit base variable and use print_buffer[i]. Result: 97.91%; loses the ninth saved register and original frame. Rejected.
- Register-dump tool unavailable: required installed mwcc-debugger script is missing. No installation attempted.

## Resumed baseline and experiment 18: ordinary-section pool boundary
- Current interrupted state scores 98.21%; all four other functions remain 100%. It is the literal-string/native unsigned shape, not experiment 17.
- Move handler/setup group after effects; flush deferred effects while pool_data is off, then emit handler immediately with pool_data on. Test whether non-pooled effect emission permits a fresh base in ordinary .rodata.
- Experiment 18 result: 91.18%; pool_data off affects handler emission despite the surrounding immediate-generation pragma. Rejected.

## Experiment 19: deferred handler after non-pooled effect flush
- Keep pool_data on through end-of-file emission, and defer handler generation after the effect flush.
- Experiment 19 result: 98.21%, unchanged +0x30 pool-base displacement; rejected.

## Experiment 20: enable pooling before immediate code generation
- Enable pool_data before defer_codegen off (rather than after it) when emitting the handler after the non-pooled effect flush.
- Experiment 20 result: 91.18%, immediate emission again loses data-base pooling; rejected.

## Experiment 21: reselect normal constant section
- Reselect const_type .rodata after the non-pooled effect flush while keeping handler deferred. Test whether a fresh section directive establishes a new pool anchor without introducing an unsupported section.
- Experiment 21 result: 98.21%; reselecting .rodata reuses its original pool base. Rejected.

## Experiment 22: named effect-prefix objects
- Name the original update table, zero Vec, and effect-name string to keep them out of the handler anonymous pool; leave native conversion doubles compiler-owned. An initial edit script failed before writing; its no-change rebuild was baseline validation only.
- Experiment 22 result: handler remains 98.21% with +0x38 displacement; named constants are also included in the base pool and effUltraHammerEntry falls to 76%. Rejected.

## Experiment 23: inline clear-colour copy on link-compatible baseline
- Restore the original external-string/signed-conversion shape and keep the verified static aliases. Test the documented inline GXColor copy recipe for the reversed source/destination stack slots.
- Experiment 23 result: 96.11%; helper adds a colour-copy slot and shifts subsequent stack locals by eight bytes. Rejected.

## Experiment 24: descending lifetime/register declaration order
- Return to native unsigned/literal shape in regular .rodata. Declare base/count/i/font/dimensions/loop pointers in the order suggested by target r31..r23 assignments.
- Experiment 24 result: 98.92%; named font/loop/dimension/argument registers now match, leaving a base/pool/retrace three-register rotation, clear-colour stack-copy reversal, texture-address folding, and +0x30 pool offsets. All other functions remain 100%. Saved as regular-section best.

## Experiment 25: propagation off for address-copy shape
- Disable value propagation only for the handler, seeking its explicit mr r3,base followed by add r3,r3,index.
- Experiment 25 result: unchanged 98.92%; propagation pragma unnecessary and rejected. Inspection of objdiff measures shows expanded .rodata lowers matched_data_percent 54.59 -> 51.35, so this cannot be kept for CI.

## Experiment 26: external-data form with reordered locals and common subexpressions on
- Retain the original 48-byte .rodata and external string base, place strings between count and i, and let address CSE combine the external string address.
- Experiment 26 result: 96.75%; CSE and named declaration placement do not improve score or remove extra base-address copy.

## Experiment 27: inline external-base materialization
- Return the existing string anchor through an inline helper to give that address web an inline-expansion rank; restore common-subexpression optimization off.
- Experiment 27 result: unchanged 96.75%; inline accessor is optimized to the same address web. Rejected.

## Experiment 28: external unsigned mask with correctly positioned union locals
- Use explicit unsigned conversion unions declared before the large buffers (to place their storage at the frame tail), with the original shared-base mask. This extends the base lifetime through rendering without adding pool data.
- Experiment 28 result: 97.53%; original argument/font/loop registers and frame size now match, and .rodata/.sdata2 remain identical. Union objects sort below the large buffers and move them by +0x10; arithmetic emits fsub+frsp. Saved as best link-compatible source.

## Experiment 29: conversion storage appended to formatted buffer
- Under MUST_MATCH group the 0x400-byte formatted buffer and two conversion unions into one stack object. This places the union storage at 0x14e0/0x14e8 while retaining the original matrix/buffer offsets. Natural form retains plain text buffer and native casts.
- Experiment 29 result: 95.04%; stack offsets match, but aggregate aliasing changes the conversion load/store schedule and score falls. Rejected.

## Experiment 30: load both dimensions before constructing conversions
- Return to experiment 28; capture both u16 dimensions as u32 locals before union stores, then construct low words before high words to mimic native conversion sequencing. This is the final source build experiment.
- Experiment 30 result: 97.49%; reordered stores do not improve the conversion shape. Rejected and restored experiment 28 (97.53%). No experiments beyond 30.

## Final retained state and blocker
- Retained experiment 28: handler 96.75 -> 97.53%; all four other functions remain 100%. Explicit unsigned word/double conversion reuses the existing str_803019E0+0x190 mask; reordered matching-only locals recover the original error/context/dsisr/dar, font, dimension, loop and index registers, and original saved-register/frame shape.
- Retained the previous run’s four static compiler-pair aliases, which fix duplicate global-symbol link collisions.
- Native unsigned/literal pooling reaches 98.92% in ordinary .rodata but adds 408 bytes and lowers matched_data_percent; its handler base includes the preceding 48-byte effect pool. Flushes and reselecting .rodata do not create a fresh base; custom sections are not mapped to retail .rodata.
- Precise remaining blocker in the retained form: explicit conversions emit fsub+frsp rather than fsubs, conversion unions occupy 0x20/0x28 rather than 0x14e0/0x14e8 and shift matrices/buffers by 0x10; extra external-base materialization, clear-colour source/destination reversal, texture-address folding, and print-buffer/retrace r30/r31 swap also remain.
- configure.py remains NonMatching because the handler has not reached 100%. Existing MSR asm block is unchanged; no tools/ edits, pushes, or commits.

## Final verification
- Final score after restoring experiment 28 and documenting matching-only shapes: N_systemErrorHandler 97.53% (1548-byte retail function); N_setupErrorHandler, effUltraHammerEntry, effUltraHammerMain, effUltraHammerDisp remain 100%.
- `ninja -j2` succeeded; `build/tools/dtk shasum -c config/G8MJ01/build.sha1 -q` prints `32 files OK`. eff_ultra_hammer remains NonMatching, so these checksums validate the existing linked project, not a newly linked handler.
- `.rodata` 48/48 bytes, `.sdata2` 32/32 bytes, `.sdata` 728/728 bytes: IDENTICAL. Four changed alias data objects and the broader origin/main data diff pass data checks. Existing MSR asm compared exactly against HEAD: unchanged.
- `python3 tools/reloc_value_check.py eff_ultra_hammer --all`: `0 finding(s) in 0 unit(s)`; this checker only evaluates instruction-identical functions and does not cover the unmatched handler. Independently verified the explicit word/double conversion equals native conversion for every possible u16 input (65,536 values).
- Required `PATH="$HOME/ttyd/.venv/bin:$PATH" CI_JOBS=2 python3 tools/ci.py --base origin/main --allow-pragma` stops at tests: 219 tests run, one error in test_mwccdbg.LockTest.test_second_holder_waits_for_the_first because sandbox rejects bind(("127.0.0.1", 0)) with PermissionError (Errno 1). No test or tool modified to bypass it; CI cannot print PASS here.
- Separately, `ci.lint('origin/main', True, False, False)` rejects a pre-existing protected tools/asm_exceptions.json diff already in HEAD relative to origin/main. Current working-tree diff from HEAD contains no tools/ changes. Lint for this run against HEAD passes.
- Separately ran unchanged CI gates: prototypes, hang guard, natural compilation (three changed C files vs origin/main), data checks (12 data objects), and global ratchet all pass; ratchet reports no regressions and three improved measures. Full build used explicit installed-tool paths because default configure tries to redownload the existing dtk binary when its Ninja download record is absent.
- No new header issues or reference-code imports. New match-only raw pool offset is the task's original unsigned mask at str_803019E0+0x190; the natural build uses ordinary font-dimension conversions. Total experiment count is 30/30.
