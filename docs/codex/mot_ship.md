# mot_ship matching attempts (2026-10-04)

At most 40 build experiments. No tools/ edits, external code, commits or pushes.
Explicit paths to existing tools avoid download dependencies. Requested PATH prefix is included; that venv is absent.

## 01-baseline: Rebuild unchanged source and establish scores and pool order.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.75953%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 91.81035%, .text 99.85118%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes differs

-- 44 instruction diff(s) to fix, 60 that differ only in the data symbol (!! is real, ~~ is not: it disappears when the unit links; do not chase it)

Checksum: not tested (object experiment).

## 02-pool-prefix: Deferred literal helpers after checkHarbor emission insert 15/50/30.303/180 before shipMove; retain runtime literal loads.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.75953%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 98.70689%, .text 99.85118%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes differs

-- 44 instruction diff(s) to fix, 8 that differ only in the data symbol (!! is real, ~~ is not: it disappears when the unit links; do not chase it)

Checksum: not tested (object experiment).

## 03-round-expression: Nest distance expression in the round calculation so 10 pools before 2 without changing instructions.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.75953%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.85118%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 44 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 04-data-dir-order: Swap mot_ship data and direction declaration order to raise the direction web priority.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.75953%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.85118%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 44 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 05-case0-dir: Give case 0 a separate direction local instead of sharing it with case 10.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.75953%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.85118%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 44 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 06-propagation: Disable propagation for mot_ship only to test literal factor hoisting and pointer coloring.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 98.87566%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 57.60000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.30418%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 75 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 07-case0-inline: Extract case 0 harbor setup into an inline helper with the hit pointer as a parameter.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.72082%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.82722%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 62 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 08-init-local: Give initial heap allocation its own pointer rather than the switch-wide data variable.

Build failed: BUILD FAILED: THE COMPILER HUNG and was killed (a normal compile takes about a second). Your last edit makes mwcceppc loop forever: undo it (lastgood.c is the last version that built) and write that part differently.
timeout -k 10 300 build/tools/wibo build/tools/sjiswrap.exe build/compilers/GC/2.6/mwcceppc.exe -nodefaults -proc gekko -align powerpc -enum int -multibyte -char unsigned -fp hardware -Cpp_exceptions off -pragma "cats off" -O4,p -inline auto -pragma "warn_notinlined off" -pragma "warn_padding off" -once -maxerrors 1 -nosyspath -i include -i libs/dolsdk2004/include -i libs/dolsdk2004/include/libc -i libs/musyx/include -i build/G8MJ01/include -DBUILD_VERSION=0 -DVERSION_G8MJ01 -DSDK_REVISION=1 -DMUST_MATCH -requireprotos -sym on -DNDEBUG=1 -once -fp_contract on -inline auto,deferred -use_lmw_stmw on -rostr -sdata 48 -sdata2 6 -DMUSY_VERSION_MAJOR=2 -DMUSY_VERSION_MINOR=0 -DMUSY_VERSION_PATCH=3 -lang=c -MMD -c src/motion/mot_ship.c -o build/G8MJ01/src/motion || { r=$?; if [ $r -eq 124 ] || [ $r -eq 137 ]; then echo "COMPILER TIMEOUT: src/motion/mot_ship.c"; fi; exit $r; } && "/Library/Frameworks/Python.framework/Versions/3.12/bin/python3" tools/transform_dep.py build/G8MJ01/src/motion/mot_ship.d build/G8MJ01/src/motion/mot_ship.d


## 09-case0-m-before-hit: Declare the case 0 Mario pointer before the hit pointer while preserving load and call order.

Build failed: BUILD FAILED: THE COMPILER HUNG and was killed (a normal compile takes about a second). Your last edit makes mwcceppc loop forever: undo it (lastgood.c is the last version that built) and write that part differently.
timeout -k 10 300 build/tools/wibo build/tools/sjiswrap.exe build/compilers/GC/2.6/mwcceppc.exe -nodefaults -proc gekko -align powerpc -enum int -multibyte -char unsigned -fp hardware -Cpp_exceptions off -pragma "cats off" -O4,p -inline auto -pragma "warn_notinlined off" -pragma "warn_padding off" -once -maxerrors 1 -nosyspath -i include -i libs/dolsdk2004/include -i libs/dolsdk2004/include/libc -i libs/musyx/include -i build/G8MJ01/include -DBUILD_VERSION=0 -DVERSION_G8MJ01 -DSDK_REVISION=1 -DMUST_MATCH -requireprotos -sym on -DNDEBUG=1 -once -fp_contract on -inline auto,deferred -use_lmw_stmw on -rostr -sdata 48 -sdata2 6 -DMUSY_VERSION_MAJOR=2 -DMUSY_VERSION_MINOR=0 -DMUSY_VERSION_PATCH=3 -lang=c -MMD -c src/motion/mot_ship.c -o build/G8MJ01/src/motion || { r=$?; if [ $r -eq 124 ] || [ $r -eq 137 ]; then echo "COMPILER TIMEOUT: src/motion/mot_ship.c"; fi; exit $r; } && "/Library/Frameworks/Python.framework/Versions/3.12/bin/python3" tools/transform_dep.py build/G8MJ01/src/motion/mot_ship.d build/G8MJ01/src/motion/mot_ship.d


