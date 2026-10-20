#include "eff/n64/eff_stamp_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "math_ext.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_80423274;
__declspec(section ".sdata") GXColor unk_80423270;


// .sdata2 shared float pool
extern const f32 float_deg2rad_8041fde0;
extern const f32 float_0p03125_8041fde4;
extern const f32 float_0p9_8041fdf0;
extern const f32 float_0p1_8041fdf4;
extern const f32 float_2p4_8041fdf8;
extern const f32 float_0p2_8041fdfc;
extern const f32 float_0p07_8041fe00;
extern const f32 float_100_8041fe04;
extern const f32 float_0p05_8041fe08;
extern const f32 float_7_8041fe0c;
extern const f32 float_0p6_8041fe10;
extern const f32 float_10_8041fe14;
extern const f32 float_6p2832_8041fe18;
extern const f32 float_90_8041fe1c;
extern const f32 float_360_8041fe20;
extern const f32 float_1_8041fdec;
extern const f32 float_0_8041fde8;

// .sbss2 GXColor scratch globals used to seed the TEV colours.

// .data
u32 size16x16_tex64x32_vtx[0x38 / 4] = {
    0xFFF80000, 0x00000000, 0x00000000, 0x00000008, 0x00000000, 0x08000000, 0x00000000,
    0x000A0010, 0x00000800, 0x04000000, 0x00FFFFF6, 0x00100000, 0x00000400, 0x000000FF,
};

EffectEntry* effStampN64Entry(s32 type, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    effStampData* data;
    effStampData* petal;
    f32 rad;
    f32 angle;
    s32 i;

    effect = effEntry();
    effect->type = "StampN64";
    effect->count = 8;
    data = __memAlloc(HEAP_EFFECT, 0x200);
    effect->userdata = data;
    effect->callback = effStampMain;
    data->life = 30;
    data->counter = 0;
    data->type = type;
    data->x = x;
    data->y = y;
    data->z = z;
    data->alpha = 255;

    switch (type) {
    case 0:
        data->tevReg0R = data->tevReg1R = 20;
        data->tevReg0G = data->tevReg1G = 21;
        data->tevReg0B = data->tevReg1B = 242;
        break;
    case 1:
        data->tevReg0R = data->tevReg1R = 200;
        data->tevReg0G = data->tevReg1G = 21;
        data->tevReg0B = data->tevReg1B = 212;
        break;
    case 2:
        data->tevReg0R = data->tevReg1R = 255;
        data->tevReg0G = data->tevReg1G = 181;
        data->tevReg0B = data->tevReg1B = 218;
        break;
    }

    petal = data + 1;
    for (i = 1; i < 8; i++, petal++) {
        angle = (i - 1) * 360 / 7;
        rad = float_6p2832_8041fe18 * (float_90_8041fe1c + angle) / float_360_8041fe20;
        petal->x = float_1_8041fdec * (f32)cos(rad);
        petal->y = float_1_8041fdec * (f32)sin(rad);
        petal->z = float_0_8041fde8;
        petal->field_0x10 = float_0_8041fde8;
        petal->field_0x14 = float_0_8041fde8;
        petal->field_0x18 = float_0_8041fde8;
        petal->scaleY = float_0p1_8041fdf4;
        petal->scaleX = float_0p1_8041fdf4;
        petal->radius = float_1_8041fdec;
        petal->angle = angle;
    }
    return effect;
}

