#include "eff/n64/eff_kemuri7_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"

u32 foot_vtx[14] = {
    0xFFFB0000, 0xFFFB0000, 0x04000000, 0x00FF0005,
    0x0000FFFB, 0x04000400, 0x000000FF, 0x00050000,
    0x00050400, 0x00000000, 0x00FFFFFB, 0x00000005,
    0x00000000, 0x000000FF,
};

// .sdata2 constants (defined by this unit in the original).
extern const u32 dat_8041f3e8;
extern const f32 float_0p03125_8041f3ec;
extern const f32 float_0_8041f3f0;
extern const f32 float_neg1_8041f3f4;
extern const f32 float_1_8041f3f8;
extern const f32 float_deg2rad_8041f3fc;
extern const f32 float_30_8041f400;
extern const f32 float_15_8041f404;
extern const f32 float_180_8041f408;
extern const f32 float_6p2832_8041f40c;
extern const f32 float_360_8041f410;
extern const f32 float_45_8041f414;
extern const f32 float_5_8041f418;


// .data vertex array defined by this unit.

// Build the TEV register colour as the original does; the inline keeps the GXColor
// local at r1+0x8 with its outgoing copy at r1+0xc.
static inline void kemuri7SetColor(u8 a) {
    GXColor color = *(GXColor*)&dat_8041f3e8;
    color.a = a;
    GXSetTevColor(1, color);
}

typedef struct effKemuri7Data {
    s32 field_0x0;               // 0x0
    u8 pad_4[2];                 // 0x4
    s16 field_0x6;               // 0x6
    u8 field_0x8;                // 0x8
    u8 pad_9[3];                 // 0x9
    Vec position;                // 0xC
    f32 field_0x18;              // 0x18
    f32 field_0x1C;              // 0x1C
    f32 field_0x20;              // 0x20
    f32 field_0x24;              // 0x24
    f32 field_0x28;              // 0x28
    f32 field_0x2C;              // 0x2C
    Mtx field_0x30;              // 0x30
    f32 field_0x60;              // 0x60
    u8 pad_64[0x6C - 0x64];      // 0x64
    f32 field_0x6C;              // 0x6C
    f32 field_0x70;              // 0x70
    f32 field_0x74;              // 0x74
    u8 pad_78[0x80 - 0x78];      // 0x78
    f32 field_0x80;              // 0x80
    f32 field_0x84;              // 0x84
} effKemuri7Data;

// Local copy of system.c's inline helper so it inlines here as in the original.
inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

EffectEntry* effKemuri7N64Entry(f32 x, f32 y, f32 z, f32 angle, f32 scale) {
    EffectEntry* effect;
    effKemuri7Data* data;
    f32 field28;
    f32 a;
    f32 b;
    f32 r;
    f32 r2;
    s32 i;

    effect = effEntry();
    effect->type = "Kemuri7N64";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, 0x88);
    effect->userdata = data;
    effect->callback = effKemuri7Main;

    for (i = 0; i < 1; i++) {
        field28 = float_30_8041f400 * scale + angle - float_15_8041f404;
        data->field_0x0 = 1;
        data->field_0x6C = float_0_8041f3f0;
        data->position.x = x;
        data->position.y = y;
        data->position.z = z;
        data->field_0x18 = float_1_8041f3f8;
        data->field_0x1C = float_1_8041f3f8;
        data->field_0x20 = float_1_8041f3f8;
        data->field_0x8 = 0xFF;
        data->field_0x60 = scale;
        data->field_0x70 = float_0_8041f3f0;
        data->field_0x74 = float_0_8041f3f0;
        data->field_0x6 = 0xFF;
        data->field_0x24 = float_0_8041f3f0;
        data->field_0x28 = field28;
        data->field_0x2C = float_0_8041f3f0;

        b = float_180_8041f408 * scale;
        a = reviseAngle(angle);
        r = float_6p2832_8041f40c * a / float_360_8041f410;
        data->field_0x80 = (f32)sin(r);
        data->field_0x84 = (f32)cos(r);

        r2 = float_6p2832_8041f40c * (float_45_8041f414 + a - b) / float_360_8041f410;
        data->position.x += float_5_8041f418 * (f32)sin(r2);
        data->position.z += float_5_8041f418 * (f32)cos(r2);
        data->field_0x8 = 0xFF;
    }
    return effect;
}

void effKemuri7Main(EffectEntry* effect) {
    effKemuri7Data* data = effect->userdata;
    Vec position;
    Mtx m1;
    Mtx m0;
    s32 i;
    s32 active;

    position = initializeVec(&data->position);
    active = 0;

    for (i = 0; i < effect->count; i++, data++) {
        if (data->field_0x0 == 0) {
            continue;
        }
        if (--data->field_0x6 <= 0) {
            data->field_0x0 = 0;
            continue;
        }
        active = 1;
        PSMTXTrans(m0, data->position.x, data->position.y, data->position.z);
        PSMTXRotRad(m1, 'y', float_deg2rad_8041f3fc * data->field_0x28);
        PSMTXConcat(m0, m1, data->field_0x30);
        data->field_0x8--;
    }

    if (active == 0) {
        effDelete(effect);
    }
    else {
        dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKemuri7Disp, effect);
    }
}

void effKemuri7Disp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effKemuri7Data* data;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx work;
    Mtx scale;
    s32 i;

    cam = camGetPtr(camId);
    data = effect->userdata;

    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
    PSMTXScale(scale, float_0p03125_8041f3ec, float_0p03125_8041f3ec, float_0_8041f3f0);
    GXLoadTexMtxImm(scale, 0x1e, 1);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xff);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xf, 0xf, 0xf, 0xf);
    GXSetTevAlphaIn(0, 7, 4, 1, 7);
    effGetTexObjN64(0x4f, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetCullMode(0);
    effSetVtxDescN64((effVtxArray*)foot_vtx);

    for (i = 0; i < effect->count; i++, data++) {
        if (data->field_0x0 == 0) {
            continue;
        }
        kemuri7SetColor(data->field_0x8);
        PSMTXConcat(cam->view, data->field_0x30, work);
        if (data->field_0x60 == float_0_8041f3f0) {
            PSMTXScale(scale, float_neg1_8041f3f4, float_1_8041f3f8, float_1_8041f3f8);
            PSMTXConcat(work, scale, work);
        }
        GXLoadPosMtxImm(work, 0);
        GXBegin(0x90, 0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}

// The pool constants, defined after the code that loads them (defined before, the compiler folds them).
const u32 dat_8041f3e8 = 0x70601800;
const f32 float_0p03125_8041f3ec = 0.03125f;
const f32 float_0_8041f3f0 = 0.0f;
const f32 float_neg1_8041f3f4 = -1.0f;
const f32 float_1_8041f3f8 = 1.0f;
const f32 float_deg2rad_8041f3fc = 0.017453292f;
const f32 float_30_8041f400 = 30.0f;
const f32 float_15_8041f404 = 15.0f;
const f32 float_180_8041f408 = 180.0f;
const f32 float_6p2832_8041f40c = 6.2831855f;
const f32 float_360_8041f410 = 360.0f;
const f32 float_45_8041f414 = 45.0f;
const f32 float_5_8041f418 = 5.0f;
