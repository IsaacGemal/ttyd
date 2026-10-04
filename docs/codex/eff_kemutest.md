# eff_kemutest matching attempts — 2026-10-04

Final outcome: partial progress; configure.py remains NonMatching. No function or scored data section regressed. No tools/ files were changed, and nothing was pushed.

- Added three explicit NUL bytes to the pooled effect-name string: .rodata score 99.12024% -> 100%.
- Initialized quad dimensions immediately before GXBegin under MUST_MATCH: .sdata2 98.43750% -> 100%, while effKemuTestDisp remains 100%. MWCC pools the 2.0 shadow offset first and still emits the dimension loads/multiply in the inner-loop preheader.
- Defined the unit's own zero-initialized colour template and retained its padding: .sbss2 now matches at 100% (eight bytes).
- All regressing or ineffective experiments were reverted. Matching-only changes retain their natural forms under #else.

| Function | Bytes | Before | After |
|---|---:|---:|---:|
| effKemuTestEntry | 3580 | 99.98883% | 99.98883% |
| effKemuTestMain | 2080 | 100% | 100% |
| effKemuTestDisp | 1840 | 100% | 100% |
| effKemuTestDrawCam | 12 | 100% | 100% |
| effKemuTestSetRxRz | 16 | 100% | 100% |

Final section scores: .text 99.99469%; .rodata, .data, .sdata, .sdata2, and .sbss2 100%. sdata2_diff reports both data pools IDENTICAL. These scores do not establish that the unit can link: link_diag still reports four trailing zero bytes absent from .rodata and 28 from .data. Those padding discrepancies existed at baseline; the linker may supply alignment padding, but that has not been proven with a Matching flip.

Remaining code difference: the case-4/13 bom_dir/bom_spd base-register assignment is reversed (r21/r22), affecting their two loads. Putting angle first corrects the bases but changes five caller-saved FP-register instructions. Tried scopes, expression splits, inline helpers, pointer induction, index forms, redundant uses, and optimization controls without retaining an improvement.

Verification: ninja completed successfully and checked all 32 checksum files; the final ninja run and git diff --check also passed. The build still uses the original object for this NonMatching unit. No header problems or new raw offsets.

## Attempts

