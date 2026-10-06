# evt_mobj natural rodata pool experiments

Limit: 35 build experiments. Changes remain uncommitted. No tools/ or configure.py edits.

Baseline: compiled the unchanged source using the generated MWCC command. 95 functions, 79 at 100%; .rodata 2576 bytes, .sdata2 487 bytes (target 288). First raw .rodata mismatch +0x3fc (signed conversion double is missing from +0x400 and pools at the end). Direct relocation check: 0 findings.

Build setup: configure.py run with existing local tools. The normal ninja regeneration requires absent orig/G8MJ01/files/rel/aaa.rel; experiments use ninja -t commands and the exact unit compiler command, then a one-unit objdiff report. pool_plan.py --no-probe was read before source edits; full probing would exceed the capped scope and alter unrelated sdata2 floats.

Starting partial scores:
- evt_mobj_floatswitch_blue: 99.775280
- evt_mobj_tornadoswitch_blue: 99.775280
- evt_mobj_floatswitch_red: 99.775280
- evt_mobj_rideswitch_lightblue: 99.793816
- mobj_switch_float_blk: 99.853660
- evt_mobj_switch_float_blk: 99.967740
- evt_mobj_signboard: 99.740260
- mobj_recovery_blk: 98.210526
- mobj_badgeblk: 99.347150
- evt_mobj_badgeblk: 99.963300
- kururing_capture: 87.358140
- mobj_kururing_floor: 99.938774
- evt_mobj_koopa_badgeblk: 99.797980
- evt_mobj_koopa_hidden_blk: 99.809525
- evt_mobj_koopa_fireber_dodai: 98.829790
- kpa_score_to_str: 94.914635

## Experiment 1: Pool_data on; replace tail base-offset string reads and named tail strings/f64 values with literals
Compile failed (see build/evt_mobj_compile_1.log).

## Experiment 2: Restrict tail literal conversion to this rodata address range (fix experiment 1 range error)

evt_mobj_switch_blue: 100.000000 -> 99.971695
mobj_lv_blk_evt: 100.000000 -> 97.842470
evt_mobj_float_blk: 100.000000 -> 99.923080
mobj_itembox: 100.000000 -> 97.423080
evt_mobj_itembox: 100.000000 -> 97.330360
mobj_badgeblk: 99.347150 -> 99.321240
evt_mobj_badgeblk: 99.963300 -> 99.981650
mobj_powerupblk: 100.000000 -> 99.950310
mobj_brickblk: 100.000000 -> 91.905760
evt_mobj_brick: 100.000000 -> 99.966830
kururing_capture: 87.358140 -> 85.613950
evt_mobj_kururing_floor: 100.000000 -> 97.049580
evt_mobj_koopa_dokan: 100.000000 -> 97.694916

.rodata: 2576 -> 2460 bytes; first byte difference 0x4d9; tail >=0x400 False
.sdata2: 288 -> 487 bytes; first byte difference 0x120; tail >=0x400 True

== .sdata2: CONTENT
   EXTRA (1):
     6d6500                   @3410                        evt_mobj_hitevt_onoff, evt_mobj_set_camid, evt_mobj_wait_animation_end, evt_mobj_set_anim
   DUP_OURS (48):
     150                      @2015                        mobj_koopa_fireber_dodai_evt
     9.58617e+11              @2141                        evt_mobj_koopa_hidden_blk, evt_mobj_koopa_badgeblk, mobj_koopa_badgeblk, evt_mobj_brick
     360                      @2675                        mobj_badgeblk
     0.6                      @2427                        mobj_kururing_floor
     75                       @2021                        mobj_koopa_fireber_dodai_evt
     0.99                     @2077                        fire_func
     -1                       @3340                        mobj_timerswitch, mobj_switch_blue   [neg]
     1                        @1941                        evt_mobj_koopa_fireber_dodai, fire_func, mobj_koopa_badgeblk, mobj_koopa_brickblk   [neg]
     0.3                      @3339                        mobj_timerswitch
     30                       @2067                        mobj_koopa_sango_evt, mobj_kururing_floor
     0.7                      @2189                        mobj_koopa_badgeblk, mobj_koopa_ojama_blk_evt
     0.9                      @3286                        mobj_rideswitch_lightblue
== .rodata: CONTENT
   MISSING (8):
     'MOBJ_trap'              str_802C42B8                 
     (200, 0, 200)            vec3_802c405c                
     'MOBJ_BombRock'          str_802C429C                 
     'OFF_d_bom'              str_802C42AC                 
     'MOBJ_RedTornadoSwitch'  str_802C4720                 
     'MOBJ_RideSwitchB'       str_802C46F8                 
     'MOBJ_Arrow'             str_802C444C                 
     'MOBJ_RideSwitchA'       str_802C470C                 
   DUP_TARGET (7):
     (0, 0, 0)                vec3_802c40a0                evt_mobj_koopa_ojama_blk
     (0, 0, 0)                vec3_802c40ac                evt_mobj_koopa_stone_blk
     (0, 0, 0)                vec3_802c40b8                
     (0, 0, 0)                vec3_802c40c4                evt_mobj_koopa_sango
     (0, 0, 0)                vec3_802c40d0                
     (0, 0, 0)                vec3_802c4168                
     (0, 0, 0)                vec3_802c4174                

Relocation value findings: 0.

## Experiment 3: Use actual prefix Vec objects; retain seven unused names as function-local static templates with dead address assignments
Compile failed (see build/evt_mobj_compile_3.log).

## Experiment 4: Preserve the lv-block template macro base missed by the dead-base cleanup
Compile failed (see build/evt_mobj_compile_4.log).

## Experiment 5: Declare each actual prefix Vec object before use

mobj_lv_blk_evt: 100.000000 -> 97.842470
evt_mobj_float_blk: 100.000000 -> 99.923080
mobj_itembox: 100.000000 -> 97.541664
evt_mobj_itembox: 100.000000 -> 99.946430
mobj_badgeblk: 99.347150 -> 99.305700
mobj_powerupblk: 100.000000 -> 99.950310
mobj_brickblk: 100.000000 -> 91.497380
evt_mobj_brick: 100.000000 -> 99.943130
kururing_capture: 87.358140 -> 90.032555
evt_mobj_kururing_floor: 100.000000 -> 99.942150
mobj_koopa_brickblk: 100.000000 -> 99.981480
evt_mobj_koopa_brick: 100.000000 -> 99.964910
evt_mobj_koopa_dokan: 100.000000 -> 99.949150
evt_mobj_koopa_fireber_dodai: 98.829790 -> 94.824470
evt_mobj_koopa_pole: 100.000000 -> 88.343950

