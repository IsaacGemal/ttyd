#include "battle/ac/ac_shot.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_disp.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "driver/icondrv.h"
#include "pmario_sound.h"
#include "system.h"
#include <math.h>
#include <string.h>

// .rodata constants used by _draw_target_mark and actionCommandDisp. The original emitted these as
// named symbols at 0xC offsets; they are one contiguous array here.
extern Vec vec3_802f92b8[6];

// SFX name the original unit emitted into .rodata.
const char str_802F9300[] = "SFX_AC_CURSOR_MOVE1";

// Double literals the original emitted into .rodata.
const f64 double_0p5_802f9318 = 0.5;
const f64 double_to_int_802f9320 = 4503601774854144.0;

// .sdata2 constants used by the whole unit; the original emits them as named symbols.
extern u32 dat_8041e4a8;
extern const GXColor dat_8041e4ac;
extern const GXColor dat_8041e4b0;
extern const GXColor dat_8041e4b4;
extern const f32 float_0_8041e4b8;
extern const f32 float_1_8041e4bc;
extern const f32 float_60_8041e4c0;
extern const f32 float_30_8041e4c4;
extern const f32 float_255_8041e4c8;
extern const f32 float_neg1_8041e4cc;
extern const f32 float_neg200_8041e4d0;
extern const f32 float_25_8041e4d4;
extern const f32 float_100_8041e4d8;
extern const f32 float_0p01_8041e4dc;
extern const f32 float_88_8041e4e0;
extern const f32 float_33_8041e4e4;
extern const f32 float_0p1_8041e4e8;
extern const f32 float_neg1000_8041e4ec;
extern const f32 float_70_8041e4f0;
extern const f32 float_8_8041e4f4;
extern const f32 float_50_8041e4f8;
extern const f32 float_0p5_8041e4fc;
extern const f32 float_900_8041e500;

// .sbss2 default colour the original emitted; referenced when building the icon colour.
extern u32 unk_80423078;

// _disp_target_mark's action-command work area at BattleWork+0x1F4C. No header declares it,
// so the fields are named from the original's uses. Offsets here are relative to 0x1F4C.
typedef struct TargetMarkWork {
    u8 field_0x0[0x24]; // 0x00
    f32 rotZ;           // 0x24 (BattleWork+0x1F70), marker angle
    u8 field_0x28[4];   // 0x28 (BattleWork+0x1F74), unused by this function
    f32 field_0x2C;     // 0x2C (BattleWork+0x1F78), passed as the f4 argument
    f32 x;              // 0x30 (BattleWork+0x1F7C)
    f32 y;              // 0x34 (BattleWork+0x1F80)
    f32 z;              // 0x38 (BattleWork+0x1F84)
} TargetMarkWork;

// The full ac_shot work area at BattleWork+0x1F4C, used by battleAcMain_Shot, actionCommandDisp and
// _disp_target_mark_afterimage. Offsets are relative to 0x1F4C.
typedef struct AcShotWork {
    s32 field_0x00;      // 0x00, hit timer
    s32 field_0x04;      // 0x04
    s32 index;           // 0x08, history write index
    f32 field_0x0C;      // 0x0C
    f32 field_0x10;      // 0x10
    f32 field_0x14;      // 0x14
    f32 field_0x18;      // 0x18
    f32 field_0x1C;      // 0x1C, gauge value
    f32 field_0x20;      // 0x20
    f32 rotZ;            // 0x24, marker angle
    f32 field_0x28;      // 0x28, angular velocity
    f32 field_0x2C;      // 0x2C, afterimage roll angle
    f32 x;               // 0x30
    f32 y;               // 0x34
    f32 z;               // 0x38
    s32 field_0x3C;      // 0x3C, end-of-AC timer
    f32 rotHistory[3];   // 0x40
    f32 angleHistory[3]; // 0x4C
    f32 valueHistory[3]; // 0x58
    u32 sfx;             // 0x64, psndSFXOn handle
} AcShotWork;

static void _ac_disp_init(BattleWork* work);
static void actionCommandDisp(f32 x, f32 y);

