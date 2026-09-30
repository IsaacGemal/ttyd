#include "eff/n64/eff_cool_leaf_n64.h"
#include "eff/eff_n64.h"
#include "driver/effdrv.h"
#include "memory.h"
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
