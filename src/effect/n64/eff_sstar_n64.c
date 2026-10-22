#include "eff/n64/eff_sstar_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/hitdrv.h"
#include "eff/eff_n64.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include "math_ext.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os/OSCache.h>
#include <dolphin/os/OSFastCast.h>
#include <stdlib.h>
#include <string.h>

// Data objects defined later in this file or in other units, declared before their first use.
extern const f32 float_deg2rad_8041fd84;
extern const f32 float_0p015625_8041fd88;
extern const f32 float_0_8041fd8c;
extern const f32 float_0p03125_8041fd90;
extern const f32 float_1_8041fd94;
extern const f32 float_0p01_8041fd98;
extern const f32 float_32_8041fd9c;
extern const f32 lbl_8041FDA0;
extern const f32 float_neg1_8041fda4;
extern const f32 float_42_8041fda8;
extern const f32 float_0p6_8041fdac;
extern const f32 float_0p7_8041fdb0;
extern const f32 float_10_8041fdb4;
extern const f32 float_neg0p5_8041fdb8;
extern const f32 float_300_8041fdbc;
extern const f32 float_neg300_8041fdc0;
extern const f32 float_0p2_8041fdc4;
extern const f32 float_0p5_8041fdc8;
extern const f32 float_6p2832_8041fdcc;
extern const f32 float_360_8041fdd0;
extern const f32 float_neg20_8041fdd4;
extern const f32 float_20_8041fdd8;

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") f32 lbl_803A5C90[2] = { -1.0f, 1.0f };

int __float_nan[];
#define NAN (*(f32*)__float_nan)

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

static inline s32 __fpclassifyf(f32 x) {
    switch (*(s32*)&x & 0x7F800000) {
        case 0x7F800000:
            if (*(s32*)&x & 0x007FFFFF) {
                return FP_NAN;
            } else {
                return FP_INFINITE;
            }
            break;
        case 0:
            if (*(s32*)&x & 0x007FFFFF) {
                return FP_SUBNORMAL;
            } else {
                return FP_ZERO;
            }
            break;
        default:
            return FP_NORMAL;
    }
}

#define fpclassify(x) __fpclassifyf(x)
#define isnan(x) (fpclassify(x) == FP_NAN)

