#include "battle/ac/ac_shot_target.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_disp.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "pmario_sound.h"
#include "system.h"
#include "math_ext.h"
// math.h pools two sqrt doubles this unit does not have; only its fabs intrinsic is needed.
#define fabsf(x) __fabsf(x)
#include <string.h>

// Per-action-command work area at BattleWork + 0x1F4C (inside BattleActions.field_0x2BC).
// No header declares it: the exact fields are only used by this file.
typedef struct ShotTargetWork {
    s32 field_0x00; // 0x00, countdown (loaded from field_34[0x0C])
    s32 field_0x04; // 0x04, end-of-AC countdown
    Vec target;     // 0x08, target hit position
    Vec pos;        // 0x14, animated cursor position
    s32 field_0x20; // 0x20, within-28-units flag
    f32 field_0x24; // 0x24, rotation animation value
    s16 field_0x28; // 0x28, rumble period 1
    s16 field_0x2a; // 0x2A, rumble timer 1
    f32 field_0x2C; // 0x2C, rumble amplitude 1
    s16 field_0x30; // 0x30, rumble period 2
    s16 field_0x32; // 0x32, rumble timer 2
    f32 field_0x34; // 0x34, rumble amplitude 2
    s32 field_0x38; // 0x38, sound handle
} ShotTargetWork;

// The cursor is drawn as four screenspace quads; the original unit emitted these .rodata vectors.
// They are one contiguous block (the original has a separate symbol per 0xC bytes) so the code can
// use base+offset addressing.
const Vec vec3_802ff950[9] = {
    {0.0f, 60.0f, 0.0f},
    {1.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 0.0f},
    {1.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 0.0f},
    {1.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 0.0f},
    {1.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 0.0f},
};

const char str_802FF9BC[] = "SFX_AC_CURSOR_MOVE1";

// The original reads the nine .rodata vectors through a single vec3_802ff950 base register.
// MUST_MATCH reaches them through one `vecs` base; the natural form names each symbol.
#ifdef MUST_MATCH
#define RODATA_VEC(i, name) (vecs[i])
#else
#define RODATA_VEC(i, name) (name)
#endif

// Data constants declared extern before use so MWCC references them by name instead of folding their
// values into the instruction stream. They are defined at the bottom of this file. The original unit
// emitted all of them; the definitions keep the .rodata/.sdata2 layout and byte values.
extern const f64 double_3_802ff9d0;
extern const f64 double_28_802ff9d8;
extern const u32 dat_804218f8;
extern const u32 dat_804218fc;
extern const u32 dat_80421900;
extern const u32 dat_80421904;
extern const u32 dat_80421908;
extern const f32 float_2_8042190c;
extern const f32 float_4_80421910;
extern const f32 float_neg200_80421914;
extern const f32 float_0_80421918;
extern const f32 float_30_8042191c;
extern const f32 float_25_80421920;
extern const f32 float_neg1_80421924;
extern const f32 float_0p6_80421928;
extern const f32 float_0p25_8042192c;
extern const f32 float_0p5_80421930;
extern const f32 float_0p8_80421934;
extern const f32 float_1p5_80421938;
extern const f32 float_3_8042193c;
extern const f32 float_10_80421940;

// Forward declarations needed because the functions are written in the original's reverse-source
// (-inline deferred) order; prototypes for the file's own static helpers and the global helper.
static void actionCommandDisp(f32 x, f32 y);
static void _ac_disp_init(BattleWork* work);

