// The original object was built with -O4,s: optimize_for_size is required to match its codegen
// (see REPORT.md). Remove this pragma once configure.py sets extra_cflags=["-O4,s"].
#pragma optimize_for_size on
#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_database_common.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include "eff/eff_starstone.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>
#include <string.h>
#include "battle/sac/sac_zubastar.h"

extern BattleWork* _battleWorkPointer;

// .sdata2 constants referenced by this file's original.
extern f32 float_200_804217c8;
extern f32 float_60_804217cc;
extern f32 float_1_804217d0;
extern f32 float_0p01_804217d4;
extern f32 float_128_804217d8;
extern f32 float_255_804217dc;
extern f32 float_0_804217e0;
extern f32 float_20_804217e4;
extern f32 float_deg2rad_804217e8;
extern f32 float_96_804217ec;
extern f32 float_400_804217f0;
extern f32 float_240_804217f4;
extern f32 float_480_804217f8;
extern f32 float_600_804217fc;
extern f32 float_104_80421800;
extern f32 float_70_80421804;
extern f32 float_32767_80421808;
extern f32 float_35_8042180c;
extern f32 float_2_80421810;
extern f32 float_neg1p25_80421814;
extern f32 float_100_80421818;
extern f32 float_2p8_8042181c;
extern f32 float_neg96_80421820;
extern f32 float_344_80421824;
extern f32 float_264_80421828;
extern f32 float_neg240_8042182c;
extern f32 float_5_80421830;
extern f32 float_300_80421834;
extern f32 float_8_80421838;
extern f32 float_neg1_8042183c;
extern f32 float_neg1000_80421840;
extern f32 float_50_80421844;
extern f32 float_0p25_80421848;
extern f32 float_683_8042184c;
extern f32 float_4096_80421850;
extern f32 float_neg0p9_80421854;
extern f32 float_0p1_80421858;
extern f32 float_900_8042185c;
extern f32 float_1Eneg05_80421860;
extern f32 float_37_80421864;
extern f32 float_2160_80421868;
extern f32 float_1p5_8042186c;
extern u32 dat_804217b8;
extern u32 dat_804217bc;
extern const Vec vec3_802ff6b0;
extern const Vec vec3_802ff698;
extern const Vec vec3_802ff6a4;
extern const Vec vec3_802ff6bc;
extern const Vec vec3_802ff6c8;
extern const Vec vec3_802ff7ac;
extern const Vec vec3_802ff7b8;
extern const Vec vec3_802ff7c4;
extern const char str_802FF80C[];

typedef struct ZubaStarBomb {
    s32 state;       // 0x00
    s32 count;       // 0x04
    Vec* points;     // 0x08
    Vec field_0x0C;  // 0x0C
    Vec field_0x18;  // 0x18
    Vec field_0x24;  // 0x24
    Vec field_0x30;  // 0x30
    f32 field_0x3C;  // 0x3C
    f32 field_0x40;  // 0x40
    f32 field_0x44;  // 0x44
    s32 timer;       // 0x48
    s32 maxTimer;    // 0x4C
} ZubaStarBomb;

typedef struct ZubaStarStar {
    s32 state;     // 0x00
    s32 field_0x04; // 0x04
    s32 timer;     // 0x08
    f32 x0;        // 0x0C
    f32 y0;        // 0x10
    f32 x1;        // 0x14
    f32 y1;        // 0x18
} ZubaStarStar;

struct ZubaStarWork {
    u8 field_0x000[0x04];          // 0x000
    s32 field_0x004;               // 0x004
    u8 field_0x008[0x08];          // 0x008
    s32 field_0x010;               // 0x010
    s32 bombCount;                 // 0x014
    s32 field_0x018;               // 0x018
    u8 field_0x01C[0x04];          // 0x01C
    Vec field_0x020;               // 0x020
    Vec field_0x02C[4];            // 0x02C
    u8 field_0x05C[0x20];          // 0x05C
    s32 field_0x07C;               // 0x07C
    u8 field_0x080[0x04];          // 0x080
    f32 field_0x084;               // 0x084
    u8 field_0x088[0x0C];          // 0x088
    f32 field_0x094;               // 0x094
    f32 field_0x098;               // 0x098
    u8 field_0x09C[0x05];          // 0x09C
    u8 field_0x0A1;                // 0x0A1
    u8 field_0x0A2[0x06];          // 0x0A2
    ZubaStarBomb* bombs[0xC8];     // 0x0A8
    ZubaStarStar stars[0x10];      // 0x3C8
    smartEntry* tex;               // 0x588
    s32 starState;                 // 0x58C
    s32 starCounter;               // 0x590
    Vec starCur;                   // 0x594
    Vec starStart;                 // 0x5A0
    Vec starEnd;                   // 0x5AC
    Vec field_0x5B8;               // 0x5B8
    Vec scale1;                    // 0x5C4
    Vec scale2;                    // 0x5D0
    EffectEntry* effect;           // 0x5DC
};

