#include "eff/n64/eff_butterfly_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "system.h"
#include "math_ext.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>

// .rodata
const Vec vec3_802fa398 = { 0.0f, 0.0f, 0.0f };

// .sdata2
extern const u32 dat_8041eb68;
extern const f32 float_0p3_8041eb6c;
extern const f32 float_deg2rad_8041eb70;
extern const f32 float_0p02_8041eb74;
extern const f32 float_0p03125_8041eb78;
extern const f32 float_0p015625_8041eb7c;
extern const f32 float_0_8041eb80;
extern const f32 float_50_8041eb90;
extern const f32 float_30_8041eb94;
extern const f32 float_6p2832_8041eb84;
extern const f32 float_360_8041eb88;
extern const f32 float_1_8041eb8c;

// .data / .sdata
extern u8 but_02_v[];

// .sdata
static s8 y_data[32] = {
    -1, -2, -3, -4, -5, -6, -7, -8,
    -9, -10, -11, -12, -13, -14, -15, -16,
    -17, -18, -19, -17, -15, -13, -11, -9,
    -7, -5, -3, -1, 0, 0, 0, 0,
};

typedef struct effButterflyData {
    s32 type;       // 0x0
    s32 field_0x4;  // 0x4
    s32 field_0x8;  // 0x8
    Vec position;   // 0xC
    Vec field_0x18; // 0x18
    s32 field_0x24; // 0x24
    f32 field_0x28; // 0x28
    s32 field_0x2C; // 0x2C
    s32 field_0x30; // 0x30
    f32 field_0x34; // 0x34
    f32 field_0x38; // 0x38
    Vec velocity;   // 0x3C
} effButterflyData;

EffectEntry* effButterflyN64Entry(s32 a, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    effButterflyData* data;

    effect = effEntry();
    effect->type = "ButterflyN64";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, 0x48);
    effect->userdata = data;
    effect->callback = effButterflyMain;
    effect->flags |= 2;
    data->type = a;
    data->field_0x4 = 0x3E8;
    data->field_0x24 = 0xFF;
    data->position.x = x;
    data->field_0x18.x = x;
    data->position.y = y;
    data->field_0x18.y = y;
    data->position.z = z;
    data->field_0x18.z = z;
    data->field_0x8 = 0;
    data->field_0x2C = 0;
    data->field_0x30 = 0;
    data->field_0x34 = float_50_8041eb90;
    data->field_0x38 = float_30_8041eb94;
    return effect;
}

// Inlined MetroWerks sqrtf (same shape as eff_sstar_n64.c / eff_blow_n64.c).
int __float_nan[];
#define NAN (*(f32*)__float_nan)

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

static inline s32 butterflyFpClassify(f32 x) {
    switch (*(s32*)&x & 0x7F800000) {
    case 0x7F800000:
        if (*(s32*)&x & 0x007FFFFF) {
            return FP_NAN;
        }
        return FP_INFINITE;
    case 0:
        if (*(s32*)&x & 0x007FFFFF) {
            return FP_SUBNORMAL;
        }
        return FP_ZERO;
    default:
        return FP_NORMAL;
    }
}

