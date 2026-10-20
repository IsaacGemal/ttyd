#include "eff/n64/eff_small_star_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "math_ext.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sbss2: shared scratch GXColor, only its byte 0 is overwritten (see eff_magic2_n64.c).
extern GXColor unk_80423260;

// .sdata2 float constants (named after their original addresses).
extern const f32 float_6p2832_8041fc84;
extern const f32 float_360_8041fc88;
extern const f32 kCosX4;
extern const f32 kCosX2;
extern const f32 float_1_8041fc94;
extern const f32 float_3p1416_8041fc98;
extern const f32 float_1p5708_8041fc9c;
extern const f32 float_4p7124_8041fca0;
extern const f32 float_4_8041fca4;
extern const f32 float_0p94_8041fca8;
extern const f32 float_0_8041fcac;
extern const f32 float_0p8_8041fcb0;
extern const f32 float_0p6_8041fcb4;
extern const f32 float_0p7_8041fcb8;
extern const f32 float_neg1_8041fcbc;
extern const f32 float_8_8041fcc0;
extern const f32 float_100_8041fcc4;
extern const f32 float_50_8041fcc8;
extern const f32 float_20_8041fccc;
extern const f32 float_0p5_8041fcd0;
extern const f32 float_15_8041fcd4;
extern const f32 float_deg2rad_8041fc80;

// .sdata: byte-indexed color rotation table (36 bytes).
static u8 color_rotation_data[36] = {
    0xFE, 0xAC, 0xAC, 0xFE, 0xAC, 0xD5, 0xFE, 0xB4, 0x9A, 0xD5, 0xB4, 0xFE,
    0xB4, 0xB4, 0xFE, 0xB4, 0xDD, 0xFE, 0xB4, 0xFE, 0xFE, 0xB4, 0xFE, 0xD5,
    0xB4, 0xFE, 0xB4, 0xD5, 0xFE, 0xB4, 0xFE, 0xFE, 0xB4, 0xFE, 0xD5, 0xAC,
};

// .data: one effVtxArray (0x38 bytes) for the textured quad.
static u32 size16x16_tex32x32_vtx[0x38 / 4] = {
    0xFFF8FFF8, 0x00000000, 0x00000000, 0x00FF0008,
    0xFFF80000, 0x00400000, 0x000000FF, 0x00080008,
    0x00000040, 0x00200000, 0x00FFFFF8, 0x00080000,
    0x00000020, 0x000000FF,
};