// Appends the current marker pose to the afterimage history and records it in outputParams.
static inline void _acShotAppendHistory(AcShotWork* aw, BattleWork* work) {
    work->actionCommands.outputParams[2] = (s32)aw->rotZ;
    work->actionCommands.outputParams[0] = 1;
    aw->rotHistory[aw->index] = aw->rotZ;
    aw->angleHistory[aw->index] = aw->field_0x2C;
    aw->valueHistory[aw->index] = float_70_8041e4f0;
    aw->index++;
    work->actionCommands.outputParams[3]++;
}

// NONMATCHING (99.65%): all instructions and their order match; only the aw->field_0x1C FPR
// colouring and a one-instruction prologue `addi`/`mr` spelling differ.
s32 battleAcMain_Shot(BattleWork* work) {
    const char* sfx = str_802F9300;
    BattleActions* ac = &work->actionCommands;
    AcShotWork* aw = (AcShotWork*)&ac->field_0x2BC[0];
    BattleUnit* unit;
    s32 i;
    s32 cont;

    do {
        cont = 0;
        switch (work->actionCommands.mAcState) {
            case 0:
                _ac_disp_init(work);
                work->actionCommands.result = 1;
                aw->field_0x0C = (f32)*(s32*)&work->actionCommands.field_34[0x0C];
                aw->field_0x10 = (f32)*(s32*)&work->actionCommands.field_34[0x10];
                aw->field_0x14 = (f32)*(s32*)&work->actionCommands.field_34[0x14];
                aw->field_0x18 = (f32)*(s32*)&work->actionCommands.field_34[0x18];
                aw->field_0x20 = float_100_8041e4d8 * aw->field_0x0C / aw->field_0x10;
                aw->field_0x1C = aw->field_0x20;
                aw->rotZ = aw->field_0x14 + (f32)irand((s32)(aw->field_0x18 - aw->field_0x14));
                aw->field_0x28 = float_0p1_8041e4e8 * (f32)*(s32*)&work->actionCommands.field_34[0x1C];
                aw->index = 0;
                work->actionCommands.field_0x8 = 0x1E;
                unit = BattleGetUnitPtr(work, *(s32*)&work->actionCommands.field_34[0x04]);
                BtlUnit_GetPos(unit, &aw->x, &aw->y, &aw->z);
                aw->y += 0.5 * ((f32)BtlUnit_GetHeight(unit) * unit->mSizeMultiplier);
                work->actionCommands.outputParams[0] = 0;
                work->actionCommands.outputParams[3] = 0;
                for (i = 0; i < 3; i++) {
                    aw->rotHistory[i] = float_neg1000_8041e4ec;
                    aw->angleHistory[i] = float_0_8041e4b8;
                }
                aw->sfx = -1;
                work->actionCommands.mAcState = 0x63;
                break;
            case 0x63:
                break;
            case 0x64:
                aw->field_0x00 = 0x168;
                work->actionCommands.mAcState = 0x3E8;
                aw->sfx = psndSFXOn(sfx);
                // fallthrough
            case 0x3E8:
                if ((work->actionCommands.field_0x4 & 1) &&
                    (work->actionCommands.mAcUnit->mTokenFlags & kConfusedAction)) {
                    if (work->actionCommands.field_0x4 & 2) {
                        work->actionCommands.field_0x8--;
                        if (work->actionCommands.field_0x8 >= 0) {
                            break;
                        }
                    }
                    {
                        f32 range = aw->field_0x10 - aw->field_0x0C;
                        s32 r;
                        if (range != float_0_8041e4b8) {
                            r = (s32)(aw->field_0x0C + (f32)irand((s32)range));
                        } else {
                            r = (s32)aw->field_0x0C;
                        }
                        work->actionCommands.outputParams[1] =
                            (s32)(aw->field_0x10 * (f32)r / float_100_8041e4d8);
                    }
                    if ((f32)work->actionCommands.outputParams[1] < aw->field_0x0C) {
                        work->actionCommands.outputParams[1] = (s32)aw->field_0x0C;
                    }
                    _acShotAppendHistory(aw, work);
                    if (aw->index >= *(s32*)&work->actionCommands.field_34[0x08]) {
                        work->actionCommands.result = 2;
                        return 0;
                    }
                    if (work->actionCommands.field_0x4 & 2) {
                        work->actionCommands.field_0x8 = 0x1E;
                    }
                    break;
                }
                if (!(BattlePadCheckTrigger(0x100) != 0 || work->actionCommands.outputParams[0] != 0 ||
                      *(s16*)&work->actionCommands.field_78[0x10] > 0)) {
                    if (--aw->field_0x00 <= 0) {
                        work->actionCommands.outputParams[1] =
                            (s32)(aw->field_0x10 * aw->field_0x1C / float_100_8041e4d8);
                        if ((f32)work->actionCommands.outputParams[1] < aw->field_0x0C) {
                            work->actionCommands.outputParams[1] = (s32)aw->field_0x0C;
                        }
                        _acShotAppendHistory(aw, work);
                        if (aw->index >= *(s32*)&work->actionCommands.field_34[0x08]) {
                            work->actionCommands.result = 0;
                            work->actionCommands.mAcState = 0x3EA;
                        } else {
                            aw->field_0x00 = 0x168;
                        }
                        cont = 1;
                    }
                } else {
                    if (work->actionCommands.outputParams[0] == 0) {
                        work->actionCommands.mAcState = 0x3E9;
                    }
                }
                if (aw->field_0x28 != float_0_8041e4b8) {
                    aw->rotZ += aw->field_0x28;
                    if (aw->field_0x28 > float_0_8041e4b8) {
                        if (aw->rotZ >= aw->field_0x18) {
                            aw->rotZ = aw->field_0x18;
                            aw->field_0x28 = aw->field_0x28 * float_neg1_8041e4cc;
                        }
                    } else {
                        if (aw->rotZ <= aw->field_0x14) {
                            aw->rotZ = aw->field_0x14;
                            aw->field_0x28 = aw->field_0x28 * float_neg1_8041e4cc;
                        }
                    }
                }
                aw->field_0x2C += float_8_8041e4f4;
                break;
            case 0x3E9:
                if (*(s16*)&work->actionCommands.field_78[0x10] > 0) {
                    work->actionCommands.outputParams[1] =
                        (s32)(float_50_8041e4f8 * aw->field_0x10 / float_100_8041e4d8);
                    if ((f32)work->actionCommands.outputParams[1] < aw->field_0x0C) {
                        work->actionCommands.outputParams[1] = (s32)aw->field_0x0C;
                    }
                    _acShotAppendHistory(aw, work);
                    if (aw->index >= *(s32*)&work->actionCommands.field_34[0x08]) {
                        (*(s16*)&work->actionCommands.field_78[0x10])--;
                        work->actionCommands.result = 2;
                        work->actionCommands.mAcState = 0x3EA;
                    } else {
                        work->actionCommands.mAcState = 0x3E8;
                    }
                    cont = 1;
                }
                {
                    f32 g = aw->field_0x1C;
                    g = g + float_1_8041e4bc;
                    aw->field_0x1C = g;
                    if (aw->field_0x1C > float_100_8041e4d8) {
                        aw->field_0x1C = float_100_8041e4d8;
                    }
                }
                if (*(s32*)&work->actionCommands.field_34[0] & 1) {
                    work->actionCommands.outputParams[1] =
                        (s32)((aw->field_0x0C + aw->field_0x10) * float_0p5_8041e4fc);
                    _acShotAppendHistory(aw, work);
                    if (aw->index >= *(s32*)&work->actionCommands.field_34[0x08]) {
                        work->actionCommands.mAcState = 0x3EA;
                        work->actionCommands.result = 2;
                    } else {
                        work->actionCommands.mAcState = 0x3E8;
                    }
                } else {
                    if ((s32)BattlePadCheckNow(0x100) == 0) {
                        work->actionCommands.outputParams[1] =
                            (s32)(aw->field_0x10 * aw->field_0x1C / float_100_8041e4d8);
                        if ((f32)work->actionCommands.outputParams[1] < aw->field_0x0C) {
                            work->actionCommands.outputParams[1] = (s32)aw->field_0x0C;
                        }
                        _acShotAppendHistory(aw, work);
                        if (aw->index >= *(s32*)&work->actionCommands.field_34[0x08]) {
                            work->actionCommands.result = 2;
                            work->actionCommands.mAcState = 0x3EA;
                        } else {
                            work->actionCommands.mAcState = 0x3E8;
                        }
                    }
                }
                break;
            case 0x3EA:
                work->actionCommands.mAcState = 0x3EB;
                break;
            case 0x3EC:
                aw->field_0x3C = 0x3C;
                work->actionCommands.mAcState = 0x3ED;
                if (aw->sfx != -1) {
                    psndSFXOff(aw->sfx);
                }
                break;
            case 0x3ED:
                aw->field_0x3C--;
                if (aw->field_0x3C <= 0) {
                    work->actionCommands.mAcState = 0x3EE;
                }
                break;
            case 0x3EE:
                work->actionCommands.maincb = NULL;
                work->actionCommands.dispcb = NULL;
                work->actionCommands.resultcb = NULL;
                work->actionCommands.deletecb = NULL;
                break;
            default:
                break;
        }
    } while (cont);

    if (work->actionCommands.mAcState == 0x3E8 || work->actionCommands.mAcState == 0x3E9) {
        dispEntry(8, float_900_8041e500, 1, _disp_target_mark, work);
    }
    dispEntry(8, float_900_8041e500, 1, _disp_target_mark_afterimage, work);
    return 1;
}

