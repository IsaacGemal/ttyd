#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sdata2 float constants (named after their original addresses).
extern const f32 float_0p0056818_8041ee18;
extern const f32 float_0p045455_8041ee1c;
extern const f32 float_0_8041ee20;
extern const f32 float_2p0944_8041ee24;
extern const f32 float_neg2p0944_8041ee28;

// .sbss2 zero GXColor scratch the original builds the TEV color from.
extern GXColor unk_804230f8;

// Local to this unit in the original (`.fn main_dl, local`); defined further down.
static void main_dl(EffectEntry* effect, Mtx matrix);

typedef struct effFireDustData {
    s32 type;       // 0x00
    f32 x;          // 0x04
    f32 y;          // 0x08
    f32 z;          // 0x0C
    f32 field_0x10; // 0x10
    f32 field_0x14; // 0x14
    f32 field_0x18; // 0x18
    f32 field_0x1C; // 0x1C
    f32 field_0x20; // 0x20
    f32 field_0x24; // 0x24
    f32 field_0x28; // 0x28
    f32 field_0x2C; // 0x2C
    s32 field_0x30; // 0x30
    f32 field_0x34; // 0x34
    s32 field_0x38; // 0x38
    s32 field_0x3C; // 0x3C
    s32 field_0x40; // 0x40
    s32 field_0x44; // 0x44
    s32 field_0x48; // 0x48
    s32 field_0x4C; // 0x4C
    s32 field_0x50; // 0x50
    s32 field_0x54; // 0x54
    s32 field_0x58; // 0x58
    s32 field_0x5C; // 0x5C
    f32 field_0x60; // 0x60
    f32 field_0x64; // 0x64
    f32 field_0x68; // 0x68
} effFireDustData;

// Inlined in the original; keeping the TEV colour build in its own helper also reproduces
// the stack slot the compiler picks for the by-value GXColor copy.
static inline void effFireDustSetColor(effFireDustData* data) {
    GXColor color = unk_804230f8;
    color.r = data->field_0x40;
    color.g = data->field_0x44;
    color.b = data->field_0x48;
    color.a = data->field_0x5C;
    GXSetTevColor(GX_TEVREG1, color);
}

// NOTE: this file's functions are emitted in reverse source order. In the original the
// order is main_dl, effFireDustDisp, effFireDustMain, effFireDustN64Entry, so the
// not-yet-written Entry/Main belong above this definition and main_dl below it.

void effFireDustDisp(CameraId camId, void* param) {
    effFireDustData* data;
    EffectEntry* effect;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx transMtx;
    Mtx scaleMtx;
    Mtx rotMtx;
    Mtx outMtx;
    Mtx dispMtx;

    effect = param;
    cam = camGetPtr(camId);
    data = effect->userdata;

    PSMTXTrans(transMtx, data->x, data->y, data->z);
    PSMTXScale(scaleMtx, data->field_0x60, data->field_0x60, data->field_0x60);
    PSMTXConcat(transMtx, scaleMtx, outMtx);

    effFireDustSetColor(data);

    effGetTexObjN64(0x20, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, 0x7D);
    PSMTXScale(scaleMtx, float_0p0056818_8041ee18, float_0p045455_8041ee1c, float_0_8041ee20);
    GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C0, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetCullMode(GX_CULL_NONE);

    PSMTXConcat(cam->view, outMtx, dispMtx);
    main_dl(effect, dispMtx);

    PSMTXRotRad(rotMtx, 'y', float_2p0944_8041ee24);
    PSMTXConcat(outMtx, rotMtx, dispMtx);
    PSMTXConcat(cam->view, dispMtx, dispMtx);
    main_dl(effect, dispMtx);

    PSMTXRotRad(rotMtx, 'y', float_neg2p0944_8041ee28);
    PSMTXConcat(outMtx, rotMtx, dispMtx);
    PSMTXConcat(cam->view, dispMtx, dispMtx);
    main_dl(effect, dispMtx);
}
