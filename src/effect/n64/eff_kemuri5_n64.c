#include "eff/n64/eff_kemuri5_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"
#include <stdlib.h>

// The zero Vec, the "Kemuri5N64" string and the int->float conversion double are pooled by the
// compiler from the compound literal, the string literal and the (f32) conversion below.

// .sdata2 float constants (named after their original addresses).
extern const f32 float_12_8041f368;
extern const f32 float_6p2832_8041f36c;
extern const f32 float_360_8041f370;
extern const f32 float_0p1_8041f374;
extern const f32 float_0p83_8041f378;
extern const f32 float_0p006_8041f37c;
extern const f32 float_7p5_8041f380;
extern const f32 float_0p98_8041f384;
extern const f32 float_0p9_8041f388;
extern const f32 float_0p0625_8041f35c;
extern const f32 float_0_8041f360;
extern const f32 float_deg2rad_8041f364;
extern const f32 float_1_8041f38c;
extern const f32 float_0p03_8041f390;
extern const f32 float_1p7_8041f394;
extern const f32 float_0p5_8041f398;
extern const f32 float_neg0p02_8041f39c;
extern const f32 float_5Eneg05_8041f3a0;
extern const f32 float_neg3p9_8041f3a4;

// .sdata2 GXColor: r=0x70, g=0x60, b=0x18, a=0x00.
extern const volatile u32 dat_8041f358;

// .data quad vertex array for effSetVtxDescN64 (kmr_a_1_v).
static u32 kmr_a_1_v[14] = {
    0xFFF80007, 0x00000000, 0x0000FFFF, 0xFFFF0007,
    0x00070000, 0x02000000, 0xFFFFFFFF, 0xFFF8FFF8,
    0x00000000, 0x0200FFFF, 0xFFFF0007, 0xFFF80000,
    0x02000200, 0xFFFFFFFF,
};

typedef struct effKemuri5Data {
    s32 field_0x0;   // 0x0
    s16 field_0x4;   // 0x4
    s16 field_0x6;   // 0x6
    u8 field_0x8;    // 0x8
    u8 pad_9[3];     // 0x9
    Vec position;    // 0xC
    f32 field_0x18;  // 0x18
    f32 field_0x1C;  // 0x1C
    f32 field_0x20;  // 0x20
    f32 field_0x24;  // 0x24
    f32 field_0x28;  // 0x28
    f32 field_0x2C;  // 0x2C
    f32 field_0x30;  // 0x30
    f32 field_0x34;  // 0x34
    f32 field_0x38;  // 0x38
    f32 field_0x3C;  // 0x3C
    f32 field_0x40;  // 0x40
    f32 field_0x44;  // 0x44
    f32 field_0x48;  // 0x48
} effKemuri5Data;

// Local copy of system.c's inline helper so it inlines here (as in eff_water_damage_n64.c).
// The compound literal resolves to the original's zero Vec (eff_water_damage_n64.c does the same).
static inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };

    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

// One local holds the angle offset and then the radians: converting `i * 45` before the reset stores
// gives the original's register colouring.
EffectEntry* effKemuri5N64Entry(f32 x, f32 y, f32 z, f32 a4) {
    EffectEntry* effect;
    effKemuri5Data* data;
    f32 rad;
    s32 i;

    effect = effEntry();
    effect->type = "Kemuri5N64";
    effect->count = 8;
    data = __memAlloc(HEAP_EFFECT, sizeof(effKemuri5Data) * 8);
    effect->userdata = data;
    effect->callback = effKemuri5Main;

    for (i = 0; i < 8; i++, data++) {
        data->field_0x0 = 1;
        data->field_0x40 = float_0_8041f360;
        data->position.x = x;
        data->position.y = y;
        data->position.z = z;
        data->field_0x18 = float_1_8041f38c;
        data->field_0x1C = float_1_8041f38c;
        data->field_0x20 = float_1_8041f38c;
        data->field_0x8 = 0xFF;
        data->field_0x24 = float_0p03_8041f390 * (f32)(rand() % 11) + float_1_8041f38c;
        data->field_0x28 = float_0p03_8041f390 * (f32)(rand() % 11) + float_1p7_8041f394;
        data->field_0x2C = (f32)(rand() % 61);
        data->field_0x6 = 0x1E;
        rad = (f32)(i * 45);
        data->field_0x34 = float_0p5_8041f398;
        data->field_0x38 = float_neg0p02_8041f39c;
        data->field_0x3C = float_5Eneg05_8041f3a0;
        data->field_0x30 = float_neg3p9_8041f3a4;
        rad = float_6p2832_8041f36c * reviseAngle(a4 + rad) / float_360_8041f370;
        data->field_0x44 = sin(rad);
        data->field_0x48 = cos(rad);
    }

    return effect;
}

