#include "eff/n64/eff_ice_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>

// .sdata2 colour the original copies and then overwrites the alpha byte of (volatile: per-element reload).
extern const volatile u32 dat_8041f198;
// .sdata2 angle/packing constants (named after their original addresses).
extern const f32 float_deg2rad_8041f19c;
extern const f32 float_0p015625_8041f1a0;
extern const f32 float_0_8041f1a4;
extern const f32 float_0p03125_8041f1a8;

// .data vertex arrays passed to effSetVtxDescN64.
extern effVtxArray ice_vtx;
extern effVtxArray ice_s_vtx;

typedef struct effIceData {
    s32 type;       // 0x00
    f32 x;          // 0x04
    f32 y;          // 0x08
    f32 z;          // 0x0C
    f32 field_0x10; // 0x10
    f32 field_0x14; // 0x14
    f32 field_0x18; // 0x18
    f32 field_0x1C; // 0x1C
    f32 field_0x20; // 0x20
    s32 field_0x24; // 0x24
    s32 field_0x28; // 0x28
    s32 field_0x2C; // 0x2C
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
    data->field_0x28 = 30;
    data->field_0x2C = 0;
    data->field_0x24 = 0;
    ptr = data + 1;
    for (i = 1; i < 9; i++, ptr++) {
        ptr->x = (f32)(rand() % 10 - 5);
        ptr->y = (f32)(rand() % 10 + 5);
        ptr->z = 10.0f;
        ptr->field_0x18 = (f32)(rand() % 360);
        ptr->field_0x20 = (f32)(rand() % 360);
        ptr->field_0x1C = (f32)(rand() % 100 / 10);
        ptr->field_0x10 = 0.0f;
        ptr->field_0x14 = 2.0f;
    }
    return effect;
}

static inline void iceSetColor(u8 a) {
    GXColor color = *(GXColor*)&dat_8041f198;
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

    iceSetColor(data->field_0x24);

    for (i = 1, data++; i < effect->count; i++, data++) {
        if (i & 1) {
            vtx = &ice_s_vtx;
            effGetTexObjN64(8, &texobj);
            GXLoadTexObj(&texobj, GX_TEXMAP0);
            PSMTXScale(mScale, float_0p015625_8041f1a0, float_0p015625_8041f1a0, float_0_8041f1a4);
            GXLoadTexMtxImm(mScale, GX_TEXMTX0, GX_MTX2x4);
        } else {
            vtx = &ice_vtx;
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
            PSMTXRotRad(mRot, 'z', float_deg2rad_8041f19c * data->field_0x18);
            PSMTXConcat(mPos, mRot, mPos);
            PSMTXRotRad(mRot, 'y', float_deg2rad_8041f19c * data->field_0x20);
            PSMTXConcat(mPos, mRot, mPos);
        }

        PSMTXConcat(mBase, mPos, mPos);
        GXLoadPosMtxImm(mPos, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}
