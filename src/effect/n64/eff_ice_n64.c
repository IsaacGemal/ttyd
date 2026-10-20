#include "eff/n64/eff_ice_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"
#include <stdlib.h>

u32 ice_vtx[14] = {
    0xFFF0FFF0, 0x00000000, 0x00000000, 0x00FF0010,
    0xFFF00000, 0x08000000, 0x000000FF, 0x00100010,
    0x00000800, 0x08000000, 0x00FFFFF0, 0x00100000,
    0x00000800, 0x000000FF,
};

u32 ice_s_vtx[14] = {
    0xFFFCFFFC, 0x00000000, 0x00000000, 0x00FF0004,
    0xFFFC0000, 0x08000000, 0x000000FF, 0x00040004,
    0x00000800, 0x08000000, 0x00FFFFFC, 0x00040000,
    0x00000800, 0x000000FF,
};

// .sdata2 colour the original copies and then overwrites the alpha byte of.
extern const GXColor dat_8041f198;
// .sdata2 angle/packing constants (named after their original addresses).
extern const f32 float_deg2rad_8041f19c;
extern const f32 float_0p015625_8041f1a0;
extern const f32 float_0_8041f1a4;
extern const f32 float_0p03125_8041f1a8;

// effIceMain's .rodata origin vector and int->float conversion constant.
extern const Vec vec3_802fa820;
// effIceMain's .sdata2 particle-motion constants (named after their original addresses).
extern const f32 float_0p3_8041f1ac;
extern const f32 float_0p8_8041f1b0;
extern const f32 float_6p2832_8041f1b4;
extern const f32 float_2_8041f1b8;
extern const f32 float_360_8041f1bc;
extern const f32 float_neg0p05_8041f1c0;
extern const f32 float_0p2_8041f1c4;
extern const f32 float_0p92_8041f1c8;
extern const f32 float_10_8041f1cc;
extern const f32 float_0p5_8041f1d0;

// .data vertex arrays passed to effSetVtxDescN64.

typedef struct effIceData {
    s32 type;    // 0x00
    f32 x;       // 0x04
    f32 y;       // 0x08
    f32 z;       // 0x0C
    f32 velX;    // 0x10
    f32 velY;    // 0x14
    f32 rotZ;    // 0x18
    f32 spin;    // 0x1C
    f32 rotY;    // 0x20
    s32 alpha;   // 0x24
    s32 life;    // 0x28
    s32 age;     // 0x2C
} effIceData;

static void effIceMain(EffectEntry* effect);

EffectEntry* effIceN64Entry(s32 type, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    effIceData* data;
    effIceData* ptr;
    s32 i;

    effect = effEntry();
    effect->type = "IceN64";
    effect->count = 9;
    data = __memAlloc(HEAP_EFFECT, sizeof(effIceData) * 9);
    effect->userdata = data;
    effect->callback = effIceMain;
    data->type = type;
    data->x = x;
    data->y = y;
    data->z = z;
    data->life = 30;
    data->age = 0;
    data->alpha = 0;
    ptr = data + 1;
    for (i = 1; i < 9; i++, ptr++) {
        ptr->x = (f32)(rand() % 10 - 5);
        ptr->y = (f32)(rand() % 10 + 5);
        ptr->z = float_10_8041f1cc;
        ptr->rotZ = (f32)(rand() % 360);
        ptr->rotY = (f32)(rand() % 360);
        ptr->spin = (f32)(rand() % 100 / 10);
        ptr->velX = float_0_8041f1a4;
        ptr->velY = float_2_8041f1b8;
    }
    return effect;
}

static void effIceDisp(CameraId camId, void* param);

