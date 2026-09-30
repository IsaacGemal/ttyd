#include "eff/n64/eff_kemuri11_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "eff/n64/eff_kemuri1_n64.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math.h>

// .sdata2 float constants (named after their original addresses).
extern const f32 float_deg2rad_8041f4e8;
extern const f32 float_0p3_8041f4ec;
extern const f32 float_0p015625_8041f4f0;
extern const f32 float_0p03125_8041f4f4;
extern const f32 float_1_8041f4f8;
extern const f32 float_0_8041f4fc;
extern const f32 float_0p25_8041f500;
extern const f32 float_0p125_8041f504;
extern const f32 float_0p8_8041f508;
extern const f32 float_0p94_8041f50c;
extern const f32 float_6p2832_8041f510;
extern const f32 float_360_8041f514;
extern const f32 float_0p5_8041f518;
extern const f32 float_32_8041f51c;
extern const f32 float_8_8041f520;
extern const f32 float_16_8041f524;
extern const f32 float_0p9_8041f528;
extern const f32 float_0p75_8041f52c;
extern const f32 float_0p99_8041f530;
extern const f32 float_0p88_8041f534;
extern const f32 float_0p98_8041f538;
extern const f32 float_0p95_8041f53c;
extern const f32 float_1p002_8041f540;
extern const f32 float_128_8041f544;

// .sbss2 zero GXColor scratch objects the original builds its TEV colors from.
extern GXColor unk_80423180;
extern GXColor unk_80423184;
extern GXColor unk_80423188;
extern GXColor unk_8042318c;

// .rodata constants (named after their original addresses).
const Vec vec3_802faa00 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802faa10 = 4503601774854144.0;
const char str_802FAA18[] = "Kemuri11N64";

typedef struct effKemuri11Data {
    u16 type;        // 0x0
    u8 pad_2[2];     // 0x2
    Vec position;    // 0x4
    f32 field_0x10;  // 0x10
    f32 field_0x14;  // 0x14
    f32 field_0x18;  // 0x18
    f32 field_0x1C;  // 0x1C
    f32 field_0x20;  // 0x20
    f32 field_0x24;  // 0x24
    s32 field_0x28;  // 0x28
    s32 field_0x2C;  // 0x2C
    f32 field_0x30;  // 0x30
    f32 field_0x34;  // 0x34
    s32 field_0x38;  // 0x38
    f32 field_0x3C;  // 0x3C
    f32 field_0x40;  // 0x40
    f32 field_0x44;  // 0x44
    f32 field_0x48;  // 0x48
    s32 field_0x4C;  // 0x4C
    u8 color[6];     // 0x50
    u8 pad_56[2];    // 0x56
} effKemuri11Data;

// Vertex data for the hammer smoke mesh. The original is a raw 0x118-byte
// .data object; keep it as words and pass its address to effSetVtxDescN64.
static u32 stg_thruhammer_v[0x118 / 4] = {
    0xFFDEFFFE, 0x001410AB, 0x0400FFFF, 0xFFFFFFCC, 0x00320000, 0x0F000800, 0xFFFFFF00,
    0xFFDEFFFE, 0xFFEC0D55, 0x0400FFFF, 0xFFFFFFCC, 0x0032FFE2, 0x0D550800, 0xFFFFFF00,
    0xFFE60032, 0xFFD30BAB, 0x0800FFFF, 0xFF000000, 0xFFFEFFD8, 0x0A000400, 0xFFFFFFFF,
    0x00000032, 0xFFC40A00, 0x0800FFFF, 0xFF00001A, 0x0032FFD3, 0x08550800, 0xFFFFFF00,
    0x0022FFFE, 0xFFEC06AB, 0x0400FFFF, 0xFFFFFFCC, 0x0032001E, 0x10AB0800, 0xFFFFFF00,
    0xFFE60032, 0x002D1255, 0x0800FFFF, 0xFF000000, 0xFFFE0028, 0x14000400, 0xFFFFFFFF,
    0x00000032, 0x003C1400, 0x0800FFFF, 0xFF00001A, 0x0032002D, 0x15AB0800, 0xFFFFFF00,
    0x00340032, 0xFFE206AB, 0x0800FFFF, 0xFF000034, 0x00320000, 0x05000800, 0xFFFFFF00,
    0x0022FFFE, 0x00140355, 0x0400FFFF, 0xFFFF0034, 0x0032001E, 0x03550800, 0xFFFFFF00,
    0x001A0032, 0x002D01AB, 0x0800FFFF, 0xFF000000, 0xFFFE0028, 0x00000400, 0xFFFFFFFF,
};

// Local copy of system.c's inline helper so it inlines here (as in eff_rankup.c).
inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

