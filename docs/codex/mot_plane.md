# mot_plane linking attempts (2026-10-04)

19 build experiments, within the 40-experiment budget. All work stayed in this checkout;
no tools/ edits, external code, commits, or pushes. Rejected instruction regressions were
experimental only; the final source keeps all eight functions at 100%. Checksums marked
"no (object experiment)" mean not tested for that variant, not a failed checksum.
The requested ~/ttyd/.venv/bin directory was absent; PATH included it and the existing
Python, Ninja, pyelftools, and build tools were used. Explicit tool paths avoided downloads.


## 01-baseline: Rebuild original source; nine duplicate pool values confirmed.

Functions: marioPlaneGetFlyingDir 100.00000%, marioGetPlaneStatus 100.00000%, mario_plane_cancel 100.00000%, marioForcePlaneAnime 100.00000%, mot_plane 100.00000%, flyMain 100.00000%, mot_plane_post 100.00000%, chkCollision 100.00000%

Sections: .data 100.00000%, .rodata 99.72752%, .sdata 100.00000%, .sdata2 99.36508%, .text 100.00000%

Bytes (ours/target): .rodata: 183/184 B, differs; .data: 512/512 B, identical; .sdata: 16/16 B, identical; .sdata2: 216/184 B, differs.

Checksum: no (object experiment; Matching not tested).

## 02-named-tail: Move every named constant after code and remove collision literals; reproduce correct pool before addressing register webs.

Functions: marioPlaneGetFlyingDir 100.00000%, marioGetPlaneStatus 100.00000%, mario_plane_cancel 100.00000%, marioForcePlaneAnime 100.00000%, mot_plane 99.37905%, flyMain 99.28819%, mot_plane_post 100.00000%, chkCollision 97.36364%

Sections: .data 100.00000%, .rodata 99.72752%, .sdata 100.00000%, .sdata2 99.36508%, .text 99.09253%

Bytes (ours/target): .rodata: 183/184 B, differs; .data: 512/512 B, identical; .sdata: 16/16 B, identical; .sdata2: 184/184 B, identical.

Checksum: no (object experiment; Matching not tested).

## 03-clamp-locals: Named-tail pool: read clamped field into a local before named limit; remove remaining zero literal.

Functions: marioPlaneGetFlyingDir 100.00000%, marioGetPlaneStatus 100.00000%, mario_plane_cancel 100.00000%, marioForcePlaneAnime 100.00000%, mot_plane 99.37905%, flyMain 99.50231%, mot_plane_post 100.00000%, chkCollision 97.36364%

Sections: .data 100.00000%, .rodata 99.72752%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.17289%

Bytes (ours/target): .rodata: 183/184 B, differs; .data: 512/512 B, identical; .sdata: 16/16 B, identical; .sdata2: 180/184 B, differs.

Checksum: no (object experiment; Matching not tested).

## 04-prefix-literals: Nine named prefix constants, literals for the remainder; local clamps retain literal register allocation.

Functions: marioPlaneGetFlyingDir 100.00000%, marioGetPlaneStatus 100.00000%, mario_plane_cancel 100.00000%, marioForcePlaneAnime 100.00000%, mot_plane 96.91685%, flyMain 99.50231%, mot_plane_post 100.00000%, chkCollision 97.36364%

Sections: .data 100.00000%, .rodata 99.72752%, .sdata 100.00000%, .sdata2 100.00000%, .text 98.18245%

Bytes (ours/target): .rodata: 183/184 B, differs; .data: 512/512 B, identical; .sdata: 16/16 B, identical; .sdata2: 180/184 B, differs.

Checksum: no (object experiment; Matching not tested).

## 05-collision-reloads: All named tail: use explicit width/ten reads and reload ten per later query, avoiding a saved FPR.

Functions: marioPlaneGetFlyingDir 100.00000%, marioGetPlaneStatus 100.00000%, mario_plane_cancel 100.00000%, marioForcePlaneAnime 100.00000%, mot_plane 99.37905%, flyMain 99.50231%, mot_plane_post 100.00000%, chkCollision 95.97947%