// Every instruction, constant and memory access matches except the colouring of two callee-saved
// registers: the original keeps rumbleType in r24 and sfx in r28, MWCC here gives rumbleType r28
// and sfx r24 (7 objdiff `!!` lines: the two preheader loads and five `mr r3` argument setups).
// Tried with the dont_inline fix in place: declaration order (all permutations of the scalars, plus
// hit/unit/part/ac/st placements), assignment order and position, types (s32/u32/int/long for
// rumbleType; const char*/char*/u8*/s8*/const void*/int*/u32/s32/ulong for sfx), `register`/
// `volatile`, per-block copies, the ac_shot comma hack, extra no-op uses, every relevant opt_*/
// peephole/scheduling/global_optimizer/optimize_for_size/optimization_level pragma (single, pairs
// and triples, on and off), MWCC versions 1.0-3.0, case-0 statement order, and 4000+ random source
// mutations; none moves the swap.
// The `ac->`-qualified work-area access is required: without it st folds into work (18%).
// NONMATCHING (99.90%): rumbleType/sfx are coloured swapped (target r24/r28, ours r28/r24).
// The `#pragma dont_inline on` above `_ac_rumble_param_set` is required: without it MWCC inlines
// the whole body at all four call sites and the function drops to 19.82%.
s32 battleAcMain_ShotTarget(BattleWork* work) {
    BattleActions* ac = &work->actionCommands;
    ShotTargetWork* st = (ShotTargetWork*)&ac->field_0x2BC[0];
    s32 rumbleType;
    s32 cont;
    s32 smooth = 0;
    const char* sfx;
    Vec hit;
    BattleUnit* unit;
    BattleUnitPart* part;

    if (ac->mAcUnit->badgesEquipped.AutoCommand != 0) {
        smooth = 1;
    }
    rumbleType = *(s32*)&ac->field_34[0x10];

    do {
        cont = 0;
        // The name is a constant, so MWCC hoists this load into the loop preheader; writing it
        // here lands the hoisted `addi` between the rumble load and the two hoisted hit-address
        // temps, the order the original has.
        sfx = str_802FF9BC;
        switch (ac->mAcState) {
            case 0:
                _ac_disp_init(work);
                ac->result = 1;
                unit = BattleGetUnitPtr(work, *(s32*)&ac->field_34[0x04]);
                part = BtlUnit_GetPartsPtr(unit, *(s32*)&ac->field_34[0x08]);
                BtlUnit_GetHitPos(unit, part, &hit.x, &hit.y, &hit.z);
#ifdef MUST_MATCH
                // Match-only: volatile keeps MWCC from reordering these four stores, which the
                // original issues in exactly this order (target stores, field_0x20, pos, field_0x24).
                *(volatile Vec*)&st->target = hit;
                *(volatile u32*)&st->field_0x20 = 0;
                *(volatile Vec*)&st->pos = *(volatile Vec*)&vec3_802ff950[0];
                *(volatile f32*)&st->field_0x24 = float_0_80421918;
#else
                st->target = hit;
                st->field_0x20 = 0;
                st->pos = vec3_802ff950[0];
                st->field_0x24 = float_0_80421918;
#endif
                _ac_rumble_param_set(rumbleType, &st->field_0x28, &st->field_0x2a, &st->field_0x2C);
                _ac_rumble_param_set(rumbleType, &st->field_0x30, &st->field_0x32, &st->field_0x34);
                st->field_0x38 = -1;
                ac->mAcState = 0x63;
                break;
            case 99:
                break;
            case 100:
                st->field_0x00 = *(s32*)&ac->field_34[0x0C];
                ac->mAcState = 1000;
                st->field_0x38 = psndSFXOn(sfx);
                // fallthrough
            case 1000:
                if (*(s16*)&ac->field_78[0x10] > 0) {
                    (*(s16*)&ac->field_78[0x10])--;
                    ac->result = 2;
                    ac->mResultCount++;
                    ac->mAcState = 1001;
                    cont = 1;
                    break;
                }
                st->field_0x2a--;
                if (st->field_0x2a < 0) {
                    _ac_rumble_param_set(rumbleType, &st->field_0x28, &st->field_0x2a, &st->field_0x2C);
                }
                {
                    f32 t = sinfd((f32)((st->field_0x2a + 1) * 180 / st->field_0x28)) * st->field_0x2C;
                    st->pos.x += st->field_0x2C * sinfd((f32)(st->field_0x2a * 180 / st->field_0x28)) - t;
                }
                st->field_0x32--;
                if (st->field_0x32 < 0) {
                    _ac_rumble_param_set(rumbleType, &st->field_0x30, &st->field_0x32, &st->field_0x34);
                }
                {
                    f32 t = sinfd((f32)((st->field_0x32 + 1) * 180 / st->field_0x30)) * st->field_0x34;
                    st->pos.y += st->field_0x34 * sinfd((f32)(st->field_0x32 * 180 / st->field_0x30)) - t;
                }
                st->pos.x += (f32)work->padWork[0].mPadData[0].stickX / float_10_80421940;
                st->pos.y += (f32)work->padWork[0].mPadData[0].stickY / float_10_80421940;

                if (smooth) {
                    if (fabsf(st->pos.x - st->target.x) < double_3_802ff9d0) {
                        st->pos.x = st->target.x;
                    } else if (st->pos.x > st->target.x) {
                        st->pos.x -= float_3_8042193c;
                    } else {
                        st->pos.x += float_3_8042193c;
                    }
                    if (fabsf(st->pos.y - st->target.y) < double_3_802ff9d0) {
                        st->pos.y = st->target.y;
                    } else if (st->pos.y > st->target.y) {
                        st->pos.y -= float_3_8042193c;
                    } else {
                        st->pos.y += float_3_8042193c;
                    }
                }
                if (fabsf(distABf(st->pos.x, st->pos.y, st->target.x, st->target.y)) <= double_28_802ff9d8) {
                    st->field_0x20 = 1;
                } else {
                    st->field_0x20 = 0;
                }
                st->field_0x00--;
                if ((ac->field_0x4 & 1) &&
                    (ac->mAcUnit->mTokenFlags & kConfusedAction)) {
                    if (irand(100) >= 0) {
                        ac->result = 0;
                    } else {
                        ac->result = 2;
                        ac->mResultCount++;
                    }
                    ac->mAcState = 1001;
                } else if (st->field_0x00 < 0) {
                    if (st->field_0x20 != 0) {
                        ac->result = 2;
                        ac->mResultCount++;
                    } else {
                        ac->result = 0;
                    }
                    ac->mAcState = 1001;
                }
                break;
            case 1001:
                ac->mAcState = 1002;
                break;
            case 1002:
                break;
            case 1003:
                st->field_0x04 = 0x3C;
                ac->mAcState = 1004;
                if ((u32)st->field_0x38 != 0xFFFFFFFFu) {
                    psndSFXOff(st->field_0x38);
                }
                break;
            case 1004:
                st->field_0x04--;
                if (st->field_0x04 <= 0) {
                    ac->mAcState = 1005;
                }
                break;
            case 1005:
                ac->maincb = NULL;
                ac->dispcb = NULL;
                ac->resultcb = NULL;
                ac->deletecb = NULL;
                break;
        }
    } while (cont);

    return 1;
}