static inline f32 sqrtf(f32 x) {
    const f64 _half = .5;
    const f64 _three = 3.0;

    if (x > float_0_8041fd8c) {
        f64 guess = __frsqrte(x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        return x * guess;
    } else if (x < 0.0) {
        return NAN;
    } else if (isnan(x)) {
        return NAN;
    }
    return x;
}

//.rodata

//.data (the three vtx blocks are reached through lbl_803A5C90 + 0x40/0x78/0xC0,
// so the 0x38-byte table of -1/1 pairs must be emitted before them).
u32 sstar_vtx_pad[14] = {
    0xBF800000, 0x3F800000, 0x3F800000, 0xBF800000,
    0x3F800000, 0xBF800000, 0xBF800000, 0x3F800000,
    0xBF800000, 0x3F800000, 0x3F800000, 0xBF800000,
    0x3F800000, 0xBF800000,
};

static u32 size64x64_tex64x64_vtx[14] = {
    0xFFE2FFE2, 0x00000000, 0x00000000, 0x00BF001E,
    0xFFE20000, 0x08000000, 0x000000BF, 0x001E001E,
    0x00000800, 0x08000000, 0x00BFFFE2, 0x001E0000,
    0x00000800, 0x000000BF,
};

u32 size128x128_tex32x32_vtx[18] = {
    0xFF800000, 0x00000000, 0x0000FFFF, 0xFF000000,
    0xFF800000, 0x04000000, 0xFFFFFF00, 0x00800000,
    0x00000400, 0x0400FFFF, 0xFF000000, 0x00800000,
    0x00000400, 0xFFFFFF00, 0x00000000, 0x00000000,
    0x0000FFFF, 0xFFFF0000,
};

u32 shooting_dust_vtx[36] = {
    0x00000008, 0x00000000, 0x00000000, 0x00BF0000,
    0xFFF80000, 0x00000800, 0x000000BF, 0x00000009,
    0x00000200, 0x00000000, 0x00BF0000, 0xFFF70000,
    0x02000800, 0x000000BF, 0x0000000A, 0x00000300,
    0x00000000, 0x00BF0000, 0xFFF60000, 0x03000800,
    0x000000BF, 0x0000000A, 0x00000300, 0x00000000,
    0x00BF0000, 0xFFF60000, 0x03000800, 0x000000BF,
    0x0000000B, 0x00000380, 0x00000000, 0x00BF0000,
    0xFFF50000, 0x03800800, 0x000000BF, 0x00000000,
};

// Local copy of system.c's inline helper so it inlines here (as in eff_puniballoon.c).
inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

// Copies vertex idx of a table into dst and transforms its position by mtx. The OSs16tof32/
// OSf32tos16 fast-cast calls are the SDK inlines (libs/dolsdk2004/include/dolphin/os/OSFastCast.h);
// without them MWCC 2.6 lowers the casts to the lha/fctiwz integer path.
static inline void sstarSetVtx(SstarVtx* dst, Mtx mtx, SstarVtx* tbl, s32 idx) {
    Vec v;
    memcpy(dst, &tbl[idx], sizeof(SstarVtx));
    OSInitFastCast();
    OSs16tof32(&dst->pos[0], &v.x);
    OSs16tof32(&dst->pos[1], &v.y);
    OSs16tof32(&dst->pos[2], &v.z);
    PSMTXMultVec(mtx, &v, &v);
    OSInitFastCast();
    OSf32tos16(&v.x, (s16*)dst);
    OSf32tos16(&v.y, &dst->pos[1]);
    OSf32tos16(&v.z, &dst->pos[2]);
}

// MWCC's FPR allocation follows the local declaration order: with negSin declared next to
// negCos (the file's earlier 99.65% shape) negSin lands in f4 and d2 in f5; the order below
// gives the original's negSin=f5, d2=f4. The two zero tests read the named float_0_8041fd8c so
// they share the original's constant; opt_common_subs off keeps MWCC from merging the two
// loads, which is what reproduces the original's `fcmpu f31,f0`/`fcmpo` operand order. Do not
// reorder/simplify.
#pragma opt_common_subs off
EffectEntry* effSstarN64Entry(s32 type, f32 x, f32 y, f32 z, f32 targetX, f32 targetY, f32 targetZ, f32 speed) {
    EffectEntry* effect;
    SstarWork* data;
    f32 dx, dy, dz;
    f32 dist;
    f32 scale;
    f32 angle;
    f32 negCos;
    f32 camVelX;
    f32 camVelZ;
    f32 horizSpeed;
    f32 negSin;
    s32 i;

    effect = effEntry();
    effect->type = "SstarN64\0\0\0";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(SstarWork));
    effect->userdata = data;
    effect->callback = effSstarMain;

    data->canBounce = 1;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->bounceCount = float_0_8041fd8c;
    data->rollAngle = float_0_8041fd8c;
    data->field_0x28 = float_0_8041fd8c;
    data->type = type;

    if (type == 3) {
        data->scale = float_0p2_8041fdc4;
        data->colR = rand() % 256;
        data->colG = rand() % (255 - data->colR);
        data->colB = (255 - data->colR) - data->colG;
    } else {
        data->scale = float_0p5_8041fdc8;
        data->colR = 0xD2;
        data->colG = 0xBA;
        data->colB = 0x5A;
    }

    dx = targetX - x;
    dy = targetY - y;
    dz = targetZ - z;
    dist = dx * dx + dy * dy + dz * dz;
    scale = dist;
    if (dist != float_0_8041fd8c) {
        dist = sqrtf(dist);
        scale = speed / dist;
    }
    data->velX = dx * scale;
    data->velY = dy * scale;
    data->velZ = dz * scale;

    angle = (float_6p2832_8041fdcc * camGetPtr(CAMERA_3D)->viewYaw) / float_360_8041fdd0;
    negCos = -(f32)cos(angle);
    negSin = -(f32)sin(angle);
    camVelX = negCos * data->velX;
    camVelZ = negSin * data->velZ;
    horizSpeed = camVelX * camVelX + camVelZ * camVelZ;
    if (horizSpeed != float_0_8041fd8c) {
        horizSpeed = sqrtf(horizSpeed);
    }
    if (negCos * data->velX + negSin * data->velZ < float_0_8041fd8c) {
        horizSpeed = -horizSpeed;
        data->rollSpeed = float_neg20_8041fdd4;
    } else {
        data->rollSpeed = float_20_8041fdd8;
    }
    negCos = -horizSpeed;
    angle = angleABf(float_0_8041fd8c, float_0_8041fd8c, -data->velY, negCos);
    data->rollAngle = angle;
    data->angle = angle;
    data->horizVel = negCos;
    data->lifetime = (s32)(dist / speed);
    for (i = 0; i < 8; i++) {
        PSMTXTrans(data->mtx[i], data->position.x, data->position.y, data->position.z);
    }
    data->histIndex = -1;
    return effect;
}
#pragma opt_common_subs reset

