#include "eff/n64/eff_flower_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>

// .sdata2: effFlowerDisp's TEV colour, first in the original unit's .sdata2.
const u32 dat_8041efc8 = 0x000000FF;

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
    data->field_0x10 = 0.0f;
    data->field_0x14 = 0.0f;
    data->field_0x18 = 0.0f;

    if (type == 0) {
        data->field_0x1C = 0.12f;
        data->field_0x20 = -0.0152f;
        data->field_0x18 = 0.0f;
        data->field_0x24 = 0.0f;
        data->field_0x28 = 0.0f;
        data->field_0x2C = 3.0f;
        data->field_0x30 = kaiten != 0 ? -10 : 10;
        data->field_0x34 = lifetime;
    } else {
        data->field_0x1C = 0.12f;
        data->field_0x20 = -0.0152f;
        data->field_0x24 = (rand() % 2) * 30 - 15;
        data->field_0x28 = rand() % 361;
        data->field_0x2C = 0.0f;
        data->field_0x30 = (rand() % 2) * 8 - 4;
        data->field_0x34 = lifetime;
        data->field_0x38 = 0;
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
    if (data->field_0x34 < 150) {
        data->field_0x10 -= 0.001f;
        data->position.x += data->field_0x10;
        data->position.z += data->field_0x18;
        data->field_0x24 += data->field_0x2C;
        if (type == 0) {
            if (data->field_0x24 < 10.0f) {
                data->field_0x24 = 10.0f;
                data->field_0x2C = -data->field_0x2C;
            }
            if (data->field_0x24 > 45.0f) {
                data->field_0x24 = 45.0f;
                data->field_0x2C = -data->field_0x2C;
            }
        }
        data->field_0x28 += data->field_0x30;
        if (type == 0) {
            data->field_0x14 = 1.4f;
        } else {
            data->field_0x14 = 1.0f;
        }
    } else {
        data->field_0x20 += 0.0007f;
        data->field_0x1C += data->field_0x20;
        data->field_0x14 += data->field_0x1C;
        if (data->field_0x14 > 1.0f) {
            data->field_0x14 = 1.0f;
        }
    }

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

    data->position.y += data->field_0x14;
    data->field_0x34--;
    data->field_0x38++;
    if (data->field_0x34 < 0) {
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
    GXColor color;
    Mtx trans;
    Mtx scale;
    Mtx rot;

    effect = param;
    cam = camGetPtr(camId);
    data = effect->userdata;

    PSMTXTrans(trans, data->position.x, data->position.y, data->position.z);
    PSMTXRotRad(rot, 'y', 0.017453292f * data->field_0x28);
    PSMTXConcat(trans, rot, trans);
    PSMTXRotRad(rot, 'x', 0.017453292f * data->field_0x24);
    PSMTXConcat(trans, rot, trans);
    PSMTXConcat(cam->view, trans, trans);
    GXLoadPosMtxImm(trans, 0);

    color = *(GXColor*)&dat_8041efc8;
    GXSetTevColor(1, color);
    GXSetCullMode(0);
    effGetTexObjN64(0x4c, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
    PSMTXScale(scale, 0.0625f, 0.0625f, 0.0f);
    GXLoadTexMtxImm(scale, 0x1e, 1);
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xff);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xf, 8, 3, 0xf);
    GXSetTevAlphaIn(0, 7, 4, 1, 7);
    effSetVtxDescN64((effVtxArray*)flowers_v);
    GXBegin(0x90, 0, 0x18);
    tri2(0, 1, 2, 0, 0, 2, 3, 0);
    tri2(4, 2, 5, 0, 4, 5, 6, 0);
    tri2(7, 8, 2, 0, 7, 2, 9, 0);
    tri2(10, 2, 11, 0, 10, 11, 12, 0);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0xff, 0xff, 4);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xf, 0xa, 3, 0xf);
    GXSetTevAlphaIn(0, 7, 5, 1, 7);
    effSetVtxDescN64((effVtxArray*)((u8*)flowers_v + 0xb6));
    GXBegin(0x90, 0, 0x18);
    tri2(0, 1, 2, 0, 0, 2, 3, 0);
    tri2(2, 4, 5, 0, 2, 5, 3, 0);
    tri2(6, 7, 8, 0, 6, 8, 9, 0);
    tri2(8, 10, 11, 0, 8, 11, 9, 0);
}
