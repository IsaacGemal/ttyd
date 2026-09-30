#include "eff/n64/eff_opuku_jetw_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// The two 10-byte-per-entry spiral tables live in .data and are addressed through the
// shared base symbol lbl_803A33D8 by the original display code.
extern f32 lbl_803A33D8[];
s32 dat_ry[8] = { 0, 0x28, 0x37, 0xBE, 0x14, 0x82, 0x10E, 0x12C };
const f32 float_deg2rad_8041f7c0 = 0.017453292f;
const f32 float_0p03125_8041f7c4 = 0.03125f;
const f32 float_0_8041f7c8 = 0.0f;
const f32 float_1_8041f7cc = 1.0f;
const f32 float_10_8041f7d0 = 10.0f;
const f32 float_0p01_8041f7d4 = 0.01f;
GXColor unk_804231c8;
GXColor unk_804231cc;
GXColor unk_804231d0;

// Local copy of system.c's inline helper so it inlines here (as in eff_kemuri11.c).
inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

EffectEntry* effOpukuJetwN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
    EffectEntry* effect = effEntry();
    effOpukuJetwData* data;

    effect->type = "OpukuJetwN64";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, 0x44);
    effect->userdata = data;
    effect->callback = effOpukuJetwMain;
    effect->flags |= 2;

    data->type = type;
    data->counter = 0;
    if (duration <= 0) {
        data->lifetime = 1000;
    } else {
        data->lifetime = duration;
    }

    data->field_0x24 = 0xFF;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->scale = scale;
    data->field_0x18 = 0x46;
    data->field_0x1C = 0xB4;
    data->field_0x20 = 0xFF;
    data->field_0x28 = 0xFF;
    data->field_0x2C = 0xFF;
    data->field_0x30 = 0xFF;
    data->field_0x38 = 0.0f;
    data->field_0x3C = 1.0f;
    data->field_0x40 = 1.0f;

    return effect;
}

void effOpukuJetwMain(EffectEntry* effect) {
    effOpukuJetwData* data = effect->userdata;
    Vec position = initializeVec(&data->position);

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->lifetime = 0x10;
    }
    if (data->lifetime < 1000) {
        data->lifetime--;
    }
    data->counter++;
    if (data->lifetime < 0) {
        effDelete(effect);
    } else {
        if (data->counter < 8) {
            data->field_0x24 = data->counter * 32 + 31;
        }
        dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effOpukuJetwDisp, effect);
    }
}