// Source order below is Entry, Main, Disp. MWCC emits functions in reverse, so the object's
// .text comes out Disp, Main, Entry, as in the original. Main must precede Disp in the source
// because effSstarDisp's s16->f32 conversion emits an extra magic double: with Main after Disp
// its pool constants and lfd offsets would shift by 8 bytes. Only the source position differs;
// the code is the original's.
void effSstarMain(EffectEntry* effect) {
    SstarWork* data = effect->userdata;
    MarioWork* mario;
    Vec position;
    f32 distance;
    f32 prevX, prevY, prevZ;
    f32 hitX, hitZ;
    f32 hitNX, hitNY, hitNZ;

    mario = marioGetPtr();
    position = initializeVec(&data->position);

    if (data->type >= 2) {
        distance = data->velX * data->velX + data->velY * data->velY + data->velZ * data->velZ;
        if (distance > float_0p01_8041fd98) {
            distance = float_1_8041fd94 / sqrtf(distance);
        }
        prevX = data->position.x - float_32_8041fd9c * (data->velX * distance);
        prevY = data->position.y - float_32_8041fd9c * (data->velY * distance);
        prevZ = data->position.z - float_32_8041fd9c * (data->velZ * distance);
        distance = lbl_8041FDA0;
        if (data->canBounce != 0 && data->velY < float_0_8041fd8c) {
            if (hitCheckFilter(NULL, prevX, prevY, prevZ, float_0_8041fd8c, float_neg1_8041fda4, float_0_8041fd8c, &hitX,
                    &hitX, &hitZ, &distance, &hitNX, &hitNY, &hitNZ) != NULL) {
                if (distance < float_42_8041fda8) {
                    data->velY = float_0p6_8041fdac * -data->velY;
                    data->velX *= float_0p7_8041fdb0;
                    data->horizVel *= float_0p7_8041fdb0;
                    data->rollSpeed *= float_0p7_8041fdb0;
                    data->bounceCount += float_1_8041fd94;
                    data->canBounce = 0;
                    if (data->bounceCount >= float_10_8041fdb4) {
                        data->lifetime = -1;
                    }
                }
            }
        }
        if (data->bounceCount != float_0_8041fd8c) {
            data->velY += float_neg0p5_8041fdb8;
            data->rollAngle += data->rollSpeed;
            data->angle = angleABf(float_0_8041fd8c, float_0_8041fd8c, -data->velY, data->horizVel);
        }
    }
    if (gp->inBattle == 0) {
        if (mario->position.y - data->position.y > float_300_8041fdbc) {
            data->lifetime = -1;
        }
    } else if (data->position.y < float_neg300_8041fdc0) {
        data->lifetime = -1;
    }
    data->position.x += data->velX;
    data->position.y += data->velY;
    data->position.z += data->velZ;
    if (data->lifetime < 0) {
        effDelete(effect);
    } else {
        dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effSstarDisp, effect);
    }
}