Sections: .data 100.00000%, .rodata 99.72752%, .sdata 100.00000%, .sdata2 100.00000%, .text 98.96786%

Bytes (ours/target): .rodata: 183/184 B, differs; .data: 512/512 B, identical; .sdata: 16/16 B, identical; .sdata2: 180/184 B, differs.

Checksum: no (object experiment; Matching not tested).

## 06-all-literal: Write all floats as literals using baseline locals, without pool-plan reload wrappers; test true anonymous pool.

Functions: marioPlaneGetFlyingDir 100.00000%, marioGetPlaneStatus 100.00000%, mario_plane_cancel 100.00000%, marioForcePlaneAnime 100.00000%, mot_plane 97.41901%, flyMain 99.86111%, mot_plane_post 100.00000%, chkCollision 100.00000%

Sections: .data 100.00000%, .rodata 99.72752%, .sdata 100.00000%, .sdata2 95.55556%, .text 98.90964%

Bytes (ours/target): .rodata: 183/184 B, differs; .data: 512/512 B, identical; .sdata: 16/16 B, identical; .sdata2: 180/184 B, differs.

Checksum: no (object experiment; Matching not tested).

## 07-mot-propagation: Keep literal-initialized locals early in mot_plane with opt_propagation off.

Functions: marioPlaneGetFlyingDir 100.00000%, marioGetPlaneStatus 100.00000%, mario_plane_cancel 100.00000%, marioForcePlaneAnime 100.00000%, mot_plane 99.20626%, flyMain 99.86111%, mot_plane_post 100.00000%, chkCollision 100.00000%

Sections: .data 100.00000%, .rodata 99.72752%, .sdata 100.00000%, .sdata2 95.55556%, .text 99.62859%

Bytes (ours/target): .rodata: 183/184 B, differs; .data: 512/512 B, identical; .sdata: 16/16 B, identical; .sdata2: 180/184 B, differs.

Checksum: no (object experiment; Matching not tested).

## 08-value-locals: Explicit clamped values under propagation-off; literal zero local restores flyMain clamp load order.

Functions: marioPlaneGetFlyingDir 100.00000%, marioGetPlaneStatus 100.00000%, mario_plane_cancel 100.00000%, marioForcePlaneAnime 100.00000%, mot_plane 99.56264%, flyMain 99.86111%, mot_plane_post 100.00000%, chkCollision 100.00000%

Sections: .data 100.00000%, .rodata 99.72752%, .sdata 100.00000%, .sdata2 95.55556%, .text 99.77193%

Bytes (ours/target): .rodata: 183/184 B, differs; .data: 512/512 B, identical; .sdata: 16/16 B, identical; .sdata2: 180/184 B, differs.

Checksum: no (object experiment; Matching not tested).

## 09-local-order: Reorder clamp locals, splash locals, and fall factors; simplify allocation lookup and use truth test in flyMain.

Functions: marioPlaneGetFlyingDir 100.00000%, marioGetPlaneStatus 100.00000%, mario_plane_cancel 100.00000%, marioForcePlaneAnime 100.00000%, mot_plane 99.97840%, flyMain 100.00000%, mot_plane_post 100.00000%, chkCollision 100.00000%

Sections: .data 100.00000%, .rodata 99.72752%, .sdata 100.00000%, .sdata2 95.55556%, .text 99.99131%

Bytes (ours/target): .rodata: 183/184 B, differs; .data: 512/512 B, identical; .sdata: 16/16 B, identical; .sdata2: 180/184 B, differs.

Checksum: no (object experiment; Matching not tested).

## 10-inactive-local: Explicit inactive sound-ID local declared ahead of the data pointer restores pointer/id register order.

Functions: marioPlaneGetFlyingDir 100.00000%, marioGetPlaneStatus 100.00000%, mario_plane_cancel 100.00000%, marioForcePlaneAnime 100.00000%, mot_plane 99.97840%, flyMain 100.00000%, mot_plane_post 100.00000%, chkCollision 100.00000%

