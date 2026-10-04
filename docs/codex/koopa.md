# Koopa link attempts (2026-10-04)

Baseline: 33/34 functions 100%; kpaFireAttack 99.99% (eight lfd offsets 0xf8 vs 0x10); .rodata 256 vs 248 bytes; .sdata2 -20 first instead of ninth in kpaFall. Source backup preserved for mandatory rollback.

- Pool-plan read-only probe: no valid automatic plan; 99.96% FireAttack with literal strings; named float plan duplicates inline constants and regresses Fall/JumpPara.
- Literal strings, helpers under defer_codegen off: MarioSt/koopa: 32/34 functions at 100%; 99.94%    384 B  kpaPreInit; 99.96%   3868 B  kpaFireAttack; .rodata 248; magic +0xf0
- Literal strings, helpers under inline auto: MarioSt/koopa: 33/34 functions at 100%; 99.96%   3868 B  kpaFireAttack; .rodata 247; magic +0xb8
- Literal strings, helpers under inline all: MarioSt/koopa: 33/34 functions at 100%; 99.96%   3868 B  kpaFireAttack; .rodata 247; magic +0xb8
- Literal strings, helpers under dont_inline off: MarioSt/koopa: 33/34 functions at 100%; 99.96%   3868 B  kpaFireAttack; .rodata 247; magic +0xb8
- local static const char: MarioSt/koopa: 33/34 functions at 100%; 99.96%   3868 B  kpaFireAttack; .rodata 247; magic +0xb8; first KPA +0xc
- local const char: MarioSt/koopa: 33/34 functions at 100%; 87.22%   3868 B  kpaFireAttack; .rodata 247; magic +0xb8; first KPA +0xc
- local static char: MarioSt/koopa: 32/34 functions at 100%; 99.94%    384 B  kpaPreInit; 92.49%   3868 B  kpaFireAttack; .rodata 79; magic +0x10; first KPA +-0x1
- local static const char* const: MarioSt/koopa: 33/34 functions at 100%; 89.08%   3868 B  kpaFireAttack; .rodata 247; magic +0xb8; first KPA +0xc
- local const char* const: MarioSt/koopa: 33/34 functions at 100%; 91.98%   3868 B  kpaFireAttack; .rodata 247; magic +0xb8; first KPA +0xc
- literal pool_strings off: MarioSt/koopa: 33/34 functions at 100%; 99.96%   3868 B  kpaFireAttack; .rodata 247; magic +0xb8; first KPA +0xc
- literal pool_strings on: MarioSt/koopa: 29/34 functions at 100%; 97.50%    160 B  kpaGetBodyStatus; 97.59%    332 B  kpa_jumpNpc; 80.61%    384 B  kpaPreInit; 97.87%    612 B  kpaFall; 90.91%   3868 B  kpaFireAttack; .rodata 209; magic +0x10; first KPA +0x18
- literal dont_reuse_strings on: MarioSt/koopa: 32/34 functions at 100%; 99.94%    384 B  kpaPreInit; 89.89%   3868 B  kpaFireAttack; .rodata 391; magic +0x130; first KPA +0x18
- literal readonly_strings off: MarioSt/koopa: 29/34 functions at 100%; 95.62%    160 B  kpaGetBodyStatus; 96.14%    332 B  kpa_jumpNpc; 91.07%    384 B  kpaPreInit; 98.95%    612 B  kpaFall; 92.49%   3868 B  kpaFireAttack; .rodata 24; magic +0x10; first KPA +-0x1
- literal defer_codegen off: MarioSt/koopa: 33/34 functions at 100%; 99.96%   3868 B  kpaFireAttack; .rodata 247; magic +0xb8; first KPA +0xc
- string pool then data pool: MarioSt/koopa: 29/34 functions at 100%; 97.50%    160 B  kpaGetBodyStatus; 97.59%    332 B  kpa_jumpNpc; 80.61%    384 B  kpaPreInit; 97.87%    612 B  kpaFall; 90.91%   3868 B  kpaFireAttack; .rodata 209 magic 0x10
- data pool then string pool: MarioSt/koopa: 29/34 functions at 100%; 97.50%    160 B  kpaGetBodyStatus; 97.59%    332 B  kpa_jumpNpc; 80.61%    384 B  kpaPreInit; 97.87%    612 B  kpaFall; 90.91%   3868 B  kpaFireAttack; .rodata 209 magic 0x10
- readonly then data: MarioSt/koopa: 33/34 functions at 100%; 99.96%   3868 B  kpaFireAttack; .rodata 247 magic 0xb8
- reuse then data: MarioSt/koopa: 32/34 functions at 100%; 99.94%    384 B  kpaPreInit; 89.89%   3868 B  kpaFireAttack; .rodata 391 magic 0x130
- global inline nodefer: MarioSt/koopa: 32/34 functions at 100%; 99.94%    384 B  kpaPreInit; 99.96%   3868 B  kpaFireAttack; magic 0xf0; helpers []
- global nodefer: MarioSt/koopa: 32/34 functions at 100%; 99.94%    384 B  kpaPreInit; 99.97%   3868 B  kpaFireAttack; magic 0x10; helpers [('kpaFireBreath', 460, 612), ('kpaFireBreath2', 1072, 612)]
- static inline nodefer: MarioSt/koopa: 32/34 functions at 100%; 99.94%    384 B  kpaPreInit; 99.96%   3868 B  kpaFireAttack; magic 0xf0; helpers []
- global inline deferred: MarioSt/koopa: 33/34 functions at 100%; 99.96%   3868 B  kpaFireAttack; magic 0xb8; helpers []
- Helper storage static, defer_codegen off/on around helpers only: MarioSt/koopa: 34/34 functions at 100%; magic 0x10; helpers [('kpaFireBreath', 0, 612), ('kpaFireBreath2', 1072, 612)]
- Helper storage global, defer_codegen off/on around helpers only: MarioSt/koopa: 34/34 functions at 100%; magic 0x10; helpers [('kpaFireBreath', 0, 612), ('kpaFireBreath2', 1072, 612)]
- Helper storage extern inline, defer_codegen off/on around helpers only: MarioSt/koopa: 33/34 functions at 100%; 99.96%   3868 B  kpaFireAttack; magic 0xb8; helpers []
- Move ordinary static helpers before all target functions, defer_codegen off/on: 34/34 100%, magic +0x10; Matching build strips both helper bodies and preserves all target addresses; checksum fails only on .sdata2 (95 bytes in DOL: 42 text address bytes, 53 data bytes). This is a working pool-ownership mechanism; remaining order is helper 0.4/0.3/1.4/1.5 before FireMain deg2rad/7, plus Fall -20 before eight parameters.
- Literal strings + pool_data on, no named double: 33/34 100%; FireAttack 99.96%, 34 offset-only differences; magic +0xb8.
- Ordinary helpers at file top + mixed named tail of 14 floats, no explicit sections: 33/34 100%; helper constants incorrectly .sdata, and a positive-literal substitution also changed -0.4/-0.3 to unary named loads (JumpPara 94.47%).
- Explicit .sdata2 declarations/definitions on those 14 floats: .sdata fixed; JumpPara still 94.47% due to that substitution error.
- Restore literal -0.4/-0.3 in JumpPara: 34/34 100%; .sdata2 identical; Matching build passes all 32 checksums.
- Remove unnecessary pool_data on: 34/34 100%; Matching build passes all 32 checksums. Default pool_data is sufficient.
- Guard ordinary first-helper storage and defer_codegen off/on with MUST_MATCH, keep natural static inline #else, remove stale NONMATCHING comments: 34/34 100%; Matching build passes all 32 checksums.
- Restore second helper to static inline (only the first needs immediate ordinary generation): 34/34 100%; Matching build passes all 32 checksums; dol: 0 bytes differ.
- Compile natural #else without MUST_MATCH using the unit's remaining build flags: succeeds. Normalize named float/string uses back to their literal values: all 34 public function bodies are unchanged from baseline.