.rodata: 2576 -> 2572 bytes; first byte difference 0x44; tail >=0x400 False
.sdata2: 288 -> 487 bytes; first byte difference 0x120; tail >=0x400 True

== .sdata2: CONTENT
   EXTRA (1):
     6d6500                   @3417                        evt_mobj_hitevt_onoff, evt_mobj_set_camid, evt_mobj_wait_animation_end, evt_mobj_set_anim
   DUP_OURS (48):
     200                      @2026                        mobj_koopa_fireber_dodai_evt
     9.58633e+11              @2537                        evt_mobj_brick
     42.42                    @2435                        mobj_kururing_floor
     0.3                      @3346                        mobj_timerswitch
     500                      @2197                        mobj_koopa_badgeblk, mobj_koopa_ojama_blk_evt
     -1000                    @2025                        mobj_koopa_fireber_dodai_evt
     9.58617e+11              @2148                        evt_mobj_koopa_hidden_blk, evt_mobj_koopa_badgeblk, mobj_koopa_badgeblk, evt_mobj_brick
     1.57057e+16              @2867                        evt_mobj_lock
     10                       @2073                        mobj_koopa_sango_evt, fire_func, kururing_capture
     0.04                     @2031                        mobj_koopa_fireber_dodai_evt
     2                        @2030                        mobj_koopa_fireber_dodai_evt
     360                      @2682                        mobj_badgeblk
== .rodata: CONTENT
   MISSING (1):
     (200, 0, 200)            vec3_802c405c                
   DUP_TARGET (7):
     (0, 0, 0)                vec3_802c40a0                evt_mobj_koopa_ojama_blk
     (0, 0, 0)                vec3_802c40ac                evt_mobj_koopa_stone_blk
     (0, 0, 0)                vec3_802c40b8                
     (0, 0, 0)                vec3_802c40c4                evt_mobj_koopa_sango
     (0, 0, 0)                vec3_802c40d0                
     (0, 0, 0)                vec3_802c4168                
     (0, 0, 0)                vec3_802c4174                

Relocation value findings: 0.

## Experiment 6: Retain unused names using ordinary deferred literal helpers rather than named static arrays

mobj_lv_blk_evt: 100.000000 -> 97.893840
evt_mobj_switch_float_blk: 99.967740 -> 100.000000
mobj_itembox: 100.000000 -> 97.439100
evt_mobj_itembox: 100.000000 -> 97.357140
evt_mobj_badgeblk: 99.963300 -> 100.000000
mobj_brickblk: 100.000000 -> 91.905760
kururing_capture: 87.358140 -> 85.613950
evt_mobj_kururing_floor: 100.000000 -> 97.049580
evt_mobj_koopa_dokan: 100.000000 -> 97.694916

.rodata: 2576 -> 2572 bytes; first byte difference 0xa0c; tail >=0x400 False
.sdata2: 288 -> 487 bytes; first byte difference 0x120; tail >=0x400 True

== .sdata2: CONTENT
   EXTRA (1):
     6d6500                   @3445                        evt_mobj_hitevt_onoff, evt_mobj_set_camid, evt_mobj_wait_animation_end, evt_mobj_set_anim
   DUP_OURS (48):
     0.2                      @2047                        mobj_koopa_fireber_dodai_evt, mobj_rideswitch_lightblue
     13.9497                  @2616                        evt_mobj_powerupblk, mobj_powerupblk, mobj_badgeblk, mobj_rideswitch_lightblue
     30                       @2088                        mobj_koopa_sango_evt, mobj_kururing_floor
     1                        @1962                        evt_mobj_koopa_fireber_dodai, fire_func, mobj_koopa_badgeblk, mobj_koopa_brickblk   [neg]
     200                      @2040                        mobj_koopa_fireber_dodai_evt
     0.04                     @2045                        mobj_koopa_fireber_dodai_evt
     9.58633e+11              @2557                        evt_mobj_brick
     4.8                      @2043                        mobj_koopa_fireber_dodai_evt
     0.5                      @1921                        kpa_disp_pole_score, kururing_capture, mobj_lock
     -1                       @3373                        mobj_timerswitch, mobj_switch_blue   [neg]
     360                      @2702                        mobj_badgeblk
     2.762e-128 (double)      @1965                        evt_mobj_koopa_fireber_dodai, mobj_koopa_fireber_dodai_evt
== .rodata: CONTENT
   MISSING (1):
     (200, 0, 200)            vec3_802c405c                
   DUP_TARGET (7):
     (0, 0, 0)                vec3_802c40a0                evt_mobj_koopa_ojama_blk
     (0, 0, 0)                vec3_802c40ac                evt_mobj_koopa_stone_blk
     (0, 0, 0)                vec3_802c40b8                
     (0, 0, 0)                vec3_802c40c4                evt_mobj_koopa_sango
     (0, 0, 0)                vec3_802c40d0                
     (0, 0, 0)                vec3_802c4168                
     (0, 0, 0)                vec3_802c4174                

Relocation value findings: 0.

## Experiment 7: Replace prefix Vec/template base reads in tail-literal functions with their real objects to remove duplicate rodata base registers

mobj_lv_blk_evt: 100.000000 -> 99.020546
evt_mobj_switch_float_blk: 99.967740 -> 100.000000
mobj_itembox: 100.000000 -> 97.576920
evt_mobj_badgeblk: 99.963300 -> 100.000000
mobj_brickblk: 100.000000 -> 97.130890
kururing_capture: 87.358140 -> 90.088370
evt_mobj_koopa_fireber_dodai: 98.829790 -> 94.824470
evt_mobj_koopa_pole: 100.000000 -> 88.343950

.rodata: 2576 -> 2572 bytes; first byte difference 0xa0c; tail >=0x400 False
.sdata2: 288 -> 487 bytes; first byte difference 0x120; tail >=0x400 True