// dont_inline: the original keeps _ac_rumble_param_set as a real call (`bl _ac_rumble_param_set`);
// without it MWCC inlines the body at both call sites.
#pragma dont_inline on
void _ac_rumble_param_set(u32 type, s16* period, s16* timer, f32* amplitude) {
    *period = irand(0x78) + 0x78;
    *timer = *period - 1;
    *amplitude = float_25_80421920;
    if (irand(100) < 50) {
        *amplitude = float_neg1_80421924 * *amplitude;
    }

    switch (type) {
        case 0:
            *amplitude = float_0p6_80421928 * (float_0p25_8042192c * *amplitude);
            break;
        case 1:
            *amplitude = float_0p6_80421928 * (float_0p5_80421930 * *amplitude);
            break;
        case 2:
            *amplitude = float_0p6_80421928 * (float_0p8_80421934 * *amplitude);
            break;
        case 3:
            *amplitude = float_0p6_80421928 * *amplitude;
            break;
        case 4:
            *amplitude = float_0p6_80421928 * (float_1p5_80421938 * *amplitude);
            break;
        case 5:
            *amplitude = float_0p6_80421928 * (float_2_8042190c * *amplitude);
            break;
        case 6:
        default:
            *amplitude = float_0p6_80421928 * (float_3_8042193c * *amplitude);
            break;
    }
}
#pragma dont_inline reset