// NONMATCHING (89.00%): register allocation and loop codegen differ. Details:
// The original keeps the .data base in r20 and the selected spiral table in r22; mwcc here uses
// r31 for the base and spills the table to 0x12c(r1), reloading it every inner iteration. It
// also hoists `columns - 1` and `half` into stack slots (0x130/0x128) instead of recomputing
// `columns - 1` inline and keeping `half` in a register, which in turn pushes the cached data
// fields into r19-r23 instead of r25-r31. Because the GXColor call temporaries overlap here,
// every Mtx stack slot is 8 bytes lower (work 0x38 not 0x40), and the dat_ry index is `slwi`
// here but `clrlslwi r0, r0, 29, 2` in the original. All instructions and constants are
// otherwise the ones from build/G8MJ01/asm/effect/n64/eff_opuku_jetw_n64.s.
// NONMATCHING (89.00%): register allocation / loop codegen only; see the comment above.
void effOpukuJetwDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effOpukuJetwData* data;
    CameraEntry* cam;
    effOpukuJetwPoint* table;
    effOpukuJetwPoint* ptr;
    effOpukuJetwPoint* colPtr;
    GXTexObj texObj;
    Mtx trans;
    Mtx scale;
    Mtx rot;
    Mtx work;
    GXColor color;
    s32 counter;
    s32 lifetime;
    s32 field24;
    s32 field18;
    s32 field1c;
    s32 field20;
    s32 columns;
    s32 rows;
    s32 i;
    s32 j;
    s32 col;
    s32 threshold;
    s32 half;
    s32 rowOffset;
    u8* base = (u8*)lbl_803A33D8;

    cam = camGetPtr(camId);
    data = effect->userdata;
    counter = data->counter;
    lifetime = data->lifetime;
    field24 = data->field_0x24;
    field18 = data->field_0x18;
    field1c = data->field_0x1C;
    field20 = data->field_0x20;

    if (data->type == 0) {
        table = (effOpukuJetwPoint*)(base + 0x40);
        columns = 20;
    } else {
        table = (effOpukuJetwPoint*)(base + 0x298);
        columns = 11;
    }
    rows = 3;

    PSMTXTrans(trans, data->position.x, data->position.y, data->position.z);
    PSMTXRotRad(rot, 'y', float_deg2rad_8041f7c0 * -camGetPtr(CAMERA_3D)->viewYaw);
    PSMTXScale(scale, data->scale, data->scale, data->scale);
    PSMTXConcat(trans, rot, trans);
    PSMTXConcat(trans, scale, work);
    PSMTXConcat(cam->view, work, work);

    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    effGetTexObjN64(0x34, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    PSMTXScale(scale, float_0p03125_8041f7c4, float_0p03125_8041f7c4, float_0_8041f7c8);
    GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
    GXSetCullMode(GX_CULL_NONE);

    PSMTXRotRad(rot, 'z', float_deg2rad_8041f7c0 * data->field_0x38);
    PSMTXScale(scale, data->field_0x3C, data->field_0x40, float_1_8041f7cc);
    PSMTXConcat(rot, scale, scale);
    PSMTXConcat(work, scale, work);

    color = unk_804231c8;
    color.r = data->field_0x28;
    color.g = data->field_0x2C;
    color.b = data->field_0x30;
    color.a = field24;
    GXSetTevColor(GX_TEVREG2, color);

    if (columns - 1 <= lifetime) {
        color = unk_804231cc;
        color.r = field18;
        color.g = field1c;
        color.b = field20;
        color.a = field24;
        GXSetTevColor(GX_TEVREG1, color);
        GXLoadPosMtxImm(work, 0);
        GXSetCurrentMtx(0);
        effSetVtxDescN64((effVtxArray*)(base + 0x3e4));
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }

    threshold = counter * 5;
    half = columns / 2;
    for (i = 0; i < half; i++) {
        col = (counter + i * 2) % columns;
        rowOffset = 0;
        for (j = 0; j < rows; j++) {
            ptr = &table[rowOffset] + col;
            rowOffset += columns;
            if (col > threshold) {
                continue;
            }
            if (columns - 1 - col > lifetime) {
                continue;
            }
            colPtr = &table[col];
            PSMTXTrans(trans, (f32)colPtr->field_0x0, float_10_8041f7d0 + (f32)colPtr->field_0x1, float_0_8041f7c8);
            PSMTXScale(scale, (f32)colPtr->field_0x2 * float_0p01_8041f7d4, (f32)colPtr->field_0x4 * float_0p01_8041f7d4, float_1_8041f7cc);
            PSMTXConcat(trans, scale, trans);
            PSMTXRotRad(rot, 'z', float_deg2rad_8041f7c0 * ((f32)ptr->field_0x6 + (f32)dat_ry[i + j]));
            PSMTXConcat(trans, rot, trans);
            PSMTXConcat(work, trans, trans);
            GXLoadPosMtxImm(trans, 0);
            GXSetCurrentMtx(0);
            color = unk_804231d0;
            color.r = field18;
            color.g = field1c;
            color.b = field20;
            color.a = (field24 * colPtr->field_0x8) >> 8;
            GXSetTevColor(GX_TEVREG1, color);
            effSetVtxDescN64((effVtxArray*)(base + 0x3e4));
            GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
            tri2(0, 1, 2, 0, 0, 2, 3, 0);
        }
    }
}
