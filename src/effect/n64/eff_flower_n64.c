#include "eff/n64/eff_flower_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>

// Local copy of system.c's inline helper so it inlines here (as in eff_water_damage_n64.c).
static inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

EffectEntry* effFlowerN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z) {
    static s32 kaiten;
    EffectEntry* effect;
    effFlowerData* data;

    effect = effEntry();
    effect->type = "FlowerN64";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(effFlowerData));
    effect->userdata = data;
    effect->callback = effFlowerMain;

    data->type = type;
    data->position.x = x;
    data->position.y = y + 10.0f;
    data->position.z = z;
    data->velX = 0.0f;
    data->velY = 0.0f;
    data->velZ = 0.0f;

    if (type == 0) {
        data->accelY = 0.12f;
        data->accelYDelta = -0.0152f;
        data->velZ = 0.0f;
        data->angleX = 0.0f;
        data->angleY = 0.0f;
        data->angleXVel = 3.0f;
        data->angleYVel = kaiten != 0 ? -10 : 10;
        data->life = lifetime;
    } else {
        data->accelY = 0.12f;
        data->accelYDelta = -0.0152f;
        data->angleX = (rand() % 2) * 30 - 15;
        data->angleY = rand() % 361;
        data->angleXVel = 0.0f;
        data->angleYVel = (rand() % 2) * 8 - 4;
        data->life = lifetime;
        data->age = 0;
        data->field_0x44 = rand() % 11;
        data->field_0x40 = rand() % 21;
        data->field_0x3C = (rand() % 2) * 2 - 1;
    }

    kaiten++;
    if (kaiten > 1) {
        kaiten = 0;
    }
    return effect;
}

void effFlowerMain(EffectEntry* effect) {
    effFlowerData* data;
    Vec position;
    s32 type;

    data = effect->userdata;
    position = initializeVec(&data->position);
    type = data->type;
    // With fewer than 150 frames left, drift sideways and spin; before that, ramp the rise speed up to 1.0.
    if (data->life < 150) {
        data->velX -= 0.001f;
        data->position.x += data->velX;
        data->position.z += data->velZ;
        data->angleX += data->angleXVel;
        if (type == 0) {
            if (data->angleX < 10.0f) {
                data->angleX = 10.0f;
                data->angleXVel = -data->angleXVel;
            }
            if (data->angleX > 45.0f) {
                data->angleX = 45.0f;
                data->angleXVel = -data->angleXVel;
            }
        }
        data->angleY += data->angleYVel;
        if (type == 0) {
            data->velY = 1.4f;
        } else {
            data->velY = 1.0f;
        }
    } else {
        data->accelYDelta += 0.0007f;
        data->accelY += data->accelYDelta;
        data->velY += data->accelY;
        if (data->velY > 1.0f) {
            data->velY = 1.0f;
        }
    }

    // Type 1 oscillates field_0x40 between 0 and 20.
    if (type == 1) {
        data->field_0x40 += data->field_0x3C;
        if (data->field_0x40 >= 20) {
            data->field_0x40 = 20;
            data->field_0x3C = -1;
        } else if (data->field_0x40 <= 0) {
            data->field_0x40 = 0;
            data->field_0x3C = 1;
        }
    }

    data->position.y += data->velY;
    data->life--;
    data->age++;
    if (data->life < 0) {
        effDelete(effect);
        return;
    }
    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effFlowerDisp, effect);
}

void effFlowerDisp(CameraId camId, void* param) {
    EffectEntry* effect;
    CameraEntry* cam;
    effFlowerData* data;
    GXTexObj texObj;
    Mtx trans;
    Mtx scale;
    Mtx rot;

    effect = param;
    cam = camGetPtr(camId);
    data = effect->userdata;

    PSMTXTrans(trans, data->position.x, data->position.y, data->position.z);
    PSMTXRotRad(rot, 'y', 0.017453292f * data->angleY);
    PSMTXConcat(trans, rot, trans);
    PSMTXRotRad(rot, 'x', 0.017453292f * data->angleX);
    PSMTXConcat(trans, rot, trans);
    PSMTXConcat(cam->view, trans, trans);
    GXLoadPosMtxImm(trans, GX_PNMTX0);

    GXSetTevColor(GX_TEVREG0, (GXColor){ 0x00, 0x00, 0x00, 0xFF });
    GXSetCullMode(GX_CULL_NONE);
    effGetTexObjN64(0x4c, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    PSMTXScale(scale, 0.0625f, 0.0625f, 0.0f);
    GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_A0, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
    effSetVtxDescN64(flowers_v);
    GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0x18);
    tri2(0, 1, 2, 0, 0, 2, 3, 0);
    tri2(4, 2, 5, 0, 4, 5, 6, 0);
    tri2(7, 8, 2, 0, 7, 2, 9, 0);
    tri2(10, 2, 11, 0, 10, 11, 12, 0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_A0, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_A0, GX_CA_ZERO);
    // Second vertex block in the shared flowers_v array.
    effSetVtxDescN64(flowers_v + 0xb6);
    GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0x18);
    tri2(0, 1, 2, 0, 0, 2, 3, 0);
    tri2(2, 4, 5, 0, 2, 5, 3, 0);
    tri2(6, 7, 8, 0, 6, 8, 9, 0);
    tri2(8, 10, 11, 0, 8, 11, 9, 0);
}