== .sdata2: CONTENT
   EXTRA (1):
     6d6500                   @3445                        evt_mobj_hitevt_onoff, evt_mobj_set_camid, evt_mobj_wait_animation_end, evt_mobj_set_anim
   DUP_OURS (48):
     13.9497                  @2616                        evt_mobj_powerupblk, mobj_powerupblk, mobj_badgeblk, mobj_rideswitch_lightblue
     200                      @2040                        mobj_koopa_fireber_dodai_evt
     9.586e+11                @2112                        evt_mobj_koopa_dokan, evt_mobj_koopa_blk, evt_mobj_koopa_hidden_blk, evt_mobj_koopa_badgeblk
     9.58617e+11              @2162                        evt_mobj_koopa_hidden_blk, evt_mobj_koopa_badgeblk, mobj_koopa_badgeblk, evt_mobj_brick
     0.7                      @2210                        mobj_koopa_badgeblk, mobj_koopa_ojama_blk_evt
     13.95                    @2592                        mobj_10countblk, evt_mobj_switch_float_blk
     4.8                      @2043                        mobj_koopa_fireber_dodai_evt
     12.5                     @2038                        mobj_koopa_fireber_dodai_evt, mobj_koopa_brickblk, mobj_timerswitch, mobj_switch_blue
     75                       @2042                        mobj_koopa_fireber_dodai_evt
     10                       @2087                        mobj_koopa_sango_evt, fire_func, kururing_capture
     0.3                      @3372                        mobj_timerswitch
     360                      @2702                        mobj_badgeblk
== .rodata: CONTENT
   MISSING (1):
     (200, 0, 200)            vec3_802c405c                
   DUP_TARGET (7):
     (0, 0, 0)                vec3_802c40a0                evt_mobj_koopa_ojama_blk
     (0, 0, 0)                vec3_802c40ac                evt_mobj_koopa_stone_blk
     (0, 0, 0)                vec3_802c40b8                
     (0, 0, 0)                vec3_802c40c4                evt_mobj_koopa_sango
     (0, 0, 0)                vec3_802c40d0                
     (0, 0, 0)                vec3_802c4168                
     (0, 0, 0)                vec3_802c4174                

Relocation value findings: 0.

## Experiment 8: Retain prefix-only functions; share the real defined rodata base in the three remaining regressing callbacks

mobj_lv_blk_evt: 100.000000 -> 95.202060
evt_mobj_switch_float_blk: 99.967740 -> 100.000000
mobj_itembox: 100.000000 -> 85.746796
evt_mobj_badgeblk: 99.963300 -> 100.000000
mobj_brickblk: 100.000000 -> 92.607330
kururing_capture: 87.358140 -> 90.088370

.rodata: 2576 -> 2572 bytes; first byte difference 0xa0c; tail >=0x400 False
.sdata2: 288 -> 487 bytes; first byte difference 0x120; tail >=0x400 True

== .sdata2: CONTENT
   EXTRA (1):
     6d6500                   @3445                        evt_mobj_hitevt_onoff, evt_mobj_set_camid, evt_mobj_wait_animation_end, evt_mobj_set_anim
   DUP_OURS (48):
     0.99                     @2098                        fire_func
     175                      @1964                        evt_mobj_koopa_fireber_dodai
     9.58633e+11              @2557                        evt_mobj_brick
     0.125                    @2097                        fire_func
     360                      @2702                        mobj_badgeblk
     180                      @1821                        evt_mobj_koopa_pole
     200                      @2040                        mobj_koopa_fireber_dodai_evt
     1.57057e+16              @2889                        evt_mobj_lock
     120                      @2212                        mobj_koopa_badgeblk, mobj_koopa_ojama_blk_evt
     13.9497                  @2616                        evt_mobj_powerupblk, mobj_powerupblk, mobj_badgeblk, mobj_rideswitch_lightblue
     0.5                      @1921                        kpa_disp_pole_score, kururing_capture, mobj_lock
     2                        @2044                        mobj_koopa_fireber_dodai_evt
== .rodata: CONTENT
   MISSING (1):
     (200, 0, 200)            vec3_802c405c                
   DUP_TARGET (7):
     (0, 0, 0)                vec3_802c40a0                evt_mobj_koopa_ojama_blk
     (0, 0, 0)                vec3_802c40ac                evt_mobj_koopa_stone_blk
     (0, 0, 0)                vec3_802c40b8                
     (0, 0, 0)                vec3_802c40c4                evt_mobj_koopa_sango
     (0, 0, 0)                vec3_802c40d0                
     (0, 0, 0)                vec3_802c4168                
     (0, 0, 0)                vec3_802c4174                

Relocation value findings: 0.

## Experiment 9: Static const prefix Vec templates, aggregate name-template copies, and early object-parameter save
Compile failed (see build/evt_mobj_compile_9.log).

## Experiment 10: Give static Vec forward declarations their actual one-element array size
Compile failed (see build/evt_mobj_compile_10.log).

## Experiment 11: Align the remaining local Vec declarations with static const definitions
Compile failed (see build/evt_mobj_compile_11.log).

## Experiment 12: Use a private static Vec name because evt_mobj.h already exports a mutable vec3_802c3ee8

mobj_lv_blk_evt: 100.000000 -> 99.246574
evt_mobj_switch_float_blk: 99.967740 -> 100.000000
mobj_itembox: 100.000000 -> 97.016030
evt_mobj_badgeblk: 99.963300 -> 100.000000
mobj_brickblk: 100.000000 -> 96.753930
kururing_capture: 87.358140 -> 90.088370

.rodata: 2576 -> 2572 bytes; first byte difference 0xa0c; tail >=0x400 False
.sdata2: 288 -> 487 bytes; first byte difference 0x120; tail >=0x400 True

== .sdata2: CONTENT
   EXTRA (1):
     6d6500                   @3429                        evt_mobj_hitevt_onoff, evt_mobj_set_camid, evt_mobj_wait_animation_end, evt_mobj_set_anim
   DUP_OURS (48):
     1                        @1954                        evt_mobj_koopa_fireber_dodai, fire_func, mobj_koopa_badgeblk, mobj_koopa_brickblk   [neg]
     3                        @2038                        mobj_koopa_fireber_dodai_evt
     30                       @2080                        mobj_koopa_sango_evt, mobj_kururing_floor
     -1                       @3357                        mobj_timerswitch, mobj_switch_blue   [neg]
     13.9497                  @2608                        evt_mobj_powerupblk, mobj_powerupblk, mobj_badgeblk, mobj_rideswitch_lightblue
     9.586e+11                @2104                        evt_mobj_koopa_dokan, evt_mobj_koopa_blk, evt_mobj_koopa_hidden_blk, evt_mobj_koopa_badgeblk
     200                      @2032                        mobj_koopa_fireber_dodai_evt
     25                       @1955                        evt_mobj_koopa_fireber_dodai, mobj_koopa_brickblk
     9.58633e+11              @2549                        evt_mobj_brick
     0.1                      @2327                        evt_mobj_kururing_floor
     2.762e-128 (double)      @1957                        evt_mobj_koopa_fireber_dodai, mobj_koopa_fireber_dodai_evt
     120                      @2204                        mobj_koopa_badgeblk, mobj_koopa_ojama_blk_evt
