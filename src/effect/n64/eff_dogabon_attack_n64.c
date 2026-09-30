#include "eff/n64/eff_dogabon_attack_n64.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "math_ext.h"
#include "driver/camdrv.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

#include <stdlib.h>

typedef struct DogabonAttackData {
    s32 type;           // 0x0
    f32 x;              // 0x4
    f32 y;              // 0x8
    f32 z;              // 0xC
    s32 lifetime;       // 0x10
    s32 field_0x14;     // 0x14
    s32 field_0x18;     // 0x18
    s32 field_0x1C;     // 0x1C
    s32 field_0x20;     // 0x20
    s32 field_0x24;     // 0x24
    f32 scale;          // 0x28
    f32 field_0x2C[25]; // 0x2C
    f32 field_0x90[25]; // 0x90
    f32 field_0xF4[25]; // 0xF4
    f32 field_0x158[25]; // 0x158
    f32 field_0x1BC[25]; // 0x1BC
    f32 field_0x220[25]; // 0x220
    f32 field_0x284[25]; // 0x284
    f32 field_0x2E8[25]; // 0x2E8
    f32 field_0x34C[25]; // 0x34C
    f32 field_0x3B0[25]; // 0x3B0
    f32 field_0x414[25]; // 0x414
    f32 field_0x478[25]; // 0x478
    f32 field_0x4DC[25]; // 0x4DC
    f32 field_0x540[25]; // 0x540
    f32 field_0x5A4[25]; // 0x5A4
    f32 field_0x608[25]; // 0x608
    s32 field_0x66C[25]; // 0x66C
    s32 field_0x6D0[25]; // 0x6D0
    s32 field_0x734[25]; // 0x734
} DogabonAttackData;

static s8 y_data[28] = {
    -5, -5, -5, -4, -4, -4, -3, -3, -2, -2, -1, -1, 0, 0,
    1, 2, 2, 3, 3, 4, 4, 4, 5, 5, 5, 0, 0, 0,
};

extern const f32 float_0_8041ecf4;
extern const f32 float_0p03125_8041ecf0;
extern const f32 float_deg2rad_8041ecf8;
extern const f32 float_1_8041ecfc;
extern const f32 float_6p2832_8041ed0c;
extern const f32 float_360_8041ed10;
extern const f32 float_15_8041ed18;
extern const f32 float_120_8041ed28;

// .data vertex array for the 12x12 textured quad.
u32 size12x12_tex32x32_vtx[14] = {
    0xFFFAFFFA, 0x00000000, 0x00000000, 0x00FF0006, 0xFFFA0000, 0x04000000, 0x000000FF,
    0x00060006, 0x00000400, 0x04000000, 0x00FFFFFA, 0x00060000, 0x00000400, 0x000000FF,
};

const char str_802FA4D0[20] = "DogabonAttackN64";

EffectEntry* effDogabonAttackN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 scale) {
    EffectEntry* effect;
    DogabonAttackData* data;
    s32 i;
    f32 s;
    f32 c;
    f32 angle;
    s32 r;

    effect = effEntry();
    effect->type = str_802FA4D0;
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(DogabonAttackData));
    effect->userdata = data;
    effect->callback = effDogabonAttackMain;
    effect->flags |= 2;
    data->type = type;
    data->field_0x14 = 0;
    if (count <= 0) {
        data->lifetime = 1000;
    } else {
        data->lifetime = count;
    }
    data->field_0x24 = 0xFF;
    data->x = x;
    data->y = y;
    data->z = z;
    data->scale = scale;
    data->field_0x18 = 0x46;
    data->field_0x1C = 0xB4;
    data->field_0x20 = 0x78;

    for (i = 0; i < 25; i++) {
        angle = (f32)(i * 1080 / 25) * float_6p2832_8041ed0c / float_360_8041ed10;
        s = (f32)sin(angle);
        c = (f32)cos(angle);
        r = rand();
        data->field_0x478[i] = (f32)(r % 360 - 180);
        data->field_0x284[i] = float_15_8041ed18 * s;
        data->field_0x2E8[i] = (f32)(y_data[i] + 10);
        data->field_0x34C[i] = float_15_8041ed18 * c;
        data->field_0x158[i] = float_120_8041ed28 * s;
        data->field_0x1BC[i] = (f32)(y_data[i] * 8 + 40);
        data->field_0x220[i] = float_120_8041ed28 * c;
        data->field_0x5A4[i] = float_0_8041ecf4;
        data->field_0x66C[i] = i * 2 + 1;
        data->field_0x6D0[i] = 0;
        data->field_0x734[i] = 0;
        data->field_0x4DC[i] = float_0_8041ecf4;
    }

    return effect;
}

void effDogabonAttackDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    DogabonAttackData* data;
    CameraEntry* camera;
    GXTexObj texObj;
    Mtx transMtx;
    Mtx scaleMtx;
    Mtx rotMtx;
    Mtx baseMtx;
    s32 alpha;
    s32 i;

    camera = camGetPtr(camId);
    data = effect->userdata;
    alpha = data->field_0x24;

    PSMTXTrans(transMtx, data->x, data->y, data->z);
    PSMTXScale(scaleMtx, data->scale, data->scale, data->scale);
    PSMTXConcat(transMtx, scaleMtx, baseMtx);
    PSMTXConcat(camera->view, baseMtx, baseMtx);

    GXSetTevColor(GX_TEVREG0, (GXColor){ data->field_0x18, data->field_0x1C, data->field_0x20, alpha });
    GXSetTevColor(GX_TEVREG1, (GXColor){ 0, 0, 0, 0 });

    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_TEXC, GX_CC_C0, GX_CC_C1, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, 0x7D);
    PSMTXScale(scaleMtx, float_0p03125_8041ecf0, float_0p03125_8041ecf0, float_0_8041ecf4);
    GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);
    effGetTexObjN64(0x8C, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetCullMode(GX_CULL_NONE);
    effSetVtxDescN64((effVtxArray*)size12x12_tex32x32_vtx);

    for (i = 0; i < 25; i++) {
        if (data->field_0x6D0[i] == 0 || data->field_0x6D0[i] == 7) {
            continue;
        }
        PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041ecf8 * data->field_0x4DC[i]);
        PSMTXTrans(transMtx, data->field_0x2C[i], data->field_0x90[i], data->field_0xF4[i]);
        PSMTXConcat(rotMtx, transMtx, transMtx);
        PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041ecf8 * -data->field_0x4DC[i]);
        PSMTXConcat(transMtx, transMtx, transMtx);
        PSMTXRotRad(rotMtx, 'z', float_deg2rad_8041ecf8 * data->field_0x478[i]);
        PSMTXConcat(transMtx, rotMtx, transMtx);
        PSMTXScale(scaleMtx, data->field_0x3B0[i], data->field_0x414[i], float_1_8041ecfc);
        PSMTXConcat(transMtx, scaleMtx, transMtx);
        PSMTXConcat(baseMtx, transMtx, transMtx);
        GXLoadPosMtxImm(transMtx, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        GXBegin(0x90, GX_VTXFMT0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}