s32 battleAcResult_Shot(BattleWork* work) {
    return work->actionCommands.result;
}

void battleAcDisp_Shot(CameraId camId, void* param) {
    BattleActions* ac = &((BattleWork*)param)->actionCommands;
    s32 state = ac->mAcState;
    BattleACDispParams* disp = &ac->dispParams;

    switch (state) {
        case 0x63:
        case 0x64:
        case 0x3E8:
        case 0x3E9:
        case 0x3EA:
        case 0x3EB:
            disp->iconX =
                intplGetValue(4, 0x14 - disp->counter, 0x14, float_neg200_8041e4d0, float_30_8041e4c4);
            actionCommandDisp(disp->iconX, disp->iconY);
            if (disp->counter > 0) {
                disp->counter--;
            }
            break;
        case 0x3EC:
        case 0x3ED:
            if (disp->counter >= 0x28) {
                disp->iconX =
                    intplGetValue(4, disp->counter - 0x28, 0x14, float_30_8041e4c4, float_neg200_8041e4d0);
            } else {
                disp->iconX = float_30_8041e4c4;
            }
            actionCommandDisp(disp->iconX, disp->iconY);
            if (disp->counter < 0x3C) {
                disp->counter++;
            }
            break;
    }
}

void battleAcDelete_Shot(BattleWork* work) {
    work->actionCommands.mAcState = 0x3EC;
}