- Baseline: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry angle before speed: Entry 99.93296%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry explicit pointers dir first: Entry 99.08044%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Entry explicit pointers spd first: Entry 99.08044%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Entry array locals dir first 14: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry array locals spd first 14: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry array locals dir first 17: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry array locals spd first 17: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry array locals dir first 18: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry array locals spd first 18: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry angle first speed declaration first: Entry 99.89945%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry nested angle first scope: Entry 99.89945%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry comma angle then speed: Entry 99.89945%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry pointers at 13 dir: build failed (MWCC rejected declarations inserted after statements); no scores available. Reverted.
- Entry pointers at 13 spd: build failed (MWCC rejected declarations inserted after statements); no scores available. Reverted.
- Entry pointers at 20 dir: build failed (MWCC rejected declarations inserted after statements); no scores available. Reverted.
- Entry pointers at 20 spd: build failed (MWCC rejected declarations inserted after statements); no scores available. Reverted.
- Entry pointers at 19 dir: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry pointers at 19 spd: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry angle first commuted 46: Entry 99.93296%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry angle first commuted 46: Entry 99.93296%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Disp lexical on: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Disp inner constants lexical on: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Disp lexical reset: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Disp inner constants lexical reset: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Disp inner constants invariants enabled: Entry 99.98883%, Disp 98.10435%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 96.87500%, .rodata 99.12024%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Disp two local before half: Entry 99.98883%, Disp 99.71739%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry late pointers 20 dir: Entry 99.08044%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Entry late pointers 20 spd: Entry 99.08044%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Entry late pointers 19 dir: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry late pointers 19 spd: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry late pointers 13 dir: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry late pointers 13 spd: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry split angle 0: Entry 99.93296%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry split angle 1: Entry 99.93296%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry split angle 2: Entry 99.93296%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry split angle 3: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry split angle 4: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry split angle 5: Entry 99.94414%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry split angle 6: Entry 99.94414%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry split angle 7: Entry 99.93296%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry advancing pointers dir 0 19: Entry 98.22123%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Entry advancing pointers dir 0 13: Entry 98.22123%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Entry advancing pointers dir 1 19: Entry 99.21341%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry advancing pointers dir 1 13: Entry 99.39218%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry advancing pointers spd 0 19: Entry 98.22123%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Entry advancing pointers spd 0 13: Entry 98.22123%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Entry advancing pointers spd 1 19: Entry 99.21341%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry advancing pointers spd 1 13: Entry 99.39218%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry angle first dead spd = 0.0f;: Entry 99.93296%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry angle first dead angle = 0.0f;: Entry 99.93296%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry angle first dead spd = 0.0f; angle = 0.0f;: Entry 99.93296%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry angle first dead angle = 0.0f; spd = 0.0f;: Entry 99.93296%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry angle first speed expr 21: Entry 99.93296%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry angle first speed expr 33: Entry 99.72626%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Pool named prefix volatile_all: build failed (trial declarations appeared before the f32 type definition); no scores available. Reverted.
- Pool named prefix volatile_zero_one: build failed (trial declarations appeared before the f32 type definition); no scores available. Reverted.
- Pool named prefix plain_all: build failed (trial declarations appeared before the f32 type definition); no scores available. Reverted.
- Disp half spelling dsize / 2.0f before: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Pool named prefix volatile_all: Entry 93.61564%, Disp 95.20217%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 99.12024%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Pool named prefix volatile_zero_one: Entry 93.99218%, Disp 96.01087%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 99.12024%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Pool named prefix plain_all: Entry 99.74860%, Disp 94.90000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 99.12024%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Disp half spelling dsize / 2.0f before: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Disp half spelling dsize / 2.0f before: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Disp half spelling dsize / 2.0f inside: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Disp half spelling dsize * (1.0f / 2.0f) before: Entry 99.98883%, Disp 99.97826%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Disp half spelling dsize * (1.0f / 2.0f) inside: Entry 99.98883%, Disp 99.97826%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Disp half spelling dsize * half before: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Disp half spelling dsize * half inside: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 99.12024%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Rodata padded effect name: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Kept padded effect-name string under MUST_MATCH: Entry 99.98883%, other functions 100%; .rodata 100% (was 99.12024%), .sdata2 98.43750%, .data/.sdata 100%.
- Entry index spd (u32)i - 1: Entry 97.29385%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Entry index spd (u32)(i - 1): Entry 96.62570%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Entry index spd i + -1: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry index spd i - 1U: Entry 96.62570%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Entry index spd (s32)(i - 1): Entry 96.62570%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Entry index spd (unsigned short)i - 1: Entry 96.62570%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Entry index dir (u32)i - 1: Entry 97.22681%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Entry index dir (u32)(i - 1): Entry 96.78771%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Entry index dir i + -1: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry index dir i - 1U: Entry 96.78771%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Entry index dir (s32)(i - 1): Entry 96.78771%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Entry index dir (unsigned short)i - 1: Entry 96.78771%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Entry function scope angle: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry function scope spd: Entry 99.95531%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry shared local index: Entry 96.30727%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Entry pragma opt_lifetimes: Entry 98.04469%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry pragma opt_common_subs: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry pragma opt_propagation: Entry 94.14190%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 95.31250%, .rodata 100.00000%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Entry pragma opt_dead_assignments: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry pragma opt_loop_invariants: Entry 97.80335%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 95.31250%, .rodata 100.00000%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Owned sbss2 color: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry inline pair dir: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry inline void pair dir: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry inline pair spd: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry inline void pair spd: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Disp inline constants False: Entry 99.98883%, Disp 99.92391%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Disp inline constants True: Entry 99.98883%, Disp 99.92391%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Disp dimensions after branch scoped: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Disp dimensions after branch assignments: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Disp late half 25: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Disp late size 25: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Disp late both 25: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Disp late half 18: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Disp late size 18: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Disp late both 18: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 98.43750%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Kept late dimension initialization under MUST_MATCH: Entry 99.98883%, all other functions 100%; .sdata2/.rodata/.data/.sdata 100%. Pool now emits 2.0 before 0.5 and 16.0; loads and multiply still match the inner-loop preheader.
- Entry early redundant angle 14: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry early redundant angle 22: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry early redundant angle 46: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry early redundant spd 14: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry early redundant spd 22: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry early redundant spd 46: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry case pointer dir const f32*: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry case pointer dir register f32*: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry case pointer spd const f32*: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry case pointer spd register f32*: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry nested assignment 0: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry nested assignment 1: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry nested assignment 2: Entry 99.96648%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry nested assignment 3: Entry 96.57542%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 0.00000%, .sdata 100.00000%. Reverted.
- Entry nested assignment 4: Entry 99.93296%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry nested division 0: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry nested division 1: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry nested division 2: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry nested division 3: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry nested division 4: Entry 99.94414%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry nested division 5: Entry 99.94414%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry nested division 6: Entry 99.94414%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Entry nested division 7: Entry 99.72067%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Rodata full trailing padding: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 98.85057%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Owned sbss2 color and padding: Entry 99.98883%, Disp 100.00000%, Main 100.00000%, Draw/Set 100.00000/100.00000%; .sdata2 100.00000%, .rodata 100.00000%, .data 100.00000%, .sdata 100.00000%. Reverted.
- Kept owned zero-initialized GXColor and four-byte .sbss2 padding: section is now eight bytes, as in retail; Entry 99.98883%, all other functions 100%; .sdata2/.rodata/.data/.sdata 100%.