// .text:0x0 | 0x8022E590 | size: 0x368
// NONMATCHING (77.09%): frame size 0x90 vs 0x80, needs an f31 hoist of float_20, and the iconID
// computation codegen differs.
void zubastar_disp2D(CameraId cameraId, void* param) {
    ZubaStarWork* work = GetZubaStarPtr();
    s32 i;
    s32 alpha;
    Vec pos;
    GXColor color;

    if (work->field_0x07C >= 5) {
        s32 icon;

        pos = vec3_802ff7c4;
        pos.x = work->field_0x094 - float_200_804217c8;
        pos.y = float_60_804217cc + work->field_0x098;
        icon = 0x64 + (work->field_0x0A1 > 3);
        iconDispGx(&pos, 0x10, icon, float_1_804217d0);
        BattleAcDrawGauge((s32)work->field_0x094, (s32)work->field_0x098, 0xB1, 4, 0x1A, 0x34, 0x4E,
                          0, float_0p01_804217d4 * work->field_0x084);
    }
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXSetCurrentMtx(0);
    GXLoadPosMtxImm(camGetPtr(cameraId)->view, 0);
    GXSetLineWidth(0x18, 0);
    for (i = 0; i < 0x10; i++) {
        if (work->stars[i].timer <= 0x1E) {
            alpha = (s32)intplGetValue(0, work->stars[i].timer, 0x1E, float_128_804217d8,
                                       float_255_804217dc);
        } else {
            alpha = (s32)intplGetValue(0, work->stars[i].timer - 0x1E, 0x1E, float_255_804217dc,
                                       float_128_804217d8);
        }
        if (work->stars[i].state == 5) {
            GXSetChanMatColor(GX_COLOR0A0, (GXColor){alpha, alpha, alpha, 0xFF});
            GXBegin(0xA8, 0, 2);
            GXPosition3f32(work->stars[i].x0, work->stars[i].y0, float_0_804217e0);
            GXPosition3f32(work->stars[i].x1, work->stars[i].y1, float_0_804217e0);
        }
    }
    iconDispGx(&work->field_0x020, 0x10, 0x19D, float_1_804217d0);
    if (work->field_0x018 >= 0x19 && work->field_0x018 < 0x1E) {
        for (i = 0; i < 4; i++) {
            Mtx mtx;

            PSMTXTrans(mtx, work->field_0x02C[i].x, work->field_0x02C[i].y - float_20_804217e4,
                       float_0_804217e0);
            color = (GXColor){0xFF, 0xFF, 0xFF, 0};
            color.a = 0xFF - (i + 1) * 0x32;
            iconDispGxCol(mtx, 0x10, 0x19D, color);
        }
    }
}

