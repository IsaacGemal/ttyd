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

// .sdata2 float constants (named after their original addresses).
extern const f32 float_0_8041f4b8;
extern const f32 float_0p3_8041f4bc;
extern const f32 float_0p8_8041f4c0;
extern const f32 float_0p6_8041f4c4;
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
    norm1 = 1.0f / sqrtf(mag);
    dirX = dirX * norm1;
    dirY = dirY * norm1;
    dirZ = dirZ * norm1;

    if (dirX != float_0_8041f4b8 || dirZ != float_0_8041f4b8) {
        upX = dirZ;
        upY = float_0_8041f4b8;
        upZ = -dirX;
    }
    else {
        upX = 1.0f;
        upZ = float_0_8041f4b8;
        upY = float_0_8041f4b8;
    }

    mag = upZ * upZ + (upX * upX + upY * upY);
    if (mag == float_0_8041f4b8) {
        return NULL;
    }
    norm2 = 1.0f / sqrtf(mag);
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
    norm3 = 1.0f / sqrtf(mag);
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
    data->velX = 4.0f * -dirX;
    data->velY = 4.0f * -dirY;
    data->velZ = 4.0f * -dirZ;
    data->life = 16;
    data->texScroll = 278.0f;
    data->texScrollSpeed = 32.0f;
    data->tevReg0A = 0;
    data->timer = 0;
    data->tevReg0R = 0xF8;
    data->tevReg0G = 0x5C;
    data->tevReg0B = 0xE5;
    data->field_0x67[0] = 0xF8;
    data->field_0x67[1] = 0x5C;
    data->field_0x67[2] = 0xE5;
    data->orient[0][0] = upX;
    data->orient[1][0] = upY;
    data->orient[2][0] = upZ;
    data->orient[0][1] = sideX;
    data->orient[1][1] = sideY;
    data->orient[2][1] = sideZ;
    data->orient[0][2] = dirX;
    data->orient[1][2] = dirY;
    data->orient[2][2] = dirZ;
    data->orient[0][3] = float_0_8041f4b8;
    data->orient[1][3] = float_0_8041f4b8;
    data->orient[2][3] = float_0_8041f4b8;
    data->scale = 0.1f * scale;
    data->camId = CAMERA_3D;

    return effect;
}

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
    camera = data->camId;
    data->life--;
    data->timer++;
    phase = data->life;
    if (phase < 0) {
        effDelete(effect);
        return;
    }

    for (i = 0; i < effect->count; i++, data++) {
        if (phase > 5) {
            data->tevReg0A = (s32)(float_0p3_8041f4bc * (f32)(255 - data->tevReg0A) + (f32)data->tevReg0A);
            data->velX *= float_0p8_8041f4c0;
            data->velY *= float_0p8_8041f4c0;
            data->velZ *= float_0p8_8041f4c0;
        }
        if (phase < 5) {
            data->tevReg0A = (s32)((f32)data->tevReg0A * float_0p6_8041f4c4);
            data->velX *= 0.7f;
            data->velY *= 0.7f;
            data->velZ *= 0.7f;
        }
        data->position.x += data->velX;
        data->position.y += data->velY;
        data->position.z += data->velZ;
        data->texScroll += data->texScrollSpeed;
        data->scale *= 1.05f;
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
    color.r = data->tevReg0R;
    color.g = data->tevReg0G;
    color.b = data->tevReg0B;
    color.a = data->tevReg0A;
    GXSetTevColor(GX_TEVREG0, color);
}

void effKemuri10Disp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effKemuri10Data* data;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx m0;
    Mtx m1;
    s32 scrollU;

    cam = camGetPtr(camId);
    data = effect->userdata;
    scrollU = (s32)data->texScroll;

    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    PSMTXScale(m1, float_0p015625_8041f4b0, float_0p03125_8041f4b4, float_0_8041f4b8);
    PSMTXTrans(m0, (f32)scrollU, float_0_8041f4b8, float_0_8041f4b8);
    PSMTXConcat(m1, m0, m0);
    GXLoadTexMtxImm(m0, GX_TEXMTX0, GX_MTX2x4);
    effGetTexObjN64(0x1f, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
    kemuri10SetColor(data);
    PSMTXTrans(m0, data->position.x, data->position.y, data->position.z);
    PSMTXConcat(m0, data->orient, m0);
    PSMTXScale(m1, data->scale, data->scale, data->scale);
    PSMTXConcat(m0, m1, m0);
    PSMTXConcat(cam->view, m0, m0);
    GXLoadPosMtxImm(m0, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    effSetVtxDescN64((effVtxArray*)stg_houki_kemuri_v);
    GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0xc0);
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