## Final source diff

```diff
diff --git a/src/effect/eff_kemutest.c b/src/effect/eff_kemutest.c
index 5a47ab5c..901f2ec3 100644
--- a/src/effect/eff_kemutest.c
+++ b/src/effect/eff_kemutest.c
@@ -11,7 +11,17 @@
 #include <dolphin/mtx.h>
 #include "math_ext.h"
 
-extern GXColor unk_804232d8;
+#ifdef MUST_MATCH
+/* The retail unit owns this zero-initialized colour template and its padding. */
+#pragma section ".sdata" ".sbss2"
+__declspec(section ".sdata") GXColor unk_804232d8;
+#pragma force_active on
+__declspec(section ".sdata") u32 gap_10_804232DC_sbss2;
+#pragma force_active reset
+#pragma section ".sdata" ".sbss"
+#else
+GXColor unk_804232d8;
+#endif
 #ifdef MUST_MATCH
 /* Match-only trick: named .sdata2 constants, defined at the end of the file, so the compiler can't
    fold `-32.0f * 0.5f` into -16 (the original does the multiply at runtime). The original's 0.5 and 16.0
@@ -125,7 +135,12 @@ EffectEntry* effKemuTestEntry(s32 type, f32 x, f32 y, f32 z, f32 scale) {
 		break;
 	}
 
+#ifdef MUST_MATCH
+	/* Retail keeps three additional NUL bytes in this pooled string. */
+	entry->type = "KemuTest\0\0\0";
+#else
 	entry->type = "KemuTest";
+#endif
 	entry->count = count;
 	data = __memAlloc(HEAP_EFFECT, entry->count * sizeof(effKemuTestData));
 	entry->userdata = data;
@@ -785,9 +800,14 @@ void effKemuTestDisp(CameraId camId, void* param) {
 			PSMTXConcat(mtx, scale, mtx);
 			GXLoadTexMtxImm(mtx, 0x21, 1);
 			{
+#ifdef MUST_MATCH
+				f32 dsize_half;
+				f32 size;
+#else
 				f32 half = 0.5f;
 				f32 dsize_half = dsize * half;
 				f32 size = 16.0f;
+#endif
 
 				for (j = flag; j < 2; j++) {
 					if (j == 0) {
@@ -824,6 +844,16 @@ void effKemuTestDisp(CameraId camId, void* param) {
 						PSMTXConcat(mtx, scale, mtx);
 						GXLoadPosMtxImm(mtx, 0);
 					}
+#ifdef MUST_MATCH
+					/* Pool the shadow offset before the dimensions; MWCC still moves
+					   these loads and the multiply to the inner-loop preheader. */
+					{
+						f32 half = 0.5f;
+
+						dsize_half = dsize * half;
+						size = 16.0f;
+					}
+#endif
 					GXBegin(0x80, 0, 4);
 					GXPosition3f32(dsize_half, size, 0.0f);
 					GXTexCoord2f32(0.0f, 0.0f);
```
