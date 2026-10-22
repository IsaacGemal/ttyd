#include "battle/ac/ac_shot.h"
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_disp.h"
#include "battle/battle_disp_tex4.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "pmario_sound.h"
#include "system.h"
#include <string.h>

// .sdata2: dat_8041e4a8 is a mutable 0xFFFFFFFF word in the original; __declspec(section) keeps it
// in .sdata2 so the text loads it instead of folding the immediate. The colour literals' templates
// follow it.
__declspec(section ".sdata2") u32 dat_8041e4a8 = 0xFFFFFFFF;

// _disp_target_mark's action-command work area at BattleWork+0x1F4C. No header declares it,
// so the fields are named from the original's uses. Offsets here are relative to 0x1F4C.
typedef struct TargetMarkWork {
    u8 pad_0x00[0x24];  // 0x00
    f32 rotZ;           // 0x24 (BattleWork+0x1F70), marker angle
    u8 pad_0x28[4];     // 0x28 (BattleWork+0x1F74), unused by this function
    f32 rollAngle;      // 0x2C (BattleWork+0x1F78), passed as the f4 argument
    f32 x;              // 0x30 (BattleWork+0x1F7C)
    f32 y;              // 0x34 (BattleWork+0x1F80)
    f32 z;              // 0x38 (BattleWork+0x1F84)
} TargetMarkWork;

// The full ac_shot work area at BattleWork+0x1F4C, used by battleAcMain_Shot, actionCommandDisp and
// _disp_target_mark_afterimage. Offsets are relative to 0x1F4C.
typedef struct AcShotWork {
    s32 timer;           // 0x00, frames until the marker stops by itself (0x168 at start)
    s32 pad_0x04;        // 0x04, never accessed
    s32 index;           // 0x08, history write index
    f32 minValue;        // 0x0C, acParams[2] as f32: the lowest gauge value a result can take
    f32 maxValue;        // 0x10, acParams[3] as f32: the scale the gauge percent is taken against
    f32 angleMin;        // 0x14, lower bound of the marker sweep (acParams[4])
    f32 angleMax;        // 0x18, upper bound of the marker sweep (acParams[5])
    f32 gauge;           // 0x1C, 0..100, rises 1 per frame while the button is held
    f32 gaugeStart;      // 0x20, 100 * minValue / maxValue: the gauge value at the start
    f32 rotZ;            // 0x24, marker angle
    f32 rotSpeed;        // 0x28, angular velocity (acParams[6] / 10); flips sign at the bounds
    f32 rollAngle;       // 0x2C, afterimage roll angle, +8 per frame
    f32 x;               // 0x30
    f32 y;               // 0x34
    f32 z;               // 0x38
    s32 endTimer;        // 0x3C, end-of-AC timer
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
    aw->angleHistory[aw->index] = aw->rollAngle;
    aw->valueHistory[aw->index] = 70.0f;
    aw->index++;
    work->actionCommands.outputParams[3]++;
}

