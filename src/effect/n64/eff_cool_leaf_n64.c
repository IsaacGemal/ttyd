#include "eff/n64/eff_cool_leaf_n64.h"
#include "eff/eff_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <math.h>
#include <stdlib.h>

extern const f32 float_0_8041ec60;
extern const f32 float_0p1_8041ecb0;
extern const f32 float_neg3_8041ecb4;
extern const f32 float_3_8041ecb8;
extern const f32 float_2_8041ec70;
extern const f32 float_neg6_8041ecbc;
extern const f32 float_6_8041ecc0;
extern const f32 float_0p008_8041ecc4;
extern const f32 float_0p05_8041ec88;
extern const f32 float_deg2rad_8041ec54;
extern const f32 float_4_8041ec58;
extern const f32 float_1_8041ec5c;
extern const f32 float_0p3_8041ec64;
extern const f32 float_0p8_8041ec68;
extern const f32 float_6p2832_8041ec6c;
extern const f32 float_360_8041ec74;
extern const f32 float_neg0p05_8041ec78;
extern const f32 float_0p2_8041ec7c;
extern const f32 float_0p94_8041ec80;
extern const f32 float_0p95_8041ec84;
extern const f32 float_0p92_8041ec8c;
extern const f32 float_10_8041ec90;
extern const f32 float_0p5_8041ec94;
extern const f32 float_30_8041ec98;
extern const f32 float_neg200_8041ec9c;
extern const f32 float_0p4_8041eca0;
extern const f32 float_0p98_8041eca4;
extern const f32 float_0p8464_8041eca8;
extern const f32 float_0p9604_8041ecac;
extern const u32 dat_8041ec50;
extern u8 size12x12_tex64x64_brown_vtx[];
extern u8 size12x12_tex64x64_red_vtx[];

static inline void effCoolLeafSetColor(u8 a) {
    GXColor color = *(GXColor*)&dat_8041ec50;
    color.a = a;
    GXSetTevColor(GX_TEVREG0, color);
}

const f64 double_to_int_802fa470 = 4503601774854144.0;
const char str_802FA478[] = "CoolLeafN64";

typedef struct effCoolLeafData {
    s32 type;        // 0x00
    f32 x;           // 0x04
    f32 y;           // 0x08
    f32 z;           // 0x0C
    f32 field_0x10;  // 0x10
    f32 field_0x14;  // 0x14
    f32 field_0x18;  // 0x18
    f32 field_0x1C;  // 0x1C
    f32 field_0x20;  // 0x20
    s32 field_0x24;  // 0x24
    s32 field_0x28;  // 0x28
    s32 field_0x2C;  // 0x2C
    f32 field_0x30;  // 0x30
    f32 field_0x34;  // 0x34
    f32 field_0x38;  // 0x38
} effCoolLeafData;

EffectEntry* effCoolLeafN64Entry(s32 type, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    effCoolLeafData* data;
    effCoolLeafData* ptr;
    s32 i;
    s32 rnd01;
    s32 rnd11;
    s32 rnd361;
    s32 rnd101;

    effect = effEntry();
    effect->type = str_802FA478;
    effect->count = 5;
    data = __memAlloc(HEAP_EFFECT, sizeof(effCoolLeafData) * 5);
    effect->userdata = data;
    effect->callback = effCoolLeafMain;
    data->type = type;
    data->x = x;
    data->y = y;
    data->z = z;
    data->field_0x28 = 100;
    data->field_0x2C = 0;
    data->field_0x24 = 0;
    ptr = data + 1;
    for (i = 1; i < 5; i++, ptr++) {
        rnd01 = rand() % 51;
        rnd11 = rand() % 21;
        rnd361 = rand() % 361;
        rnd101 = rand() % 101;
        ptr->x = (f32)(rnd01 + 75);
        ptr->y = (f32)(rnd11 + 70);
        ptr->z = float_0_8041ec60;
        ptr->field_0x18 = (f32)rnd361;
        ptr->field_0x20 = (f32)(360 - rnd361);
        switch (type) {
            case 0:
                ptr->field_0x1C = float_0p1_8041ecb0 * (f32)rnd101;
                ptr->field_0x10 = float_0_8041ec60;
                ptr->field_0x14 = float_0_8041ec60;
                ptr->field_0x30 = float_neg3_8041ecb4;
                ptr->field_0x34 = float_0_8041ec60;
                ptr->field_0x38 = float_3_8041ecb8;
                break;
            case 1:
                ptr->field_0x1C = float_0p1_8041ecb0 * (f32)rnd101 * float_2_8041ec70;
                ptr->field_0x10 = float_0_8041ec60;
                ptr->field_0x14 = float_0_8041ec60;
                ptr->field_0x30 = float_neg6_8041ecbc;
                ptr->field_0x34 = float_0_8041ec60;
                ptr->field_0x38 = float_6_8041ecc0;
                break;
            case 2:
                ptr->field_0x1C = float_0p1_8041ecb0 * (f32)rnd101 * float_2_8041ec70;
                ptr->field_0x10 = -ptr->x * (float_0p008_8041ecc4 * (f32)(rand() % 11) + float_0p05_8041ec88);
                ptr->field_0x14 = -ptr->y * (float_0p008_8041ecc4 * (f32)(rand() % 11) + float_0p05_8041ec88);
                break;
            default:
                break;
        }
    }
    return effect;
}

