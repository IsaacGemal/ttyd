#include "eff/n64/eff_kemuri10_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"

#include <dolphin/mtx.h>

u32 stg_houki_kemuri_v[126] = {
    0x0000FFCE, 0x00140000, 0x0200FFFF, 0xFFFF0023,
    0xFFDD0014, 0x01000200, 0xFFFFFFFF, 0x0015FFEB,
    0x00000100, 0x0100FFFF, 0xFFFF0000, 0xFFE20000,
    0x00000100, 0xFFFFFFFF, 0x0023FFDD, 0xFFEC0100,
    0x0000FFFF, 0xFFFF0000, 0xFFCEFFEC, 0x00000000,
    0xFFFFFFFF, 0x001E0000, 0x00000200, 0x0100FFFF,
    0xFFFF0032, 0x0000FFEC, 0x02000000, 0xFFFFFFFF,
    0x00320000, 0x00140200, 0x0200FFFF, 0xFFFF0046,
    0x00000000, 0x02000100, 0xFFFFFFFF, 0x0031FFCF,
    0x00000100, 0x0100FFFF, 0xFFFF0000, 0xFFBA0000,
    0x00000100, 0xFFFFFFFF, 0x00150015, 0x00000300,
    0x0100FFFF, 0xFFFF0023, 0x0023FFEC, 0x03000000,
    0xFFFFFFFF, 0x00230023, 0x00140300, 0x0200FFFF,
    0xFFFF0031, 0x00310000, 0x03000100, 0xFFFFFFFF,
    0xFFDDFFDD, 0x00140700, 0x0200FFFF, 0xFFFF0000,
    0xFFCE0014, 0x08000200, 0xFFFFFFFF, 0x0000FFE2,
    0x00000800, 0x0100FFFF, 0xFFFFFFEB, 0xFFEB0000,
    0x07000100, 0xFFFFFFFF, 0xFFCFFFCF, 0x00000700,
    0x0100FFFF, 0xFFFF0000, 0xFFBA0000, 0x08000100,
    0xFFFFFFFF, 0xFFBA0000, 0x00000600, 0x0100FFFF,
    0xFFFFFFCE, 0x00000014, 0x06000200, 0xFFFFFFFF,
    0xFFE20000, 0x00000600, 0x0100FFFF, 0xFFFF0000,
    0xFFCEFFEC, 0x08000000, 0xFFFFFFFF, 0xFFDDFFDD,
    0xFFEC0700, 0x0000FFFF, 0xFFFFFFCE, 0x0000FFEC,
    0x06000000, 0xFFFFFFFF, 0xFFDD0023, 0x00140500,
    0x0200FFFF, 0xFFFFFFEB, 0x00150000, 0x05000100,
    0xFFFFFFFF, 0xFFDD0023, 0xFFEC0500, 0x0000FFFF,
    0xFFFFFFCF, 0x00310000, 0x05000100, 0xFFFFFFFF,
    0x0000001E, 0x00000400, 0x0100FFFF, 0xFFFF0000,
    0x0032FFEC, 0x04000000, 0xFFFFFFFF, 0x00000046,
    0x00000400, 0x0100FFFF, 0xFFFF0000, 0x00320014,
    0x04000200, 0xFFFFFFFF,
};

typedef struct effKemuri10Data {
    s32 field_0x0;    // 0x0
    Vec position;     // 0x4
    f32 field_0x10;   // 0x10
    f32 field_0x14;   // 0x14
    f32 field_0x18;   // 0x18
    f32 field_0x1C;   // 0x1C
    f32 field_0x20;   // 0x20
    f32 field_0x24;   // 0x24
    f32 field_0x28;   // 0x28
    f32 field_0x2C;   // 0x2C
    f32 field_0x30;   // 0x30
    f32 field_0x34;   // 0x34
    f32 field_0x38;   // 0x38
    f32 field_0x3C;   // 0x3C
    f32 field_0x40;   // 0x40
    f32 field_0x44;   // 0x44
    f32 field_0x48;   // 0x48
    f32 field_0x4C;   // 0x4C
    f32 field_0x50;   // 0x50
    f32 field_0x54;   // 0x54
    s32 field_0x58;   // 0x58
    s32 field_0x5C;   // 0x5C
    s32 field_0x60;   // 0x60
    u8 field_0x64[8]; // 0x64
    s32 field_0x6C;   // 0x6C
} effKemuri10Data;