s32 battleAcResult_ShotTarget(BattleWork* work) {
    return work->actionCommands.result;
}

// The `BattleActions* ac` local (rather than reading fields through `work`) is what makes MWCC
// materialise `addi r31, r4, 0x1f20` for disp; reading `&work->actionCommands.dispParams` folded the
// offset into each access instead.
void battleAcDisp_ShotTarget(CameraId camId, void* param) {
    BattleActions* ac = &((BattleWork*)param)->actionCommands;
    BattleACDispParams* disp = &ac->dispParams;

    switch (ac->mAcState) {
        case 99:
        case 100:
        case 1000:
        case 1001:
        case 1002:
            disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, float_neg200_80421914, float_30_8042191c);
            actionCommandDisp(disp->iconX, disp->iconY);
            if (disp->counter > 0) {
                disp->counter--;
            }
            break;
        case 1003:
        case 1004:
            if (disp->counter >= 0x28) {
                disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, float_30_8042191c,
                                            float_neg200_80421914);
            } else {
                disp->iconX = float_30_8042191c;
            }
            actionCommandDisp(disp->iconX, disp->iconY);
            if (disp->counter < 0x3C) {
                disp->counter++;
            }
            break;
    }
}

void battleAcDelete_ShotTarget(BattleWork* work) {
    work->actionCommands.mAcState = 0x3EB;
}

// don't_inline: battleAcMain_ShotTarget calls this through the original `bl`.
#pragma dont_inline on
// The s8 casts on `field_0x10[1..2]` keep MWCC emitting `li r0, -0x10` (a plain byte store).
static void _ac_disp_init(BattleWork* work) {
    BattleACDispParams* disp = &work->actionCommands.dispParams;

    memset(disp, 0, 0x2C);
    disp->iconX = float_neg200_80421914;
    disp->iconY = float_0_80421918;
#ifdef MUST_MATCH
    // Match-only: the volatile accesses stop MWCC hoisting the dat_804218f8 load into r5 (and
    // reordering the counter store after it); the emitted instructions are byte-identical.
    *(volatile u32*)&disp->counter = 0x14;
    disp->field_0xC = *(volatile const u32*)&dat_804218f8;
#else
    disp->counter = 0x14;
    disp->field_0xC = dat_804218f8;
#endif
    disp->field_0x10[0] = 0;
    *(s8*)&disp->field_0x10[1] = -0x10;
    *(s8*)&disp->field_0x10[2] = -0x10;
    disp->field_0x10[3] = 0;
    disp->value = float_0_80421918;
}
#pragma dont_inline reset