## 10-case0-m-before-hit: Test actual case 0 declaration swap from verified pool version; 09 repeated 08 due missing snapshot.

Build failed: BUILD FAILED: THE COMPILER HUNG and was killed (a normal compile takes about a second). Your last edit makes mwcceppc loop forever: undo it (lastgood.c is the last version that built) and write that part differently.
timeout -k 10 300 build/tools/wibo build/tools/sjiswrap.exe build/compilers/GC/2.6/mwcceppc.exe -nodefaults -proc gekko -align powerpc -enum int -multibyte -char unsigned -fp hardware -Cpp_exceptions off -pragma "cats off" -O4,p -inline auto -pragma "warn_notinlined off" -pragma "warn_padding off" -once -maxerrors 1 -nosyspath -i include -i libs/dolsdk2004/include -i libs/dolsdk2004/include/libc -i libs/musyx/include -i build/G8MJ01/include -DBUILD_VERSION=0 -DVERSION_G8MJ01 -DSDK_REVISION=1 -DMUST_MATCH -requireprotos -sym on -DNDEBUG=1 -once -fp_contract on -inline auto,deferred -use_lmw_stmw on -rostr -sdata 48 -sdata2 6 -DMUSY_VERSION_MAJOR=2 -DMUSY_VERSION_MINOR=0 -DMUSY_VERSION_PATCH=3 -lang=c -MMD -c src/motion/mot_ship.c -o build/G8MJ01/src/motion || { r=$?; if [ $r -eq 124 ] || [ $r -eq 137 ]; then echo "COMPILER TIMEOUT: src/motion/mot_ship.c"; fi; exit $r; } && "/Library/Frameworks/Python.framework/Versions/3.12/bin/python3" tools/transform_dep.py build/G8MJ01/src/motion/mot_ship.d build/G8MJ01/src/motion/mot_ship.d


## 11-init-top-local: Use a distinct initialData local declared at function entry, preserving C89 syntax.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.62170%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.76588%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 91 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 12-case31-factor-local: Give the 1.6 effect parameter its own block local ahead of the Mario and zero locals.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.75953%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.85118%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 44 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 13-case0-declarations: Correctly swap case 0 pointer declarations with all assignments below C89 declarations.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.75953%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.85118%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 44 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 14-effect-factor-param: Inline case 31 effect update with 1.6 as a float parameter to preserve early argument evaluation.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.73607%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.83666%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 52 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 15-effect-helper-propagation: Disable propagation only in the inline case 31 helper, retaining literal loads in mot_ship.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.73607%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.83666%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 52 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 16-propagation-literal-zero: Propagation off with effect zeros as direct literals and variable-first truth comparisons avoids saved zero FPR.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.51906%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 57.60000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.70236%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 48 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 17-zero-inner-scope: Propagation off: confine effect zero locals to the y store and store case 31 yaw directly.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.85044%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.90744%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 46 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 18-effect-local-order: Reverse zero addition for the target FP operands; give the two case 52 effect blocks separate data pointers in opposite declaration orders.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.86803%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.91833%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 43 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 19-effect-y-before-zero: Read effect y into a local before initializing the scoped zero; retain original switch data pointer.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.85044%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.90744%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 46 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 20-shared-m: Reuse a function-scoped Mario scratch pointer throughout mot_ship to change register-web ranking.

