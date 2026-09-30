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
#include <stdlib.h>
#include <string.h>

extern f32 lbl_803A5C90[];

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

    if (x > 0.0f) {
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

// NONMATCHING (74.94%): control flow/constants/GX state match, but MWCC emits lha/fctiwz for the
// vertex s16<->f32 conversions where the original uses psq_l/psq_st (GQR2-5 + qr5), so it saves a
// different register set (f25-f31 vs f31) and a different frame (0x1c0 vs 0x1a0).
void effSstarDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    u8* base = (u8*)lbl_803A5C90;
    Mtx mtx1;
    Mtx mtx2;
    Mtx mtx3;
    Mtx mtx4;
    GXTexObj tex;
    GXColor color;
    CameraEntry* cam;
    SstarWork* data;
    SstarVertex* vtx;
    u8* src;
    Vec v;
    Vec v2;
    f32 scale;
    s32 type;
    s32 colR;
    s32 colG;
    s32 colB;
    s32 i;
    s32 idx;

    cam = camGetPtr(camId);
    data = effect->userdata;
    scale = data->scale;
    type = data->type;
    colR = data->colR;
    colG = data->colG;
    colB = data->colB;
    PSMTXTrans(mtx1, data->position.x, data->position.y, data->position.z);
    PSMTXRotRad(mtx2, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
    PSMTXConcat(mtx1, mtx2, mtx3);
    PSMTXScale(mtx4, scale, scale, scale);
    PSMTXConcat(mtx3, mtx4, mtx3);
    PSMTXRotRad(mtx2, 'z', data->field_0x24 * 0.017453292f);
    PSMTXConcat(mtx3, mtx2, mtx3);

    GXSetCullMode(GX_CULL_NONE);
    color = (GXColor){ 0, 0, 0, 0xFF };
    color.r = colR;
    color.g = colG;
    color.b = colB;
    GXSetTevColor(1, color);
    GXSetTevColor(2, (GXColor){ 0x7F, 0x7F, 0x7F, 0x7F });
    GXSetTevColor(3, (GXColor){ 0x50, 0x50, 0x50, 0x50 });
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    PSMTXScale(mtx4, 0.015625f, 0.015625f, 0.0f);
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
    effSetVtxDescN64((effVtxArray*)(base + 0x40));
    PSMTXConcat(cam->view, mtx3, mtx1);
    GXLoadPosMtxImm(mtx1, 0);
    GXBegin(0x90, 0, 6);
    tri2(0, 1, 2, 0, 0, 2, 3, 0);

    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0xFF, 0xFF, 4);
    GXSetTevColorOp(0, 0, 0, 0, 0, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xC);
    GXSetTevAlphaIn(0, 7, 5, 3, 7);
    effSetVtxDescN64((effVtxArray*)(base + 0x78));
    GXBegin(0x90, 0, 0xC);
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
    PSMTXScale(mtx4, 0.03125f, 0.03125f, 0.0f);
    GXLoadTexMtxImm(mtx4, 0x1E, 1);
    if (type == 3) {
        effGetTexObjN64(0xB, &tex);
        GXLoadTexObj(&tex, 0);
    } else {
        effGetTexObjN64(0xA, &tex);
        GXLoadTexObj(&tex, 0);
    }
    data->field_0x3C++;
    if (data->field_0x3C >= 8) {
        data->field_0x3C = 0;
    }
    if (data->field_0x1C <= 1.0f) {
        idx = data->field_0x3C + 5;
        PSMTXTrans(mtx1, data->position.x, data->position.y, data->position.z);
        PSMTXRotRad(mtx2, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
        PSMTXConcat(mtx1, mtx2, mtx1);
        PSMTXScale(mtx4, scale, scale, scale);
        PSMTXConcat(mtx1, mtx4, mtx1);
        PSMTXRotRad(mtx2, 'z', data->angle * 0.017453292f);
        PSMTXConcat(mtx1, mtx2, data->mtx[data->field_0x3C]);

        vtx = data->vtx;
        src = base + 0xC0;
        for (i = 0; i < 5; i++, vtx++, src += 0x1C) {
            memcpy(&vtx->x, src, 0xE);
            v.x = vtx->x;
            v.y = vtx->y;
            v.z = vtx->z;
            PSMTXMultVec(data->mtx[(idx - i) % 8], &v, &v);
            vtx->x = v.x;
            vtx->y = v.y;
            vtx->z = v.z;
            memcpy(&vtx->x2, src + 0xE, 0xE);
            v2.x = vtx->x2;
            v2.y = vtx->y2;
            v2.z = vtx->z2;
            PSMTXMultVec(data->mtx[(idx - i) % 8], &v2, &v2);
            vtx->x2 = v2.x;
            vtx->y2 = v2.y;
            vtx->z2 = v2.z;
        }
        GXInvalidateVtxCache();
        DCFlushRange(data->vtx, 0x8C);
        effSetVtxDescN64((effVtxArray*)data->vtx);
        GXLoadPosMtxImm(cam->view, 0);
        for (i = 0; i < 4; i++) {
            GXBegin(0x90, 0, 6);
            tri2(i * 2, i * 2 + 1, i * 2 + 2, i * 2, i * 2 + 1, i * 2 + 3, i * 2 + 2, i * 2 + 1);
        }
    }
}

// MWCC's FPR allocation follows the local declaration order: with negSin declared next to
// negCos (the file's earlier 99.65% shape) negSin lands in f4 and d2 in f5; the order below
// gives the original's negSin=f5, d2=f4. The two `if (x)` truthiness tests (not `x != 0.0f`)
// are also needed for the original's `fcmpu f31,f0` operand order. Do not reorder/simplify.
EffectEntry* effSstarN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5, f32 a6, f32 a7, f32 a8) {
    EffectEntry* effect;
    SstarWork* data;
    f32 dx, dy, dz;
    f32 dist;
    f32 scale;
    f32 angle;
    f32 negCos;
    f32 c;
    f32 s;
    f32 d2;
    f32 negSin;
    s32 i;

    effect = effEntry();
    effect->type = "SstarN64";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(SstarWork));
    effect->userdata = data;
    effect->callback = effSstarMain;

    data->field_0x0 = 1;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->field_0x1C = 0.0f;
    data->field_0x24 = 0.0f;
    data->field_0x28 = 0.0f;
    data->type = type;

    if (type == 3) {
        data->scale = 0.2f;
        data->colR = rand() % 256;
        data->colG = rand() % (255 - data->colR);
        data->colB = (255 - data->colR) - data->colG;
    } else {
        data->scale = 0.5f;
        data->colR = 0xD2;
        data->colG = 0xBA;
        data->colB = 0x5A;
    }

    dx = a5 - x;
    dy = a6 - y;
    dz = a7 - z;
    dist = dx * dx + dy * dy + dz * dz;
    scale = dist;
    if (dist) {
        dist = sqrtf(dist);
        scale = a8 / dist;
    }
    data->velX = dx * scale;
    data->velY = dy * scale;
    data->velZ = dz * scale;

    angle = (6.2831855f * camGetPtr(CAMERA_3D)->viewYaw) / 360.0f;
    negCos = -(f32)cos(angle);
    negSin = -(f32)sin(angle);
    c = negCos * data->velX;
    s = negSin * data->velZ;
    d2 = c * c + s * s;
    if (d2) {
        d2 = sqrtf(d2);
    }
    if (negCos * data->velX + negSin * data->velZ < 0.0f) {
        d2 = -d2;
        data->field_0x2C = -20.0f;
    } else {
        data->field_0x2C = 20.0f;
    }
    negCos = -d2;
    angle = angleABf(0.0f, 0.0f, -data->velY, negCos);
    data->field_0x24 = angle;
    data->angle = angle;
    data->field_0x34 = negCos;
    data->lifetime = (s32)(dist / a8);
    for (i = 0; i < 8; i++) {
        PSMTXTrans(data->mtx[i], data->position.x, data->position.y, data->position.z);
    }
    data->field_0x3C = -1;
    return effect;
}