## Result

SUCCESS: all 34 functions exactly 100%; all 32 build.sha1 checksums pass with koopa compiled and linked as Matching; linked DOL has zero differing bytes. No commits. Only src/koopa.c and this log changed; configure.py was never edited. The Matching validation executed configure.py with its koopa Object replaced in memory; normal configuration was restored afterwards.

The first ordinary breath helper, generated with defer_codegen off before any deferred function, owns the anonymous i2f double at .rodata+0x10. Its actual call sites auto-inline, and the linker strips the unused emitted body. The second helper can stay static inline. Strings remain real literals; there is no manually defined conversion double. The first eight .sdata2 literals emit with the helper; fourteen explicit named .sdata2 floats, defined after the code, precede the remaining deferred pool. This also fixes kpaFall's pre-existing -20 ordering. The two code-generation pragmas and ordinary helper storage are under MUST_MATCH; the natural branch retains static inline.

Final function scores:

```text
MarioSt/koopa: 34/34 functions at 100%
  100.00%      8 B  getKoopaFireEfp
  100.00%     16 B  kpaGetWidth
  100.00%     16 B  kpaGetBaseScale
  100.00%     36 B  kpaNoHitEnemyCheck
  100.00%     40 B  kpaGetStageType
  100.00%     44 B  kpaMutekiCheck
  100.00%     48 B  kpaGetStageViewType
  100.00%     48 B  kpaJumpChk
  100.00%     52 B  kpaGetAttackMode
  100.00%     56 B  kpaMain
  100.00%     56 B  kpaAttackStart2
  100.00%     60 B  kpaPauseSE2
  100.00%     68 B  kpaMSetMutekiTimer
  100.00%     68 B  kpaGetLevel
  100.00%     72 B  kpaMutekiOn
  100.00%     72 B  kpaMutekiOff
  100.00%     80 B  kpaAttackStart
  100.00%     84 B  kpaHipAttackCheck
  100.00%     92 B  kpaChgPose
  100.00%     96 B  kpaCancelAttackMode
  100.00%    104 B  kpaSetLevel
  100.00%    104 B  kpaFireAttackCheck
  100.00%    104 B  unk_801f8cd0
  100.00%    112 B  kpaCamFollow
  100.00%    112 B  kpaSetFallPara
  100.00%    160 B  kpaGetBodyStatus
  100.00%    272 B  kpaSetJumpPara
  100.00%    332 B  kpa_jumpNpc
  100.00%    384 B  kpaPreInit
  100.00%    604 B  kpa3DMain
  100.00%    612 B  kpaFall
  100.00%    748 B  kpa2DMain
  100.00%    996 B  kpaFireMain
  100.00%   3868 B  kpaFireAttack
```