== .rodata: CONTENT
   MISSING (1):
     (200, 0, 200)            vec3_802c405c                
   DUP_TARGET (7):
     (0, 0, 0)                vec3_802c40a0                evt_mobj_koopa_ojama_blk
     (0, 0, 0)                vec3_802c40ac                evt_mobj_koopa_stone_blk
     (0, 0, 0)                vec3_802c40b8                
     (0, 0, 0)                vec3_802c40c4                evt_mobj_koopa_sango
     (0, 0, 0)                vec3_802c40d0                
     (0, 0, 0)                vec3_802c4168                
     (0, 0, 0)                vec3_802c4174                

Relocation value findings: 0.

## Experiment 13: Restore mutable position-vector copy lowering; name the itembox string pointer at function entry

evt_mobj_switch_float_blk: 99.967740 -> 100.000000
mobj_itembox: 100.000000 -> 96.695510
evt_mobj_badgeblk: 99.963300 -> 100.000000
mobj_brickblk: 100.000000 -> 97.905760
kururing_capture: 87.358140 -> 90.088370

.rodata: 2576 -> 2572 bytes; first byte difference 0xa0c; tail >=0x400 False
.sdata2: 288 -> 487 bytes; first byte difference 0x120; tail >=0x400 True

== .sdata2: CONTENT
   EXTRA (1):
     6d6500                   @3429                        evt_mobj_hitevt_onoff, evt_mobj_set_camid, evt_mobj_wait_animation_end, evt_mobj_set_anim
   DUP_OURS (48):
     13.9497                  @2608                        evt_mobj_powerupblk, mobj_powerupblk, mobj_badgeblk, mobj_rideswitch_lightblue
     4.8                      @2035                        mobj_koopa_fireber_dodai_evt
     500                      @2203                        mobj_koopa_badgeblk, mobj_koopa_ojama_blk_evt
     0.0174533                @2029                        mobj_koopa_fireber_dodai_evt
     3                        @2038                        mobj_koopa_fireber_dodai_evt
     30                       @2080                        mobj_koopa_sango_evt, mobj_kururing_floor
     0.3                      @3356                        mobj_timerswitch
     0.125                    @2089                        fire_func
     0.6                      @2446                        mobj_kururing_floor
     9.58617e+11              @2154                        evt_mobj_koopa_hidden_blk, evt_mobj_koopa_badgeblk, mobj_koopa_badgeblk, evt_mobj_brick
     2.762e-128 (double)      @1957                        evt_mobj_koopa_fireber_dodai, mobj_koopa_fireber_dodai_evt
     6                        @2033                        mobj_koopa_fireber_dodai_evt
== .rodata: CONTENT
   MISSING (1):
     (200, 0, 200)            vec3_802c405c                
   DUP_TARGET (7):
     (0, 0, 0)                vec3_802c40a0                evt_mobj_koopa_ojama_blk
     (0, 0, 0)                vec3_802c40ac                evt_mobj_koopa_stone_blk
     (0, 0, 0)                vec3_802c40b8                
     (0, 0, 0)                vec3_802c40c4                evt_mobj_koopa_sango
     (0, 0, 0)                vec3_802c40d0                
     (0, 0, 0)                vec3_802c4168                
     (0, 0, 0)                vec3_802c4174                

Relocation value findings: 0.

## Experiment 14: Inline boundary for itembox parameter rank; earlier strcat inputs; flush deferred pool before four-byte section tail

evt_mobj_switch_float_blk: 99.967740 -> 100.000000
mobj_itembox: 100.000000 -> 2.519231
evt_mobj_badgeblk: 99.963300 -> 100.000000
mobj_brickblk: 100.000000 -> 97.905760
kururing_capture: 87.358140 -> 90.088370

.rodata: 2576 -> 2572 bytes; first byte difference 0xa0c; tail >=0x400 False
.sdata2: 288 -> 487 bytes; first byte difference 0x120; tail >=0x400 True

== .sdata2: CONTENT
   EXTRA (1):
     6d6500                   @3424                        evt_mobj_hitevt_onoff, evt_mobj_set_camid, evt_mobj_wait_animation_end, evt_mobj_set_anim
   DUP_OURS (48):
     13.95                    @2592                        mobj_10countblk, evt_mobj_switch_float_blk
     6                        @2041                        mobj_koopa_fireber_dodai_evt
     175                      @1965                        evt_mobj_koopa_fireber_dodai
     30                       @2088                        mobj_koopa_sango_evt, mobj_kururing_floor
     0.125                    @2097                        fire_func
     1024                     @2086                        mobj_koopa_sango_evt, mobj_itembox_update
     500                      @2211                        mobj_koopa_badgeblk, mobj_koopa_ojama_blk_evt
     15                       @3299                        mobj_rideswitch_lightblue
     1                        @1963                        evt_mobj_koopa_fireber_dodai, fire_func, mobj_koopa_badgeblk, mobj_koopa_brickblk   [neg]
     230                      @1917                        kpa_disp_pole_score_main
     2                        @2044                        mobj_koopa_fireber_dodai_evt
     180                      @1822                        evt_mobj_koopa_pole
== .rodata: CONTENT
   MISSING (1):
     (200, 0, 200)            vec3_802c405c                
   DUP_TARGET (7):
     (0, 0, 0)                vec3_802c40a0                evt_mobj_koopa_ojama_blk
     (0, 0, 0)                vec3_802c40ac                evt_mobj_koopa_stone_blk
     (0, 0, 0)                vec3_802c40b8                
     (0, 0, 0)                vec3_802c40c4                evt_mobj_koopa_sango
     (0, 0, 0)                vec3_802c40d0                
     (0, 0, 0)                vec3_802c4168                
     (0, 0, 0)                vec3_802c4174                

Relocation value findings: 0.

## Experiment 15: Force the itembox inline boundary; preserve final-string NUL alignment and expose real Vec objects to pool diagnostics

