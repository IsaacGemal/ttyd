#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"
#include "eff/n64/eff_fire_dust_n64.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .rodata string used as the EffectEntry type name.
const char str_802FA578[12] = "FireDustN64";

// .sdata2 float constants (named after their original addresses).
extern const f32 float_1_8041ee54;
extern const f32 float_20_8041ee58;
extern const f32 float_0p0056818_8041ee18;
extern const f32 float_0p045455_8041ee1c;
extern const f32 float_0_8041ee20;
extern const f32 float_2p0944_8041ee24;
extern const f32 float_neg2p0944_8041ee28;
extern const f32 float_255_8041ee10;
extern const f32 float_deg2rad_8041ee14;

// .sbss2 zero GXColor scratch the original builds the TEV color from.
extern GXColor unk_804230f8;
extern GXColor unk_804230fc;

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

// Same by-value GXColor build, for the per-particle colour main_dl passes to
// GXSetTevColor; the helper scope reproduces the original's stack slots.
static inline void effFireDustSetVtxColor(s32 r, s32 g, s32 b, u8 a) {
    GXColor color = unk_804230fc;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    GXSetTevColor(GX_TEVREG0, color);
}

// NOTE: this file's functions are emitted in reverse source order. In the original the
// order is main_dl, effFireDustDisp, effFireDustMain, effFireDustN64Entry; Entry is here,
// the not-yet-written Main belongs below it, and main_dl is last.

EffectEntry* effFireDustN64Entry(s32 type, s32 a2, s32 a3, f32 x, f32 y, f32 z, f32 a7, f32 a8) {
    EffectEntry* effect;
    effFireDustData* data;
    effFireDustData* d;
    s32 i;
    f32 ratio;

    effect = effEntry();
    effect->type = str_802FA578;
    effect->count = a2 + 2;
    data = __memAlloc(3, (a2 + 2) * 0x6c);
    effect->userdata = data;
    effect->callback = effFireDustMain;
    effect->flags |= 2;

    data->type = type;
    data->field_0x3C = 0;
    if (a3 <= 0) {
        data->field_0x38 = 1000;
    } else {
        data->field_0x38 = a3;
    }
    d = data + 1;
    data->field_0x4C = 0xff;
    data->x = x;
    data->y = y;
    data->z = z;
    data->field_0x60 = float_1_8041ee54;
    data->field_0x10 = a7;
    data->field_0x14 = a8;
    data->field_0x40 = 0xff;
    data->field_0x44 = 0xff;
    data->field_0x48 = 0xff;
    data->field_0x50 = 0xff;
    data->field_0x54 = 0xff;
    data->field_0x58 = 0x7f;
    data->field_0x5C = 0xff;

    if (a3 < 0x14) {
        ratio = float_20_8041ee58 / (f32)(a2 + 1);
    } else {
        ratio = (f32)a3 / (f32)(a2 + 1);
    }

    for (i = 1; i < a2 + 2; i++, d++) {
        d->field_0x30 = (s32)(-(f32)i * ratio) - 1;
    }

    return effect;
}

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

// .data vertex strip; the original is a raw 0x1C0-byte object and main_dl
// passes 0x38-byte slices of it to effSetVtxDescN64.
static u32 size8x8_tex22x22_vtx[0x70] = {
    0xFFF8FFF8, 0x00000000, 0x00000000, 0x00FF0008, 0xFFF80000, 0x02C00000,
    0x000000FF, 0x00080008, 0x000002C0, 0x02C00000, 0x00FFFFF8, 0x00080000,
    0x000002C0, 0x000000FF, 0xFFF8FFF8, 0x000002C0, 0x00000000, 0x00FF0008,
    0xFFF80000, 0x05800000, 0x000000FF, 0x00080008, 0x00000580, 0x02C00000,
    0x00FFFFF8, 0x00080000, 0x02C002C0, 0x000000FF, 0xFFF8FFF8, 0x00000580,
    0x00000000, 0x00FF0008, 0xFFF80000, 0x08400000, 0x000000FF, 0x00080008,
    0x00000840, 0x02C00000, 0x00FFFFF8, 0x00080000, 0x058002C0, 0x000000FF,
    0xFFF8FFF8, 0x00000840, 0x00000000, 0x00FF0008, 0xFFF80000, 0x0B000000,
    0x000000FF, 0x00080008, 0x00000B00, 0x02C00000, 0x00FFFFF8, 0x00080000,
    0x084002C0, 0x000000FF, 0xFFF8FFF8, 0x00000B00, 0x00000000, 0x00FF0008,
    0xFFF80000, 0x0DC00000, 0x000000FF, 0x00080008, 0x00000DC0, 0x02C00000,
    0x00FFFFF8, 0x00080000, 0x0B0002C0, 0x000000FF, 0xFFF8FFF8, 0x00000DC0,
    0x00000000, 0x00FF0008, 0xFFF80000, 0x10800000, 0x000000FF, 0x00080008,
    0x00001080, 0x02C00000, 0x00FFFFF8, 0x00080000, 0x0DC002C0, 0x000000FF,
    0xFFF8FFF8, 0x00001080, 0x00000000, 0x00FF0008, 0xFFF80000, 0x13400000,
    0x000000FF, 0x00080008, 0x00001340, 0x02C00000, 0x00FFFFF8, 0x00080000,
    0x108002C0, 0x000000FF, 0xFFF8FFF8, 0x00001340, 0x00000000, 0x00FF0008,
    0xFFF80000, 0x16000000, 0x000000FF, 0x00080008, 0x00001600, 0x02C00000,
    0x00FFFFF8, 0x00080000, 0x134002C0, 0x000000FF,
};

static void main_dl(EffectEntry* effect, Mtx matrix) {
    effFireDustData* data;
    Mtx mtx, scale, rot;
    s32 c0, c1, c2;
    s32 i;
    effVtxArray* vtx;
    f32 alphaScale;
    f32 v;

    data = effect->userdata;
    c0 = data->field_0x50;
    c1 = data->field_0x54;
    c2 = data->field_0x58;
    alphaScale = (f32)data->field_0x4C / float_255_8041ee10;
    data++;
    for (i = 1; i < effect->count; i++, data++) {
        if (data->field_0x30 < 0) {
            continue;
        }
        PSMTXTrans(mtx, data->x, data->y, data->z);
        PSMTXRotRad(rot, 'z', float_deg2rad_8041ee14 * data->field_0x34);
        v = data->field_0x60 * alphaScale;
        PSMTXScale(scale, v, v, v);
        PSMTXConcat(mtx, rot, mtx);
        PSMTXConcat(mtx, scale, mtx);
        PSMTXConcat(matrix, mtx, mtx);
        GXLoadPosMtxImm(mtx, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        effFireDustSetVtxColor(c0, c1, c2, (u8)((f32)data->field_0x4C * alphaScale));
        switch (i & 7) {
            case 0:
                vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0xA8);
                break;
            case 1:
                vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0xE0);
                break;
            case 2:
                vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x118);
                break;
            case 3:
                vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x150);
                break;
            case 4:
                vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x188);
                break;
            case 5:
                vtx = (effVtxArray*)size8x8_tex22x22_vtx;
                break;
            case 6:
                vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x38);
                break;
            case 7:
                vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x70);
                break;
        }
        effSetVtxDescN64(vtx);
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}