// Each of the five trail segments is two table vertices (2i, 2i+1), both transformed by the
// segment's history matrix. The vertex tables are addressed by name, so MWCC reaches them through
// the unit's .data base (lbl_803A5C90). The `base = i * 2;` form of the final tri2 loop is what emits
// retail's `mr r24, r22` init.
void effSstarDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    Mtx mtx1;
    Mtx mtx2;
    Mtx mtx4;
    Mtx mtx3;
    GXTexObj tex;
    SstarWork* data;
    s32 base;
    s32 colR;
    f32 scale;
    s32 colG;
    s32 i;
    s32 idx;
    CameraEntry* cam;
    s32 colB;
    s32 type;

    cam = camGetPtr(camId);
    data = effect->userdata;
    scale = data->scale;
    type = data->type;
    colR = data->colR;
    colG = data->colG;
    colB = data->colB;
    PSMTXTrans(mtx1, data->position.x, data->position.y, data->position.z);
    PSMTXRotRad(mtx2, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_8041fd84);
    PSMTXConcat(mtx1, mtx2, mtx3);
    PSMTXScale(mtx4, scale, scale, scale);
    PSMTXConcat(mtx3, mtx4, mtx3);
    PSMTXRotRad(mtx2, 'z', float_deg2rad_8041fd84 * data->rollAngle);
    PSMTXConcat(mtx3, mtx2, mtx3);

    GXSetCullMode(GX_CULL_NONE);
    GXSetTevColor(1, (GXColor){ colR, colG, colB, 0xFF });
    GXSetTevColor(2, (GXColor){ 0x7F, 0x7F, 0x7F, 0x7F });
    GXSetTevColor(3, (GXColor){ 0x50, 0x50, 0x50, 0x50 });
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    PSMTXScale(mtx4, float_0p015625_8041fd88, float_0p015625_8041fd88, float_0_8041fd8c);
    GXLoadTexMtxImm(mtx4, 0x1E, 1);
    effGetTexObjN64(9, &tex);
    GXLoadTexObj(&tex, 0);
    GXSetNumTevStages(3);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevColorOp(0, 0, 0, 0, 0, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 8, 5, 2);
    GXSetTevAlphaIn(0, 7, 4, 1, 7);
    GXSetTevOrder(1, 0xFF, 0xFF, 0xFF);
    GXSetTevColorOp(1, 1, 0, 0, 1, 0);
    GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
    GXSetTevColorIn(1, 0xF, 4, 5, 0);
    GXSetTevAlphaIn(1, 7, 7, 7, 0);
    GXSetTevOrder(2, 0xFF, 0xFF, 4);
    GXSetTevColorOp(2, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
    GXSetTevColorIn(2, 6, 0xF, 0xF, 0);
    GXSetTevAlphaIn(2, 7, 0, 5, 7);
    effSetVtxDescN64((effVtxArray*)size64x64_tex64x64_vtx);
    PSMTXConcat(cam->view, mtx3, mtx1);
    GXLoadPosMtxImm(mtx1, 0);
    GXBegin(GX_TRIANGLES, 0, 6);
    tri2(0, 1, 2, 0, 0, 2, 3, 0);

    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0xFF, 0xFF, 4);
    GXSetTevColorOp(0, 0, 0, 0, 0, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xC);
    GXSetTevAlphaIn(0, 7, 5, 3, 7);
    effSetVtxDescN64((effVtxArray*)size128x128_tex32x32_vtx);
    GXBegin(GX_TRIANGLES, 0, 0xC);
    tri2(0, 1, 4, 0, 1, 2, 4, 0);
    tri2(0, 4, 3, 0, 4, 2, 3, 0);

    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevColorOp(0, 0, 0, 0, 0, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xC);
    GXSetTevAlphaIn(0, 7, 5, 4, 7);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    PSMTXScale(mtx4, float_0p03125_8041fd90, float_0p03125_8041fd90, float_0_8041fd8c);
    GXLoadTexMtxImm(mtx4, 0x1E, 1);
    if (type == 3) {
        effGetTexObjN64(0xB, &tex);
        GXLoadTexObj(&tex, 0);
    } else {
        effGetTexObjN64(0xA, &tex);
        GXLoadTexObj(&tex, 0);
    }
    data->histIndex++;
    if (data->histIndex >= 8) {
        data->histIndex = 0;
    }
    if (data->bounceCount <= float_1_8041fd94) {
        idx = data->histIndex + 5;
        PSMTXTrans(mtx1, data->position.x, data->position.y, data->position.z);
        PSMTXRotRad(mtx2, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_8041fd84);
        PSMTXConcat(mtx1, mtx2, mtx1);
        PSMTXScale(mtx4, scale, scale, scale);
        PSMTXConcat(mtx1, mtx4, mtx1);
        PSMTXRotRad(mtx2, 'z', float_deg2rad_8041fd84 * data->angle);
        PSMTXConcat(mtx1, mtx2, data->mtx[data->histIndex]);

        for (i = 0; i < 5; i++) {
            sstarSetVtx(&data->vtx[i * 2], data->mtx[(idx - i + 8) % 8],
                (SstarVtx*)shooting_dust_vtx, i * 2);
            sstarSetVtx(&data->vtx[i * 2 + 1], data->mtx[(idx - i + 8) % 8],
                (SstarVtx*)shooting_dust_vtx, i * 2 + 1);
        }
        GXInvalidateVtxCache();
        DCFlushRange(data->vtx, 0x8C);
        effSetVtxDescN64((effVtxArray*)data->vtx);
        GXLoadPosMtxImm(cam->view, 0);
        for (i = 0; i < 4; i++) {
            base = i * 2;
            GXBegin(GX_TRIANGLES, 0, 6);
            tri2(base, base + 1, base + 2, base, base + 1, base + 3, base + 2, base + 1);
        }
    }
}