// .text:0x368 | 0x8022E8F8 | size: 0x35C
// NONMATCHING (89.81%): GX setup matches; loop-invariant constant register mapping and inner vertex
// load/store scheduling differ.
void zubastar_disp_bomb(CameraId cameraId, void* param) {
    ZubaStarWork* work = GetZubaStarPtr();
    CameraEntry* cam = camGetPtr(8);
    GXTexObj texObj;
    Mtx m1;
    Mtx m2;
    Mtx m3;
    Mtx m4;
    Mtx mtx;
    s32 i;
    s32 j;
    f32 c96 = float_96_804217ec;
    f32 c400 = float_400_804217f0;
    f32 c240 = float_240_804217f4;
    f32 c480 = float_480_804217f8;
    f32 cdeg = float_deg2rad_804217e8;

    GXSetCullMode(GX_CULL_NONE);
    GXSetZCompLoc(GX_FALSE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_NEVER, 0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ONE);
    GXInitTexObj(&texObj, work->tex->address, 0x190, 0x1E0, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, 0);
    GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, float_0_804217e0, float_0_804217e0,
                    float_0_804217e0, 0, 0, 0);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    for (i = 0; i < work->bombCount; i++) {
        ZubaStarBomb* bomb = work->bombs[i];

        PSMTXTrans(m1, bomb->field_0x18.x, bomb->field_0x18.y, bomb->field_0x18.z);
        PSMTXTrans(m2, -bomb->field_0x18.x, -bomb->field_0x18.y, -bomb->field_0x18.z);
        PSMTXTrans(m3, bomb->field_0x0C.x, bomb->field_0x0C.y, bomb->field_0x0C.z);
        PSMTXRotRad(m4, 'z', cdeg * bomb->field_0x3C);
        PSMTXConcat(m4, m2, mtx);
        PSMTXConcat(m1, mtx, mtx);
        PSMTXConcat(m3, mtx, mtx);
        PSMTXConcat(cam->view, mtx, mtx);
        GXLoadPosMtxImm(mtx, 0);
        GXBegin(GX_TRIANGLEFAN, GX_VTXFMT0, bomb->count);
        for (j = 0; j < bomb->count; j++) {
            Vec* p = &bomb->points[j];

            GXPosition3f32(p->x, p->y, float_0_804217e0);
            GXTexCoord2f32((c96 + p->x) / c400, (c240 - p->y) / c480);
        }
    }
}

// .text:0x6C4 | 0x8022EC54 | size: 0x68
void zubastar_capture(CameraId cameraId, void* param) {
    ZubaStarWork* work = GetZubaStarPtr();

    GXSetTexCopySrc(0xD0, 0, 0x190, 0x1E0);
    GXSetTexCopyDst(0x190, 0x1E0, 4, 0);
    GXCopyTex(work->tex->address, 0);
    GXPixModeSync();
}

// .text:0x72C | 0x8022ECBC | size: 0xA4
s32 zubastar_end(EventEntry* event, BOOL isFirstCall) {
    ZubaStarWork* work = GetZubaStarPtr();
    s32 i;

    effDelete(*(EffectEntry**)((u8*)work->effect->userdata + 0x34));
    effDelete(work->effect);
    for (i = 0; i < 0xC8; i++) {
        if (work->bombs[i] != NULL) {
            if (work->bombs[i]->points != NULL) {
                BattleFree(work->bombs[i]->points);
                work->bombs[i]->points = NULL;
            }
            BattleFree(work->bombs[i]);
            work->bombs[i] = NULL;
        }
    }
    smartFree(work->tex);
    return 2;
}

// .text:0x7D0 | 0x8022ED60 | size: 0x30
s32 zubastar_init(EventEntry* event, BOOL isFirstCall) {
    memset(GetZubaStarPtr(), 0, 0x5E0);
    return 2;
}

// .text:0x800 | 0x8022ED90 | size: 0x398
// NONMATCHING (68.52%): state machine transcribed; switch compare order and rodata-base constant access
// differ.
s32 zubastar_bomb(EventEntry* event, BOOL isFirstCall) {
    ZubaStarWork* work = GetZubaStarPtr();
    s32 i;

    if (isFirstCall) {
        dispEntry(8, float_0_804217e0, 0, zubastar_capture, NULL);
        work->field_0x004 = 0;
        return 0;
    }
    if (work->field_0x004 == 0) {
        s32 count = 0;

        for (i = 0; i < work->bombCount; i++) {
            if (work->bombs[i]->state != 0xA) {
                break;
            }
            count++;
        }
        if (count == work->bombCount) {
            return 2;
        }
    }
    for (i = 0; i < work->bombCount; i++) {
        ZubaStarBomb* bomb = work->bombs[i];

        switch (bomb->state) {
        case 0: {
            s32 maxTimer = irand(0x1E) + 0x1E;
            f32 sx = float_0_804217e0;
            f32 sy = float_0_804217e0;
            f32 sz = float_0_804217e0;
            s32 j;

            bomb->maxTimer = maxTimer;
            bomb->field_0x24 = vec3_802ff7ac;
            bomb->field_0x30 = vec3_802ff7b8;
            for (j = 0; j < bomb->count; j++) {
                sx += bomb->points[j].x;
                sy += bomb->points[j].y;
                sz += bomb->points[j].z;
            }
            bomb->field_0x18.x = sx / (f32)bomb->count;
            bomb->field_0x18.y = sy / (f32)bomb->count;
            bomb->field_0x18.z = sz / (f32)bomb->count;
            bomb->field_0x30.y +=
                (bomb->field_0x18.x - float_104_80421800) * float_600_804217fc / float_200_804217c8;
            bomb->field_0x3C = float_0_804217e0;
            bomb->field_0x40 = float_70_80421804 * (f32)rand() / float_32767_80421808 - float_35_8042180c;
            {
                f32 t = (f32)maxTimer;

                bomb->field_0x44 = (float_2_80421810 * (bomb->field_0x30.y - bomb->field_0x24.y) -
                                    (t * (float_neg1p25_80421814 * t))) /
                                   (float_2_80421810 * t);
            }
            bomb->timer = 0;
            bomb->state = 5;
            psndSFXOn(str_802FF80C);
            break;
        }
        case 5:
            bomb->field_0x0C.x =
                intplGetValue(0, bomb->timer, bomb->maxTimer, bomb->field_0x24.x, bomb->field_0x30.x);
            bomb->field_0x0C.z =
                intplGetValue(0, bomb->timer, bomb->maxTimer, bomb->field_0x24.z, bomb->field_0x30.z);
            bomb->field_0x0C.y += bomb->field_0x44;
            bomb->field_0x3C += bomb->field_0x40;
            bomb->field_0x44 += float_neg1p25_80421814;
            bomb->timer++;
            if (bomb->timer > bomb->maxTimer) {
                bomb->state = 0xA;
            }
            break;
        }
    }
    dispEntry(8, float_0_804217e0, 1, zubastar_disp_bomb, NULL);
    return 0;
}