static inline Vec makeVec(f32 x, f32 y, f32 z) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = x;
    temp.y = y;
    temp.z = z;
    return temp;
}

// Every instruction is byte-identical except the callee-saved FPRs holding the
// loop-invariant constants of the type-1 and type-2 loops. The type-1 target uses
// f28=double, f29=10.0, f30=30.0, f31=0.5; this compiler hands them f31=double,
// f30=10.0, f29=0.5, f28=30.0. The type-2 target uses f22=0.5, f23=10.0, f24=360.0,
// f30=double, f31=6.2832; this compiler uses f22=0.5, f28=double, f29=6.2832,
// f30=360.0, f31=10.0. The type-0 loop (and the rest of the function) matches exactly.
// Adding a local to pin a constant (as effFallLeafMain does with `half`) shifts the
// type-0 loop too; opt_common_subs/opt_lifetimes/opt_propagation off make it worse.
// NONMATCHING (99.82%): the type-1/type-2 loop-constant FPRs are permuted.
void effCoolLeafMain(EffectEntry* effect) {
    effCoolLeafData* data = effect->userdata;
    Vec position;
    s32 type;
    s32 counter;
    s32 life;
    s32 idx;
    s32 i;
    f32 s;
    f32 t;
    f32 u;
    f32 sn;

    position = makeVec(data->x, data->y, data->z);

    data->field_0x28--;
    data->field_0x2C++;
    if (data->field_0x28 < 0) {
        effDelete(effect);
        return;
    }
    counter = data->field_0x2C;
    type = data->type;
    life = data->field_0x28;
    if (counter < 10) {
        data->field_0x24 = (s32)(float_0p3_8041ec64 * (f32)(255 - data->field_0x24) + (f32)data->field_0x24);
    }
    if (life < 10) {
        data->field_0x24 = (s32)((f32)data->field_0x24 * float_0p8_8041ec68);
    }
    data++;
    switch (type) {
    case 0:
        i = 1;
        idx = counter + 20;
        for (; i < effect->count; idx += 20, i++, data++) {
            s = (f32)sin(float_6p2832_8041ec6c * (float_2_8041ec70 * data->field_0x18) / float_360_8041ec74);
            data->field_0x14 += float_neg0p05_8041ec78;
            t = float_0p2_8041ec7c * s;
            data->field_0x10 += t;
            if (type == 0) {
                data->field_0x10 *= float_0p94_8041ec80;
                sn = (f32)sin(float_6p2832_8041ec6c * data->field_0x18 / float_360_8041ec74);
                data->field_0x14 *= float_0p05_8041ec88 * sn + float_0p95_8041ec84;
            }
            else {
                data->field_0x14 += float_neg0p05_8041ec78;
                data->field_0x10 *= float_0p92_8041ec8c;
            }
            data->field_0x1C += (f32)(s32)(effTblRandN64(50, idx) - 25);
            data->field_0x18 += float_10_8041ec90 * (f32)sin(float_6p2832_8041ec6c * data->field_0x1C / float_360_8041ec74);
            data->field_0x20 += float_10_8041ec90 * (f32)cos(float_6p2832_8041ec6c * (float_0p5_8041ec94 * data->field_0x1C) / float_360_8041ec74);
            data->x += data->field_0x10;
            data->y += data->field_0x14;
            if (data->y < float_30_8041ec98) {
                if (data->x > float_neg200_8041ec9c) {
                    data->field_0x30 -= float_0p4_8041eca0;
                }
                else {
                    data->field_0x30 += float_0p4_8041eca0;
                }
                if (data->z > float_0_8041ec60) {
                    data->field_0x38 -= float_0p4_8041eca0;
                }
                else {
                    data->field_0x38 += float_0p4_8041eca0;
                }
                data->field_0x30 *= float_0p98_8041eca4;
                data->field_0x38 *= float_0p98_8041eca4;
                data->field_0x14 *= float_0p98_8041eca4;
            }
            else {
                if (data->x > float_0_8041ec60) {
                    data->field_0x30 -= float_0p4_8041eca0;
                }
                else {
                    data->field_0x30 += float_0p4_8041eca0;
                }
                if (data->z > float_0_8041ec60) {
                    data->field_0x38 -= float_0p4_8041eca0;
                }
                else {
                    data->field_0x38 += float_0p4_8041eca0;
                }
                data->field_0x30 *= float_0p98_8041eca4;
                data->field_0x38 *= float_0p98_8041eca4;
            }
            data->x += data->field_0x30;
            data->y += data->field_0x34;
            data->z += data->field_0x38;
        }
        break;
    case 1:
        i = 1;
        idx = counter + 20;
        for (; i < effect->count; idx += 20, i++, data++) {
            s = (f32)sin(float_6p2832_8041ec6c * (float_2_8041ec70 * data->field_0x18) / float_360_8041ec74);
            data->field_0x14 += float_neg0p05_8041ec78;
            t = float_0p2_8041ec7c * s;
            u = float_2_8041ec70 * t;
            data->field_0x10 += u;
            data->field_0x14 += float_neg0p05_8041ec78;
            data->field_0x10 *= float_0p8464_8041eca8;
            data->field_0x1C += (f32)(s32)(effTblRandN64(50, idx) - 25);
            data->field_0x18 += float_10_8041ec90 * (f32)sin(float_6p2832_8041ec6c * data->field_0x1C / float_360_8041ec74);
            data->field_0x20 += float_10_8041ec90 * (f32)cos(float_6p2832_8041ec6c * (float_0p5_8041ec94 * data->field_0x1C) / float_360_8041ec74);
            data->x += data->field_0x10;
            data->y += data->field_0x14;
            if (data->y < float_30_8041ec98) {
                if (data->x > float_neg200_8041ec9c) {
                    data->field_0x30 -= float_0p8_8041ec68;
                }
                else {
                    data->field_0x30 += float_0p8_8041ec68;
                }
                if (data->z > float_0_8041ec60) {
                    data->field_0x38 -= float_0p8_8041ec68;
                }
                else {
                    data->field_0x38 += float_0p8_8041ec68;
                }
                data->field_0x30 *= float_0p9604_8041ecac;
                data->field_0x38 *= float_0p9604_8041ecac;
                data->field_0x14 *= float_0p9604_8041ecac;
            }
            else {
                if (data->x > float_0_8041ec60) {
                    data->field_0x30 -= float_0p8_8041ec68;
                }
                else {
                    data->field_0x30 += float_0p8_8041ec68;
                }
                if (data->z > float_0_8041ec60) {
                    data->field_0x38 -= float_0p8_8041ec68;
                }
                else {
                    data->field_0x38 += float_0p8_8041ec68;
                }
                data->field_0x30 *= float_0p9604_8041ecac;
                data->field_0x38 *= float_0p9604_8041ecac;
            }
            data->x += data->field_0x30;
            data->y += data->field_0x34;
            data->z += data->field_0x38;
        }
        break;
    case 2:
        i = 1;
        idx = counter + 20;
        for (; i < effect->count; idx += 20, i++, data++) {
            data->field_0x1C += (f32)(s32)(effTblRandN64(50, idx) - 25);
            data->field_0x18 += float_10_8041ec90 * (f32)sin(float_6p2832_8041ec6c * data->field_0x1C / float_360_8041ec74);
            data->field_0x20 += float_10_8041ec90 * (f32)cos(float_6p2832_8041ec6c * (float_0p5_8041ec94 * data->field_0x1C) / float_360_8041ec74);
            data->x += data->field_0x10;
            data->y += data->field_0x14;
        }
        break;
    }
    dispEntry(4, dispCalcZ(&position), 2, effCoolLeafDisp, effect);
}