// .sdata2 as in the original object: the constants this unit's functions read, the two zero-filled gaps, and the
// globals referenced by eff_stamp_n64. explicit_zero_data/force_active keep the unreferenced zero entries alive.
#pragma force_active on
#pragma explicit_zero_data on
__declspec(section ".sdata2") const f32 float_deg2rad_8041fd84 = 0.017453292f;
__declspec(section ".sdata2") const f32 float_0p015625_8041fd88 = 0.015625f;
__declspec(section ".sdata2") const f32 float_0_8041fd8c = 0.0f;
__declspec(section ".sdata2") const f32 float_0p03125_8041fd90 = 0.03125f;
__declspec(section ".sdata2") const f32 float_1_8041fd94 = 1.0f;
__declspec(section ".sdata2") const f32 float_0p01_8041fd98 = 0.01f;
__declspec(section ".sdata2") const f32 float_32_8041fd9c = 32.0f;
__declspec(section ".sdata2") const f32 lbl_8041FDA0 = 64.0f;
__declspec(section ".sdata2") const f32 float_neg1_8041fda4 = -1.0f;
__declspec(section ".sdata2") const f32 float_42_8041fda8 = 42.0f;
__declspec(section ".sdata2") const f32 float_0p6_8041fdac = 0.6f;
__declspec(section ".sdata2") const f32 float_0p7_8041fdb0 = 0.7f;
__declspec(section ".sdata2") const f32 float_10_8041fdb4 = 10.0f;
__declspec(section ".sdata2") const f32 float_neg0p5_8041fdb8 = -0.5f;
__declspec(section ".sdata2") const f32 float_300_8041fdbc = 300.0f;
__declspec(section ".sdata2") const f32 float_neg300_8041fdc0 = -300.0f;
__declspec(section ".sdata2") const f32 float_0p2_8041fdc4 = 0.2f;
__declspec(section ".sdata2") const f32 float_0p5_8041fdc8 = 0.5f;
__declspec(section ".sdata2") const f32 float_6p2832_8041fdcc = 6.2831855f;
__declspec(section ".sdata2") const f32 float_360_8041fdd0 = 360.0f;
__declspec(section ".sdata2") const f32 float_neg20_8041fdd4 = -20.0f;
__declspec(section ".sdata2") const f32 float_20_8041fdd8 = 20.0f;
__declspec(section ".sdata2") const u32 gap_09_8041FDDC_sdata2 = 0;
__declspec(section ".sdata2") const f32 float_deg2rad_8041fde0 = 0.017453292f;
__declspec(section ".sdata2") const f32 float_0p03125_8041fde4 = 0.03125f;
__declspec(section ".sdata2") const f32 float_0_8041fde8 = 0.0f;
__declspec(section ".sdata2") const f32 float_1_8041fdec = 1.0f;
__declspec(section ".sdata2") const f32 float_0p9_8041fdf0 = 0.9f;
__declspec(section ".sdata2") const f32 float_0p1_8041fdf4 = 0.1f;
__declspec(section ".sdata2") const f32 float_2p4_8041fdf8 = 2.4f;
__declspec(section ".sdata2") const f32 float_0p2_8041fdfc = 0.2f;
__declspec(section ".sdata2") const f32 float_0p07_8041fe00 = 0.07f;
__declspec(section ".sdata2") const f32 float_100_8041fe04 = 100.0f;
__declspec(section ".sdata2") const f32 float_0p05_8041fe08 = 0.05f;
__declspec(section ".sdata2") const f32 float_7_8041fe0c = 7.0f;
__declspec(section ".sdata2") const f32 float_0p6_8041fe10 = 0.6f;
__declspec(section ".sdata2") const f32 float_10_8041fe14 = 10.0f;
__declspec(section ".sdata2") const f32 float_6p2832_8041fe18 = 6.2831855f;
__declspec(section ".sdata2") const f32 float_90_8041fe1c = 90.0f;
__declspec(section ".sdata2") const f32 float_360_8041fe20 = 360.0f;
__declspec(section ".sdata2") const u32 gap_09_8041FE24_sdata2 = 0;
#pragma explicit_zero_data reset
#pragma force_active reset