// Inlined MetroWerks sqrtf (same shape as eff_blow_n64.c / eff_sstar_n64.c).
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
            } else {
                return FP_INFINITE;
            }
        case 0:
            if (*(s32*)&x & 0x007FFFFF) {
                return FP_SUBNORMAL;
            } else {
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

    if (x > float_0_8041fcac) {
        f64 guess = __frsqrte(x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        return x * guess;
    } else if (x < 0.0) {
        return NAN;
    } else if (isnan(x)) {
        return NAN;
    }
    return x;
}

// Each case builds its axis from a zero Vec literal and then writes the fields, so MWCC
// emits one .rodata zero Vec per case (the original's rodata has four, one shared with Main).
#pragma opt_common_subs off // original reloads float_0 in each sqrt block rather than CSEing it
void* effSmallStarN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 dirX, f32 dirY, f32 dirZ) {
    static f32 kaiten;
    EffectEntry* effect;
    effSmallStarN64Data* data;
    Vec axis;
    Mtx mtx;
    f32 mag;
    f32 norm;
    f32 perpX;
    f32 perpY;
    f32 perpZ;
    s32 i;

    effect = effEntry();
    if (count == 0) {
        return NULL;
    }

    mag = dirX * dirX + dirY * dirY + dirZ * dirZ;
    if (mag == float_0_8041fcac) {
        return NULL;
    }
    norm = float_neg1_8041fcbc / sqrtf(mag);
    dirX *= norm;
    dirY *= norm;
    dirZ *= norm;

    if (dirX != float_0_8041fcac) {
        perpX = -dirY / dirX;
        perpY = float_1_8041fc94;
        perpZ = float_0_8041fcac;
    } else if (dirY != float_0_8041fcac) {
        perpX = float_1_8041fc94;
        perpY = -dirX / dirY;
        perpZ = float_0_8041fcac;
    } else {
        perpX = float_1_8041fc94;
        perpY = float_0_8041fcac;
        perpZ = -dirX / dirZ;
    }

    mag = perpX * perpX + perpY * perpY + perpZ * perpZ;
    if (mag == float_0_8041fcac) {
        return NULL;
    }
    norm = float_1_8041fc94 / sqrtf(mag);
    perpX *= norm;
    perpY *= norm;
    perpZ *= norm;

    dirX *= float_8_8041fcc0;
    dirY *= float_8_8041fcc0;
    dirZ *= float_8_8041fcc0;
    perpX *= float_8_8041fcc0;
    perpY *= float_8_8041fcc0;
    perpZ *= float_8_8041fcc0;

    effect->type = "SmallStarN64";
    effect->count = count;
    data = __memAlloc(HEAP_EFFECT, count * sizeof(effSmallStarN64Data));
    effect->userdata = data;
    effect->callback = effSmallStarMain;

    for (i = 0; i < count; i++, data++) {
        f32 yawRad = (float_6p2832_8041fc84 * camGetPtr(CAMERA_3D)->viewYaw) / float_360_8041fc88;
        data->type = type;
        data->x = x;
        data->y = y;
        data->z = z;
        data->camera = CAMERA_3D;

        switch (type) {
        case 0:
        case 1:
        case 2: {
            Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
            temp.x = dirX;
            temp.y = dirY;
            temp.z = dirZ;
            axis = temp;
            PSMTXRotAxisRad(mtx, &axis, float_deg2rad_8041fc80 * (float_360_8041fc88 * (f32)i / (f32)(count - 1)));
            data->velX = dirX + mtx[0][0] * perpX + mtx[0][1] * perpY + mtx[0][2] * perpZ;
            data->velY = dirY + mtx[1][0] * perpX + mtx[1][1] * perpY + mtx[1][2] * perpZ;
            data->velZ = dirZ + mtx[2][0] * perpX + mtx[2][1] * perpY + mtx[2][2] * perpZ;
            break;
        }
        case 3: {
            Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
            f32 spreadDeg;
            temp.x = (f32)sin(yawRad);
            temp.z = -(f32)cos(yawRad);
            axis = temp;
            spreadDeg = count == 1 ? float_0_8041fcac : (float_100_8041fcc4 * (f32)i / (f32)(count - 1)) - float_50_8041fcc8;
            PSMTXRotAxisRad(mtx, &axis, float_deg2rad_8041fc80 * spreadDeg);
            data->velX = mtx[0][0] * dirX + mtx[0][1] * dirY + mtx[0][2] * dirZ;
            data->velY = mtx[1][0] * dirX + mtx[1][1] * dirY + mtx[1][2] * dirZ;
            data->velZ = mtx[2][0] * dirX + mtx[2][1] * dirY + mtx[2][2] * dirZ;
            break;
        }
        case 4: {
            Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
            temp.x = (f32)sin(yawRad);
            temp.z = -(f32)cos(yawRad);
            axis = temp;
            PSMTXRotAxisRad(mtx, &axis, float_deg2rad_8041fc80 * (float_360_8041fc88 * (f32)i / (f32)(count - 1)));
            data->velX = mtx[0][0] * dirX + mtx[0][1] * dirY + mtx[0][2] * dirZ;
            data->velY = mtx[1][0] * dirX + mtx[1][1] * dirY + mtx[1][2] * dirZ;
            data->velZ = mtx[2][0] * dirX + mtx[2][1] * dirY + mtx[2][2] * dirZ;
            break;
        }
        }

        switch (type) {
        case 0:
            data->spinSpeed = float_0_8041fcac;
            data->life = 0x18;
            break;
        case 1:
            data->spinSpeed = float_20_8041fccc;
            data->life = 0x18;
            break;
        case 2:
            data->spinSpeed = float_20_8041fccc;
            data->velX *= float_0p5_8041fcd0;
            data->velY *= float_0p5_8041fcd0;
            data->velZ *= float_0p5_8041fcd0;
            data->life = 0x28;
            break;
        case 3:
        case 4:
            data->spinSpeed = float_20_8041fccc;
            data->life = 0x18;
            break;
        }

        data->spinAngle = float_0_8041fcac;
        data->frameCount = 0;
        data->alpha = 0xFF;
    }

    kaiten = kaiten + float_15_8041fcd4;
    if (kaiten > float_360_8041fc88) {
        kaiten = float_0_8041fcac;
    }

    return effect;
}
#pragma opt_common_subs reset