static void main_dl(EffectEntry* effect, Mtx matrix) {
    effKemuri11Data* data;
    Mtx m0, m1, m2;
    s32 i;

    data = effect->userdata;
    data++;
    for (i = 1; i < effect->count; i++, data++) {
        if (data->field_0x4C < 0) {
            continue;
        }
        PSMTXTrans(m0, data->position.x, data->position.y, data->position.z);
        PSMTXRotRad(m1, 'z', float_deg2rad_8041f4e8 * data->field_0x34);
        PSMTXConcat(m0, m1, m0);
        PSMTXScale(m2, float_0p3_8041f4ec * data->field_0x1C, float_0p3_8041f4ec * data->field_0x20, float_0p3_8041f4ec * data->field_0x24);
        PSMTXConcat(m0, m2, m0);
        PSMTXRotRad(m1, 'y', float_deg2rad_8041f4e8 * data->field_0x30);
        PSMTXConcat(m0, m1, m0);
        PSMTXConcat(matrix, m0, m0);
        GXLoadPosMtxImm(m0, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        PSMTXScale(m2, float_0p015625_8041f4f0, float_0p03125_8041f4f4, float_1_8041f4f8);
        PSMTXTrans(m0, float_0_8041f4fc, -data->field_0x3C * float_0p25_8041f500, float_0_8041f4fc);
        PSMTXConcat(m2, m0, m2);
        GXLoadTexMtxImm(m2, GX_TEXMTX0, GX_MTX2x4);
        PSMTXScale(m2, float_0p125_8041f504, float_0p125_8041f504, float_1_8041f4f8);
        PSMTXTrans(m0, float_0_8041f4fc, -data->field_0x44, float_0_8041f4fc);
        PSMTXConcat(m2, m0, m2);
        GXLoadTexMtxImm(m2, GX_TEXMTX1, GX_MTX2x4);
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0x36);
        tri2(0, 1, 2, 0, 2, 3, 4, 0);
        tri2(2, 4, 5, 0, 1, 3, 2, 0);
        tri2(4, 6, 5, 0, 5, 6, 7, 0);
        tri2(5, 7, 8, 0, 0, 9, 1, 0);
        tri2(10, 9, 0, 0, 11, 10, 0, 0);
        tri2(11, 12, 10, 0, 13, 12, 11, 0);
        tri2(8, 14, 15, 0, 7, 14, 8, 0);
        tri2(8, 15, 16, 0, 15, 17, 16, 0);
        tri2(16, 17, 18, 0, 16, 18, 19, 0);
    }
}

// NONMATCHING (97.62%): register allocation of the cached colour bytes and
// effect/cam/data pointers differs, plus a branch-sense and stack-slot delta.
void effKemuri11Disp(CameraId camId, void* param) {
    EffectEntry* effect;
    effKemuri11Data* data;
    CameraEntry* cam;
    GXTexObj texobj;
    Mtx m0, m1, m2;
    s32 c0, c1, c2, c3, c4, c5;
    s32 d0, d1, d2, d3, d4, d5;
    s32 type;

    effect = param;
    cam = camGetPtr(camId);
    data = effect->userdata;
    type = data->type;
    c0 = data->color[0];
    c1 = data->color[1];
    c2 = data->color[2];
    c3 = data->color[3];
    c4 = data->color[4];
    c5 = data->color[5];
    d0 = c0;
    d1 = c1;
    d2 = c2;
    d3 = c3;
    d4 = c4;
    d5 = c5;
    if (type > 1) {
        d0 = c0 >> 2;
        d1 = c1 >> 2;
        d2 = c2 >> 2;
        d3 = c3 >> 2;
        d4 = c4 >> 2;
        d5 = c5 >> 2;
    }

    GXSetNumChans(0);
    GXSetNumTexGens(2);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, 125);
    GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, 125);

    if (type < 2 && type >= 0) {
        GXSetNumTevStages(2);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
        GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_C1, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        effGetTexObjN64(0x23, &texobj);
        GXLoadTexObj(&texobj, GX_TEXMAP0);
        effGetTexObjN64(0x24, &texobj);
        GXLoadTexObj(&texobj, GX_TEXMAP1);
    }
    else {
        GXSetNumTevStages(3);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_ZERO, GX_CC_TEXC, GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A0, GX_CA_APREV, GX_CA_ZERO);
        GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_C0, GX_CC_C1, GX_CC_CPREV, GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        effGetTexObjN64(0x25, &texobj);
        GXLoadTexObj(&texobj, GX_TEXMAP0);
        effGetTexObjN64(0x24, &texobj);
        GXLoadTexObj(&texobj, GX_TEXMAP1);
    }

    effSetVtxDescN64((effVtxArray*)stg_thruhammer_v);
    PSMTXTrans(m0, data->position.x, data->position.y, data->position.z);
    PSMTXRotRad(m1, 'y', -camGetPtr(CAMERA_3D)->viewYaw * float_deg2rad_8041f4e8);
    PSMTXConcat(m0, m1, m2);
    PSMTXConcat(cam->view, m2, m2);

    {
        GXColor color = unk_80423180;
        color.r = d0;
        color.g = d1;
        color.b = d2;
        color.a = data->field_0x38;
        GXSetTevColor(GX_TEVREG0, color);
    }
    {
        GXColor color = unk_80423184;
        color.r = d3;
        color.g = d4;
        color.b = d5;
        GXSetTevColor(GX_TEVREG1, color);
    }
    GXSetCullMode(GX_CULL_BACK);
    main_dl(effect, m2);
    {
        GXColor color = unk_80423188;
        color.r = c0;
        color.g = c1;
        color.b = c2;
        color.a = data->field_0x38;
        GXSetTevColor(GX_TEVREG0, color);
    }
    {
        GXColor color = unk_8042318c;
        color.r = c3;
        color.g = c4;
        color.b = c5;
        GXSetTevColor(GX_TEVREG1, color);
    }
    GXSetCullMode(GX_CULL_FRONT);
    main_dl(effect, m2);
}