// The unit's float constants are written as literals so MWCC pools them in the original's .sdata2
// order.
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
                aw->minValue = (f32)work->actionCommands.acParams[2];
                aw->maxValue = (f32)work->actionCommands.acParams[3];
                aw->angleMin = (f32)work->actionCommands.acParams[4];
                aw->angleMax = (f32)work->actionCommands.acParams[5];
                aw->gaugeStart = 100.0f * aw->minValue / aw->maxValue;
                aw->gauge = aw->gaugeStart;
                aw->rotZ = aw->angleMin + (f32)irand((s32)(aw->angleMax - aw->angleMin));
                aw->rotSpeed = 0.1f * (f32)work->actionCommands.acParams[6];
                aw->index = 0;
                work->actionCommands.field_0x8 = 0x1E;
                unit = BattleGetUnitPtr(work, work->actionCommands.acParams[0]);
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
                aw->timer = 0x168;
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
                        f32 range = aw->maxValue - aw->minValue;
                        s32 r;
                        if (range) {
                            r = (s32)(aw->minValue + (f32)irand((s32)range));
                        } else {
                            r = (s32)aw->minValue;
                        }
                        work->actionCommands.outputParams[1] =
                            (s32)(aw->maxValue * (f32)r / 100.0f);
                    }
                    if ((f32)work->actionCommands.outputParams[1] < aw->minValue) {
                        work->actionCommands.outputParams[1] = (s32)aw->minValue;
                    }
                    _acShotAppendHistory(aw, work);
                    if (aw->index >= work->actionCommands.acParams[1]) {
                        work->actionCommands.result = 2;
                        return 0;
                    }
                    if (work->actionCommands.field_0x4 & 2) {
                        work->actionCommands.field_0x8 = 0x1E;
                    }
                    break;
                }
                if (!(BattlePadCheckTrigger(0x100) != 0 || work->actionCommands.outputParams[0] != 0 ||
                      work->actionCommands.guardFrames > 0)) {
                    if (--aw->timer <= 0) {
                        work->actionCommands.outputParams[1] =
                            (s32)(aw->maxValue * aw->gauge / 100.0f);
                        if ((f32)work->actionCommands.outputParams[1] < aw->minValue) {
                            work->actionCommands.outputParams[1] = (s32)aw->minValue;
                        }
                        _acShotAppendHistory(aw, work);
                        if (aw->index >= work->actionCommands.acParams[1]) {
                            work->actionCommands.result = 0;
                            work->actionCommands.mAcState = 0x3EA;
                        } else {
                            aw->timer = 0x168;
                        }
                        cont = 1;
                    }
                } else {
                    if (work->actionCommands.outputParams[0] == 0) {
                        work->actionCommands.mAcState = 0x3E9;
                    }
                }
                if (aw->rotSpeed) {
                    aw->rotZ += aw->rotSpeed;
                    if (aw->rotSpeed > 0.0f) {
                        if (aw->rotZ >= aw->angleMax) {
                            aw->rotZ = aw->angleMax;
                            aw->rotSpeed *= -1.0f;
                        }
                    } else {
                        if (aw->rotZ <= aw->angleMin) {
                            aw->rotZ = aw->angleMin;
                            aw->rotSpeed *= -1.0f;
                        }
                    }
                }
                aw->rollAngle += 8.0f;
                break;
            case 0x3E9:
                if (work->actionCommands.guardFrames > 0) {
                    work->actionCommands.outputParams[1] =
                        (s32)(50.0f * aw->maxValue / 100.0f);
                    if ((f32)work->actionCommands.outputParams[1] < aw->minValue) {
                        work->actionCommands.outputParams[1] = (s32)aw->minValue;
                    }
                    _acShotAppendHistory(aw, work);
                    if (aw->index >= work->actionCommands.acParams[1]) {
                        (work->actionCommands.guardFrames)--;
                        work->actionCommands.result = 2;
                        work->actionCommands.mAcState = 0x3EA;
                    } else {
                        work->actionCommands.mAcState = 0x3E8;
                    }
                    cont = 1;
                }
                aw->gauge += 1.0f;
                if (aw->gauge > 100.0f) {
                    aw->gauge = 100.0f;
                }
                if (work->actionCommands.acFlag & 1) {
                    work->actionCommands.outputParams[1] = (s32)((aw->minValue + aw->maxValue) / 2.0f);
                    _acShotAppendHistory(aw, work);
                    if (aw->index >= work->actionCommands.acParams[1]) {
                        work->actionCommands.mAcState = 0x3EA;
                        work->actionCommands.result = 2;
                    } else {
                        work->actionCommands.mAcState = 0x3E8;
                    }
                } else {
                    if (BattlePadCheckNow(0x100) == 0) {
                        work->actionCommands.outputParams[1] =
                            (s32)(aw->maxValue * aw->gauge / 100.0f);
                        if ((f32)work->actionCommands.outputParams[1] < aw->minValue) {
                            work->actionCommands.outputParams[1] = (s32)aw->minValue;
                        }
                        _acShotAppendHistory(aw, work);
                        if (aw->index >= work->actionCommands.acParams[1]) {
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
                aw->endTimer = 0x3C;
                work->actionCommands.mAcState = 0x3ED;
                if (aw->sfx != -1) {
                    psndSFXOff(aw->sfx);
                }
                break;
            case 0x3ED:
                aw->endTimer--;
                if (aw->endTimer <= 0) {
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

static void _ac_disp_init(BattleWork* work) {
    BattleACDispParams* disp = &work->actionCommands.dispParams;

    memset(disp, 0, 0x2C);
    disp->iconX = -200.0f;
    disp->iconY = 0.0f;
    disp->counter = 0x14;
    disp->field_0xC = dat_8041e4a8;
    disp->field_0x10 = 0;
    disp->dg = -0x10;
    disp->db = -0x10;
    disp->field_0x13 = 0;
    disp->value = 0.0f;
}

static void actionCommandDisp(f32 x, f32 y) {
    BattleWork* work = _battleWorkPointer;
    BattleActions* ac = &work->actionCommands;
    AcShotWork* aw = (AcShotWork*)&ac->field_0x2BC[0];
    BattleACDispParams* disp = &ac->dispParams;
    f32 target;
    f32 baseX;

    if (work->actionCommands.acFlag & 1) {
        return;
    }

    baseX = -200.0f + x;
    iconDispGx(1.0f, &(Vec){ baseX, 25.0f + y, 0.0f }, 0x10, 0x8C);

    target = aw->gauge / 100.0f;
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
        GXColor* tint = (GXColor*)&disp->field_0xC;

        tint->g += disp->dg;
        tint->b += disp->db;
        if (tint->g <= 0x80 && disp->dg < 0) {
            disp->db = 0x10;
            disp->dg = 0x10;
        }
        if (tint->g >= 0xF5 && disp->dg > 0) {
            disp->db = -0x10;
            disp->dg = -0x10;
        }
    }

    {
        GXColor* tint = (GXColor*)&disp->field_0xC;
        Mtx mtx;
        Mtx trans;
        Mtx scale;

        PSMTXTrans(trans,
                   baseX - 88.0f * (1.0f - disp->value),
                   33.0f + y, 0.0f);
        PSMTXScale(scale, disp->value, 1.0f, 1.0f);
        PSMTXConcat(trans, scale, mtx);
        iconDispGxCol(mtx, 0x10, 0x8D, (GXColor){ tint->r, tint->g, tint->b, tint->a });
    }
}

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

void _draw_target_mark(const GXColor* color, f32 x, f32 y, f32 z, f32 angle) {
    Vec trans;

    trans = (Vec){ x, y, z };

    _drawTargetMarkQuad(0x42, trans, (Vec){ 0.5f, 0.5f, 1.0f },
                        _drawTargetMarkRot((Vec){ 0.0f, 0.0f, 0.0f }, -angle), *color);

    _drawTargetMarkQuad(0x41, trans, (Vec){ 0.5f, 0.5f, 1.0f },
                        _drawTargetMarkRot((Vec){ 0.0f, 0.0f, 0.0f }, angle), *color);
}

void _disp_target_mark(CameraId cameraId, void* param) {
    BattleWork* work = param;
    TargetMarkWork* tm = (TargetMarkWork*)&work->actionCommands.field_0x2BC[0];
    Vec pos;
    Vec screen;
    GXColor color;

    pos.x = 60.0f * sinfd(tm->rotZ) + tm->x;
    pos.y = 60.0f * cosfd(tm->rotZ) + tm->y;
    pos.z = tm->z;
    btlGetScreenPoint(&pos, &screen);
    color = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
    _draw_target_mark(&color, screen.x, screen.y, screen.z, tm->rollAngle);
}

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
                    GXColor color = {0x90, 0x80, 0xA0, 0xFF};
                    _draw_target_mark(&color, screen.x, screen.y, screen.z, aw->angleHistory[i]);
                } else {
                    s32 alpha = (s32)intplGetValue(0, (s32)(30.0f - value), 0x1E, 255.0f, 0.0f);
                    GXColor color = (GXColor){0x90, 0x80, 0xA0, alpha};
                    _draw_target_mark(&color, screen.x, screen.y, screen.z, aw->angleHistory[i]);
                }
                if (aw->valueHistory[i] <= 0.0f) {
                    aw->rotHistory[i] = -1.0f;
                }
            }
        }
    }
}

// The original's .sdata2 also has a 4-byte zero tail (gap_09_8041E504_sdata2). It is the section's
// 8-byte alignment padding: the pooled floats end at 0x5C and the linker pads the section to 0x60
// with zeros, matching the original without an explicit object.

// .sbss2 4-byte gap after the unit's colour template.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") u8 gap_10_8042307C_sbss2[4];
#pragma section ".sdata" ".sbss"