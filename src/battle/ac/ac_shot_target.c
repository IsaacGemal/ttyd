#include "battle/ac/ac_shot_target.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_disp.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "pmario_sound.h"
#include "system.h"
#include <math.h>
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

#pragma dont_inline on
// NONMATCHING (64.05%): the original materialises st (work+0x1F4C) and the vec3 base in registers;
// MWCC folds st into the work base and reloads the vec base per call (frame 0x90 vs 0xF0).
static void actionCommandDisp(f32 x, f32 y) {
    BattleWork* work = _battleWorkPointer;
    ShotTargetWork* st = (ShotTargetWork*)((u8*)work + 0x1F4C);
    Vec screen;
    Vec scale;
    Vec rot;
    GXColor color;
    s32 state = work->actionCommands.mAcState;

    if ((state >= 1000 && state < 1003) || state == 100) {
        st->field_0x24 += float_2_8042190c;
        btlGetScreenPoint(&st->pos, &screen);
        {
            Vec s = vec3_802ff950[1];
            Vec r = vec3_802ff950[2];
            GXColor c = *(GXColor*)&dat_804218fc;
            btlDispTex4(0x42, &screen, &s, &r, &c);
        }
        screen.x += float_4_80421910;
        screen.y -= float_4_80421910;
        screen.z -= float_2_8042190c;
        {
            Vec s = vec3_802ff950[3];
            Vec r = vec3_802ff950[4];
            GXColor c = *(GXColor*)&dat_80421900;
            btlDispTex4(0x42, &screen, &s, &r, &c);
        }
        btlGetScreenPoint(&st->target, &screen);
        {
            Vec s = vec3_802ff950[5];
            Vec r = vec3_802ff950[6];
            GXColor c = *(GXColor*)&dat_80421904;
            r.z = st->field_0x24;
            btlDispTex4(0x41, &screen, &s, &r, &c);
        }
        screen.x += float_4_80421910;
        screen.y -= float_4_80421910;
        screen.z -= float_2_8042190c;
        {
            Vec s = vec3_802ff950[7];
            Vec r = vec3_802ff950[8];
            GXColor c = *(GXColor*)&dat_80421908;
            r.z = st->field_0x24;
            btlDispTex4(0x41, &screen, &s, &r, &c);
        }
    }
}
#pragma dont_inline reset

// don't_inline: battleAcMain_ShotTarget calls this through the original `bl`.
#pragma dont_inline on
// NONMATCHING (92.86%): original stores counter (r4) before the dat_804218f8 load (also r4) and loads
// dat after `stfs iconY`; MWCC hoists the load to r5. The s8 casts keep `li r0, -0x10`.
static void _ac_disp_init(BattleWork* work) {
    BattleACDispParams* disp = &work->actionCommands.dispParams;

    memset(disp, 0, 0x2C);
    disp->iconX = float_neg200_80421914;
    disp->iconY = float_0_80421918;
    disp->field_0xC = dat_804218f8;
    disp->counter = 0x14;
    disp->field_0x10[0] = 0;
    *(s8*)&disp->field_0x10[1] = -0x10;
    *(s8*)&disp->field_0x10[2] = -0x10;
    disp->field_0x10[3] = 0;
    disp->value = float_0_80421918;
}
#pragma dont_inline reset

void battleAcDelete_ShotTarget(BattleWork* work) {
    work->actionCommands.mAcState = 0x3EB;
}

