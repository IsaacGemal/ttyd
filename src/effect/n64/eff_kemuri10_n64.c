#include "eff/n64/eff_kemuri10_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"

#include <dolphin/mtx.h>

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