void effStampMain(EffectEntry* effect) {
    effStampData* data;
    Vec position2;
    Vec position;
    s32 i;
    s32 counter;

    data = effect->userdata;
    position = (Vec){ 0.0f, 0.0f, 0.0f };
    position.x = data->x;
    position.y = data->y;
    position.z = data->z;
    position2 = position;
    data->counter++;
    data->life--;
    if (data->life < 0) {
        effDelete(effect);
        return;
    }
    counter = data->counter;
    if (counter > 10) {
        data->alpha = (s32)((f32)data->alpha * float_0p9_8041fdf0);
    }
    i = 1;
    data++;
    for (; i < effect->count; i++, data++) {
        if (counter > 10) {
            data->scaleY += float_0p1_8041fdf4 * (float_1_8041fdec - data->scaleY);
            data->scaleX += float_0p1_8041fdf4 * (float_2p4_8041fdf8 - data->scaleX);
            data->radius += float_0p2_8041fdfc * (float_0p07_8041fe00 * (float_100_8041fe04 - data->radius));
        }
        if (counter < 10) {
            data->scaleY += float_0p05_8041fe08 * (float_7_8041fe0c - data->scaleY);
            data->scaleX += float_0p1_8041fdf4 * (float_1_8041fdec - data->scaleX);
            data->radius += float_0p6_8041fe10 * (float_0p05_8041fe08 * (float_10_8041fe14 - data->radius));
        }
        data->x = data->radius * (f32)cos(float_6p2832_8041fe18 * (float_90_8041fe1c + data->angle) / float_360_8041fe20);
        data->y = data->radius * (f32)sin(float_6p2832_8041fe18 * (float_90_8041fe1c + data->angle) / float_360_8041fe20);
    }
    dispEntry(4, dispCalcZ(&position2), 2, effStampDisp, effect);
}

// The original builds each TEV colour in a helper-style local whose stack slot sits below its
// outgoing copy; writing the same code inline makes mwcc put the two slots the other way round
// (see eff_ripple_n64.c).
#ifdef MUST_MATCH
static inline void effStampSetTevReg0(effStampData* data) {
    GXColor color = unk_80423270;
    color.r = data->tevReg0R;
    color.g = data->tevReg0G;
    color.b = data->tevReg0B;
    color.a = data->alpha;
    GXSetTevColor(GX_TEVREG0, color);
}

static inline void effStampSetTevReg1(effStampData* data) {
    GXColor color = unk_80423274;
    color.r = data->tevReg1R;
    color.g = data->tevReg1G;
    color.b = data->tevReg1B;
    GXSetTevColor(GX_TEVREG1, color);
}
#endif

void effStampDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effStampData* data;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx mtx;
    Mtx scaleMtx;
    Mtx rotMtx;
    Mtx viewMtx;
    f32 deg2rad;
    s32 i;

    cam = camGetPtr(camId);
    data = effect->userdata;

    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C0, GX_CC_C1, GX_CC_RASA, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);

    PSMTXTrans(mtx, data->x, data->y, data->z);
    PSMTXRotRad(rotMtx, 'y', -camGetPtr(CAMERA_3D)->viewYaw * float_deg2rad_8041fde0);
    PSMTXConcat(mtx, rotMtx, mtx);
    PSMTXConcat(cam->view, mtx, viewMtx);

    {
#ifdef MUST_MATCH
        effStampSetTevReg0(data);
        effStampSetTevReg1(data);
#else
        GXColor color;
        GXColor color2;
        GXColor tmp;

        tmp = unk_80423270;
        color = tmp;
        color.r = data->tevReg0R;
        color.g = data->tevReg0G;
        color.b = data->tevReg0B;
        color.a = data->alpha;
        GXSetTevColor(GX_TEVREG0, color);
        tmp = unk_80423274;
        color2 = tmp;
        color2.r = data->tevReg1R;
        color2.g = data->tevReg1G;
        color2.b = data->tevReg1B;
        GXSetTevColor(GX_TEVREG1, color2);
#endif
    }

    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    PSMTXScale(scaleMtx, float_0p03125_8041fde4, float_0p03125_8041fde4, float_0_8041fde8);
    GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);
    effGetTexObjN64(0x2b, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    data++;
    GXSetCullMode(GX_CULL_NONE);
    effSetVtxDescN64((effVtxArray*)size16x16_tex64x32_vtx);

    deg2rad = float_deg2rad_8041fde0;
    for (i = 1; i < effect->count; i++, data++) {
        PSMTXTrans(mtx, data->x, data->y, data->z);
        PSMTXRotRad(rotMtx, 'z', deg2rad * data->angle);
        PSMTXScale(scaleMtx, data->scaleX, data->scaleY, float_1_8041fdec);
        PSMTXConcat(mtx, rotMtx, mtx);
        PSMTXConcat(mtx, scaleMtx, mtx);
        PSMTXConcat(viewMtx, mtx, mtx);
        GXLoadPosMtxImm(mtx, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}