// Without this the cos() result colours to f2 instead of the original's f3, swapping f2/f3 through
// the rest of the loop; the pragma is scoped to this function only.
#pragma opt_common_subs off
void effKemuri5Main(EffectEntry* effect) {
    effKemuri5Data* data = effect->userdata;
    Vec position;
    s32 i;
    s32 any;
    f32 v;

    position = initializeVec(&data->position);
    any = 0;
    for (i = 0; i < effect->count; i++, data++) {
        if (data->field_0x0 == 0) {
            continue;
        }
        if (--data->field_0x6 <= 0) {
            data->field_0x0 = 0;
            continue;
        }
        any = 1;
        data->field_0x2C = reviseAngle(float_12_8041f368 + data->field_0x2C);
        v = (f32)sin(float_6p2832_8041f36c * data->field_0x2C / float_360_8041f370);
        data->field_0x18 = float_0p1_8041f374 * v + data->field_0x24;
        v = (f32)cos(float_6p2832_8041f36c * data->field_0x2C / float_360_8041f370);
        data->field_0x1C = float_0p1_8041f374 * v + data->field_0x28;
        data->field_0x30 *= float_0p83_8041f378;
        data->position.x += data->field_0x30 * data->field_0x44;
        data->position.z += data->field_0x30 * data->field_0x48;
        data->field_0x38 += data->field_0x3C;
        data->field_0x34 += data->field_0x38;
        data->position.y += data->field_0x34;
        data->field_0x24 = float_0p006_8041f37c * (float_7p5_8041f380 - data->field_0x24) + data->field_0x24;
        data->field_0x28 *= float_0p98_8041f384;
        if (data->field_0x6 < 10) {
            data->field_0x28 *= float_0p9_8041f388;
        }
        if (data->field_0x6 < 15) {
            data->field_0x8 -= 0x10;
        }
    }
    if (any == 0) {
        effDelete(effect);
        return;
    }
    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKemuri5Disp, effect);
}
#pragma opt_common_subs reset

static inline void kemuri5SetColor(u8 a) {
    GXColor color = *(GXColor*)&dat_8041f358;
    color.a = a;
    GXSetTevColor(1, color);
}

void effKemuri5Disp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    CameraEntry* cam = camGetPtr(camId);
    effKemuri5Data* data = effect->userdata;
    GXTexObj texObj;
    Mtx trans;
    Mtx scale;
    Mtx rot;
    s32 i;

    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    PSMTXScale(scale, float_0p0625_8041f35c, float_0p0625_8041f35c, float_0_8041f360);
    GXLoadTexMtxImm(scale, 0x1E, 1);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 8, 15, 15, 2);
    GXSetTevAlphaIn(0, 7, 4, 1, 7);
    effGetTexObjN64(0x4E, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetCullMode(0);
    effSetVtxDescN64((effVtxArray*)kmr_a_1_v);

    for (i = 0; i < effect->count; i++, data++) {
        if (data->field_0x0 == 0) {
            continue;
        }
        kemuri5SetColor(data->field_0x8);
        PSMTXTrans(trans, data->position.x, data->position.y, data->position.z);
        PSMTXRotRad(rot, 'y', float_deg2rad_8041f364 * -camGetPtr(CAMERA_3D)->viewYaw);
        PSMTXConcat(trans, rot, trans);
        PSMTXScale(scale, data->field_0x18, data->field_0x1C, data->field_0x20);
        PSMTXConcat(trans, scale, trans);
        PSMTXConcat(cam->view, trans, trans);
        GXLoadPosMtxImm(trans, 0);
        GXBegin(0x90, 0, 6);
        tri2(0, 3, 1, 0, 0, 2, 3, 0);
    }
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const volatile u32 dat_8041f358 = 0x70601800;
const f32 float_0p0625_8041f35c = 0.0625f;
const f32 float_0_8041f360 = 0.0f;
const f32 float_deg2rad_8041f364 = 0.017453292f;
const f32 float_12_8041f368 = 12.0f;
const f32 float_6p2832_8041f36c = 6.2831855f;
const f32 float_360_8041f370 = 360.0f;
const f32 float_0p1_8041f374 = 0.1f;
const f32 float_0p83_8041f378 = 0.83f;
const f32 float_0p006_8041f37c = 0.006f;
const f32 float_7p5_8041f380 = 7.5f;
const f32 float_0p98_8041f384 = 0.98f;
const f32 float_0p9_8041f388 = 0.9f;
const f32 float_1_8041f38c = 1.0f;
const f32 float_0p03_8041f390 = 0.03f;
const f32 float_1p7_8041f394 = 1.7f;
const f32 float_0p5_8041f398 = 0.5f;
const f32 float_neg0p02_8041f39c = -0.02f;
const f32 float_5Eneg05_8041f3a0 = 0.00005f;
const f32 float_neg3p9_8041f3a4 = -3.9f;