evt_mobj_switch_float_blk: 99.967740 -> 100.000000
mobj_itembox: 100.000000 -> 97.480770
evt_mobj_badgeblk: 99.963300 -> 100.000000
mobj_brickblk: 100.000000 -> 97.905760
kururing_capture: 87.358140 -> 90.088370

.rodata: 2576 -> 2576 bytes; first byte difference none (IDENTICAL); tail >=0x400 True
.sdata2: 288 -> 487 bytes; first byte difference 0x120; tail >=0x400 True

== .sdata2: CONTENT
   EXTRA (1):
     6d6500                   @3425                        evt_mobj_hitevt_onoff, evt_mobj_set_camid, evt_mobj_wait_animation_end, evt_mobj_set_anim
   DUP_OURS (48):
     360                      @2697                        mobj_badgeblk
     150                      @2031                        mobj_koopa_fireber_dodai_evt
     0.9                      @3299                        mobj_rideswitch_lightblue
     0                        @1912                        kpa_disp_pole_score_main, evt_mobj_koopa_fireber_dodai, mobj_kururing_floor, mobj_lock   [neg]
     30                       @2083                        mobj_koopa_sango_evt, mobj_kururing_floor
     10                       @2082                        mobj_koopa_sango_evt, fire_func, kururing_capture
     12.5                     @2033                        mobj_koopa_fireber_dodai_evt, mobj_koopa_brickblk, mobj_timerswitch, mobj_switch_blue
     6                        @2036                        mobj_koopa_fireber_dodai_evt
     4.8                      @2038                        mobj_koopa_fireber_dodai_evt
     0.125                    @2092                        fire_func
     200                      @2035                        mobj_koopa_fireber_dodai_evt
     0.8                      @2091                        fire_func, evt_mobj_rideswitch_lightblue   [neg]
== .rodata: IDENTICAL

Relocation value findings: 0.

## Experiment 16: Preserve itembox local object via opt_propagation off; disable brick copy common subexpressions

evt_mobj_switch_float_blk: 99.967740 -> 100.000000
mobj_itembox: 100.000000 -> 96.919870
evt_mobj_badgeblk: 99.963300 -> 100.000000
mobj_brickblk: 100.000000 -> 90.246070
kururing_capture: 87.358140 -> 90.088370

.rodata: 2576 -> 2576 bytes; first byte difference none (IDENTICAL); tail >=0x400 True
.sdata2: 288 -> 487 bytes; first byte difference 0x120; tail >=0x400 True

== .sdata2: CONTENT
   EXTRA (1):
     6d6500                   @3425                        evt_mobj_hitevt_onoff, evt_mobj_set_camid, evt_mobj_wait_animation_end, evt_mobj_set_anim
   DUP_OURS (48):
     360                      @2690                        mobj_badgeblk
     -1000                    @2031                        mobj_koopa_fireber_dodai_evt
     -1                       @3353                        mobj_timerswitch, mobj_switch_blue   [neg]
     2.762e-128 (double)      @1957                        evt_mobj_koopa_fireber_dodai, mobj_koopa_fireber_dodai_evt
     0.45                     @2201                        mobj_koopa_badgeblk, mobj_koopa_ojama_blk_evt
     30                       @2080                        mobj_koopa_sango_evt, mobj_kururing_floor
     230                      @1908                        kpa_disp_pole_score_main
     10                       @2079                        mobj_koopa_sango_evt, fire_func, kururing_capture
     500                      @2203                        mobj_koopa_badgeblk, mobj_koopa_ojama_blk_evt
     9.586e+11                @2104                        evt_mobj_koopa_dokan, evt_mobj_koopa_blk, evt_mobj_koopa_hidden_blk, evt_mobj_koopa_badgeblk
     60                       @2081                        mobj_koopa_sango_evt
     9.58617e+11              @2154                        evt_mobj_koopa_hidden_blk, evt_mobj_koopa_badgeblk, mobj_koopa_badgeblk, evt_mobj_brick
== .rodata: IDENTICAL

Relocation value findings: 0.

## Experiment 17: Use a scalarized parameter aggregate for itembox; scheduling off for brick template copies
Compile failed (see build/evt_mobj_compile_17.log).

## Experiment 18: Assign the parameter aggregate after declaration (MWCC requires constant declaration initializers)

evt_mobj_switch_float_blk: 99.967740 -> 100.000000
mobj_itembox: 100.000000 -> 97.576920
evt_mobj_badgeblk: 99.963300 -> 100.000000
mobj_brickblk: 100.000000 -> 64.497380
kururing_capture: 87.358140 -> 90.088370

.rodata: 2576 -> 2576 bytes; first byte difference none (IDENTICAL); tail >=0x400 True
.sdata2: 288 -> 487 bytes; first byte difference 0x120; tail >=0x400 True

== .sdata2: CONTENT
   EXTRA (1):
     6d6500                   @3430                        evt_mobj_hitevt_onoff, evt_mobj_set_camid, evt_mobj_wait_animation_end, evt_mobj_set_anim
   DUP_OURS (48):
     15                       @3305                        mobj_rideswitch_lightblue
     2                        @2036                        mobj_koopa_fireber_dodai_evt
     30                       @2080                        mobj_koopa_sango_evt, mobj_kururing_floor
     200                      @2032                        mobj_koopa_fireber_dodai_evt
     150                      @2028                        mobj_koopa_fireber_dodai_evt
     1                        @1954                        evt_mobj_koopa_fireber_dodai, fire_func, mobj_koopa_badgeblk, mobj_koopa_brickblk   [neg]
     180                      @1813                        evt_mobj_koopa_pole
     9.58617e+11              @2154                        evt_mobj_koopa_hidden_blk, evt_mobj_koopa_badgeblk, mobj_koopa_badgeblk, evt_mobj_brick
     12.5                     @2030                        mobj_koopa_fireber_dodai_evt, mobj_koopa_brickblk, mobj_timerswitch, mobj_switch_blue
     500                      @2203                        mobj_koopa_badgeblk, mobj_koopa_ojama_blk_evt
     75                       @2034                        mobj_koopa_fireber_dodai_evt
     6                        @2033                        mobj_koopa_fireber_dodai_evt
== .rodata: IDENTICAL

Relocation value findings: 0.

## Experiment 19: Truncated sign-extension web for itembox; typed temporary name-template values for brick copies