// .text:0x1990 | 0x8022FF20 | size: 0x100
// NONMATCHING (86.64%): line-segment intersection logic matches; only float-register allocation differs.
s32 zubastar_get_kouten(f32 f1, f32 f2, f32 f3, f32 f4, f32 f5, f32 f6, f32 f7, f32 f8, f32* out1,
                        f32* out2) {
    f32 a = f3 - f1;
    f32 b;
    f32 c;
    f32 d;

    if (a == float_0_804217e0 && f7 - f5 == float_0_804217e0) {
        return 0;
    }
    b = (a != float_0_804217e0) ? a : float_1Eneg05_80421860;
    c = (f7 - f5 != float_0_804217e0) ? f7 - f5 : float_1Eneg05_80421860;
    d = f8 - f6;
    f8 = (f4 - f2) / b;
    b = d / c;
    d = f8 - b;
    if (d == float_0_804217e0) {
        return 0;
    }
    *out1 = ((f6 - f2) + (f8 * f1 - b * f5)) / d;
    if (a != float_0_804217e0) {
        *out2 = f8 * (*out1 - f1) + f2;
    } else {
        *out2 = b * (*out1 - f5) + f6;
    }
    a = (*out1 - f5) / c;
    if (a >= float_0_804217e0 && a <= float_1_804217d0) {
        return 1;
    }
    return 0;
}

// .text:0x1F58 | 0x802304E8 | size: 0xE4
// NONMATCHING (92.93%): one early lwz of vec3_802ff6b0 is scheduled before count is stored; the rest
// of the function is identical.
void zubastar_create_takaku(s32 count, Vec* points) {
    ZubaStarWork* work = GetZubaStarPtr();
    ZubaStarBomb* bomb;
    s32 i;

    work->bombs[work->bombCount] = BattleAlloc(0x50);
    bomb = work->bombs[work->bombCount];
    bomb->points = BattleAlloc(count * 0xC);
    bomb->state = 0;
    bomb->count = count;
    bomb->field_0x0C = vec3_802ff6b0;
    bomb->timer = 0;
    bomb->maxTimer = 1;
    for (i = 0; i < count; i++) {
        bomb->points[i] = points[i];
    }
    work->bombCount++;
}

// .text:0x203C | 0x802305CC | size: 0x3C
s32 weaponGetPower_ZubaStar(BattleUnit* unit1, BattleWeapon* weapon, BattleUnit* unit2,
                            BattleUnitPart* part) {
    ZubaStarWork* work = GetZubaStarPtr();

    return weapon->hpDamageWork[work->field_0x010];
}

// .text:0x2078 | 0x80230608 | size: 0xC
// Keep GetZubaStarPtr out of line: the original issues a call (bl) to it.
#pragma dont_inline on
ZubaStarWork* GetZubaStarPtr(void) {
    return (ZubaStarWork*)((u8*)_battleWorkPointer + 0x1F4C);
}
#pragma dont_inline reset