// MWCC's default lifetime analysis coalesces the cosine accumulator into the square's
// register (f2) instead of f5; opt_lifetimes off restores the original colouring. Reusing
// `rad` as the quadrant base keeps it in f6 and squares into f1 as the original does
// (rad is dead after this point); the natural form is under #else.
#pragma opt_lifetimes off // default lifetime analysis loses f1/f5, this fixes the FPR colouring
void effSmallStarMain(EffectEntry* effect) {
    effSmallStarN64Data* data;
    Vec position2;
    Vec position;
    s32 life;
    s32 i;
    f32 angle;
    f32 cosC0;
    f32 cosC2;
    f32 wave;
    f32 sq;
    f32 rad;

    data = effect->userdata;
    position = (Vec){ 0.0f, 0.0f, 0.0f };
    position.x = data->x;
    position.y = data->y;
    position.z = data->z;
    position2 = position;

    data->life--;
    data->frameCount++;
    life = data->life;
    if (life < 0) {
        effDelete(effect);
        return;
    }

    angle = (f32)(data->frameCount * 6) * float_6p2832_8041fc84;
    for (i = 0; i < effect->count; i++, data++) {
        if (data->type == 2) {
            wave = kCosX4;
            cosC2 = kCosX2;
            cosC0 = float_1_8041fc94;
            rad = angle / float_360_8041fc88;

            if (rad <= float_3p1416_8041fc98) {
                if (rad <= float_1p5708_8041fc9c) {
                    sq = rad * rad;
                    wave = wave * sq - cosC2;
                    wave = wave * sq + cosC0;
                } else {
#ifdef MUST_MATCH
                    rad = float_1p5708_8041fc9c - (rad - float_1p5708_8041fc9c);
                    sq = rad * rad;
#else
                    sq = float_1p5708_8041fc9c - (rad - float_1p5708_8041fc9c);
                    sq = sq * sq;
#endif
                    wave = wave * sq - cosC2;
                    wave = wave * sq + cosC0;
                    wave = -wave;
                }
            } else {
                if (rad < float_4p7124_8041fca0) {
#ifdef MUST_MATCH
                    rad = rad - float_3p1416_8041fc98;
                    sq = rad * rad;
#else
                    sq = rad - float_3p1416_8041fc98;
                    sq = sq * sq;
#endif
                    wave = wave * sq - cosC2;
                    wave = wave * sq + cosC0;
                    wave = -wave;
                } else {
#ifdef MUST_MATCH
                    rad = float_1p5708_8041fc9c - (rad - float_4p7124_8041fca0);
                    sq = rad * rad;
#else
                    sq = float_1p5708_8041fc9c - (rad - float_4p7124_8041fca0);
                    sq = sq * sq;
#endif
                    wave = wave * sq - cosC2;
                    wave = wave * sq + cosC0;
                }
            }
            data->velY = float_4_8041fca4 * wave;
            data->velX *= float_0p94_8041fca8;
            data->velZ *= float_0p94_8041fca8;
            if (data->velY < float_0_8041fcac) {
                data->spinSpeed *= float_0p8_8041fcb0;
                data->alpha = (s32)((f32)data->alpha * float_0p6_8041fcb4);
            }
        } else {
            if (life > 5) {
                data->velX *= float_0p8_8041fcb0;
                data->velY *= float_0p8_8041fcb0;
                data->velZ *= float_0p8_8041fcb0;
            }
            if (life < 20 && life > 5) {
                if (data->type == 0) {
                    data->spinSpeed += float_1_8041fc94;
                }
            } else if (life < 5) {
                data->spinSpeed *= float_0p8_8041fcb0;
                data->alpha = (s32)((f32)data->alpha * float_0p6_8041fcb4);
                data->velX *= float_0p7_8041fcb8;
                data->velY *= float_0p7_8041fcb8;
                data->velZ *= float_0p7_8041fcb8;
            }
        }
        data->x += data->velX;
        data->y += data->velY;
        data->z += data->velZ;
        data->spinAngle += data->spinSpeed;
    }

    dispEntry(((effSmallStarN64Data*)effect->userdata)->camera, dispCalcZ(&position2), 2, effSmallStarDisp, effect);
}