static void _ac_disp_init(BattleWork* work) {
    BattleACDispParams* disp = &work->actionCommands.dispParams;

    memset(disp, 0, 0x2C);
    disp->iconX = float_neg200_8041e4d0;
    disp->iconY = float_0_8041e4b8;
    disp->counter = 0x14;
    disp->field_0xC = dat_8041e4a8;
    disp->field_0x10[0] = 0;
    *(s8*)&disp->field_0x10[1] = -0x10;
    *(s8*)&disp->field_0x10[2] = -0x10;
    disp->field_0x10[3] = 0;
    disp->value = float_0_8041e4b8;
}

// The original builds the icon colour in a local and copies it to the by-value argument slot.
static inline GXColor _makeIconColor(BattleACDispParams* disp) {
    GXColor color = *(GXColor*)&unk_80423078;
    color.r = ((u8*)disp)[0xC];
    color.g = ((u8*)disp)[0xD];
    color.b = ((u8*)disp)[0xE];
    color.a = ((u8*)disp)[0xF];
    return color;
}

// optimize_for_size: actionCommandDisp's prologue saves r30-r31 with `stmw`.
#pragma optimize_for_size on
static void actionCommandDisp(f32 x, f32 y) {
    BattleWork* work = _battleWorkPointer;
    BattleActions* ac = &work->actionCommands;
    AcShotWork* aw = (AcShotWork*)&ac->field_0x2BC[0];
    BattleACDispParams* disp = &ac->dispParams;
    Vec v;
    f32 target;
    f32 baseX;

    if (*(s32*)&work->actionCommands.field_34[0] & 1) {
        return;
    }

    baseX = float_neg200_8041e4d0 + x;
    v = vec3_802f92b8[0];
    v.x = baseX;
    v.y = float_25_8041e4d4 + y;
    iconDispGx(&v, 0x10, 0x8C, float_1_8041e4bc);

    target = aw->field_0x1C / float_100_8041e4d8;
    if (disp->value <= target) {
        disp->value += float_0p01_8041e4dc;
        if (disp->value > target) {
            disp->value = target;
        }
    } else {
        disp->value -= float_0p01_8041e4dc;
        if (disp->value < target) {
            disp->value = target;
        }
    }

    if (disp->value >= float_1_8041e4bc) {
        u8* col = (u8*)&disp->field_0xC;
        s8* dir = (s8*)disp->field_0x10;

        col[1] += dir[1];
        col[2] += dir[2];
        if (col[1] <= 0x80 && (s8)dir[1] < 0) {
            dir[2] = 0x10;
            dir[1] = 0x10;
        }
        if (col[1] >= 0xF5 && (s8)dir[1] > 0) {
            dir[2] = -0x10;
            dir[1] = -0x10;
        }
    }

    {
        Mtx mtx;
        Mtx trans;
        Mtx scale;

        PSMTXTrans(trans,
                   baseX - float_88_8041e4e0 * (float_1_8041e4bc - disp->value),
                   float_33_8041e4e4 + y, float_0_8041e4b8);
        PSMTXScale(scale, disp->value, float_1_8041e4bc, float_1_8041e4bc);
        PSMTXConcat(trans, scale, mtx);
        iconDispGxCol(mtx, 0x10, 0x8D, _makeIconColor(disp));
    }
}
#pragma optimize_for_size reset

