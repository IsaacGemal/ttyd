# fontmgrTexSetup attempts

Model: GPT-6 (Codex). Date: 2026-10-04.

The baseline `ninja build/G8MJ01/report.json` score was 99.596275% (644 bytes).
The local objdiff binary's direct `diff` command also counts three relocation-name differences;
its baseline was 99.503105%. The table records that direct score, while the final report is the verdict.
Scratch sources, objects, diffs, and build logs are in ignored `build/fontmgr-task/`.

| Attempt | Idea | Direct diff score | Result |
|---|---|---:|---|
| baseline | Recompile the unchanged source. | 99.503105% | dvd/rounded in r30/r31; header/i in r6/r8. |
| rounded_before | Declare rounded after dvd; assign it before the allocation call and reuse it for the read. | 98.322980% | Fixes dvd/rounded to r31/r30, but loses the required size copy and changes scheduling. Rejected. |
| rounded_argument | Assign rounded inside the allocation call's second argument, then reuse it for the read. | 99.689440% | Fixes dvd/rounded to r31/r30 and retains the size copy in r4; only the scan registers remain. |
| rounded_read_argument | Assign rounded in the read call, leaving the allocation expression unchanged. | 99.503105% | No improvement. |
| rounded_inline | Route rounding through an inline helper with a u32 parameter. | 98.136640% | Changes the copy/schedule and does not fix dvd/rounded. Rejected. |
| scan_inline_i_first | Inline the block scan with a header parameter and locals declared i, ptr. | 99.378880% | Header reaches r8, but the counter/pointer allocation differs. Rejected. |
| scan_inline_ptr_first | Inline the block scan with a header parameter and locals declared ptr, i. | 99.720500% | Fixes header/i to r8/r6 without changing scan instructions. |
| combined_round_arg_scan_inline | Combine rounded_argument with scan_inline_ptr_first. | 99.906830% | All instruction bytes match after relocation normalization; only relocation names differ. |
| guarded_final | Guard the inline and assignment shape with MUST_MATCH; retain the original natural form under else. | 99.906830% | Final report: **100.000000%**. No helper body emitted. |

## Final validation

- `ninja build/G8MJ01/report.json`: fontmgrTexSetup **99.596275% -> 100%**, size unchanged at 644 bytes.
- Compared the baseline and final reports across all 1,234 units: no function or section score regressed.
  Only fontmgrTexSetup and its .text section changed.
- fontmgr .text: **96.364746% -> 96.381570%**; matched functions: **28/37 -> 29/37**.
- Data sections unchanged: .rodata 79.032260%, .sdata 100%, .sbss 100%, .sdata2 39.130436%, .sbss2 0%.
- Compared original and final function bytes after relocation normalization: all 644 bytes identical.
- `python3 tools/reloc_value_check.py fontmgr`: zero findings; direct check_unit also returned no findings.
- Compiled baseline and final without MUST_MATCH: the natural fontmgrTexSetup bytes are identical after relocation normalization.
- Full `ninja -j 8`: passed, including **32 files OK** in the build.sha1 checksum check.
- `git diff --check`: passed. No tools/ edits, commits, or pushes.

## Build setup

The task's ~/ttyd/.venv/bin and the mwcc-debugger installation were absent on this machine.
Generated build.ninja using the existing binaries with:

```sh
python3 configure.py --wrapper build/tools/wibo --dtk build/tools/dtk \
  --objdiff build/tools/objdiff-cli --compilers build/compilers \
  --binutils build/binutils --sjiswrap build/tools/sjiswrap.exe
```

Providing explicit paths avoided the generated download rules, whose network requests failed.
The first report build rebuilt all source units; its fresh report was saved before applying the final change.

## Compiler behavior learned

An assignment inside the allocator's argument can name the common rounded-size value and change its
allocation priority while preserving the argument-evaluation schedule. A preceding assignment does not.
An inline block-scan helper with a header parameter and ptr declared before i changes the virtual-register
ordering enough to reproduce the target's header/counter allocation. Reversing those locals fails.
