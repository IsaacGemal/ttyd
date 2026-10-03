#include "battle/ac/ac_shot.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_disp.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "driver/icondrv.h"
#include "pmario_sound.h"
#include "system.h"
#include <string.h>

// .rodata constants used by _draw_target_mark and actionCommandDisp. The original emitted these as
// six named symbols 0xC apart; one contiguous array here produces the same bytes and lets the
// code index from the single base pointer as retail does. It is non-const (as the text needs) but
// __declspec(section) forces it into .rodata where the original keeps it.
extern Vec vec3_802f92b8[6];

// SFX name the original unit emitted into .rodata (defined at the end, after the vectors).

// .sdata2 constants used by the whole unit; the original emits them as named symbols.
extern u32 dat_8041e4a8;
extern const GXColor dat_8041e4ac;
extern const GXColor dat_8041e4b0;
extern const GXColor dat_8041e4b4;

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
    aw->valueHistory[aw->index] = 70.0f;
    aw->index++;
    work->actionCommands.outputParams[3]++;
}

// optimize_for_size: the original keeps the 3-iteration history-init loop as a `bdnz` loop.
#pragma optimize_for_size on
// The unit's float constants are written as literals so MWCC pools them in the original's .sdata2
// order. A few sites load a constant into a local first (`f32 zero`, `f32 neg`, `f32 half`, the
// `f32 r = g + one` temp) because that is what reproduces retail's FPR colouring.
s32 battleAcMain_Shot(BattleWork* work) {
#ifdef MUST_MATCH
    // Match-only comma: forces MWCC to materialise the SFX name into r31 after `stmw`
    // (`addi r31,r4,@l`) instead of the shorter-but-different `addi r0,r4,@l; ...; mr r31,r0`.
    const char* sfx = ("SFX_AC_CURSOR_MOVE1", "SFX_AC_CURSOR_MOVE1");
#else
    const char* sfx = "SFX_AC_CURSOR_MOVE1";
#endif
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
                aw->field_0x20 = 100.0f * aw->field_0x0C / aw->field_0x10;
                aw->field_0x1C = aw->field_0x20;
                aw->rotZ = aw->field_0x14 + (f32)irand((s32)(aw->field_0x18 - aw->field_0x14));
                aw->field_0x28 = 0.1f * (f32)*(s32*)&work->actionCommands.field_34[0x1C];
                aw->index = 0;
                work->actionCommands.field_0x8 = 0x1E;
                unit = BattleGetUnitPtr(work, *(s32*)&work->actionCommands.field_34[0x04]);
                BtlUnit_GetPos(unit, &aw->x, &aw->y, &aw->z);
                aw->y += 0.5 * ((f32)BtlUnit_GetHeight(unit) * unit->mSizeMultiplier);
                work->actionCommands.outputParams[0] = 0;
                work->actionCommands.outputParams[3] = 0;
                for (i = 0; i < 3; i++) {
                    aw->rotHistory[i] = -1000.0f;
                    aw->angleHistory[i] = 0.0f;
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
                        f32 zero = 0.0f;
                        if (range != zero) {
                            r = (s32)(aw->field_0x0C + (f32)irand((s32)range));
                        } else {
                            r = (s32)aw->field_0x0C;
                        }
                        work->actionCommands.outputParams[1] =
                            (s32)(aw->field_0x10 * (f32)r / 100.0f);
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
                            (s32)(aw->field_0x10 * aw->field_0x1C / 100.0f);
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
                {
                    f32 zero = 0.0f;
                    f32 neg = -1.0f;
                    if (aw->field_0x28 != zero) {
                        aw->rotZ += aw->field_0x28;
                        if (aw->field_0x28 > zero) {
                            if (aw->rotZ >= aw->field_0x18) {
                                aw->rotZ = aw->field_0x18;
                                aw->field_0x28 = aw->field_0x28 * neg;
                            }
                        } else {
                            if (aw->rotZ <= aw->field_0x14) {
                                aw->rotZ = aw->field_0x14;
                                aw->field_0x28 = aw->field_0x28 * neg;
                            }
                        }
                    }
                }
                aw->field_0x2C += 8.0f;
                break;
            case 0x3E9:
                if (*(s16*)&work->actionCommands.field_78[0x10] > 0) {
                    work->actionCommands.outputParams[1] =
                        (s32)(50.0f * aw->field_0x10 / 100.0f);
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
                    f32 one = 1.0f;
                    aw->field_0x1C = g + one;
                    if (aw->field_0x1C > 100.0f) {
                        aw->field_0x1C = 100.0f;
                    }
                }
                if (*(s32*)&work->actionCommands.field_34[0] & 1) {
                    {
                        f32 sum = aw->field_0x0C + aw->field_0x10;
                        f32 half = 0.5f;
                        work->actionCommands.outputParams[1] = (s32)(sum * half);
                    }
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
                            (s32)(aw->field_0x10 * aw->field_0x1C / 100.0f);
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
        dispEntry(8, 900.0f, 1, _disp_target_mark, work);
    }
    dispEntry(8, 900.0f, 1, _disp_target_mark_afterimage, work);
    return 1;
}
#pragma optimize_for_size reset

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
                intplGetValue(4, 0x14 - disp->counter, 0x14, -200.0f, 30.0f);
            actionCommandDisp(disp->iconX, disp->iconY);
            if (disp->counter > 0) {
                disp->counter--;
            }
            break;
        case 0x3EC:
        case 0x3ED:
            if (disp->counter >= 0x28) {
                disp->iconX =
                    intplGetValue(4, disp->counter - 0x28, 0x14, 30.0f, -200.0f);
            } else {
                disp->iconX = 30.0f;
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

// dont_inline: the original keeps _ac_disp_init as its own local function (battleAcMain_Shot
// reaches it through `bl`), so it must not be inlined into its single caller.
#pragma dont_inline on
static void _ac_disp_init(BattleWork* work) {
    BattleACDispParams* disp = &work->actionCommands.dispParams;

    memset(disp, 0, 0x2C);
    disp->iconX = -200.0f;
    disp->iconY = 0.0f;
    disp->counter = 0x14;
    disp->field_0xC = dat_8041e4a8;
    disp->field_0x10[0] = 0;
    *(s8*)&disp->field_0x10[1] = -0x10;
    *(s8*)&disp->field_0x10[2] = -0x10;
    disp->field_0x10[3] = 0;
    disp->value = 0.0f;
}
#pragma dont_inline reset

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

    baseX = -200.0f + x;
    v = vec3_802f92b8[0];
    v.x = baseX;
    v.y = 25.0f + y;
    iconDispGx(&v, 0x10, 0x8C, 1.0f);

    target = aw->field_0x1C / 100.0f;
    if (disp->value <= target) {
        disp->value += 0.01f;
        if (disp->value > target) {
            disp->value = target;
        }
    } else {
        disp->value -= 0.01f;
        if (disp->value < target) {
            disp->value = target;
        }
    }

    if (disp->value >= 1.0f) {
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
                   baseX - 88.0f * (1.0f - disp->value),
                   33.0f + y, 0.0f);
        PSMTXScale(scale, disp->value, 1.0f, 1.0f);
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

// Builds a rotation vector from a base vector and an angle; returning a Vec by value is what the
// original does, giving reviseAngle's result its own stack slot before the quad copies it.
static inline Vec _drawTargetMarkRot(Vec v, f32 angle) {
    v.z = reviseAngle(angle);
    return v;
}

#ifdef MUST_MATCH
#define DRW_VEC(i, name) (vecs[i])
#else
#define DRW_VEC(i, name) (name)
#endif

// 100%: the first `*color` load is only hoisted next to the prologue `lis` (as the original has
// it) when MWCC knows the colour pointer cannot alias the stack/rodata it reads; declaring the
// parameter `__restrict` supplies that and the emitted code is byte-identical.
// Match-only qualifier: the natural form leaves the pointer unqualified.
#ifdef MUST_MATCH
#define MARK_COLOR_RESTRICT __restrict
#else
#define MARK_COLOR_RESTRICT
#endif
void _draw_target_mark(GXColor* MARK_COLOR_RESTRICT color, f32 x, f32 y, f32 z, f32 angle) {
#ifdef MUST_MATCH
    Vec* vecs = (Vec*)vec3_802f92b8;
#endif
    Vec trans;

    trans = DRW_VEC(1, vec3_802f92b8[1]);
    trans.x = x;
    trans.y = y;
    trans.z = z;

    _drawTargetMarkQuad(0x42, trans, DRW_VEC(2, vec3_802f92b8[2]),
                        _drawTargetMarkRot(DRW_VEC(3, vec3_802f92b8[3]), -angle), *color);

    _drawTargetMarkQuad(0x41, trans, DRW_VEC(4, vec3_802f92b8[4]),
                        _drawTargetMarkRot(DRW_VEC(5, vec3_802f92b8[5]), angle), *color);
}
#undef DRW_VEC
#undef MARK_COLOR_RESTRICT
#pragma dont_inline reset

void _disp_target_mark(CameraId cameraId, void* param) {
    BattleWork* work = param;
    TargetMarkWork* tm = (TargetMarkWork*)((u8*)work + 0x1F4C);
    Vec pos;
    Vec screen;
    GXColor color;

    pos.x = 60.0f * sinfd(tm->rotZ) + tm->x;
    pos.y = 60.0f * cosfd(tm->rotZ) + tm->y;
    pos.z = tm->z;
    btlGetScreenPoint(&pos, &screen);
    color = dat_8041e4ac;
    _draw_target_mark(&color, screen.x, screen.y, screen.z, tm->field_0x2C);
}

// The original builds the afterimage colour after intplGetValue and copies it to the by-value slot.
static inline GXColor _makeAfterimageColor(f32 value) {
    s32 alpha = (s32)intplGetValue(0, (s32)(30.0f - value), 0x1E, 255.0f,
                                   0.0f);
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

        if (aw->rotHistory[i] >= 0.0f) {
            if (aw->valueHistory[i] > 0.0f) {
                aw->valueHistory[i] -= 1.0f;
                pos.x = 60.0f * sinfd(aw->rotHistory[i]) + aw->x;
                pos.y = 60.0f * cosfd(aw->rotHistory[i]) + aw->y;
                pos.z = aw->z;
                btlGetScreenPoint(&pos, &screen);
                value = aw->valueHistory[i];
                if (value >= 30.0f) {
                    GXColor color = dat_8041e4b0;
                    _draw_target_mark(&color, screen.x, screen.y, screen.z, aw->angleHistory[i]);
                } else {
                    GXColor color = _makeAfterimageColor(value);
                    _draw_target_mark(&color, screen.x, screen.y, screen.z, aw->angleHistory[i]);
                }
                if (aw->valueHistory[i] <= 0.0f) {
                    aw->rotHistory[i] = -1.0f;
                }
            }
        }
    }
}
#pragma optimize_for_size reset

// .rodata: the vector table then the SFX name, in the original's order. The compiler's pooled
// 0.5/i2f doubles are emitted after these named objects. The table is non-const to keep the
// retail load scheduling, forced into .rodata.
__declspec(section ".rodata") Vec vec3_802f92b8[6] = {
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.5f, 0.5f, 1.0f},
    {0.0f, 0.0f, 0.0f},
    {0.5f, 0.5f, 1.0f},
    {0.0f, 0.0f, 0.0f},
};

// .sdata2 definitions of this unit, in the original's order. dat_8041e4a8 is a mutable
// 0xFFFFFFFF word in the original; __declspec(section) keeps it in .sdata2 so the text loads it
// instead of folding the immediate.
__declspec(section ".sdata2") u32 dat_8041e4a8 = 0xFFFFFFFF;
const GXColor dat_8041e4ac = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_8041e4b0 = {0x90, 0x80, 0xA0, 0xFF};
const GXColor dat_8041e4b4 = {0x90, 0x80, 0xA0, 0x00};

// The original's .sdata2 also has a 4-byte zero tail (gap_09_8041E504_sdata2). It is the section's
// 8-byte alignment padding: the pooled floats end at 0x5C and the linker pads the section to 0x60
// with zeros, matching the original without an explicit object.

// .sbss2 default colour followed by the 4-byte section-tail gap. Objects land in reverse
// declaration order, so the gap is declared before unk_80423078.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") u8 gap_10_8042307C_sbss2[4];
__declspec(section ".sdata") u32 unk_80423078;
#pragma section ".sdata" ".sbss"