// NONMATCHING (93.85%): outer type check compiles to `blt` instead of the
// original `bge`, and several float/stack-slot registers differ (sin/cos
// temporaries, the 0.5p/0.94 constants, the int->float stack scratch pairs).
void effKemuri11Main(EffectEntry* effect) {
    EffectEntry* e;
    effKemuri11Data* data;
    Vec position;
    s32 i;
    s32 type;
    s32 t;
    f32 v;

    e = effect;
    data = e->userdata;
    position = initializeVec(&data->position);
    data->field_0x2C++;
    data->field_0x28--;
    if (data->field_0x28 < 0) {
        effDelete(e);
        return;
    }

    type = data->type;
    t = data->field_0x2C;
    if (type < 2 && type >= 0) {
        if (t >= 8) {
            data->field_0x38 = (f32)data->field_0x38 * float_0p8_8041f508;
        }
    }
    else {
        if (t >= 8) {
            data->field_0x38 = (f32)data->field_0x38 * float_0p94_8041f50c;
        }
    }

    for (i = 1; i < e->count; i++, data++) {
        data->field_0x4C++;
        if (data->field_0x4C < 0) {
            continue;
        }
        if (data->field_0x4C == 0) {
            data->position.x = float_0_8041f4fc;
            data->position.y = float_0_8041f4fc;
            data->position.z = float_0_8041f4fc;
            data->field_0x10 = -(f32)sin(float_6p2832_8041f510 * data->field_0x34 / float_360_8041f514) * float_0p5_8041f518;
            data->field_0x14 = float_0p5_8041f518 * (f32)cos(float_6p2832_8041f510 * data->field_0x34 / float_360_8041f514);
            data->field_0x18 = float_0_8041f4fc;
            data->field_0x3C = float_32_8041f51c;
            data->field_0x40 = float_32_8041f51c;
            data->field_0x48 = float_32_8041f51c;
            data->field_0x20 = float_8_8041f520 - (f32)i;
            switch (type) {
            case 0:
            case 1:
                v = float_0p5_8041f518 + (f32)i;
                data->field_0x1C = v;
                data->field_0x24 = float_0p5_8041f518 * v;
                break;
            case 2:
            case 5:
                v = (f32)(i + 2);
                data->field_0x1C = v;
                data->field_0x24 = float_0p5_8041f518 * v;
                break;
            case 3:
                v = (f32)(i + 3);
                data->field_0x1C = v;
                data->field_0x24 = float_0p5_8041f518 * v;
                data->field_0x3C = float_16_8041f524;
                data->field_0x40 = float_16_8041f524;
                data->field_0x48 = float_16_8041f524;
                break;
            default:
                v = float_0p5_8041f518 * (f32)(i + 3);
                data->field_0x1C = v;
                data->field_0x24 = float_0p5_8041f518 * v;
                data->field_0x20 *= float_0p8_8041f508;
                data->field_0x3C = float_16_8041f524;
                if (type == 6 || type == 7) {
                    data->field_0x40 = float_8_8041f520;
                    data->field_0x48 = float_8_8041f520;
                }
                else {
                    data->field_0x40 = float_16_8041f524;
                    data->field_0x48 = float_16_8041f524;
                }
                break;
            }
            data->field_0x44 = float_32_8041f51c;
        }
        data->field_0x3C += data->field_0x40;
        data->field_0x44 += data->field_0x48;
        switch (type) {
        case 0:
        case 1:
            data->field_0x40 *= float_0p9_8041f528;
            break;
        case 2:
        case 5:
            data->field_0x40 *= float_0p75_8041f52c;
            data->field_0x48 *= float_0p99_8041f530;
            break;
        default:
            if (type == 6 || type == 7) {
                data->field_0x40 *= float_0p88_8041f534;
                data->field_0x48 *= float_0p98_8041f538;
            }
            else {
                data->field_0x40 *= float_0p75_8041f52c;
                data->field_0x48 *= float_0p95_8041f53c;
            }
            break;
        }
        data->field_0x1C *= float_1p002_8041f540;
        data->field_0x24 *= float_1p002_8041f540;
        if (data->field_0x3C > float_128_8041f544) {
            data->field_0x3C = float_128_8041f544;
        }
    }

    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKemuri11Disp, e);
}

