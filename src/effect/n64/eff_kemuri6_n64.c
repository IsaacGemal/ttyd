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



// .data quad vertex array for effSetVtxDescN64 (kmr_a_1_v).
static u32 kmr_a_1_v[14] = {
    0xFFF80007, 0x00000000, 0x0000FFFF, 0xFFFF0007,
    0x00070000, 0x02000000, 0xFFFFFFFF, 0xFFF8FFF8,
    0x00000000, 0x0200FFFF, 0xFFFF0007, 0xFFF80000,
    0x02000200, 0xFFFFFFFF,
};

typedef struct effKemuri6Data {
    s32 alive;       // 0x0, 1 while the puff runs (Main clears it at timer 0)
    u16 type;        // 0x4, entry type; 0 gets the mario-stay hold
    s16 timer;       // 0x6, frames left (15)
    u8 alpha;        // 0x8, TEVREG0 alpha, falls 15 per frame
    u8 pad_9[3];     // 0x9
    Vec position;    // 0xC
    f32 rotZ;        // 0x18, degrees, random start, Z rotation of the quad
    f32 scaleX;      // 0x1C, baseScaleX + 0.1 * sin(swayAngle)
    f32 scaleY;      // 0x20, baseScaleY + 0.1 * cos(swayAngle)
    f32 scaleZ;      // 0x24, 1.0, never changed
    f32 baseScaleX;  // 0x28, grows 0.14 per frame
    f32 baseScaleY;  // 0x2C, decays by 0.94 per frame
    f32 swayAngle;   // 0x30, degrees, +12 per frame
    f32 field_0x34;  // 0x34
    f32 velY;        // 0x38, added to position.y each frame
    f32 accelY;      // 0x3C, added to velY, starts -0.5
    f32 accelStep;   // 0x40, added to accelY each frame (0.0)
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
    effect->type = "Kemuri6N64";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(effKemuri6Data));
    effect->userdata = data;
    effect->callback = effKemuri6Main;

    for (i = 0; i < 1; i++) {
        data->alive = 1;
        data->type = type;
        data->accelStep = 0.0f;
        data->field_0x44 = 0.0f;
        data->position.x = x;
        data->position.y = y;
        data->position.z = z;
        data->scaleX = 1.0f;
        data->scaleY = 1.0f;
        data->scaleZ = 1.0f;
        data->alpha = 0xFF;
        data->baseScaleX = 0.03f * (f32)(rand() % 11) + 1.4f;
        data->baseScaleY = 0.03f * (f32)(rand() % 11) + 1.5f;
        data->swayAngle = (f32)(rand() % 61);
        data->type = type;
        data->velY = 2.0f;
        data->accelY = -0.5f;
        data->timer = 15;
        data->rotZ = (f32)(rand() % 361);
        data->alpha = 0xFF;
    }
    return effect;
}

void effKemuri6Main(EffectEntry* effect) {
    EffectEntry* e;
    effKemuri6Data* data;
    Vec position;
    s32 i;
    s32 active;
    f32 t;

    e = effect;
    data = e->userdata;
    position = initializeVec(&data->position);
    active = 0;
    for (i = 0; i < e->count; i++, data++) {
        if (data->alive != 0) {
            if (--data->timer <= 0) {
                data->alive = 0;
            }
            else {
                active = 1;
                data->swayAngle = reviseAngle(12.0f + data->swayAngle);
                t = (f32)sin(6.2831855f * data->swayAngle / 360.0f);
                data->scaleX = 0.1f * t + data->baseScaleX;
                t = (f32)cos(6.2831855f * data->swayAngle / 360.0f);
                data->scaleY = 0.1f * t + data->baseScaleY;
                if (data->type == 0
                    && data->timer > 4
                    && data->timer <= 5
                    && marioGetPtr()->currMotionId == MARIO_MOTION_STAY) {
                    data->timer++;
                    return;
                }
                data->accelY += data->accelStep;
                data->velY += data->accelY;
                data->position.y += data->velY;
                data->alpha -= 15;
                data->baseScaleX += 0.14f;
                data->baseScaleY *= 0.94f;
                if (data->velY < 0.0f) {
                    data->accelY = 0.0f;
                    data->velY = 0.0f;
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
    effSetVtxDescN64((effVtxArray*)kmr_a_1_v);

    for (i = 0; i < effect->count; i++, data++) {
        if (data->alive == 0) {
            continue;
        }
        GXSetTevColor(GX_TEVREG0, (GXColor){ 0x70, 0x60, 0x18, data->alpha });
        PSMTXTrans(model, data->position.x, data->position.y, data->position.z);
        PSMTXRotRad(rot, 'y', -camGetPtr(CAMERA_3D)->viewYaw * 0.017453292f);
        PSMTXConcat(model, rot, model);
        PSMTXScale(scale, data->scaleX, data->scaleY, data->scaleZ);
        PSMTXConcat(model, scale, model);
        PSMTXRotRad(rot, 'z', 0.017453292f * data->rotZ);
        PSMTXConcat(model, rot, model);
        PSMTXConcat(cam->view, model, model);
        GXLoadPosMtxImm(model, GX_PNMTX0);
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(0, 3, 1, 0, 0, 2, 3, 0);
    }
}