Build failed: BUILD FAILED: THE COMPILER HUNG and was killed (a normal compile takes about a second). Your last edit makes mwcceppc loop forever: undo it (lastgood.c is the last version that built) and write that part differently.
timeout -k 10 300 build/tools/wibo build/tools/sjiswrap.exe build/compilers/GC/2.6/mwcceppc.exe -nodefaults -proc gekko -align powerpc -enum int -multibyte -char unsigned -fp hardware -Cpp_exceptions off -pragma "cats off" -O4,p -inline auto -pragma "warn_notinlined off" -pragma "warn_padding off" -once -maxerrors 1 -nosyspath -i include -i libs/dolsdk2004/include -i libs/dolsdk2004/include/libc -i libs/musyx/include -i build/G8MJ01/include -DBUILD_VERSION=0 -DVERSION_G8MJ01 -DSDK_REVISION=1 -DMUST_MATCH -requireprotos -sym on -DNDEBUG=1 -once -fp_contract on -inline auto,deferred -use_lmw_stmw on -rostr -sdata 48 -sdata2 6 -DMUSY_VERSION_MAJOR=2 -DMUSY_VERSION_MINOR=0 -DMUSY_VERSION_PATCH=3 -lang=c -MMD -c src/motion/mot_ship.c -o build/G8MJ01/src/motion || { r=$?; if [ $r -eq 124 ] || [ $r -eq 137 ]; then echo "COMPILER TIMEOUT: src/motion/mot_ship.c"; fi; exit $r; } && "/Library/Frameworks/Python.framework/Versions/3.12/bin/python3" tools/transform_dep.py build/G8MJ01/src/motion/mot_ship.d build/G8MJ01/src/motion/mot_ship.d


## 21-zero-assignment-expression: Initialize zero inside each y addition to keep the zero load as the right operand without propagation folding.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.88563%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.92922%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 39 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 22-shared-m-c89: Reuse a function-scoped Mario pointer, placing assignments after each block declaration group.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.88563%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.92922%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 39 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 23-harbor-return-helper: Return harbor direction from an inline helper, passing caller vectors to preserve stack allocation.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.85044%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.90744%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 51 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 24-register-locals: Test register hints on switch data and direction without altering lifetime or control flow.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.88563%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.92922%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 39 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 25-helper-direction-first: Declare helper return direction before its Mario local to invert their allocation priority.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.85044%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.90744%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 51 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 26-allocation-expression: Assign allocated data within the motStruct store expression to alter its temporary web priority.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.87683%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.92378%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 42 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 27-lifetimes: Disable lifetime optimization only for mot_ship to test the remaining pointer coalescing.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.26980%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.54810%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 237 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 28-second-wake-inline: Extract only the second case 52 wake update with a Mario pointer parameter and a local data pointer.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.87096%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.92014%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 43 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 29-int-direction: Use MWCC int rather than s32/long for the direction web; its machine comparisons are unchanged.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.88563%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.92922%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 39 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 30-wake-assignment-load: Assign the retained ship-data pointer inside the unk10 load in both case 52 blocks.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.88563%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.92922%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 39 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 31-allocation-direct: Store the allocator result directly, removing the initial assignment to the shared data variable.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.74780%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.84392%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 86 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 32-harbor-call-assignment: Assign the Mario pointer inside hitGetAttr argument evaluation using a comma expression; keep the hit load before the calls.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.70440%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 57.60000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.81706%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 50 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 33-harbor-scalars: Group case 0 scratch values in an address-free struct to test scalar-field register ranking.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.85044%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.90744%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 51 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 34-cleanup: Restore best 21, guard match-only forms, narrow propagation pragma to mot_ship, remove dead float initialization and update blocker comment.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.88563%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.92922%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 39 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 35-final-object: Format guarded zero expressions and verify final source still keeps the best scores and exact pool.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.88563%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.92922%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 39 instruction diff(s) to fix

Checksum: not tested (object experiment).

## 36-project-validation: Build and run all CI checks after the tool-test gate.

The full CI entry point stopped before building: 200 of 201 tools tests passed; `test_second_holder_waits_for_the_first` in `tools/tests/test_mwccdbg.py:49` could not bind a localhost socket (`PermissionError: Operation not permitted`). No tools files or tests were changed or skipped in that run.

The remaining original CI functions were then called separately, with existing local tool paths passed to configure: lint, local prototypes, extend_data IDs, hang guard, build, data checks, the no-regression ratchet, and progress summary all passed. The ratchet reports zero regressions and six improved measures against the existing accepted baseline. The baseline was not replaced. Full `ninja` succeeds and an explicit `dtk shasum -c config/G8MJ01/build.sha1` verifies all 32 checksums.

Function/section scores are the same as experiment 35. The checksums still use the original mot_ship object because configure.py remains NonMatching. This does not prove that our mot_ship can link. Full CI is not labelled PASS because of the tools test sandbox error.

## 37-natural-validation: Compile the changed source without MUST_MATCH.

The original CI natural-compilation check passed for src/motion/mot_ship.c. Its output object was isolated in a temporary directory and did not replace the matching object. Runtime expressions remain under #else for the pool traversal, zero assignment/comparison, and yaw-store shapes; the propagation pragma and pool helpers are guarded.

## 38-final-comment: Clarify that FP register allocation/arithmetic match while remaining GPR differences also affect lfs addressing.