// dont_inline: the original keeps _draw_target_mark as its own function and calls it with `bl` from
// _disp_target_mark and _disp_target_mark_afterimage.
#pragma dont_inline on
// The original passes the vectors and colour by value to an inlined wrapper, which copies them
// into the btlDispTex4 argument slots.
static inline void _drawTargetMarkQuad(s32 id, Vec trans, Vec scale, Vec rot, GXColor color) {
    btlDispTex4(id, &trans, &scale, &rot, &color);
}

// NONMATCHING (80.31%): the logic and the by-value wrapper call match; retail keeps the vec3 base in
// r31 and the colour pointer in r30 and gives every wrapper parameter a fresh stack slot, while the
// compiler here keeps the colour pointer in r31, rematerialises the vec base, and reuses slots.
void _draw_target_mark(GXColor* color, f32 x, f32 y, f32 z, f32 angle) {
    Vec trans;
    Vec rot;
    Vec rot2;

    trans = vec3_802f92b8[1];
    trans.x = x;
    trans.y = y;
    trans.z = z;

    rot = vec3_802f92b8[3];
    rot.z = reviseAngle(-angle);
    _drawTargetMarkQuad(0x42, trans, vec3_802f92b8[2], rot, *color);

    rot2 = vec3_802f92b8[5];
    rot2.z = reviseAngle(angle);
    _drawTargetMarkQuad(0x41, trans, vec3_802f92b8[4], rot2, *color);
}
#pragma dont_inline reset

void _disp_target_mark(CameraId cameraId, void* param) {
    BattleWork* work = param;
    TargetMarkWork* tm = (TargetMarkWork*)((u8*)work + 0x1F4C);
    Vec pos;
    Vec screen;
    GXColor color;

    pos.x = float_60_8041e4c0 * sinfd(tm->rotZ) + tm->x;
    pos.y = float_60_8041e4c0 * cosfd(tm->rotZ) + tm->y;
    pos.z = tm->z;
    btlGetScreenPoint(&pos, &screen);
    color = dat_8041e4ac;
    _draw_target_mark(&color, screen.x, screen.y, screen.z, tm->field_0x2C);
}

