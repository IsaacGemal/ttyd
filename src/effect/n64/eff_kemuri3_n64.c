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
    s32 type;       // 0x0
    u16 field_0x4;  // 0x4
    s16 field_06;   // 0x6
    u8 field_08;    // 0x8
    u8 pad_09[3];   // 0x9
    Vec position;   // 0xC
    f32 field_18;   // 0x18
    f32 field_1C;   // 0x1C
    f32 field_20;   // 0x20
    f32 field_24;   // 0x24
    f32 field_28;   // 0x28
    f32 field_2C;   // 0x2C
    Mtx field_30;   // 0x30
    f32 field_60;   // 0x60
    f32 field_64;   // 0x64
    f32 field_68;   // 0x68
    f32 field_6C;   // 0x6C
    f32 field_70;   // 0x70
    f32 field_74;   // 0x74
    f32 field_78;   // 0x78
    f32 field_7C;   // 0x7C
    f32 field_80;   // 0x80
    f32 field_84;   // 0x84
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
        data->field_64 = 0.29999998f;
        data->field_68 = 0.0f;
        data->field_6C = 0.0f;
        data->position.x = x;
        data->position.y = y;
        data->position.z = z;
        data->field_18 = 1.0f;
        data->field_1C = 1.0f;
        data->field_20 = 1.0f;
        data->field_08 = 0xff;
        data->field_06 = 0x3c;
        data->field_24 = 0.0f;
        angle = a4 + (f32)(i * 72);
        data->field_28 = angle;
        data->field_2C = 0.0f;
        data->field_70 = 1.75f;
        data->field_74 = -0.08f;
        data->field_78 = 0.0f;
        data->field_7C = 0.0f;
        data->field_60 = -3.9f;
        angle = reviseAngle(data->field_28);
        angle = 6.2831855f * angle / 360.0f;
        data->field_80 = sin(angle);
        data->field_84 = cos(angle);
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
        if (--data->field_06 <= 0) {
            data->type = 0;
            continue;
        }
        active = 1;

        data->field_60 *= 0.85f;
        data->position.x += data->field_60 * data->field_80;
        data->position.z += data->field_60 * data->field_84;

        data->field_78 += data->field_7C;
        data->field_74 += data->field_78;
        data->field_70 += data->field_74;
        data->position.y += data->field_70;

        if (data->field_70 < 0.0f) {
            data->field_78 = 0.004f;
        }

        data->field_24 -= 1.0f;
        if (data->field_24 < -100.0f) {
            data->field_24 = -100.0f;
        }

        PSMTXRotRad(rotX, 'x', 0.017453292f * data->field_24);
        PSMTXRotRad(rotY, 'y', 0.017453292f * data->field_28);
        PSMTXRotRad(rotZ, 'z', 0.017453292f * data->field_2C);
        PSMTXTrans(trans, data->position.x, data->position.y, data->position.z);
        PSMTXScale(scale, data->field_18, data->field_1C, data->field_20);
        PSMTXConcat(rotX, rotZ, rotZ);
        PSMTXConcat(rotY, rotZ, rotZ);
        PSMTXConcat(rotZ, scale, scale);
        PSMTXConcat(trans, scale, data->field_30);
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
        GXSetTevColor(1, (GXColor){ 0x70, 0x60, 0x19, data->field_08 });
        PSMTXConcat(cam->view, data->field_30, mtx);
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