// Placed last so deferred emission makes this the first function in the object, which puts
// its sqrtf/pool constants in .rodata immediately after vec3_802fb6b8, before
// effSstarN64Entry's string. effSstarDisp's nonmatching s16->f32 conversion emits an extra
// magic double; if Main's pool constants were emitted after it (the original layout, which
// requires effSstarDisp to match) they would sit 8 bytes off and Main's lfd offsets would
// not match. Only the source position differs; the code is the original's.
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
        if (distance > 0.01f) {
            distance = 1.0f / sqrtf(distance);
        }
        prevX = data->position.x - 32.0f * (data->velX * distance);
        prevY = data->position.y - 32.0f * (data->velY * distance);
        prevZ = data->position.z - 32.0f * (data->velZ * distance);
        distance = 64.0f;
        if (data->field_0x0 != 0 && data->velY < 0.0f) {
            if (hitCheckFilter(NULL, &hitX, &hitX, &hitZ, &distance, &hitNX, &hitNY, &hitNZ,
                    prevX, prevY, prevZ, 0.0f, -1.0f, 0.0f) != NULL) {
                if (distance < 42.0f) {
                    data->velY = 0.6f * -data->velY;
                    data->velX *= 0.7f;
                    data->field_0x34 *= 0.7f;
                    data->field_0x2C *= 0.7f;
                    data->field_0x1C += 1.0f;
                    data->field_0x0 = 0;
                    if (data->field_0x1C >= 10.0f) {
                        data->lifetime = -1;
                    }
                }
            }
        }
        if (data->field_0x1C) {
            data->velY += -0.5f;
            data->field_0x24 += data->field_0x2C;
            data->angle = angleABf(0.0f, 0.0f, -data->velY, data->field_0x34);
        }
    }
    if (gp->inBattle == 0) {
        if (mario->position.y - data->position.y > 300.0f) {
            data->lifetime = -1;
        }
    } else if (data->position.y < -300.0f) {
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