N_marioShipGetDir 100.00000%, marioShipChanging 100.00000%, marioSetSwirlPower 100.00000%, marioChkSwirlPower 100.00000%, marioShipForceStop 100.00000%, marioShipSetDispDir 100.00000%, marioChkShipPlace 100.00000%, marioForceShipAnime 100.00000%, marioReInit_ship 100.00000%, mot_ship_post 100.00000%, mot_ship 99.88563%, marioShipChgFall 100.00000%, marioChkShipMoveMode 100.00000%, shipMove 100.00000%, checkHarbor 100.00000%

Sections: .data 100.00000%, .rodata 100.00000%, .sdata 100.00000%, .sdata2 100.00000%, .text 99.92922%

Raw sections: .rodata 120/120 bytes identical, .data 500/504 bytes differs, .sdata 40/40 bytes identical, .sdata2 232/232 bytes identical

-- 39 instruction diff(s) to fix

Checksum: not tested (object experiment).

## Final state and what fixed what

38 build experiments counted conservatively, including failed compiler variants, project validation, and the separate natural compile. No commit, push, tools/ edit, header edit, or external code was used. configure.py is unchanged.

| Measure | Before | Final |
|---|---:|---:|
| mot_ship (6820 bytes) | 99.75953% | 99.88563% |
| Other 14 original functions | 100% | 100% |
| .text | 99.85118% | 99.92922% |
| .sdata2 | 91.81035% | 100%, 232/232 bytes identical |
| .rodata | 100% | 100%, 120/120 bytes identical |
| .sdata | 100% | 100%, 40/40 bytes identical |
| .data | 100% | 100%, unchanged 500/504 bytes |

- Four deferred, unused literal-return helpers emit 15, 50, 30.30303, and 180 after checkHarbor and before shipMove. This fixes the pool prefix while retaining literal loads in all original functions. It avoids the named-constant CSE and duplicate zero from both earlier attempts.
- Nesting the distance expression in the rounding calculation makes 10 pool before 2 while retaining the original instructions. Every .sdata2 byte now matches.
- MUST_MATCH opt_propagation off on mot_ship hoists the case 31 scale (1.6) before the Mario/effect lookups. Restricting each zero to the y expression, assigning it inside the right operand, and storing yaw directly preserves the original FPRs and operand order without a third saved FPR or a larger frame. Removing the dead initial f30 assignment is neutral.
- All rejected register variants were restored. Declaration permutations, register hints, int/long direction, shared/separate pointer locals, assignment expressions, inline boundaries, scalar structs, and opt_lifetimes did not improve on the retained final result.

Precise remaining blocker: 39 GPR instruction-operand differences in mot_ship:

| Original function-relative offsets | Target | Ours | Count |
|---|---|---|---:|
| +0x005c..+0x008c, initialization | allocated data / inactive ID r28 | r29 | 4 |
| +0x017c..+0x0268, case 0 | Mario pointer r28; hit/direction r29 | Mario pointer r29; hit/direction r27 | 19 |
| +0x161c..+0x16fc, first case 52 wake update | Mario r30, ship data r29 | Mario r29, ship data r30 | 8 |
| +0x195c..+0x1a3c, second case 52 wake update | Mario r29, ship data r30 | Mario r30, ship data r29 | 8 |

The instruction order, immediates, calls, FPR allocation/arithmetic, and 0x90-byte frame match. The remaining GPR differences also affect addressing operands in float loads/stores. No Matching flip was attempted because the required 100% function prerequisite does not hold. The four missing trailing .data bytes are the pre-existing zero gap after the jump table; no padding object was added, as the documented jump-table rule warns that it can shift the tables.

Final verification: sdata2_diff reports .sdata2/.rodata IDENTICAL; rodatalayout reports no .sdata2 mismatch; requested reloc_value_check.py mot_ship --all is clean. Explicit enumeration confirms that MarioSt/motion/mot_ship was checked, with all 15 shared original functions present and zero findings (the checker only compares functions with normalized matching code). link_diag confirms .rodata/.sdata/.sdata2 identical and the unchanged .data tail gap; its zero DOL differences concern the NonMatching build's original object. All 32 checksums pass; natural compilation and the no-regression ratchet pass; git diff --check is clean. The full CI tools-test gate has the sandbox socket-bind limitation above.

No new raw offsets or structs were retained. The source's pre-existing raw accesses and marioSetPaperAnimeLocalTime ABI cast were not changed. Compiler variants 08/09/10/20 were rejected immediately by the wrapper, which reported a timeout; those variants put statements before C89 declarations. Experiment 09 repeated 08 because the failed build produced no snapshot. Their errors remain logged; no such form remains in the final source.