// .sdata2 float constants (named after their original addresses).
extern const f32 float_0_8041f4b8;
extern const f32 float_0p3_8041f4bc;
extern const f32 float_0p8_8041f4c0;
extern const f32 float_0p6_8041f4c4;
extern const f32 float_0p7_8041f4c8;
extern const f32 float_1p05_8041f4cc;
extern const f32 float_1_8041f4d0;
extern const f32 float_4_8041f4d4;
extern const f32 float_278_8041f4d8;
extern const f32 float_32_8041f4dc;
extern const f32 float_0p1_8041f4e0;
extern const f32 float_0p015625_8041f4b0;
extern const f32 float_0p03125_8041f4b4;

// Scalar sqrtf the original inlines: same shape as the one in libc/math.h plus
// the NAN paths the original keeps (see eff_sstar_n64.c).
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
            }
            else {
                return FP_INFINITE;
            }
        case 0:
            if (*(s32*)&x & 0x007FFFFF) {
                return FP_SUBNORMAL;
            }
            else {
                return FP_ZERO;
            }
        default:
            return FP_NORMAL;
    }
}

#define fpclassify(x) __fpclassifyf(x)
#define isnan(x) (fpclassify(x) == FP_NAN)

static inline f32 sqrtf(f32 x) {
    const f64 _half = .5;
    const f64 _three = 3.0;

    if (x > float_0_8041f4b8) {
        f64 guess = __frsqrte(x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        return x * guess;
    }
    else if (x < 0.0) {
        return NAN;
    }
    else if (isnan(x)) {
        return NAN;
    }
    return x;
}

// Prevents MWCC from CSE-ing the separate float_0 / float_1 pool loads across
// the branch-and-mag blocks, which shifts every volatile FPR by one.
#pragma opt_common_subs off
EffectEntry* effKemuri10N64Entry(s32 a1, f32 x, f32 y, f32 z, f32 dirX, f32 dirY, f32 dirZ, f32 scale) {
    EffectEntry* effect;
    effKemuri10Data* data;
    f32 mag;
    f32 norm1, norm2, norm3;
    f32 upX, upY, upZ;
    f32 sideX, sideY, sideZ;

    effect = effEntry();
    if (dirX == float_0_8041f4b8 && dirY == float_0_8041f4b8 && dirZ == float_0_8041f4b8) {
        return NULL;
    }

    mag = dirZ * dirZ + (dirX * dirX + dirY * dirY);
    if (mag == float_0_8041f4b8) {
        return NULL;
    }
    norm1 = float_1_8041f4d0 / sqrtf(mag);
    dirX = dirX * norm1;
    dirY = dirY * norm1;
    dirZ = dirZ * norm1;

    if (dirX != float_0_8041f4b8 || dirZ != float_0_8041f4b8) {
        upX = dirZ;
        upY = float_0_8041f4b8;
        upZ = -dirX;
    }
    else {
        upX = float_1_8041f4d0;
        upZ = float_0_8041f4b8;
        upY = float_0_8041f4b8;
    }

    mag = upZ * upZ + (upX * upX + upY * upY);
    if (mag == float_0_8041f4b8) {
        return NULL;
    }
    norm2 = float_1_8041f4d0 / sqrtf(mag);
    upX = upX * norm2;
    upZ = upZ * norm2;
    upY = upY * norm2;

    sideX = upY * dirZ - upZ * dirY;
    sideY = upZ * dirX - upX * dirZ;
    sideZ = upX * dirY - upY * dirX;

    mag = sideZ * sideZ + (sideX * sideX + sideY * sideY);
    if (mag == float_0_8041f4b8) {
        return NULL;
    }
    norm3 = float_1_8041f4d0 / sqrtf(mag);
    sideX = sideX * norm3;
    sideY = sideY * norm3;
    sideZ = sideZ * norm3;

    effect->type = "Kemuri10N64";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, 0x70);
    effect->userdata = data;
    effect->callback = effKemuri10Main;

    data->field_0x0 = a1;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->field_0x10 = float_4_8041f4d4 * -dirX;
    data->field_0x14 = float_4_8041f4d4 * -dirY;
    data->field_0x18 = float_4_8041f4d4 * -dirZ;
    data->field_0x5C = 16;
    data->field_0x4C = float_278_8041f4d8;
    data->field_0x50 = float_32_8041f4dc;
    data->field_0x58 = 0;
    data->field_0x60 = 0;
    data->field_0x64[0] = 0xF8;
    data->field_0x64[1] = 0x5C;
    data->field_0x64[2] = 0xE5;
    data->field_0x64[3] = 0xF8;
    data->field_0x64[4] = 0x5C;
    data->field_0x64[5] = 0xE5;
    data->field_0x1C = upX;
    data->field_0x2C = upY;
    data->field_0x3C = upZ;
    data->field_0x20 = sideX;
    data->field_0x30 = sideY;
    data->field_0x40 = sideZ;
    data->field_0x24 = dirX;
    data->field_0x34 = dirY;
    data->field_0x44 = dirZ;
    data->field_0x28 = float_0_8041f4b8;
    data->field_0x38 = float_0_8041f4b8;
    data->field_0x48 = float_0_8041f4b8;
    data->field_0x54 = float_0p1_8041f4e0 * scale;
    data->field_0x6C = 4;

    return effect;
}
#pragma opt_common_subs reset

inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

void effKemuri10Main(EffectEntry* effect) {
    effKemuri10Data* data;
    Vec position2;
    Vec position;
    s32 camera;
    s32 phase;
    s32 i;

    data = effect->userdata;
    position = initializeVec(&data->position);
    position2 = position;
    camera = data->field_0x6C;
    data->field_0x5C--;
    data->field_0x60++;
    phase = data->field_0x5C;
    if (phase < 0) {
        effDelete(effect);
        return;
    }

    for (i = 0; i < effect->count; i++, data++) {
        if (phase > 5) {
            data->field_0x58 = (s32)(float_0p3_8041f4bc * (f32)(255 - data->field_0x58) + (f32)data->field_0x58);
            data->field_0x10 *= float_0p8_8041f4c0;
            data->field_0x14 *= float_0p8_8041f4c0;
            data->field_0x18 *= float_0p8_8041f4c0;
        }
        if (phase < 5) {
            data->field_0x58 = (s32)((f32)data->field_0x58 * float_0p6_8041f4c4);
            data->field_0x10 *= float_0p7_8041f4c8;
            data->field_0x14 *= float_0p7_8041f4c8;
            data->field_0x18 *= float_0p7_8041f4c8;
        }
        data->position.x += data->field_0x10;
        data->position.y += data->field_0x14;
        data->position.z += data->field_0x18;
        data->field_0x4C += data->field_0x50;
        data->field_0x54 *= float_1p05_8041f4cc;
    }

    dispEntry(camera, dispCalcZ(&position2), 2, effKemuri10Disp, effect);
}

// .data vertex array this unit defines in the original; its address is passed
// to effSetVtxDescN64.
// .sbss2 GXColor scratch this unit defines in the original.
// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") u32 unk_80423178;

// Build the TEV register colour from the scratch colour and the effect data.
static inline void kemuri10SetColor(effKemuri10Data* data) {
    GXColor color = *(GXColor*)&unk_80423178;
    color.r = data->field_0x64[0];
    color.g = data->field_0x64[1];
    color.b = data->field_0x64[2];
    color.a = data->field_0x58;
    GXSetTevColor(GX_TEVREG0, color);
}

