#include "eff/n64/eff_kemuri3_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math_ext.h>

typedef struct effKemuri3Data {
    s32 type;       // 0x0, 0 = inactive (skipped by Main and Disp)
    u8 pad_0x4[2];  // 0x4
    s16 life;       // 0x6, counts down; the entry goes inactive at 0
    u8 alpha;       // 0x8, GX_TEVREG0 alpha
    u8 pad_09[3];   // 0x9
    Vec position;   // 0xC
    f32 scaleX;     // 0x18
    f32 scaleY;     // 0x1C
    f32 scaleZ;     // 0x20
    f32 rotX;       // 0x24, degrees
    f32 rotY;       // 0x28, degrees; also the heading of dirX/dirZ
    f32 rotZ;       // 0x2C, degrees
    Mtx modelMtx;   // 0x30, built by Main, drawn by Disp under the camera view
    f32 speed;      // 0x60, horizontal speed along (dirX, dirZ)
    f32 field_0x64; // 0x64
    f32 field_0x68; // 0x68
    f32 field_0x6C; // 0x6C
    f32 velY;       // 0x70
    f32 velYAccel;  // 0x74, added to velY every frame
    f32 velYJerk;   // 0x78, added to velYAccel every frame
    f32 velYSnap;   // 0x7C, added to velYJerk every frame
    f32 dirX;       // 0x80, sin of the heading
    f32 dirZ;       // 0x84, cos of the heading
} effKemuri3Data;

EffectEntry* effKemuri3N64Entry(f32 x, f32 y, f32 z, f32 a4) {
    EffectEntry* effect;
    effKemuri3Data* data;
    s32 i;
    // One reused float local: MWCC only colours the constants to f19-f27 and the
    // arguments to f28-f31 (matching the target) when the int->float angle and the
    // rad value share this variable; with separate `rad` they land a block lower.
    f32 angle;

    effect = effEntry();
    effect->type = "Kemuri3N64";
    effect->count = 5;
    data = __memAlloc(HEAP_EFFECT, sizeof(effKemuri3Data) * 5);
    effect->userdata = data;
    effect->callback = effKemuri3Main;

    for (i = 0; i < 5; i++, data++) {
        data->type = 1;
        data->field_0x64 = 0.29999998f;
        data->field_0x68 = 0.0f;
        data->field_0x6C = 0.0f;
        data->position.x = x;
        data->position.y = y;
        data->position.z = z;
        data->scaleX = 1.0f;
        data->scaleY = 1.0f;
        data->scaleZ = 1.0f;
        data->alpha = 0xff;
        data->life = 0x3c;
        data->rotX = 0.0f;
        angle = a4 + (f32)(i * 72);
        data->rotY = angle;
        data->rotZ = 0.0f;
        data->velY = 1.75f;
        data->velYAccel = -0.08f;
        data->velYJerk = 0.0f;
        data->velYSnap = 0.0f;
        data->speed = -3.9f;
        angle = reviseAngle(data->rotY);
        angle = 6.2831855f * angle / 360.0f;
        data->dirX = sin(angle);
        data->dirZ = cos(angle);
    }

    return effect;
}

// Local copy of system.c's inline helper so it inlines here as in the original.
inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

void effKemuri3Main(EffectEntry* effect) {
    effKemuri3Data* data = effect->userdata;
    Vec position = initializeVec(&data->position);
    Mtx rotZ;
    Mtx rotY;
    Mtx rotX;
    Mtx trans;
    Mtx scale;
    s32 i;
    s32 active;

    active = 0;

    for (i = 0; i < effect->count; i++, data++) {
        if (data->type == 0) {
            continue;
        }
        if (--data->life <= 0) {
            data->type = 0;
            continue;
        }
        active = 1;

        data->speed *= 0.85f;
        data->position.x += data->speed * data->dirX;
        data->position.z += data->speed * data->dirZ;

        data->velYJerk += data->velYSnap;
        data->velYAccel += data->velYJerk;
        data->velY += data->velYAccel;
        data->position.y += data->velY;

        if (data->velY < 0.0f) {
            data->velYJerk = 0.004f;
        }

        data->rotX -= 1.0f;
        if (data->rotX < -100.0f) {
            data->rotX = -100.0f;
        }

        PSMTXRotRad(rotX, 'x', 0.017453292f * data->rotX);
        PSMTXRotRad(rotY, 'y', 0.017453292f * data->rotY);
        PSMTXRotRad(rotZ, 'z', 0.017453292f * data->rotZ);
        PSMTXTrans(trans, data->position.x, data->position.y, data->position.z);
        PSMTXScale(scale, data->scaleX, data->scaleY, data->scaleZ);
        PSMTXConcat(rotX, rotZ, rotZ);
        PSMTXConcat(rotY, rotZ, rotZ);
        PSMTXConcat(rotZ, scale, scale);
        PSMTXConcat(trans, scale, data->modelMtx);
    }

    if (active == 0) {
        effDelete(effect);
    }
    else {
        dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKemuri3Disp, effect);
    }
}

void effKemuri3Disp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effKemuri3Data* data;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx mtx;
    s32 i;

    cam = camGetPtr(camId);
    data = effect->userdata;

    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x3c, 0, 0x7d);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xf, 0xa, 8, 0xf);
    GXSetTevAlphaIn(0, 7, 1, 4, 7);
    effGetTexObjN64(0xf, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xb, 1);
    GXSetVtxDesc(0xd, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xb, 1, 5, 0);
    GXSetVtxAttrFmt(0, 0xd, 1, 4, 0);

    for (i = 0; i < effect->count; i++, data++) {
        if (data->type == 0) {
            continue;
        }
        GXSetTevColor(1, (GXColor){ 0x70, 0x60, 0x19, data->alpha });
        PSMTXConcat(cam->view, data->modelMtx, mtx);
        GXLoadPosMtxImm(mtx, 0);
        GXBegin(0x80, 0, 4);
        GXPosition3f32(8.0f, 0.0f, 8.0f);
        GXColor4u8(0xff, 0xff, 0xff, 0xff);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(8.0f, 0.0f, -8.0f);
        GXColor4u8(0xff, 0xff, 0xff, 0xff);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(-8.0f, 0.0f, -8.0f);
        GXColor4u8(0xff, 0xff, 0xff, 0xff);
        GXTexCoord2f32(0.0f, 1.0f);
        GXPosition3f32(-8.0f, 0.0f, 8.0f);
        GXColor4u8(0xff, 0xff, 0xff, 0xff);
        GXTexCoord2f32(0.0f, 0.0f);
    }
}