evt_mobj_switch_float_blk: 99.967740 -> 100.000000
mobj_itembox: 100.000000 -> 97.016030
evt_mobj_badgeblk: 99.963300 -> 100.000000
mobj_brickblk: 100.000000 -> 92.973820
kururing_capture: 87.358140 -> 90.088370

.rodata: 2576 -> 2576 bytes; first byte difference none (IDENTICAL); tail >=0x400 True
.sdata2: 288 -> 487 bytes; first byte difference 0x120; tail >=0x400 True

== .sdata2: CONTENT
   EXTRA (1):
     6d6500                   @3429                        evt_mobj_hitevt_onoff, evt_mobj_set_camid, evt_mobj_wait_animation_end, evt_mobj_set_anim
   DUP_OURS (48):
     2                        @2036                        mobj_koopa_fireber_dodai_evt
     10                       @2079                        mobj_koopa_sango_evt, fire_func, kururing_capture
     13.95                    @2584                        mobj_10countblk, evt_mobj_switch_float_blk
     1.67174e+19              @1812                        evt_mobj_koopa_pole
     6                        @2033                        mobj_koopa_fireber_dodai_evt
     1                        @1954                        evt_mobj_koopa_fireber_dodai, fire_func, mobj_koopa_badgeblk, mobj_koopa_brickblk   [neg]
     150                      @2028                        mobj_koopa_fireber_dodai_evt
     9.58617e+11              @2154                        evt_mobj_koopa_hidden_blk, evt_mobj_koopa_badgeblk, mobj_koopa_badgeblk, evt_mobj_brick
     0.04                     @2037                        mobj_koopa_fireber_dodai_evt
     3                        @2038                        mobj_koopa_fireber_dodai_evt
     75                       @2034                        mobj_koopa_fireber_dodai_evt
     1024                     @2078                        mobj_koopa_sango_evt, mobj_itembox
== .rodata: IDENTICAL

Relocation value findings: 0.

## Experiment 20: Natural Vec and char-array initializer templates, with prefix data placed around their parse-time emission
Compile failed (see build/evt_mobj_compile_20.log).

## Experiment 21: Retain extern declarations for other functions reading the now-anonymous itembox initializer templates

evt_mobj_switch_float_blk: 99.967740 -> 100.000000
mobj_itembox: 100.000000 -> 97.576920
evt_mobj_itembox: 100.000000 -> 93.450890
evt_mobj_badgeblk: 99.963300 -> 100.000000
kururing_capture: 87.358140 -> 90.088370

.rodata: 2576 -> 2576 bytes; first byte difference none (IDENTICAL); tail >=0x400 True
.sdata2: 288 -> 487 bytes; first byte difference 0x120; tail >=0x400 True

== .sdata2: CONTENT
   EXTRA (1):
     6d6500                   @3439                        evt_mobj_hitevt_onoff, evt_mobj_set_camid, evt_mobj_wait_animation_end, evt_mobj_set_anim
   DUP_OURS (48):
     9.586e+11                @2112                        evt_mobj_koopa_dokan, evt_mobj_koopa_blk, evt_mobj_koopa_hidden_blk, evt_mobj_koopa_badgeblk
     120                      @2212                        mobj_koopa_badgeblk, mobj_koopa_ojama_blk_evt
     230                      @1916                        kpa_disp_pole_score_main
     15                       @3314                        mobj_rideswitch_lightblue
     0.6                      @2454                        mobj_kururing_floor
     9.58633e+11              @2557                        evt_mobj_brick
     3                        @2046                        mobj_koopa_fireber_dodai_evt
     1                        @1962                        evt_mobj_koopa_fireber_dodai, fire_func, mobj_koopa_badgeblk, mobj_koopa_brickblk   [neg]
     12.5                     @2038                        mobj_koopa_fireber_dodai_evt, mobj_koopa_brickblk, mobj_timerswitch, mobj_switch_blue
     1024                     @2086                        mobj_koopa_sango_evt, mobj_itembox
     1.67174e+19              @1820                        evt_mobj_koopa_pole
     0.5                      @1921                        kpa_disp_pole_score, kururing_capture, mobj_lock
== .rodata: IDENTICAL

Relocation value findings: 0.

## Experiment 22: Keep natural brick buffer templates with named vector prefix; name and load the itembox state before base initialization

evt_mobj_switch_float_blk: 99.967740 -> 100.000000
mobj_itembox: 100.000000 -> 97.576920
evt_mobj_badgeblk: 99.963300 -> 100.000000
kururing_capture: 87.358140 -> 90.088370

.rodata: 2576 -> 2576 bytes; first byte difference none (IDENTICAL); tail >=0x400 True
.sdata2: 288 -> 487 bytes; first byte difference 0x120; tail >=0x400 True

== .sdata2: CONTENT
   EXTRA (1):
     6d6500                   @3431                        evt_mobj_hitevt_onoff, evt_mobj_set_camid, evt_mobj_wait_animation_end, evt_mobj_set_anim
   DUP_OURS (48):
     4.8                      @2037                        mobj_koopa_fireber_dodai_evt
     30                       @2082                        mobj_koopa_sango_evt, mobj_kururing_floor
     10                       @2081                        mobj_koopa_sango_evt, fire_func, kururing_capture
     9.58633e+11              @2551                        evt_mobj_brick
     0.45                     @2203                        mobj_koopa_badgeblk, mobj_koopa_ojama_blk_evt
     2                        @2038                        mobj_koopa_fireber_dodai_evt
     0.04                     @2039                        mobj_koopa_fireber_dodai_evt
     9.586e+11                @2106                        evt_mobj_koopa_dokan, evt_mobj_koopa_blk, evt_mobj_koopa_hidden_blk, evt_mobj_koopa_badgeblk
     175                      @1958                        evt_mobj_koopa_fireber_dodai
     0.2                      @2041                        mobj_koopa_fireber_dodai_evt, mobj_rideswitch_lightblue
     0.7                      @2204                        mobj_koopa_badgeblk, mobj_koopa_ojama_blk_evt
     12.5                     @2032                        mobj_koopa_fireber_dodai_evt, mobj_koopa_brickblk, mobj_timerswitch, mobj_switch_blue
== .rodata: IDENTICAL

Relocation value findings: 0.

## Experiment 23: Unsigned conversion web and opt_lifetimes off for the sole remaining itembox register swap

evt_mobj_switch_float_blk: 99.967740 -> 100.000000
mobj_itembox: 100.000000 -> 97.576920
evt_mobj_badgeblk: 99.963300 -> 100.000000
kururing_capture: 87.358140 -> 90.088370