Sections: .data 100.00000%, .rodata 99.72752%, .sdata 100.00000%, .sdata2 95.55556%, .text 99.99131%

Bytes (ours/target): .rodata: 183/184 B, differs; .data: 512/512 B, identical; .sdata: 16/16 B, identical; .sdata2: 180/184 B, differs.

Checksum: no (object experiment; Matching not tested).

## 11-inline-init: Write the sound-ID through the returned pointer expression, eliminating the retained data local.

Functions: marioPlaneGetFlyingDir 100.00000%, marioGetPlaneStatus 100.00000%, mario_plane_cancel 100.00000%, marioForcePlaneAnime 100.00000%, mot_plane 99.77862%, flyMain 100.00000%, mot_plane_post 100.00000%, chkCollision 100.00000%

Sections: .data 100.00000%, .rodata 99.72752%, .sdata 100.00000%, .sdata2 95.55556%, .text 99.91095%

Bytes (ours/target): .rodata: 183/184 B, differs; .data: 512/512 B, identical; .sdata: 16/16 B, identical; .sdata2: 180/184 B, differs.

Checksum: no (object experiment; Matching not tested).

## 12-init-helper: Inline the sound-ID store through a pointer parameter to alter its register web without instruction changes.

Functions: marioPlaneGetFlyingDir 100.00000%, marioGetPlaneStatus 100.00000%, mario_plane_cancel 100.00000%, marioForcePlaneAnime 100.00000%, mot_plane 99.97840%, flyMain 100.00000%, mot_plane_post 100.00000%, chkCollision 100.00000%

Sections: .data 100.00000%, .rodata 99.72752%, .sdata 100.00000%, .sdata2 95.55556%, .text 99.99131%

Bytes (ours/target): .rodata: 183/184 B, differs; .data: 512/512 B, identical; .sdata: 16/16 B, identical; .sdata2: 180/184 B, differs.

Checksum: no (object experiment; Matching not tested).

## 13-inactive-after: Reverse pointer/id declaration order relative to experiment 10.

Functions: marioPlaneGetFlyingDir 100.00000%, marioGetPlaneStatus 100.00000%, mario_plane_cancel 100.00000%, marioForcePlaneAnime 100.00000%, mot_plane 100.00000%, flyMain 100.00000%, mot_plane_post 100.00000%, chkCollision 100.00000%

Sections: .data 100.00000%, .rodata 99.72752%, .sdata 100.00000%, .sdata2 95.55556%, .text 100.00000%

Bytes (ours/target): .rodata: 183/184 B, differs; .data: 512/512 B, identical; .sdata: 16/16 B, identical; .sdata2: 180/184 B, differs.

Checksum: no (object experiment; Matching not tested).

## 14-pool-helpers: Unused deferred scalar helpers pool 4 first and 270 between collision and reset code; linker strips the bodies.

Functions: marioPlaneGetFlyingDir 100.00000%, marioGetPlaneStatus 100.00000%, mario_plane_cancel 100.00000%, marioForcePlaneAnime 100.00000%, mot_plane 100.00000%, flyMain 100.00000%, mot_plane_post 100.00000%, chkCollision 100.00000%

Sections: .data 100.00000%, .rodata 99.72752%, .sdata 100.00000%, .sdata2 100.00000%, .text 100.00000%

Bytes (ours/target): .rodata: 183/184 B, differs; .data: 512/512 B, identical; .sdata: 16/16 B, identical; .sdata2: 180/184 B, differs.

Checksum: no (object experiment; Matching not tested).

## 15-matching-link: flip configure.py to Matching and build the complete project

Functions: all 8 at 100%. Sections: .text/.data/.sdata/.sdata2 100%, .rodata 99.72752%. Checksum: YES, all 32 files OK. link_diag: dol: 0 bytes differ. Both pools IDENTICAL; missing object tail padding is supplied by linker alignment. Relocation-value check: zero findings.