static void effIceMain(EffectEntry* effect) {
    effIceData* data = effect->userdata;
    Vec position2;
    Vec position;
    s32 age;
    s32 lifeLeft;
    s32 idx;
    s32 i;
    f32 sway;
    f32 swayAccel;

    position = vec3_802fa820;
    position.x = data->x;
    position.y = data->y;
    position.z = data->z;
    position2 = position;

    data->life--;
    data->age++;
    if (data->life < 0) {
        effDelete(effect);
        return;
    }
    age = data->age;
    lifeLeft = data->life;
    if (age < 10) {
        data->alpha = (s32)(float_0p3_8041f1ac * (f32)(0xFF - data->alpha) + (f32)data->alpha);
    }
    if (lifeLeft < 10) {
        data->alpha = (s32)((f32)data->alpha * float_0p8_8041f1b0);
    }
    idx = lifeLeft + 20;
    i = 1;
    data++;
    for (; i < effect->count; idx += 20, i++, data++) {
        sway = (f32)sin(float_6p2832_8041f1b4 * (float_2_8041f1b8 * data->rotZ) / float_360_8041f1bc);
        data->velY += float_neg0p05_8041f1c0;
        swayAccel = float_0p2_8041f1c4 * sway;
        data->velX += swayAccel;
        data->velY += float_neg0p05_8041f1c0;
        data->velX *= float_0p92_8041f1c8;
        data->spin += (f32)(s32)(effTblRandN64(50, idx) - 25);
        data->rotZ += float_10_8041f1cc * (f32)sin(float_6p2832_8041f1b4 * data->spin / float_360_8041f1bc);
        data->rotY += float_10_8041f1cc * (f32)cos(float_6p2832_8041f1b4 * (data->spin * float_0p5_8041f1d0) / float_360_8041f1bc);
        data->x += data->velX;
        data->y += data->velY;
    }
    dispEntry(4, dispCalcZ(&position2), 2, effIceDisp, effect);
}

static inline void iceSetColor(u8 a) {
    GXColor color = dat_8041f198;
    color.a = a;
    GXSetTevColor(GX_TEVREG0, color);
}

static void effIceDisp(CameraId camId, void* param) {
    EffectEntry* effect;
    effIceData* data;
    CameraEntry* cam;
    GXTexObj texobj;
    Mtx mPos, mScale, mRot, mBase;
    s32 i;
    effVtxArray* vtx;

    effect = param;
    cam = camGetPtr(camId);
    data = effect->userdata;

    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ONE);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetCullMode(GX_CULL_NONE);

    PSMTXTrans(mPos, data->x, data->y, data->z);
    PSMTXRotRad(mRot, 'y', -camGetPtr(CAMERA_3D)->viewYaw * float_deg2rad_8041f19c);
    PSMTXConcat(mPos, mRot, mBase);
    PSMTXConcat(cam->view, mBase, mBase);

    iceSetColor(data->alpha);

    for (i = 1, data++; i < effect->count; i++, data++) {
        if (i & 1) {
            vtx = (effVtxArray*)ice_s_vtx;
            effGetTexObjN64(8, &texobj);
            GXLoadTexObj(&texobj, GX_TEXMAP0);
            PSMTXScale(mScale, float_0p015625_8041f1a0, float_0p015625_8041f1a0, float_0_8041f1a4);
            GXLoadTexMtxImm(mScale, GX_TEXMTX0, GX_MTX2x4);
        } else {
            vtx = (effVtxArray*)ice_vtx;
            effGetTexObjN64(7, &texobj);
            GXLoadTexObj(&texobj, GX_TEXMAP0);
            PSMTXScale(mScale, float_0p03125_8041f1a8, float_0p03125_8041f1a8, float_0_8041f1a4);
            GXLoadTexMtxImm(mScale, GX_TEXMTX0, GX_MTX2x4);
        }
        effSetVtxDescN64(vtx);

        if (i & 1) {
            PSMTXTrans(mPos, data->x, data->y, data->z);
        } else {
            PSMTXTrans(mPos, data->x, data->y, data->z);
            PSMTXRotRad(mRot, 'z', float_deg2rad_8041f19c * data->rotZ);
            PSMTXConcat(mPos, mRot, mPos);
            PSMTXRotRad(mRot, 'y', float_deg2rad_8041f19c * data->rotY);
            PSMTXConcat(mPos, mRot, mPos);
        }

        PSMTXConcat(mBase, mPos, mPos);
        GXLoadPosMtxImm(mPos, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const Vec vec3_802fa820 = { 0.0f, 0.0f, 0.0f };
const GXColor dat_8041f198 = {0x14, 0x64, 0x14, 0x00};
const f32 float_deg2rad_8041f19c = 0.017453292f;
const f32 float_0p015625_8041f1a0 = 0.015625f;
const f32 float_0_8041f1a4 = 0.0f;
const f32 float_0p03125_8041f1a8 = 0.03125f;
const f32 float_0p3_8041f1ac = 0.3f;
const f32 float_0p8_8041f1b0 = 0.8f;
const f32 float_6p2832_8041f1b4 = 6.2831855f;
const f32 float_2_8041f1b8 = 2.0f;
const f32 float_360_8041f1bc = 360.0f;
const f32 float_neg0p05_8041f1c0 = -0.05f;
const f32 float_0p2_8041f1c4 = 0.2f;
const f32 float_0p92_8041f1c8 = 0.92f;
const f32 float_10_8041f1cc = 10.0f;
const f32 float_0p5_8041f1d0 = 0.5f;