#pragma opt_lifetimes reset
void effSmallStarDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    s32 rIdx;
    s32 gIdx;
    s32 bIdx;
    effSmallStarN64Data* data;
    s32 i;
    CameraEntry* cam;
    s32 colorBase;
    GXTexObj texObj;
    Mtx mtx1;
    Mtx mtx2;
    Mtx mtx3;
    GXColor color;

    cam = camGetPtr(camId);
    data = effect->userdata;
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    effGetTexObjN64(0x1D, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetCullMode(GX_CULL_NONE);
    effSetVtxDescN64((effVtxArray*)size16x16_tex32x32_vtx);

    // The split multiply/modulo keeps colorBase in r4; writing it as one expression moves it to r3.
    colorBase = (data->frameCount - 1) * 3;
    colorBase %= 36;
    rIdx = colorBase;
    gIdx = colorBase + 1;
    bIdx = colorBase + 2;
    for (i = 0; i < effect->count; i++, data++) {
        color = unk_80423260;
        color.r = color_rotation_data[rIdx % 36];
        color.g = color_rotation_data[gIdx % 36];
        color.b = color_rotation_data[bIdx % 36];
        color.a = data->alpha;
        GXSetTevColor(GX_TEVREG0, color);

        PSMTXTrans(mtx1, data->x, data->y, data->z);
        PSMTXRotRad(mtx2, 'y', -camGetPtr(CAMERA_3D)->viewYaw * float_deg2rad_8041fc80);
        PSMTXRotRad(mtx3, 'z', float_deg2rad_8041fc80 * data->spinAngle);
        PSMTXConcat(mtx1, mtx2, mtx1);
        PSMTXConcat(mtx1, mtx3, mtx1);
        PSMTXConcat(cam->view, mtx1, mtx1);
        GXLoadPosMtxImm(mtx1, 0);
        GXSetCurrentMtx(0);
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);

        rIdx += 3;
        gIdx += 3;
        bIdx += 3;
    }
}
void effSmallStarN64CamChg(EffectEntry* effect, s32 camId) {
    effSmallStarN64Data* data = effect->userdata;
    s32 i = 0;

    while (i < effect->count) {
        data->camera = camId;
        i++;
        data++;
    }
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_deg2rad_8041fc80 = 0.017453292f;
const f32 float_6p2832_8041fc84 = 6.2831855f;
const f32 float_360_8041fc88 = 360.0f;
const f32 kCosX4 = 0.03705f;
const f32 kCosX2 = 0.4967f;
const f32 float_1_8041fc94 = 1.0f;
const f32 float_3p1416_8041fc98 = 3.1415927f;
const f32 float_1p5708_8041fc9c = 1.5707964f;
const f32 float_4p7124_8041fca0 = 4.712389f;
const f32 float_4_8041fca4 = 4.0f;
const f32 float_0p94_8041fca8 = 0.94f;
const f32 float_0_8041fcac = 0.0f;
const f32 float_0p8_8041fcb0 = 0.8f;
const f32 float_0p6_8041fcb4 = 0.6f;
const f32 float_0p7_8041fcb8 = 0.7f;
const f32 float_neg1_8041fcbc = -1.0f;
const f32 float_8_8041fcc0 = 8.0f;
const f32 float_100_8041fcc4 = 100.0f;
const f32 float_50_8041fcc8 = 50.0f;
const f32 float_20_8041fccc = 20.0f;
const f32 float_0p5_8041fcd0 = 0.5f;
const f32 float_15_8041fcd4 = 15.0f;

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_80423260;
