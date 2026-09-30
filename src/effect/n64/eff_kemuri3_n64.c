#include "eff/n64/eff_kemuri3_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math_ext.h>

const char str_802FA8E4[];

extern const f32 float_0_8041f2f0;
extern const f32 float_1_8041f2f4;
extern const f32 float_0p85_8041f2fc;
extern const f32 float_0p004_8041f300;
extern const f32 float_neg100_8041f304;
extern const f32 float_deg2rad_8041f308;
extern const f32 float_0p3_8041f30c;
extern const f32 float_1p75_8041f310;
extern const f32 float_neg0p08_8041f314;
extern const f32 float_neg3p9_8041f318;
extern const f32 float_6p2832_8041f31c;
extern const f32 float_360_8041f320;
extern const f32 float_8_8041f2ec;
extern const f32 float_neg8_8041f2f8;
extern const u32 dat_8041f2e8;

typedef struct effKemuri3Data {
    s32 type;       // 0x0
    u16 unk_04;     // 0x4
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

// NONMATCHING (98.85%): instructions differ; see check.py
EffectEntry* effKemuri3N64Entry(f32 x, f32 y, f32 z, f32 a4) {
    EffectEntry* effect;
    effKemuri3Data* data;
    s32 i;
    s32 angle;
    f32 rad;

    effect = effEntry();
    effect->type = str_802FA8E4;
    effect->count = 5;
    data = __memAlloc(HEAP_EFFECT, sizeof(effKemuri3Data) * 5);
    effect->userdata = data;
    effect->callback = effKemuri3Main;

    for (i = 0; i < 5; i++, data++) {
        data->type = 1;
        data->field_64 = float_0p3_8041f30c;
        data->field_68 = float_0_8041f2f0;
        data->field_6C = float_0_8041f2f0;
        data->position.x = x;
        data->position.y = y;
        data->position.z = z;
        data->field_18 = float_1_8041f2f4;
        data->field_1C = float_1_8041f2f4;
        data->field_20 = float_1_8041f2f4;
        data->field_08 = 0xff;
        data->field_06 = 0x3c;
        data->field_24 = float_0_8041f2f0;
        data->field_28 = a4 + (f32)(i * 72);
        data->field_2C = float_0_8041f2f0;
        data->field_70 = float_1p75_8041f310;
        data->field_74 = float_neg0p08_8041f314;
        data->field_78 = float_0_8041f2f0;
        data->field_7C = float_0_8041f2f0;
        data->field_60 = float_neg3p9_8041f318;
        rad = float_6p2832_8041f31c * reviseAngle(data->field_28) / float_360_8041f320;
        data->field_80 = sin(rad);
        data->field_84 = cos(rad);
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

#pragma opt_common_subs off // subexpression elimination merges the field_24 decrement with the compare, changing FPR allocation
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

        data->field_60 *= float_0p85_8041f2fc;
        data->position.x += data->field_60 * data->field_80;
        data->position.z += data->field_60 * data->field_84;

        data->field_78 += data->field_7C;
        data->field_74 += data->field_78;
        data->field_70 += data->field_74;
        data->position.y += data->field_70;

        if (data->field_70 < float_0_8041f2f0) {
            data->field_78 = float_0p004_8041f300;
        }

        data->field_24 -= float_1_8041f2f4;
        if (data->field_24 < float_neg100_8041f304) {
            data->field_24 = float_neg100_8041f304;
        }

        PSMTXRotRad(rotX, 'x', float_deg2rad_8041f308 * data->field_24);
        PSMTXRotRad(rotY, 'y', float_deg2rad_8041f308 * data->field_28);
        PSMTXRotRad(rotZ, 'z', float_deg2rad_8041f308 * data->field_2C);
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
#pragma opt_common_subs reset

static inline void kemuri3SetColor(u8 alpha) {
    GXColor color = *(GXColor*)&dat_8041f2e8;
    color.a = alpha;
    GXSetTevColor(1, color);
}

// Volatile load forces the original's one-load-per-store cadence for the shared
// .sdata2 vertex constants; plain const loads get CSE'd into FPRs.
#ifdef MUST_MATCH
static inline void kemuri3WriteF32(const f32* value) {
    GXWGFifo.f32 = *(volatile const f32*)value;
}
#endif

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
        kemuri3SetColor(data->field_08);
        PSMTXConcat(cam->view, data->field_30, mtx);
        GXLoadPosMtxImm(mtx, 0);
        GXBegin(0x80, 0, 4);
#ifdef MUST_MATCH
        kemuri3WriteF32(&float_8_8041f2ec);
        kemuri3WriteF32(&float_0_8041f2f0);
        kemuri3WriteF32(&float_8_8041f2ec);
        GXColor4u8(0xff, 0xff, 0xff, 0xff);
        kemuri3WriteF32(&float_1_8041f2f4);
        kemuri3WriteF32(&float_0_8041f2f0);
        kemuri3WriteF32(&float_8_8041f2ec);
        kemuri3WriteF32(&float_0_8041f2f0);
        kemuri3WriteF32(&float_neg8_8041f2f8);
        GXColor4u8(0xff, 0xff, 0xff, 0xff);
        kemuri3WriteF32(&float_1_8041f2f4);
        kemuri3WriteF32(&float_1_8041f2f4);
        kemuri3WriteF32(&float_neg8_8041f2f8);
        kemuri3WriteF32(&float_0_8041f2f0);
        kemuri3WriteF32(&float_neg8_8041f2f8);
        GXColor4u8(0xff, 0xff, 0xff, 0xff);
        kemuri3WriteF32(&float_0_8041f2f0);
        kemuri3WriteF32(&float_1_8041f2f4);
        kemuri3WriteF32(&float_neg8_8041f2f8);
        kemuri3WriteF32(&float_0_8041f2f0);
        kemuri3WriteF32(&float_8_8041f2ec);
        GXColor4u8(0xff, 0xff, 0xff, 0xff);
        kemuri3WriteF32(&float_0_8041f2f0);
        kemuri3WriteF32(&float_0_8041f2f0);
#else
        GXPosition3f32(float_8_8041f2ec, float_0_8041f2f0, float_8_8041f2ec);
        GXColor4u8(0xff, 0xff, 0xff, 0xff);
        GXTexCoord2f32(float_1_8041f2f4, float_0_8041f2f0);
        GXPosition3f32(float_8_8041f2ec, float_0_8041f2f0, float_neg8_8041f2f8);
        GXColor4u8(0xff, 0xff, 0xff, 0xff);
        GXTexCoord2f32(float_1_8041f2f4, float_1_8041f2f4);
        GXPosition3f32(float_neg8_8041f2f8, float_0_8041f2f0, float_neg8_8041f2f8);
        GXColor4u8(0xff, 0xff, 0xff, 0xff);
        GXTexCoord2f32(float_0_8041f2f0, float_1_8041f2f4);
        GXPosition3f32(float_neg8_8041f2f8, float_0_8041f2f0, float_8_8041f2ec);
        GXColor4u8(0xff, 0xff, 0xff, 0xff);
        GXTexCoord2f32(float_0_8041f2f0, float_0_8041f2f0);
#endif
    }
}

const char str_802FA8E4[] = "Kemuri3N64";
