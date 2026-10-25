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

// .data quad vertex array for effSetVtxDescN64 (kmr_a_1_v).
static u32 kmr_a_1_v[14] = {
    0xFFF80007, 0x00000000, 0x0000FFFF, 0xFFFF0007,
    0x00070000, 0x02000000, 0xFFFFFFFF, 0xFFF8FFF8,
    0x00000000, 0x0200FFFF, 0xFFFF0007, 0xFFF80000,
    0x02000200, 0xFFFFFFFF,
};

typedef struct effKemuri5Data {
    s32 alive;       // 0x0, 1 while the puff runs (Main clears it at timer 0)
    u8 pad_0x4[2];   // 0x4
    s16 timer;       // 0x6, frames left (30)
    u8 alpha;        // 0x8, TEVREG0 alpha, falls 16 per frame after timer 15
    u8 pad_9[3];     // 0x9
    Vec position;    // 0xC
    f32 scaleX;      // 0x18, 0.1 * sin(swayAngle) + baseScaleX
    f32 scaleY;      // 0x1C, 0.1 * cos(swayAngle) + baseScaleY
    f32 scaleZ;      // 0x20, 1.0, never changed
    f32 baseScaleX;  // 0x24, grows towards 7.5
    f32 baseScaleY;  // 0x28, decays by 0.98 (0.9 after timer 10)
    f32 swayAngle;   // 0x2C, degrees, +12 per frame
    f32 driftSpeed;  // 0x30, decays by 0.83 per frame
    f32 velY;        // 0x34, added to position.y each frame
    f32 accelY;      // 0x38, added to velY each frame, starts -0.02
    f32 accelStep;   // 0x3C, added to accelY each frame (0.00005)
    f32 field_0x40;  // 0x40
    f32 dirX;        // 0x44, sin of the drift angle, times driftSpeed moves position.x
    f32 dirZ;        // 0x48, cos of the drift angle, times driftSpeed moves position.z
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
EffectEntry* effKemuri5N64Entry(f32 x, f32 y, f32 z, f32 angle) {
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
        data->alive = 1;
        data->field_0x40 = 0.0f;
        data->position.x = x;
        data->position.y = y;
        data->position.z = z;
        data->scaleX = 1.0f;
        data->scaleY = 1.0f;
        data->scaleZ = 1.0f;
        data->alpha = 0xFF;
        data->baseScaleX = 0.03f * (f32)(rand() % 11) + 1.0f;
        data->baseScaleY = 0.03f * (f32)(rand() % 11) + 1.7f;
        data->swayAngle = (f32)(rand() % 61);
        data->timer = 0x1E;
        rad = (f32)(i * 45);
        data->velY = 0.5f;
        data->accelY = -0.02f;
        data->accelStep = 0.00005f;
        data->driftSpeed = -3.9f;
        rad = 6.2831855f * reviseAngle(angle + rad) / 360.0f;
        data->dirX = sin(rad);
        data->dirZ = cos(rad);
    }

    return effect;
}

void effKemuri5Main(EffectEntry* effect) {
    effKemuri5Data* data = effect->userdata;
    Vec position;
    s32 i;
    s32 any;
    f32 v;

    position = initializeVec(&data->position);
    any = 0;
    for (i = 0; i < effect->count; i++, data++) {
        if (data->alive == 0) {
            continue;
        }
        if (--data->timer <= 0) {
            data->alive = 0;
            continue;
        }
        any = 1;
        data->swayAngle = reviseAngle(12.0f + data->swayAngle);
        v = (f32)sin(6.2831855f * data->swayAngle / 360.0f);
        data->scaleX = 0.1f * v + data->baseScaleX;
        v = (f32)cos(6.2831855f * data->swayAngle / 360.0f);
        data->scaleY = 0.1f * v + data->baseScaleY;
        data->driftSpeed *= 0.83f;
        data->position.x += data->driftSpeed * data->dirX;
        data->position.z += data->driftSpeed * data->dirZ;
        data->accelY += data->accelStep;
        data->velY += data->accelY;
        data->position.y += data->velY;
        data->baseScaleX += 0.006f * (7.5f - data->baseScaleX);
        data->baseScaleY *= 0.98f;
        if (data->timer < 10) {
            data->baseScaleY *= 0.9f;
        }
        if (data->timer < 15) {
            data->alpha -= 0x10;
        }
    }
    if (any == 0) {
        effDelete(effect);
        return;
    }
    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKemuri5Disp, effect);
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
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    PSMTXScale(scale, 0.0625f, 0.0625f, 0.0f);
    GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_TEXC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
    effGetTexObjN64(0x4E, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetCullMode(GX_CULL_NONE);
    effSetVtxDescN64(kmr_a_1_v);

    for (i = 0; i < effect->count; i++, data++) {
        if (data->alive == 0) {
            continue;
        }
        GXSetTevColor(GX_TEVREG0, (GXColor){ 0x70, 0x60, 0x18, data->alpha });
        PSMTXTrans(trans, data->position.x, data->position.y, data->position.z);
        PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
        PSMTXConcat(trans, rot, trans);
        PSMTXScale(scale, data->scaleX, data->scaleY, data->scaleZ);
        PSMTXConcat(trans, scale, trans);
        PSMTXConcat(cam->view, trans, trans);
        GXLoadPosMtxImm(trans, GX_PNMTX0);
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(0, 3, 1, 0, 0, 2, 3, 0);
    }
}
