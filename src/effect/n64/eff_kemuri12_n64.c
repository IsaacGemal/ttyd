#include "eff/n64/eff_kemuri12_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "system.h"

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_80423194;
__declspec(section ".sdata") GXColor unk_80423190;


#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sdata2 float constants (named after their original addresses).

// .sbss2 zero GXColor scratch objects the original builds its TEV colors from.

// .rodata constants (named after their original addresses).
extern const Vec vec3_802faa28;

typedef struct effKemuri12Data {
    u16 type;        // 0x0
    u8 pad_2[2];     // 0x2
    Vec position;    // 0x4
    f32 field_0x10;  // 0x10
    s32 field_0x14;  // 0x14
    s32 field_0x18;  // 0x18
    s32 field_0x1c;  // 0x1c
    f32 field_0x20;  // 0x20
    u8 color[4];     // 0x24
} effKemuri12Data;

// .data vertex data for the two Kemuri12 quad meshes. The original keeps the
// first object local and the second global; kept as words and cast at the call.
static u32 size32x32_tex32x32_vtx[0x38 / 4] = {
    0xFFF0FFF0, 0x00000000, 0x00000000, 0x00FF0010, 0xFFF00000, 0xFC000000, 0x000000FF,
    0x00100010, 0x0000FC00, 0x04000000, 0x00FFFFF0, 0x00100000, 0x00000400, 0x000000FF,
};

u32 size32x32_tex32x32_yougan_vtx[0xA8 / 4] = {
    0xFFF0FFF0, 0x000F0000, 0x00000000, 0x00FF0010, 0xFFF0000F, 0xFC000000, 0x000000FF,
    0x00100010, 0x000FFC00, 0x04000000, 0x00FFFFF0, 0x0010000F, 0x00000400, 0x000000FF,
    0xFFFAFFF0, 0xFFF10000, 0x00000000, 0x00FF001A, 0xFFF0FFF1, 0xFC000000, 0x000000FF,
    0x001A0010, 0xFFF1FC00, 0x04000000, 0x00FFFFFA, 0x0010FFF1, 0x00000400, 0x000000FF,
    0xFFE6FFF0, 0xFFF10000, 0x00000000, 0x00FF0006, 0xFFF0FFF1, 0xFC000000, 0x000000FF,
    0x00060010, 0xFFF1FC00, 0x04000000, 0x00FFFFE6, 0x0010FFF1, 0x00000400, 0x000000FF,
};

// Local copy of system.c's inline helper so it inlines here (as in eff_kemuri11.c).
static inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

EffectEntry* effKemuri12N64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale) {
    EffectEntry* effect;
    effKemuri12Data* data;

    effect = effEntry();
    effect->type = "Kemuri12N64";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(effKemuri12Data));
    effect->userdata = data;
    effect->callback = effKemuri12Main;
    data->field_0x14 = life;
    data->field_0x18 = 0;
    data->field_0x1c = life;
    data->field_0x20 = 0.0f;
    data->type = type;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->field_0x10 = scale;

    // The colour stores are ordered to reproduce the original's store scheduling.
    switch (type) {
    case 0:
        data->color[2] = 0x00;
        data->color[1] = 0x00;
        data->color[0] = 0x00;
        data->color[3] = 0x82;
        break;
    case 1:
        data->color[0] = 0xff;
        data->color[2] = 0x00;
        data->color[1] = 0x00;
        data->color[3] = 0x82;
        break;
    default:
        data->color[0] = 0xe1;
        data->color[1] = 0xd7;
        data->color[2] = 0xff;
        data->color[3] = 0xb4;
        break;
    }
    return effect;
}

void effKemuri12Main(EffectEntry* effect) {
    EffectEntry* e;
    effKemuri12Data* data;
    Vec position;

    e = effect;
    data = e->userdata;
    position = initializeVec(&data->position);
    data->field_0x20 = 8.0f * (f32)data->field_0x18 / (f32)data->field_0x1c;
    data->field_0x18++;
    data->field_0x14--;
    if (data->field_0x14 < 0) {
        effDelete(e);
        return;
    }

    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKemuri12Disp, e);
}