## 16-guarded: Guard matching local declaration/assignment shapes and propagation pragma; remove redundant clamp local and stale comments.

Functions: marioPlaneGetFlyingDir 100.00000%, marioGetPlaneStatus 100.00000%, mario_plane_cancel 100.00000%, marioForcePlaneAnime 100.00000%, mot_plane 100.00000%, flyMain 100.00000%, mot_plane_post 100.00000%, chkCollision 100.00000%

Sections: .data 100.00000%, .rodata 99.72752%, .sdata 100.00000%, .sdata2 100.00000%, .text 100.00000%

Bytes (ours/target): .rodata: 183/184 B, differs; .data: 512/512 B, identical; .sdata: 16/16 B, identical; .sdata2: 180/184 B, differs.

Checksum: no (object experiment; Matching not tested).

## 17-cleanup: Group guarded declarations and guard the pre-existing volatile setup copy; natural code uses ordinary field stores.

Functions: marioPlaneGetFlyingDir 100.00000%, marioGetPlaneStatus 100.00000%, mario_plane_cancel 100.00000%, marioForcePlaneAnime 100.00000%, mot_plane 100.00000%, flyMain 100.00000%, mot_plane_post 100.00000%, chkCollision 100.00000%

Sections: .data 100.00000%, .rodata 99.72752%, .sdata 100.00000%, .sdata2 100.00000%, .text 100.00000%

Bytes (ours/target): .rodata: 183/184 B, differs; .data: 512/512 B, identical; .sdata: 16/16 B, identical; .sdata2: 180/184 B, differs.

Checksum: no (object experiment; Matching not tested).

## 18-baseline-validation: rebuild the clean starting source/configuration for the CI ratchet

Functions: all 8 at 100%. Sections: .text/.data/.sdata 100%, .rodata 99.72752%, .sdata2 99.36508%. Checksum: YES, baseline NonMatching build uses the original mot_plane object and passes all 32 checksums. `ci.py --init` established a fresh known-good baseline; the previous accepted report was backed up under build/motplane-task/.

## 19-final-validation: restore cleaned Matching source and run the complete CI gate

Functions: all 8 at 100%. Sections: .text/.data/.sdata/.sdata2 100%, .rodata 99.72752%. Checksum: YES, all 32 checksums pass. `tools/ci.py --allow-pragma`: PASS, including tools tests, lint, prototypes, natural compilation, data checks, and no-regression ratchet. Experiments 16 and 17 also retain the verified Matching checksum.

### What fixed what

- Replace all named float definitions and the short pose-name object with literals: nine duplicate float values disappear, reducing .sdata2 from 216 to 180 bytes before linker padding.
- MUST_MATCH `opt_propagation off` on mot_plane keeps the factor locals loaded before the direction branches. Explicit field/factor locals and declaration order restore the original clamp, fall, splash, and sound-ID registers. Natural forms remain under #else.
- The MUST_MATCH truth test for flyMain's nonzero speed preserves the original variable-first float comparison and load order.
- Deferred unused scalar helpers pool 4 before 90 and 270 before the reset constants. Both helper bodies are absent from MarioSt.elf.
- Linker alignment supplies the one-byte .rodata tail and four-byte .sdata2 tail. Adding explicit padding is unnecessary; the linked checksum is exact despite .rodata's object-level 99.72752% score.

Final state: configure.py says `Object(Matching, "motion/mot_plane.c")`; 8/8 functions at exactly 100%; `.sdata2` and `.rodata` pool comparisons IDENTICAL; requested `reloc_value_check.py mot_plane --all` clean, and explicit `check_unit` covers all eight original functions with zero findings. Whole-project function scores have no changes or regressions. `link_diag.py`: dol: 0 bytes differ. `cmp` confirms MarioSt.dol equals orig/G8MJ01/sys/main.dol. `git diff --check` clean. No remaining blocker.