// dont_inline: the original keeps actionCommandDisp as its own local function.
#pragma dont_inline on
// opt_common_subs off: with it on, the last screen2 update's 4.0f lands in f2 (original uses f1).
#pragma opt_common_subs off
// The by-value temporaries copied into the btlDispTex4 argument slots are what the original does:
// every call gets its own trans/scale/rot/color copies (like ac_monosiri_target's actionCommandDisp).
// `vecs` must be declared before `work`: MWCC allocates the two address temps by declaration order
// (the earlier local gets the higher register), so vecs->r4 and work->r3 as in the original.
static void actionCommandDisp(f32 x, f32 y) {
#ifdef MUST_MATCH
    Vec* vecs = (Vec*)vec3_802ff950;
#endif
    BattleWork* work = _battleWorkPointer;
    BattleActions* ac = &work->actionCommands;
    s32 state = ac->mAcState;
    ShotTargetWork* st = (ShotTargetWork*)&ac->field_0x2BC[0];
    Vec screen;
    Vec screen2;

    if (state < 1000) {
        if (state != 100) {
            return;
        }
    } else {
        if (state >= 1003) {
            return;
        }
    }
    {
        st->field_0x24 += float_2_8042190c;
        btlGetScreenPoint(&st->pos, &screen);
        {
            Vec trans;
            Vec scale;
            Vec rot;
            GXColor color;
            color = *(const GXColor*)&dat_804218fc;
            rot = RODATA_VEC(2, vec3_802ff950[2]);
            scale = RODATA_VEC(1, vec3_802ff950[1]);
            trans = screen;
            btlDispTex4(0x42, &trans, &scale, &rot, &color);
        }
        screen.x += float_4_80421910;
        screen.y -= float_4_80421910;
        screen.z -= float_2_8042190c;
        {
            Vec trans;
            Vec scale;
            Vec rot;
            GXColor color;
            color = *(const GXColor*)&dat_80421900;
            rot = RODATA_VEC(4, vec3_802ff950[4]);
            scale = RODATA_VEC(3, vec3_802ff950[3]);
            trans = screen;
            btlDispTex4(0x42, &trans, &scale, &rot, &color);
        }
        btlGetScreenPoint(&st->target, &screen2);
        {
            Vec trans;
            Vec scale;
            Vec rot;
            Vec tmp;
            GXColor color;
            color = *(const GXColor*)&dat_80421904;
            tmp = RODATA_VEC(6, vec3_802ff950[6]);
            tmp.z = st->field_0x24;
            rot = tmp;
            scale = RODATA_VEC(5, vec3_802ff950[5]);
            trans = screen2;
            btlDispTex4(0x41, &trans, &scale, &rot, &color);
        }
        screen2.x += float_4_80421910;
        screen2.y -= float_4_80421910;
        screen2.z -= float_2_8042190c;
        {
            Vec trans;
            Vec scale;
            Vec rot;
            Vec tmp;
            GXColor color;
            color = *(const GXColor*)&dat_80421908;
            tmp = RODATA_VEC(8, vec3_802ff950[8]);
            tmp.z = st->field_0x24;
            rot = tmp;
            scale = RODATA_VEC(7, vec3_802ff950[7]);
            trans = screen2;
            btlDispTex4(0x41, &trans, &scale, &rot, &color);
        }
    }
}
#pragma opt_common_subs reset
#pragma dont_inline reset

const f64 double_3_802ff9d0 = 3.0;
const f64 double_28_802ff9d8 = 28.0;
const u32 dat_804218f8 = 0xFFFFFFFF;
const u32 dat_804218fc = 0xFFFFFFFF;
const u32 dat_80421900 = 0x808080FF;
const u32 dat_80421904 = 0xFFFFFFFF;
const u32 dat_80421908 = 0x808080FF;
const f32 float_2_8042190c = 2.0f;
const f32 float_4_80421910 = 4.0f;
const f32 float_neg200_80421914 = -200.0f;
const f32 float_0_80421918 = 0.0f;
const f32 float_30_8042191c = 30.0f;
const f32 float_25_80421920 = 25.0f;
const f32 float_neg1_80421924 = -1.0f;
const f32 float_0p6_80421928 = 0.6f;
const f32 float_0p25_8042192c = 0.25f;
const f32 float_0p5_80421930 = 0.5f;
const f32 float_0p8_80421934 = 0.8f;
const f32 float_1p5_80421938 = 1.5f;
const f32 float_3_8042193c = 3.0f;
const f32 float_10_80421940 = 10.0f;

// 4 zero bytes that end the unit's .sdata2, the original's gap_09_80421944_sdata2.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata2") u32 gap_09_80421944_sdata2[1] = {0};
#pragma force_active reset
#pragma explicit_zero_data reset