// NONMATCHING (98.78%): only the disp-parameter pointer differs. The original materialises
// `disp = param + 0x1F20` once and uses `0x20(r31)`; ours keeps `param` in r31 and folds the offset
// into `0x1f40(r31)` (same known issue as battleAcDisp_ButtonDown/TimingWide).
void battleAcDisp_ShotTarget(CameraId camId, void* param) {
    BattleWork* work = (BattleWork*)param;
    BattleACDispParams* disp = &work->actionCommands.dispParams;

    switch (work->actionCommands.mAcState) {
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

s32 battleAcResult_ShotTarget(BattleWork* work) {
    return work->actionCommands.result;
}

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

// NONMATCHING (8.29%): state machine and logic match, but MWCC folds the work-area pointer
// (work+0x1F4C) into the work base and uses different callee-saved registers (work=r27 vs r23), so
// nearly every instruction differs in its register field.
s32 battleAcMain_ShotTarget(BattleWork* work) {
    ShotTargetWork* st = (ShotTargetWork*)((u8*)work + 0x1F4C);
    const char* sfx = str_802FF9BC;
    s32 smooth = 0;
    s32 rumbleType;
    s32 cont;
    f32 hx;
    f32 hy;
    f32 hz;
    BattleUnit* unit;
    BattleUnitPart* part;

    if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand != 0) {
        smooth = 1;
    }
    rumbleType = *(s32*)&work->actionCommands.field_34[0x10];

    do {
        cont = 0;
        switch (work->actionCommands.mAcState) {
            case 0:
                _ac_disp_init(work);
                work->actionCommands.result = 1;
                unit = BattleGetUnitPtr(work, *(s32*)&work->actionCommands.field_34[0x04]);
                part = BtlUnit_GetPartsPtr(unit, *(s32*)&work->actionCommands.field_34[0x08]);
                BtlUnit_GetHitPos(unit, part, &hx, &hy, &hz);
                st->target.x = hx;
                st->target.y = hy;
                st->target.z = hz;
                st->field_0x20 = 0;
                st->pos = vec3_802ff950[0];
                st->field_0x24 = float_0_80421918;
                _ac_rumble_param_set(rumbleType, &st->field_0x28, &st->field_0x2a, &st->field_0x2C);
                _ac_rumble_param_set(rumbleType, &st->field_0x30, &st->field_0x32, &st->field_0x34);
                st->field_0x38 = -1;
                work->actionCommands.mAcState = 0x63;
                break;
            case 99:
                break;
            case 100:
                st->field_0x00 = *(s32*)&work->actionCommands.field_34[0x0C];
                work->actionCommands.mAcState = 1000;
                st->field_0x38 = psndSFXOn(sfx);
                // fallthrough
            case 1000:
                if (*(s16*)&work->actionCommands.field_78[0x10] > 0) {
                    (*(s16*)&work->actionCommands.field_78[0x10])--;
                    work->actionCommands.result = 2;
                    work->actionCommands.mResultCount++;
                    work->actionCommands.mAcState = 1001;
                    cont = 1;
                    break;
                }
                st->field_0x2a--;
                if (st->field_0x2a < 0) {
                    _ac_rumble_param_set(rumbleType, &st->field_0x28, &st->field_0x2a, &st->field_0x2C);
                }
                {
                    f32 t = sinfd((f32)((st->field_0x2a + 1) * 180 / st->field_0x28)) * st->field_0x2C;
                    f32 s = sinfd((f32)(st->field_0x2a * 180 / st->field_0x28));
                    st->pos.x += st->field_0x2C * s - t;
                }
                st->field_0x32--;
                if (st->field_0x32 < 0) {
                    _ac_rumble_param_set(rumbleType, &st->field_0x30, &st->field_0x32, &st->field_0x34);
                }
                {
                    f32 t = sinfd((f32)((st->field_0x32 + 1) * 180 / st->field_0x30)) * st->field_0x34;
                    f32 s = sinfd((f32)(st->field_0x32 * 180 / st->field_0x30));
                    st->pos.y += st->field_0x34 * s - t;
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
                if ((work->actionCommands.field_0x4 & 1) &&
                    (work->actionCommands.mAcUnit->mTokenFlags & kConfusedAction)) {
                    if (irand(100) >= 0) {
                        work->actionCommands.result = 0;
                    } else {
                        work->actionCommands.result = 2;
                        work->actionCommands.mResultCount++;
                    }
                    work->actionCommands.mAcState = 1001;
                } else if (st->field_0x00 < 0) {
                    if (st->field_0x20 != 0) {
                        work->actionCommands.result = 2;
                        work->actionCommands.mResultCount++;
                    } else {
                        work->actionCommands.result = 0;
                    }
                    work->actionCommands.mAcState = 1001;
                }
                break;
            case 1001:
                work->actionCommands.mAcState = 1002;
                break;
            case 1002:
                break;
            case 1003:
                st->field_0x04 = 0x3C;
                work->actionCommands.mAcState = 1004;
                if (st->field_0x38 != -1) {
                    psndSFXOff(st->field_0x38);
                }
                break;
            case 1004:
                st->field_0x04--;
                if (st->field_0x04 <= 0) {
                    work->actionCommands.mAcState = 1005;
                }
                break;
            case 1005:
                work->actionCommands.maincb = NULL;
                work->actionCommands.dispcb = NULL;
                work->actionCommands.resultcb = NULL;
                work->actionCommands.deletecb = NULL;
                break;
        }
    } while (cont);

    return 1;
}

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