void effCoolLeafDisp(CameraId cameraId, void* param) {
    EffectEntry* effect = param;
    CameraEntry* cam = camGetPtr(cameraId);
    effCoolLeafData* data = effect->userdata;
    GXTexObj texObj;
    Mtx trans;
    Mtx scale;
    Mtx rot;
    Mtx work;
    s32 type = data->type;
    s32 i;
    f32 deg2rad;

    PSMTXTrans(trans, data->x, data->y, data->z);
    PSMTXRotRad(rot, 'y', float_deg2rad_8041ec54 * -camGetPtr(CAMERA_3D)->viewYaw);
    PSMTXConcat(trans, rot, work);
    PSMTXConcat(cam->view, work, work);

    effCoolLeafSetColor(data->field_0x24);

    effGetTexObjN64(0x14, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);

    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    PSMTXScale(scale, float_4_8041ec58, float_1_8041ec5c, float_0_8041ec60);
    GXLoadTexMtxImm(scale, 0x1E, 1);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xA, 8, 0xF);
    GXSetTevAlphaIn(0, 7, 1, 4, 7);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 3);
    GXSetVtxDesc(0xB, 3);
    GXSetVtxDesc(0xD, 3);
    GXSetVtxAttrFmt(0, 9, 1, 3, 0);
    GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 3, 0xC);

    deg2rad = float_deg2rad_8041ec54;
    for (i = 1, data++; i < effect->count; i++, data++) {
        PSMTXTrans(trans, data->x, data->y, data->z);
        PSMTXRotRad(rot, 'z', deg2rad * data->field_0x18);
        PSMTXConcat(trans, rot, trans);
        PSMTXRotRad(rot, 'y', deg2rad * data->field_0x20);
        PSMTXConcat(trans, rot, trans);
        PSMTXConcat(work, trans, trans);
        GXLoadPosMtxImm(trans, 0);
        GXSetCurrentMtx(0);
        switch (type) {
        case 0:
            GXSetArray(9, size12x12_tex64x64_brown_vtx, 0xE);
            GXSetArray(0xB, size12x12_tex64x64_brown_vtx + 0xA, 0xE);
            GXSetArray(0xD, size12x12_tex64x64_brown_vtx + 6, 0xE);
            break;
        case 1:
            GXSetArray(9, size12x12_tex64x64_red_vtx, 0xE);
            GXSetArray(0xB, size12x12_tex64x64_red_vtx + 0xA, 0xE);
            GXSetArray(0xD, size12x12_tex64x64_red_vtx + 6, 0xE);
            break;
        }
        GXBegin(0x90, 0, 0xC);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
        tri2(6, 5, 4, 4, 7, 6, 4, 4);
    }
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const u32 dat_8041ec50 = 0x14641400;
const f32 float_deg2rad_8041ec54 = 0.017453292f;
const f32 float_4_8041ec58 = 4.0f;
const f32 float_1_8041ec5c = 1.0f;
const f32 float_0_8041ec60 = 0.0f;
const f32 float_0p3_8041ec64 = 0.3f;
const f32 float_0p8_8041ec68 = 0.8f;
const f32 float_6p2832_8041ec6c = 6.2831855f;
const f32 float_2_8041ec70 = 2.0f;
const f32 float_360_8041ec74 = 360.0f;
const f32 float_neg0p05_8041ec78 = -0.05f;
const f32 float_0p2_8041ec7c = 0.2f;
const f32 float_0p94_8041ec80 = 0.94f;
const f32 float_0p95_8041ec84 = 0.95f;
const f32 float_0p05_8041ec88 = 0.05f;
const f32 float_0p92_8041ec8c = 0.92f;
const f32 float_10_8041ec90 = 10.0f;
const f32 float_0p5_8041ec94 = 0.5f;
const f32 float_30_8041ec98 = 30.0f;
const f32 float_neg200_8041ec9c = -200.0f;
const f32 float_0p4_8041eca0 = 0.4f;
const f32 float_0p98_8041eca4 = 0.98f;
const f32 float_0p8464_8041eca8 = 0.8464f;
const f32 float_0p9604_8041ecac = 0.96040004f;
const f32 float_0p1_8041ecb0 = 0.1f;
const f32 float_neg3_8041ecb4 = -3.0f;
const f32 float_3_8041ecb8 = 3.0f;
const f32 float_neg6_8041ecbc = -6.0f;
const f32 float_6_8041ecc0 = 6.0f;
const f32 float_0p008_8041ecc4 = 0.008f;
