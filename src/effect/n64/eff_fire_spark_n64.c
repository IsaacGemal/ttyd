#include "eff/n64/eff_fire_spark_n64.h"
#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

typedef struct effFireSparkData {
    s32 type;             // 0x00
    f32 field_0x04;       // 0x04
    f32 field_0x08;       // 0x08
    f32 field_0x0C;       // 0x0C
    f32 field_0x10;       // 0x10
    f32 field_0x14;       // 0x14
    s32 life;             // 0x18
    s32 field_0x1C;       // 0x1C
    s32 field_0x20;       // 0x20
    s32 field_0x24;       // 0x24
    s32 field_0x28;       // 0x28
    s32 field_0x2C;       // 0x2C
    s32 field_0x30;       // 0x30
    s32 field_0x34;       // 0x34
    s32 field_0x38;       // 0x38
    s32 field_0x3C;       // 0x3C
    f32 field_0x40;       // 0x40
    u8 field_0x44[0x18];  // 0x44
    f32 field_0x5C;       // 0x5C
    f32 field_0x60;       // 0x60
    s32 field_0x64;       // 0x64
    f32 field_0x68;       // 0x68
    f32 field_0x6C;       // 0x6C
} effFireSparkData;

// .sdata2 and .rodata constants named after their original addresses (defined by the split map).
extern const f32 float_30_8041eec0;
extern const f32 float_0p5_8041eeac;
extern const char str_802FA5C8[];
extern const f32 float_deg2rad_8041ee9c;
extern const f32 float_0p03125_8041eea0;
extern const f32 float_0p0078125_8041eea4;
extern const f32 float_0_8041eea8;
const f64 double_to_int_802fa5c0 = 4503601774854144.0;
extern const GXColor dat_8041ee98;
extern GXColor unk_80423100;
extern GXColor unk_80423104;
extern u32 size4x4_tex32x32_vtx[];

EffectEntry* effFireSparkN64Entry(s32 type, s32 count, s32 life, f32 a4, f32 a5, f32 a6, f32 a7, f32 a8, f32 a9, f32 a10, f32 a11) {
    EffectEntry* effect;
    effFireSparkData* data;
    effFireSparkData* ptr;
    f32 ratio;
    s32 i;

    effect = effEntry();
    effect->type = str_802FA5C8;
    effect->count = count;
    data = __memAlloc(HEAP_EFFECT, sizeof(effFireSparkData) * count);
    effect->userdata = data;
    effect->callback = effFireSparkMain;
    effect->flags |= 2;

    data->type = type;
    data->field_0x1C = 0;
    if (life <= 0) {
        data->life = 1000;
    } else {
        data->life = life;
    }
    data->field_0x2C = 0;
    data->field_0x04 = a4;
    data->field_0x08 = a5;
    data->field_0x0C = a6;
    data->field_0x40 = a9;
    data->field_0x10 = a7;
    data->field_0x14 = a8;
    data->field_0x68 = a10;
    data->field_0x6C = a11;
    data->field_0x20 = 0x7f;
    data->field_0x24 = 0;
    data->field_0x28 = 0;
    data->field_0x30 = 0x7f;
    data->field_0x34 = 0x7f;
    data->field_0x38 = 0;
    data->field_0x3C = 0xff;

    ratio = float_30_8041eec0 / (f32)(count - 1);
    ratio *= float_0p5_8041eeac;
    ptr = data + 1;
    for (i = 1; i < count; i++, ptr++) {
        ptr->field_0x64 = (s32)(-(f32)i * ratio) - 1;
    }

    return effect;
}

// NONMATCHING (97.29%): the two GXColor stack temporaries land at different slots (target locals 0x14/0xc, outgoing 0x18/0x10; ours locals 0x14/0x18, outgoing 0x10/0xc) and the loop's signed `i % 4 * 32` float conversion schedules its constant load one instruction differently (target r3/r4/r5, ours r0/r3/r4).
void effFireSparkDisp(CameraId camId, void* param) {
    EffectEntry* effect;
    effFireSparkData* data;
    CameraEntry* cam;
    GXTexObj texobj;
    Mtx m0;
    Mtx m2;
    Mtx m1;
    Mtx viewMtx;
    s32 i;
    s32 alpha;

    effect = param;
    cam = camGetPtr(camId);
    data = effect->userdata;
    alpha = data->field_0x2C;

    PSMTXTrans(m0, data->field_0x04, data->field_0x08, data->field_0x0C);
    PSMTXRotRad(m1, 'y', -camGetPtr(CAMERA_3D)->viewYaw * float_deg2rad_8041ee9c);
    PSMTXScale(m2, data->field_0x40, data->field_0x40, data->field_0x40);
    PSMTXConcat(m0, m1, m0);
    PSMTXConcat(m0, m2, m0);
    PSMTXConcat(cam->view, m0, viewMtx);

    {
        GXColor color2;
        GXColor color1;

        color1 = unk_80423100;
        color1.r = data->field_0x20;
        color1.g = data->field_0x24;
        color1.b = data->field_0x28;
        color1.a = alpha;
        GXSetTevColor(GX_TEVREG0, color1);
        color2 = unk_80423104;
        color2.r = data->field_0x30;
        color2.g = data->field_0x34;
        color2.b = data->field_0x38;
        color2.a = data->field_0x3C;
        GXSetTevColor(GX_TEVREG1, color2);
    }

    GXSetNumChans(0);
    GXSetNumTevStages(2);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C1, GX_CC_C0);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_KONST, GX_CC_C1, GX_CC_CPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
    GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0);
    GXSetNumTexGens(2);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, 125);
    GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, 125);
    GXSetTevKColor(GX_KCOLOR0, dat_8041ee98);
    effGetTexObjN64(0x8B, &texobj);
    GXLoadTexObj(&texobj, GX_TEXMAP0);
    effGetTexObjN64(0x65, &texobj);
    GXLoadTexObj(&texobj, GX_TEXMAP1);
    GXSetCullMode(GX_CULL_NONE);
    effSetVtxDescN64((effVtxArray*)size4x4_tex32x32_vtx);

    data++;
    for (i = 1; i < effect->count; i++, data++) {
        if (data->field_0x64 < 0) {
            continue;
        }
        PSMTXTrans(m0, data->field_0x04, data->field_0x08, data->field_0x0C);
        PSMTXConcat(viewMtx, m0, m0);
        GXLoadPosMtxImm(m0, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        PSMTXScale(m2, float_0p03125_8041eea0, float_0p0078125_8041eea4, float_0_8041eea8);
        PSMTXTrans(m0, float_0_8041eea8, (f32)(i % 4 * 32), float_0_8041eea8);
        PSMTXConcat(m2, m0, m2);
        GXLoadTexMtxImm(m2, GX_TEXMTX0, GX_MTX2x4);
        PSMTXScale(m2, float_0p03125_8041eea0, float_0p03125_8041eea0, float_0_8041eea8);
        PSMTXTrans(m0, data->field_0x5C, data->field_0x60, float_0_8041eea8);
        PSMTXConcat(m2, m0, m2);
        GXLoadTexMtxImm(m2, GX_TEXMTX1, GX_MTX2x4);
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}