.rodata: 2576 -> 2576 bytes; first byte difference none (IDENTICAL); tail >=0x400 True
.sdata2: 288 -> 487 bytes; first byte difference 0x120; tail >=0x400 True

== .sdata2: CONTENT
   EXTRA (1):
     6d6500                   @3429                        evt_mobj_hitevt_onoff, evt_mobj_set_camid, evt_mobj_wait_animation_end, evt_mobj_set_anim
   DUP_OURS (48):
     0.6                      @2448                        mobj_kururing_floor
     1.57057e+16              @2881                        evt_mobj_lock
     -1000                    @2033                        mobj_koopa_fireber_dodai_evt
     180                      @1815                        evt_mobj_koopa_pole
     25                       @1957                        evt_mobj_koopa_fireber_dodai, mobj_koopa_brickblk
     1                        @1956                        evt_mobj_koopa_fireber_dodai, fire_func, mobj_koopa_badgeblk, mobj_koopa_brickblk   [neg]
     6                        @2035                        mobj_koopa_fireber_dodai_evt
     0.7                      @2204                        mobj_koopa_badgeblk, mobj_koopa_ojama_blk_evt
     4.8                      @2037                        mobj_koopa_fireber_dodai_evt
     120                      @2206                        mobj_koopa_badgeblk, mobj_koopa_ojama_blk_evt
     13.9495                  @2202                        mobj_koopa_badgeblk, mobj_koopa_brickblk, mobj_breaking_floor, mobj_10countblk
     30                       @2082                        mobj_koopa_sango_evt, mobj_kururing_floor
== .rodata: IDENTICAL

Relocation value findings: 0.

## Experiment 24: Preserve the single itembox callback match-only base shape; its strings and double are pooled naturally by the entry function

evt_mobj_switch_float_blk: 99.967740 -> 100.000000
evt_mobj_badgeblk: 99.963300 -> 100.000000
kururing_capture: 87.358140 -> 90.088370

.rodata: 2576 -> 2576 bytes; first byte difference none (IDENTICAL); tail >=0x400 True
.sdata2: 288 -> 487 bytes; first byte difference 0x120; tail >=0x400 True

== .sdata2: CONTENT
   EXTRA (1):
     6d6500                   @3431                        evt_mobj_hitevt_onoff, evt_mobj_set_camid, evt_mobj_wait_animation_end, evt_mobj_set_anim
   DUP_OURS (48):
     12.5                     @2032                        mobj_koopa_fireber_dodai_evt, mobj_koopa_brickblk, mobj_timerswitch, mobj_switch_blue
     13.95                    @2586                        mobj_10countblk, evt_mobj_switch_float_blk
     -1000                    @2033                        mobj_koopa_fireber_dodai_evt
     1024                     @2080                        mobj_koopa_sango_evt, mobj_itembox
     9.58633e+11              @2551                        evt_mobj_brick
     0.04                     @2039                        mobj_koopa_fireber_dodai_evt
     0.5                      @1915                        kpa_disp_pole_score, kururing_capture, mobj_lock
     2                        @2038                        mobj_koopa_fireber_dodai_evt
     0.8                      @2090                        fire_func, evt_mobj_rideswitch_lightblue   [neg]
     500                      @2205                        mobj_koopa_badgeblk, mobj_koopa_ojama_blk_evt
     30                       @2082                        mobj_koopa_sango_evt, mobj_kururing_floor
     1.67174e+19              @1814                        evt_mobj_koopa_pole
== .rodata: IDENTICAL

Relocation value findings: 0.

## Experiment 25: Remove obsolete macros, duplicate guards, unused bases and stale comments; keep the verified single callback match shape
Compile failed (see build/evt_mobj_compile_25.log).

## Experiment 26: Keep the remaining macro-owned itembox base local during cleanup

evt_mobj_switch_float_blk: 99.967740 -> 100.000000
evt_mobj_badgeblk: 99.963300 -> 100.000000
kururing_capture: 87.358140 -> 90.088370

.rodata: 2576 -> 2576 bytes; first byte difference none (IDENTICAL); tail >=0x400 True
.sdata2: 288 -> 487 bytes; first byte difference 0x120; tail >=0x400 True

== .sdata2: CONTENT
   EXTRA (1):
     6d6500                   @3431                        evt_mobj_hitevt_onoff, evt_mobj_set_camid, evt_mobj_wait_animation_end, evt_mobj_set_anim
   DUP_OURS (48):
     0.9                      @3305                        mobj_rideswitch_lightblue
     230                      @1910                        kpa_disp_pole_score_main
     -1                       @3359                        mobj_timerswitch, mobj_switch_blue   [neg]
     0.3                      @3358                        mobj_timerswitch
     13.9495                  @2202                        mobj_koopa_badgeblk, mobj_koopa_brickblk, mobj_breaking_floor, mobj_10countblk
     300                      @1914                        kpa_disp_pole_score, mobj_kururing_floor
     15                       @3306                        mobj_rideswitch_lightblue
     0.1                      @2329                        evt_mobj_kururing_floor
     175                      @1958                        evt_mobj_koopa_fireber_dodai
     360                      @2696                        mobj_badgeblk
     6                        @2035                        mobj_koopa_fireber_dodai_evt
     30                       @2082                        mobj_koopa_sango_evt, mobj_kururing_floor
== .rodata: IDENTICAL

Relocation value findings: 0.

## Experiment 27: Final source cleanup: plain powerup literals and accurate pool/template comments

evt_mobj_switch_float_blk: 99.967740 -> 100.000000
evt_mobj_badgeblk: 99.963300 -> 100.000000
kururing_capture: 87.358140 -> 90.088370

.rodata: 2576 -> 2576 bytes; first byte difference none (IDENTICAL); tail >=0x400 True
.sdata2: 288 -> 487 bytes; first byte difference 0x120; tail >=0x400 True