// The original builds the afterimage colour after intplGetValue and copies it to the by-value slot.
static inline GXColor _makeAfterimageColor(f32 value) {
    s32 alpha = (s32)intplGetValue(0, (s32)(float_30_8041e4c4 - value), 0x1E, float_255_8041e4c8,
                                   float_0_8041e4b8);
    GXColor color = dat_8041e4b4;
    color.a = (u8)alpha;
    return color;
}

// optimize_for_size: _disp_target_mark_afterimage's prologue saves r28-r31 with `stmw`.
#pragma optimize_for_size on
void _disp_target_mark_afterimage(CameraId cameraId, void* param) {
    BattleWork* work = param;
    BattleActions* ac = &work->actionCommands;
    AcShotWork* aw = (AcShotWork*)&ac->field_0x2BC[0];
    s32 i;

    for (i = 0; i < 3; i++) {
        Vec pos;
        Vec screen;
        f32 value;

        if (aw->rotHistory[i] >= float_0_8041e4b8) {
            if (aw->valueHistory[i] > float_0_8041e4b8) {
                aw->valueHistory[i] -= float_1_8041e4bc;
                pos.x = float_60_8041e4c0 * sinfd(aw->rotHistory[i]) + aw->x;
                pos.y = float_60_8041e4c0 * cosfd(aw->rotHistory[i]) + aw->y;
                pos.z = aw->z;
                btlGetScreenPoint(&pos, &screen);
                value = aw->valueHistory[i];
                if (value >= float_30_8041e4c4) {
                    GXColor color = dat_8041e4b0;
                    _draw_target_mark(&color, screen.x, screen.y, screen.z, aw->angleHistory[i]);
                } else {
                    GXColor color = _makeAfterimageColor(value);
                    _draw_target_mark(&color, screen.x, screen.y, screen.z, aw->angleHistory[i]);
                }
                if (aw->valueHistory[i] <= float_0_8041e4b8) {
                    aw->rotHistory[i] = float_neg1_8041e4cc;
                }
            }
        }
    }
}
#pragma optimize_for_size reset

Vec vec3_802f92b8[6] = {
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.5f, 0.5f, 1.0f},
    {0.0f, 0.0f, 0.0f},
    {0.5f, 0.5f, 1.0f},
    {0.0f, 0.0f, 0.0f},
};
const f32 float_neg200_8041e4d0 = -200.0f;
const f32 float_0_8041e4b8 = 0.0f;
u32 dat_8041e4a8 = 0xFFFFFFFF;
const f32 float_60_8041e4c0 = 60.0f;
const GXColor dat_8041e4ac = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_8041e4b0 = {0x90, 0x80, 0xA0, 0xFF};
const GXColor dat_8041e4b4 = {0x90, 0x80, 0xA0, 0x00};
const f32 float_1_8041e4bc = 1.0f;
const f32 float_30_8041e4c4 = 30.0f;
const f32 float_255_8041e4c8 = 255.0f;
const f32 float_neg1_8041e4cc = -1.0f;
const f32 float_25_8041e4d4 = 25.0f;
const f32 float_100_8041e4d8 = 100.0f;
const f32 float_0p01_8041e4dc = 0.01f;
const f32 float_88_8041e4e0 = 88.0f;
const f32 float_33_8041e4e4 = 33.0f;
const f32 float_0p1_8041e4e8 = 0.1f;
const f32 float_neg1000_8041e4ec = -1000.0f;
const f32 float_70_8041e4f0 = 70.0f;
const f32 float_8_8041e4f4 = 8.0f;
const f32 float_50_8041e4f8 = 50.0f;
const f32 float_0p5_8041e4fc = 0.5f;
const f32 float_900_8041e500 = 900.0f;
u32 unk_80423078;