// NONMATCHING (96.47%): the particle-init loop's index expression is
// strength-reduced to precomputed 2*i/3*i induction variables instead of the
// original `subi/slwi|mulli/subfic` per-iteration form.
EffectEntry* effKemuri11N64Entry(s32 type, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    effKemuri11Data* data;
    s32 count;
    s32 i;
    s32 angle;

    effect = effEntry();
    switch (type) {
    case 0:
        type = 3;
        break;
    case 1:
        type = 2;
        break;
    case 2:
        type = 4;
        break;
    case 4:
        type = 6;
        break;
    case 5:
        type = 7;
        break;
    default:
        type = 5;
        break;
    }

    switch (type) {
    case 0:
        count = 3;
        break;
    case 1:
        count = 1;
        break;
    case 2:
    case 5:
        count = 3;
        break;
    default:
        count = 2;
        break;
    }

    effect->type = str_802FAA18;
    effect->count = count + 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(effKemuri11Data) * (count + 1));
    effect->userdata = data;
    effect->callback = effKemuri11Main;
    data->field_0x28 = type >= 2 ? 60 : 30;
    data->field_0x2C = 0;
    data->type = type;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->field_0x38 = 0xff;

    switch (type) {
    case 0:
    case 1:
        data->color[0] = 0x00;
        data->color[1] = 0xff;
        data->color[2] = 0x7a;
        data->color[3] = 0xf0;
        data->color[4] = 0xff;
        data->color[5] = 0xfa;
        break;
    case 2:
        data->color[0] = 0x7d;
        data->color[1] = 0x78;
        data->color[2] = 0x64;
        data->color[3] = 0xff;
        data->color[4] = 0xff;
        data->color[5] = 0xf0;
        break;
    case 3:
        data->color[0] = 0xdc;
        data->color[1] = 0xd2;
        data->color[2] = 0xc8;
        data->color[3] = 0xff;
        data->color[4] = 0xff;
        data->color[5] = 0xfa;
        break;
    case 5:
        data->color[0] = 0xe1;
        data->color[1] = 0xcc;
        data->color[2] = 0x5d;
        data->color[3] = 0xe8;
        data->color[4] = 0xe7;
        data->color[5] = 0xab;
        break;
    case 6:
        data->field_0x28 = 50;
        data->color[0] = 0xd4;
        data->color[1] = 0xb7;
        data->color[2] = 0x05;
        data->color[3] = 0xc1;
        data->color[4] = 0xa3;
        data->color[5] = 0x09;
        break;
    case 7:
        data->field_0x28 = 50;
        data->color[0] = 0x21;
        data->color[1] = 0x8c;
        data->color[2] = 0x8d;
        data->color[3] = 0x18;
        data->color[4] = 0xa5;
        data->color[5] = 0xa3;
        break;
    case 4:
    default:
        data->field_0x28 = 50;
        data->color[0] = 0xd0;
        data->color[1] = 0x88;
        data->color[2] = 0x28;
        data->color[3] = 0xd8;
        data->color[4] = 0xa9;
        data->color[5] = 0x41;
        break;
    }

    angle = 30;
    {
        effKemuri11Data* p = data + 1;

        for (i = 1; i < count + 1; i++, p++) {
            if (type < 2) {
                if (type >= 0) {
                    p->field_0x4C = 1 - 2 * i;
                }
                else {
                    p->field_0x4C = 2 - 3 * i;
                }
            }
            else {
                p->field_0x4C = 2 - 3 * i;
            }
            p->field_0x34 = float_0_8041f4fc;
            p->field_0x30 = angle;
            angle += 30;
        }
    }

    if (type == 6) {
        effKemuri1N64Entry(5, x, y, z, float_0_8041f4fc);
    }
    else if (type == 7) {
        effKemuri1N64Entry(6, x, y, z, float_0_8041f4fc);
    }
    return effect;
}