== .sdata2: CONTENT
   EXTRA (1):
     6d6500                   @3431                        evt_mobj_hitevt_onoff, evt_mobj_set_camid, evt_mobj_wait_animation_end, evt_mobj_set_anim
   DUP_OURS (48):
     1024                     @2080                        mobj_koopa_sango_evt, mobj_itembox
     6                        @2035                        mobj_koopa_fireber_dodai_evt
     0.2                      @2041                        mobj_koopa_fireber_dodai_evt, mobj_rideswitch_lightblue
     30                       @2082                        mobj_koopa_sango_evt, mobj_kururing_floor
     500                      @2205                        mobj_koopa_badgeblk, mobj_koopa_ojama_blk_evt
     0.7                      @2204                        mobj_koopa_badgeblk, mobj_koopa_ojama_blk_evt
     25                       @1957                        evt_mobj_koopa_fireber_dodai, mobj_koopa_brickblk
     0.8                      @2090                        fire_func, evt_mobj_rideswitch_lightblue   [neg]
     0.99                     @2092                        fire_func
     0.04                     @2039                        mobj_koopa_fireber_dodai_evt
     0.125                    @2091                        fire_func
     2.762e-128 (double)      @1959                        evt_mobj_koopa_fireber_dodai, mobj_koopa_fireber_dodai_evt
== .rodata: IDENTICAL

Relocation value findings: 0.

## Final verification and outcome

27 numbered source experiments; 32 total MWCC compiler invocations including the unchanged baseline, one baseline verification rebuild, and three natural-build checks. Infrastructure-only ninja failures never reached MWCC. No function is below its starting score. All changes are uncommitted.

- `.rodata`: **IDENTICAL**, all 2576 bytes (including the prefix, both conversion doubles, both f64 values, and the final four zero bytes). The goal from +0x400 onward is therefore met.
- `.sdata2`: byte-identical to the starting build (487 bytes, target 288). Its existing 48 DUP_OURS and one EXTRA are unchanged; no new duplicate or extra entries were introduced.
- `tools/reloc_value_check.py --partial evt_mobj`: 0 findings; direct checks of baseline and final objects also return 0.
- Matching compile and final natural compile: PASS.
- Original functions at 100%: 79/95 -> 81/95. `evt_mobj_switch_float_blk` 99.967740 -> 100; `evt_mobj_badgeblk` 99.963300 -> 100; `kururing_capture` 87.358140 -> 90.088370. All other starting scores are unchanged.

What fixed what: `pool_data on` plus literal uses emits the signed conversion double at +0x400 and the remaining strings/f64 values in original codegen order. Seven unused animation strings require ordinary deferred helpers under MUST_MATCH: local static arrays were a failed approach because they are emitted at parse time into the prefix. Natural `o_`/`i_` buffer initializers and moving their prefix data ahead of mobj_brickblk preserve template positions and restore that callback's 100% score. The final string carries explicit NUL padding under MUST_MATCH to retain the section tail. Renaming real Vec objects previously called gap_* allows sdata2_diff to include them instead of misclassifying the identical prefix as missing values.

Remaining recipe limitation: `mobj_itembox` still uses its existing documented MUST_MATCH IB_R/IB_VEC/IB_F64 base-address form. Converting that callback completely to direct objects/literals makes the compiler's pool-base temporary take r31 and the object parameter r30 (97.576920%); rank, copy, and pragma experiments did not restore 100%. Keeping this one shape preserves 100% while evt_mobj_itembox naturally emits every string and the double it reads. The natural build uses ordinary literal/object values through the existing #else macros. All other targeted tail string base-offset sites were replaced. This is a byte-identical naturally emitted pool, but not a complete elimination of the matching callback's base-address macros.

Whole-project ninja/CI could not run: the generated split edge needs absent `orig/G8MJ01/files/rel/aaa.rel`. Existing local compiler/tool paths were supplied to configure.py without editing it; matching/natural checks use the exact generated unit compile command. No tools/ files or configure.py were modified. Supplemental lint has advisory data-extern/template-struct and existing raw-field-offset notes; newly matched functions are verified against the fresh unit report.

Final changed original functions (starting -> final scores):

- `evt_mobj_badgeblk`: 99.963300 -> 100.000000
- `evt_mobj_breaking_floor`: 100.000000 -> 100.000000
- `evt_mobj_brick`: 100.000000 -> 100.000000
- `evt_mobj_float_blk`: 100.000000 -> 100.000000
- `evt_mobj_floatswitch_blue`: 99.775280 -> 99.775280
- `evt_mobj_floatswitch_red`: 99.775280 -> 99.775280
- `evt_mobj_itembox`: 100.000000 -> 100.000000
- `evt_mobj_koopa_badgeblk`: 99.797980 -> 99.797980
- `evt_mobj_koopa_blk`: 100.000000 -> 100.000000
- `evt_mobj_koopa_brick`: 100.000000 -> 100.000000
- `evt_mobj_koopa_dokan`: 100.000000 -> 100.000000
- `evt_mobj_koopa_hidden_blk`: 99.809525 -> 99.809525
- `evt_mobj_koopa_sango`: 100.000000 -> 100.000000
- `evt_mobj_kururing_floor`: 100.000000 -> 100.000000
- `evt_mobj_lock`: 100.000000 -> 100.000000
- `evt_mobj_lock_unlock`: 100.000000 -> 100.000000
- `evt_mobj_lv_blk`: 100.000000 -> 100.000000
- `evt_mobj_powerupblk`: 100.000000 -> 100.000000
- `evt_mobj_recovery_blk`: 100.000000 -> 100.000000
- `evt_mobj_rideswitch_lightblue`: 99.793816 -> 99.793816
- `evt_mobj_save_blk`: 100.000000 -> 100.000000
- `evt_mobj_signboard`: 99.740260 -> 99.740260
- `evt_mobj_switch_blue`: 100.000000 -> 100.000000
- `evt_mobj_switch_float_blk`: 99.967740 -> 100.000000
- `evt_mobj_switch_red`: 100.000000 -> 100.000000
- `evt_mobj_tornadoswitch_blue`: 99.775280 -> 99.775280
- `kururing_capture`: 87.358140 -> 90.088370
- `mobj_10countblk`: 100.000000 -> 100.000000
- `mobj_badgeblk`: 99.347150 -> 99.347150
- `mobj_brickblk`: 100.000000 -> 100.000000
- `mobj_itembox`: 100.000000 -> 100.000000
- `mobj_koopa_badgeblk`: 100.000000 -> 100.000000
- `mobj_koopa_brickblk`: 100.000000 -> 100.000000
- `mobj_koopa_dokan_evt`: 100.000000 -> 100.000000
- `mobj_lock`: 100.000000 -> 100.000000
- `mobj_lv_blk_evt`: 100.000000 -> 100.000000
- `mobj_powerupblk`: 100.000000 -> 100.000000
- `mobj_recovery_blk`: 98.210526 -> 98.210526