static inline f32 butterflySqrtf(f32 x) {
    const f64 _half = .5;
    const f64 _three = 3.0;

    if (x > float_0_8041eb80) {
        f64 guess = __frsqrte(x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        return x * guess;
    } else if (x < 0.0) {
        return NAN;
    } else if (butterflyFpClassify(x) == FP_NAN) {
        return NAN;
    }
    return x;
}

static inline Vec butterflyMakeVec(f32 x, f32 y, f32 z) {
    Vec temp = vec3_802fa398;
    temp.x = x;
    temp.y = y;
    temp.z = z;
    return temp;
}

// opt_common_subs off required: with CSE on MWCC reuses the float_0_8041eb80 result
// register for the later sqrtf `x > 0.0f` test (f2 cached vs the target's fresh f0).
#pragma opt_common_subs off
// The position components are loaded into scalars before the delta math; with the direct
// data->position member form MWCC assigns the x/z temporaries to the opposite FPRs (f5/f3).
void effButterflyMain(EffectEntry* effect) {
    effButterflyData* data = effect->userdata;
    Vec position;
    Vec position2;
    f32 angle;
    f32 targetX;
    f32 targetY;
    f32 targetZ;
    f32 px;
    f32 py;
    f32 pz;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 mag;

    position = butterflyMakeVec(data->position.x, data->position.y, data->position.z);
    position2 = position;
    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->field_0x4 = 0x384;
    }
    if (data->field_0x4 < 1000) {
        data->field_0x4--;
    }
    data->field_0x8++;
    if (data->field_0x4 < 0) {
        effDelete(effect);
        return;
    }
    if (data->field_0x4 < 0x10) {
        data->field_0x24 = data->field_0x4 << 4;
    }
    data->field_0x30--;
    if (data->field_0x30 <= 0) {
        angle = float_6p2832_8041eb84 * (f32)(rand() % 360) / float_360_8041eb88;
        targetX = data->field_0x34 * (f32)sin(angle) + data->field_0x18.x;
        targetY = data->field_0x18.y + (f32)(rand() % (s32)data->field_0x38);
        targetZ = data->field_0x34 * (f32)cos(angle) + data->field_0x18.z;
        data->field_0x30 = rand() % 100 + 10;
        px = data->position.x;
        py = data->position.y;
        pz = data->position.z;
        dx = targetX - px;
        dy = targetY - py;
        dz = targetZ - pz;
        if (float_0_8041eb80 == dx && float_0_8041eb80 == dy && float_0_8041eb80 == dz) {
            dx = data->field_0x18.x - px;
            dy = data->field_0x18.y - py;
            dz = data->field_0x18.z - pz;
            if (float_0_8041eb80 == dx && float_0_8041eb80 == dy && float_0_8041eb80 == dz) {
                dx = float_1_8041eb8c;
                dy = float_0_8041eb80;
                dz = float_0_8041eb80;
            }
        }
        mag = dx * dx + dy * dy + dz * dz;
        if (mag != float_0_8041eb80) {
            mag = float_1_8041eb8c / butterflySqrtf(mag);
        }
        dx *= mag;
        dy *= mag;
        dz *= mag;
        data->velocity.x = dx;
        data->velocity.y = dy;
        data->velocity.z = dz;
        data->field_0x28 = angleABf(float_0_8041eb80, float_0_8041eb80, -data->velocity.x, data->velocity.z);
    }
    data->field_0x2C += 3;
    if (data->field_0x2C >= 0x1E) {
        data->field_0x2C -= 0x1E;
    }
    data->position.x += data->velocity.x;
    data->position.y += data->velocity.y;
    data->position.z += data->velocity.z;
    dispEntry(CAMERA_3D, dispCalcZ(&position2), 1, effButterflyDisp, effect);
}
#pragma opt_common_subs reset

#ifdef MUST_MATCH
static inline GXColor butterflyColor(u8 v) {
    GXColor c = *(GXColor*)&dat_8041eb68;
    c.r = v;
    c.g = v;
    c.b = v;
    return c;
}
#endif

void effButterflyDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    CameraEntry* cam;
    effButterflyData* data;
    s32 alpha;
    s32 type;
    GXColor color;
    GXTexObj texObj;
    Mtx trans;
    Mtx scale;
    Mtx rot;

    cam = camGetPtr(camId);
    data = effect->userdata;
    alpha = data->field_0x24;
    type = data->type;

    PSMTXTrans(trans, data->position.x,
               float_0p3_8041eb6c * (f32)y_data[data->field_0x2C] + data->position.y, data->position.z);
    PSMTXRotRad(rot, 'y', float_deg2rad_8041eb70 * data->field_0x28);
    PSMTXConcat(trans, rot, trans);
    PSMTXScale(scale, float_0p02_8041eb74, float_0p02_8041eb74, float_0p02_8041eb74);
    PSMTXConcat(trans, scale, trans);
    PSMTXConcat(cam->view, trans, trans);
    GXLoadPosMtxImm(trans, 0);
    GXSetCurrentMtx(0);

#ifdef MUST_MATCH
    color = butterflyColor(alpha);
#else
    color = *(GXColor*)&dat_8041eb68;
    color.r = alpha;
    color.g = alpha;
    color.b = alpha;
#endif
    GXSetTevColor(1, color);
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 2, 8, 0xF);
    GXSetTevAlphaIn(0, 7, 7, 7, 4);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    PSMTXScale(scale, float_0p03125_8041eb78, float_0p015625_8041eb7c, float_0_8041eb80);
    GXLoadTexMtxImm(scale, 0x1E, 1);
    effGetTexObjN64(type + 0x78, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetCullMode(0);
    effSetVtxDescN64((effVtxArray*)(but_02_v + data->field_0x2C * 0x54));
    GXBegin(0x90, 0, 0xC);
    tri2(0, 1, 2, 0, 0, 2, 3, 0);
    tri2(3, 4, 0, 0, 3, 5, 4, 0);
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const u32 dat_8041eb68 = 0x000000FF;
const f32 float_0p3_8041eb6c = 0.3f;
const f32 float_deg2rad_8041eb70 = 0.017453292f;
const f32 float_0p02_8041eb74 = 0.02f;
const f32 float_0p03125_8041eb78 = 0.03125f;
const f32 float_0p015625_8041eb7c = 0.015625f;
const f32 float_0_8041eb80 = 0.0f;
const f32 float_6p2832_8041eb84 = 6.2831855f;
const f32 float_360_8041eb88 = 360.0f;
const f32 float_1_8041eb8c = 1.0f;
const f32 float_50_8041eb90 = 50.0f;
const f32 float_30_8041eb94 = 30.0f;