// Building each GXColor *inside* the inline and calling GXSetTevColor there makes MWCC
// allocate each colour's local+outgoing-arg pair adjacently (r1+0x10/0x14, r1+0x8/0xc),
// as the original does.
#ifdef MUST_MATCH
static inline void setKemuriColor(GXTevRegID id, u8 r, u8 g, u8 b, u8 a) {
    GXColor c = unk_80423190;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
    GXSetTevColor(id, c);
}

static inline void setKemuriAlphaColor(GXTevRegID id, u8 a) {
    GXColor c = unk_80423194;
    c.a = a;
    GXSetTevColor(id, c);
}
#endif

void effKemuri12Disp(CameraId camId, void* param) {
    EffectEntry* effect;
    effKemuri12Data* data;
    CameraEntry* cam;
    GXTexObj texobj;
    Mtx m0, m1, m2;
    s32 type;
    s32 ia;
    s32 scaled;
    s32 frac256;
    s32 isLast;
    u32* vtx;

    effect = param;
    cam = camGetPtr(camId);
    data = effect->userdata;
    type = data->type;
    ia = (s32)data->field_0x20;
    frac256 = (s32)(256.0f * (data->field_0x20 - (f32)ia));
    isLast = (ia == 7) ? 1 : 0;

    GXSetNumChans(0);
    GXSetNumTevStages(3);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_TEXA, GX_CA_A1, GX_CA_ZERO);
    GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
    effGetTexObjN64(0xE, &texobj);
    GXLoadTexObj(&texobj, GX_TEXMAP0);
    GXLoadTexObj(&texobj, GX_TEXMAP1);

    PSMTXTrans(m0, data->position.x, data->position.y, data->position.z);
    PSMTXRotRad(m2, 'y', -camGetPtr(CAMERA_3D)->viewYaw * 0.017453292f);
    PSMTXScale(m1, data->field_0x10, data->field_0x10, data->field_0x10);
    PSMTXConcat(m0, m2, m0);
    PSMTXConcat(m0, m1, m0);
    PSMTXRotRad(m2, 'z', 0.34906584f);
    PSMTXConcat(m0, m2, m0);
    PSMTXConcat(cam->view, m0, m0);
    GXLoadPosMtxImm(m0, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);

#ifdef MUST_MATCH
    setKemuriColor(GX_TEVREG0, data->color[0], data->color[1], data->color[2], data->color[3]);
    setKemuriAlphaColor(GX_TEVREG1, frac256);
#else
    {
        GXColor color = unk_80423190;
        color.r = data->color[0];
        color.g = data->color[1];
        color.b = data->color[2];
        color.a = data->color[3];
        GXSetTevColor(GX_TEVREG0, color);
    }
    {
        GXColor color = unk_80423194;
        color.a = frac256;
        GXSetTevColor(GX_TEVREG1, color);
    }
#endif

    GXSetNumTexGens(2);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);

    scaled = ia * 32;
    PSMTXScale(m1, 0.00390625f, 0.03125f, 0.0f);
    PSMTXTrans(m0, (f32)scaled, 0.0f, 0.0f);
    PSMTXConcat(m1, m0, m1);
    GXLoadTexMtxImm(m1, GX_TEXMTX0, GX_MTX2x4);
    if (isLast) {
        PSMTXScale(m1, 0.00390625f, 0.03125f, 0.0f);
        PSMTXTrans(m0, (f32)(scaled + 32), 32.0f, 0.0f);
        PSMTXConcat(m1, m0, m1);
        GXLoadTexMtxImm(m1, GX_TEXMTX1, GX_MTX2x4);
    }
    else {
        PSMTXScale(m1, 0.00390625f, 0.03125f, 0.0f);
        PSMTXTrans(m0, (f32)(scaled + 32), 0.0f, 0.0f);
        PSMTXConcat(m1, m0, m1);
        GXLoadTexMtxImm(m1, GX_TEXMTX1, GX_MTX2x4);
    }

    switch (type) {
    case 0:
    case 2:
        vtx = size32x32_tex32x32_vtx;
        break;
    default:
        vtx = size32x32_tex32x32_yougan_vtx;
        break;
    }
    GXSetCullMode(GX_CULL_NONE);
    effSetVtxDescN64((effVtxArray*)vtx);

    switch (type) {
    case 0:
    case 2:
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
        break;
    default:
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0x12);
        tri2(8, 9, 10, 0, 8, 10, 11, 0);
        tri2(4, 5, 6, 0, 4, 6, 7, 0);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
        break;
    }
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