// .text:0x2084 | 0x80230614 | size: 0x4
// The original body is a no-op; this exists only as a dispEntry callback.
void zubaStarDispStar(CameraId cameraId, void* param) {
    (void)cameraId;
    (void)param;
}

// Star-stone effect userdata fields touched by main_star (no header declares this struct).
typedef struct ZubaStarEffectData {
    u8 field_0x00[0x8];  // 0x00
    f32 positionX;       // 0x08
    f32 positionY;       // 0x0C
    f32 positionZ;       // 0x10
    f32 scale;           // 0x14
    f32 scaleX;          // 0x18
    f32 scaleY;          // 0x1C
    f32 scaleZ;          // 0x20
} ZubaStarEffectData;

// .text:0x2088 | 0x80230618 | size: 0x33C
// NONMATCHING (70.73%): int->float cast references an anonymous constant instead of
// double_to_int_802ff828, and the switch case compare order differs.
static s32 main_star(EventEntry* event, BOOL isFirstCall) {
    ZubaStarWork* work = GetZubaStarPtr();
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    f32 scale;

    switch (work->starState) {
    case 1:
        work->starState = 2;
        work->starCounter = 0;
        work->effect = effStarStoneEntry(6, float_0_804217e0, float_neg1000_80421840,
                                         float_0_804217e0, float_1_804217d0);
        BtlUnit_GetPos(mario, &work->starCur.x, &work->starCur.y, &work->starCur.z);
        work->starCur.y += mario->mSizeMultiplier * (f32)mario->mHeight + float_37_80421864;
        work->starStart = work->starCur;
        work->starEnd = work->starCur;
        work->starEnd.y += float_50_80421844;
        work->starEnd.z += float_neg1_8042183c;
        work->scale1 = vec3_802ff698;
        work->scale2 = vec3_802ff6a4;
        // fallthrough
    case 2:
        work->starCounter++;
        if (work->starCounter <= 0x64) {
            work->starCur.x = intplGetValue(0, work->starCounter, 0x64, work->starStart.x, work->starEnd.x);
            work->starCur.y = intplGetValue(0, work->starCounter, 0x64, work->starStart.y, work->starEnd.y);
            work->starCur.z = intplGetValue(0, work->starCounter, 0x64, work->starStart.z, work->starEnd.z);
            scale = intplGetValue(0, work->starCounter, 0x64, float_0_804217e0, float_2_80421810);
            work->scale2.x = scale;
            work->scale2.y = scale;
            work->scale2.z = scale;
        } else {
            work->starCur = work->starEnd;
            work->scale2.x = float_2_80421810;
            work->scale2.y = float_2_80421810;
            work->scale2.z = float_2_80421810;
        }
        work->scale2.y = intplGetValue(4, work->starCounter, 0x78, float_0_804217e0, float_2160_80421868);
        if (work->starCounter >= 0x78) {
            work->starState = 3;
            work->starCounter = 0;
            work->starStart = work->starCur;
            work->starEnd.y = float_300_80421834;
        }
        break;
    case 3:
        work->starCounter++;
        work->starCur.y = intplGetValue(1, work->starCounter, 0x3C, work->starStart.y, work->starEnd.y);
        if (work->starCounter >= 0x3C) {
            work->starState = 4;
            work->starCounter = 0;
        }
        break;
    }
    if (work->effect != NULL) {
        ZubaStarEffectData* data = work->effect->userdata;

        data->positionX = work->starCur.x;
        data->positionY = work->starCur.y;
        data->positionZ = work->starCur.z;
        data->scaleX = work->scale2.x;
        data->scaleY = work->scale2.y;
        data->scaleZ = work->scale2.z;
        data->scale = float_1p5_8042186c * work->scale1.x;
    }
    dispEntry(4, float_0_804217e0, 2, zubaStarDispStar, NULL);
    return 0;
}

// .text:0x23C4 | 0x80230954 | size: 0x38
s32 wait_star_stone_up(EventEntry* event, BOOL isFirstCall) {
    ZubaStarWork* work = GetZubaStarPtr();

    return (work->starState == 4) ? 2 : 0;
}

// .text:0x23FC | 0x8023098C | size: 0x2C
static s32 star_stone_appear(EventEntry* event, BOOL isFirstCall) {
    ZubaStarWork* work = GetZubaStarPtr();

    work->starState = 1;
    return 2;
}
