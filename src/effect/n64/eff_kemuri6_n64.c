#include "eff/n64/eff_kemuri6_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"
#include "mario/mario.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math.h>
#include <stdlib.h>

const f64 double_to_int_802fa950 = 4503601774854144.0;
const char str_802FA944[] = "Kemuri6N64";

extern const f32 float_0_8041f3b0;
extern const f32 float_1_8041f3d0;
extern const f32 float_1p4_8041f3d4;
extern const f32 float_0p03_8041f3d8;
extern const f32 float_1p5_8041f3dc;
extern const f32 float_2_8041f3e0;
extern const f32 float_neg0p5_8041f3e4;
extern const f32 float_0p0625_8041f3ac;
extern const f32 float_deg2rad_8041f3b4;

// .data quad vertex array for effSetVtxDescN64 (kmr_a_1_v).
static u32 kmr_a_1_v[14] = {
    0xFFF80007, 0x00000000, 0x0000FFFF, 0xFFFF0007,
    0x00070000, 0x02000000, 0xFFFFFFFF, 0xFFF8FFF8,
    0x00000000, 0x0200FFFF, 0xFFFF0007, 0xFFF80000,
    0x02000200, 0xFFFFFFFF,
};

typedef struct effKemuri6Data {
    s32 field_0x0;   // 0x0
    u16 field_0x4;   // 0x4
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
    f32 field_0x4C;  // 0x4C
} effKemuri6Data;

// Local copy of system.c's inline helper so it inlines here (as in eff_kemuri11_n64.c).
inline Vec initializeVec(Vec* input) {
    Vec temp = { 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

EffectEntry* effKemuri6N64Entry(s32 type, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    effKemuri6Data* data;
    s32 i;

    effect = effEntry();
    effect->type = str_802FA944;
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(effKemuri6Data));
    effect->userdata = data;
    effect->callback = effKemuri6Main;

    for (i = 0; i < 1; i++) {
        data->field_0x0 = 1;
        data->field_0x4 = type;
        data->field_0x40 = float_0_8041f3b0;
        data->field_0x44 = float_0_8041f3b0;
        data->position.x = x;
        data->position.y = y;
        data->position.z = z;
        data->field_0x1C = float_1_8041f3d0;
        data->field_0x20 = float_1_8041f3d0;
        data->field_0x24 = float_1_8041f3d0;
        data->field_0x8 = 0xFF;
        data->field_0x28 = float_0p03_8041f3d8 * (f32)(rand() % 11) + float_1p4_8041f3d4;
        data->field_0x2C = float_0p03_8041f3d8 * (f32)(rand() % 11) + float_1p5_8041f3dc;
        data->field_0x30 = (f32)(rand() % 61);
        data->field_0x4 = type;
        data->field_0x38 = float_2_8041f3e0;
        data->field_0x3C = float_neg0p5_8041f3e4;
        data->field_0x6 = 15;
        data->field_0x18 = (f32)(rand() % 361);
        data->field_0x8 = 0xFF;
    }
    return effect;
}

void effKemuri6Main(EffectEntry* effect) {
    EffectEntry* e;
    effKemuri6Data* data;
    Vec position;
    s32 i;
    s32 active;

    e = effect;
    data = e->userdata;
    position = initializeVec(&data->position);
    active = 0;
    for (i = 0; i < e->count; i++, data++) {
        if (data->field_0x0 != 0) {
            if (--data->field_0x6 <= 0) {
                data->field_0x0 = 0;
            }
            else {
                active = 1;
                data->field_0x30 = reviseAngle(12.0f + data->field_0x30);
                data->field_0x1C = 0.1f * (f32)sin(6.2831855f * data->field_0x30 / 360.0f) + data->field_0x28;
                data->field_0x20 = 0.1f * (f32)cos(6.2831855f * data->field_0x30 / 360.0f) + data->field_0x2C;
                if (data->field_0x4 == 0
                    && data->field_0x6 > 4
                    && data->field_0x6 <= 5
                    && marioGetPtr()->currMotionId == MARIO_MOTION_STAY) {
                    data->field_0x6++;
                    return;
                }
                data->field_0x3C += data->field_0x40;
                data->field_0x38 += data->field_0x3C;
                data->position.y += data->field_0x38;
                data->field_0x8 -= 15;
                data->field_0x28 += 0.14f;
                data->field_0x2C *= 0.94f;
                if (data->field_0x38 < 0.0f) {
                    data->field_0x3C = 0.0f;
                    data->field_0x38 = 0.0f;
                }
            }
        }
    }
    if (active == 0) {
        effDelete(e);
    }
    else {
        dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKemuri6Disp, e);
    }
}

void effKemuri6Disp(CameraId camId, void* param) {
    EffectEntry* effect;
    effKemuri6Data* data;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx model;
    Mtx scale;
    Mtx rot;
    s32 i;

    effect = param;
    cam = camGetPtr(camId);
    data = effect->userdata;
    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, GX_TG_MTX2x4, GX_TG_TEX0, 0x1E, 0, 0x7D);
    PSMTXScale(scale, float_0p0625_8041f3ac, float_0p0625_8041f3ac, float_0_8041f3b0);
    GXLoadTexMtxImm(scale, 0x1E, 1);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 8, 0xF, 0xF, 2);
    GXSetTevAlphaIn(0, 7, 4, 1, 7);
    effGetTexObjN64(0x4E, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetCullMode(0);
    effSetVtxDescN64((effVtxArray*)kmr_a_1_v);

    for (i = 0; i < effect->count; i++, data++) {
        if (data->field_0x0 == 0) {
            continue;
        }
        GXSetTevColor(1, (GXColor){ 0x70, 0x60, 0x18, data->field_0x8 });
        PSMTXTrans(model, data->position.x, data->position.y, data->position.z);
        PSMTXRotRad(rot, 'y', -camGetPtr(CAMERA_3D)->viewYaw * float_deg2rad_8041f3b4);
        PSMTXConcat(model, rot, model);
        PSMTXScale(scale, data->field_0x1C, data->field_0x20, data->field_0x24);
        PSMTXConcat(model, scale, model);
        PSMTXRotRad(rot, 'z', float_deg2rad_8041f3b4 * data->field_0x18);
        PSMTXConcat(model, rot, model);
        PSMTXConcat(cam->view, model, model);
        GXLoadPosMtxImm(model, 0);
        GXBegin(0x90, 0, 6);
        tri2(0, 3, 1, 0, 0, 2, 3, 0);
    }
}