void effKemuri10Disp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effKemuri10Data* data;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx m0;
    Mtx m1;
    s32 field4C;

    cam = camGetPtr(camId);
    data = effect->userdata;
    field4C = (s32)data->field_0x4C;

    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
    PSMTXScale(m1, float_0p015625_8041f4b0, float_0p03125_8041f4b4, float_0_8041f4b8);
    PSMTXTrans(m0, (f32)field4C, float_0_8041f4b8, float_0_8041f4b8);
    PSMTXConcat(m1, m0, m0);
    GXLoadTexMtxImm(m0, 0x1e, 1);
    effGetTexObjN64(0x1f, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xff);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xf, 0xf, 0xf, 2);
    GXSetTevAlphaIn(0, 7, 4, 1, 7);
    kemuri10SetColor(data);
    PSMTXTrans(m0, data->position.x, data->position.y, data->position.z);
    PSMTXConcat(m0, (MtxPtr)&data->field_0x1C, m0);
    PSMTXScale(m1, data->field_0x54, data->field_0x54, data->field_0x54);
    PSMTXConcat(m0, m1, m0);
    PSMTXConcat(cam->view, m0, m0);
    GXLoadPosMtxImm(m0, 0);
    GXSetCurrentMtx(0);
    effSetVtxDescN64((effVtxArray*)stg_houki_kemuri_v);
    GXBegin(0x90, 0, 0xc0);
    tri2(0, 1, 2, 0, 0, 2, 3, 0);
    tri2(3, 2, 4, 0, 3, 4, 5, 0);
    tri2(2, 6, 7, 0, 2, 7, 4, 0);
    tri2(1, 8, 6, 0, 1, 6, 2, 0);
    tri2(4, 7, 9, 0, 4, 9, 10, 0);
    tri2(5, 4, 10, 0, 5, 10, 11, 0);
    tri2(10, 9, 8, 0, 10, 8, 1, 0);
    tri2(11, 10, 1, 0, 11, 1, 0, 0);
    tri2(6, 12, 13, 0, 6, 13, 7, 0);
    tri2(8, 14, 12, 0, 8, 12, 6, 0);
    tri2(7, 13, 15, 0, 7, 15, 9, 0);
    tri2(9, 15, 14, 0, 9, 14, 8, 0);
    tri2(16, 17, 18, 0, 16, 18, 19, 0);
    tri2(20, 21, 17, 0, 20, 17, 16, 0);
    tri2(22, 20, 16, 0, 22, 16, 23, 0);
    tri2(23, 16, 19, 0, 23, 19, 24, 0);
    tri2(19, 18, 25, 0, 19, 25, 26, 0);
    tri2(24, 19, 26, 0, 24, 26, 27, 0);
    tri2(28, 23, 24, 0, 28, 24, 29, 0);
    tri2(29, 24, 27, 0, 29, 27, 30, 0);
    tri2(26, 25, 21, 0, 26, 21, 20, 0);
    tri2(27, 26, 20, 0, 27, 20, 22, 0);
    tri2(30, 27, 22, 0, 30, 22, 31, 0);
    tri2(31, 22, 23, 0, 31, 23, 28, 0);
    tri2(32, 29, 30, 0, 32, 30, 33, 0);
    tri2(12, 32, 33, 0, 12, 33, 13, 0);
    tri2(13, 33, 34, 0, 13, 34, 15, 0);
    tri2(33, 30, 31, 0, 33, 31, 34, 0);
    tri2(34, 31, 28, 0, 34, 28, 35, 0);
    tri2(15, 34, 35, 0, 15, 35, 14, 0);
    tri2(35, 28, 29, 0, 35, 29, 32, 0);
    tri2(14, 35, 32, 0, 14, 32, 12, 0);
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_0p015625_8041f4b0 = 0.015625f;
const f32 float_0p03125_8041f4b4 = 0.03125f;
const f32 float_0_8041f4b8 = 0.0f;
const f32 float_0p3_8041f4bc = 0.3f;
const f32 float_0p8_8041f4c0 = 0.8f;
const f32 float_0p6_8041f4c4 = 0.6f;
const f32 float_0p7_8041f4c8 = 0.7f;
const f32 float_1p05_8041f4cc = 1.05f;
const f32 float_1_8041f4d0 = 1.0f;
const f32 float_4_8041f4d4 = 4.0f;
const f32 float_278_8041f4d8 = 278.0f;
const f32 float_32_8041f4dc = 32.0f;
const f32 float_0p1_8041f4e0 = 0.1f;
