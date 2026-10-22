#include "eff/n64/eff_lensflare_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"
#include <stdlib.h>

// .sdata2 constants live in this translation unit
// (see config/G8MJ01/symbols.txt).

// .data vertex arrays, local to this file.
static u32 size48x48_tex32x32_vtx[0x38 / 4] = {
    0xFFE8FFE8, 0x00000000, 0x00000000, 0x00FF0018, 0xFFE80000, 0x04000000, 0x000000FF,
    0x00180018, 0x00000400, 0x04000000, 0x00FFFFE8, 0x00180000, 0x00000400, 0x000000FF,
};
static u32 size10x10_tex32x32_vtx[0xE0 / 4] = {
    0x0000FFFB, 0x00000000, 0x00000000, 0x00FF000A, 0xFFFB0000, 0x04000000, 0x000000FF, 0x000A0005,
    0x00000400, 0x04000000, 0x00FF0000, 0x00050000, 0x00000400, 0x000000FF,
    0x0000FFFB, 0x00000000, 0x04000000, 0x00FF000A, 0xFFFB0000, 0x04000400, 0x000000FF, 0x000A0005,
    0x00000400, 0x08000000, 0x00FF0000, 0x00050000, 0x00000800, 0x000000FF,
    0xFFFBFFFB, 0x00000000, 0x08000000, 0x00FF0005, 0xFFFB0000, 0x04000800, 0x000000FF, 0x00050005,
    0x00000400, 0x0C000000, 0x00FFFFFB, 0x00050000, 0x00000C00, 0x000000FF,
    0xFFFBFFFB, 0x00000000, 0x0C000000, 0x00FF0005, 0xFFFB0000, 0x04000C00, 0x000000FF, 0x00050005,
    0x00000400, 0x10000000, 0x00FFFFFB, 0x00050000, 0x00001000, 0x000000FF,
};

EffectEntry* effLensflareN64Entry(s32 type, s32 a1, f32 x, f32 y, f32 z, f32 a2) {
    EffectEntry* effect;
    LensflareData* data;
    LensflareEntry* entry;
    s32 i;

    effect = effEntry();
    effect->type = "LensflareN64";
    effect->count = 6;
    data = __memAlloc(HEAP_EFFECT, sizeof(LensflareData));
    effect->userdata = data;
    effect->callback = effLensflareMain;
    effect->flags |= 2;
    data->field_4 = type;
    data->field_1C = 0;
    if (a1 <= 0) {
        data->field_18 = 100;
    } else {
        data->field_18 = a1;
    }
    data->field_14 = 0;
    data->x = x;
    data->y = y;
    data->z = z;

    entry = data->entries;
    for (i = 1; i < 6; i++) {
        entry->field_20 = (f32)(rand() % 360);
        entry->field_2C = (f32)((i * 2) % 20 + 60);
        entry->field_28 = 0.1f * (f32)(rand() % 16) + 0.4f;
        entry->field_30 = 0.1f * (f32)(rand() % 11) + 0.1f;
        entry++;
    }
    return effect;
}

// The original walks the header fields and the entry array through ONE pointer: it loads userdata into
// `data`, reads the head fields through it, then steps it once (`for (i = 1, data++; ...)`) into
// the entries. Keeping a separate `entry` variable makes MWCC spill userdata to a caller-saved register
// (r6) and materialise the loop pointer with `addi r31, r6, 0x34`, which flips the later register roles.
void effLensflareMain(EffectEntry* effect) {
    LensflareEntry* data = effect->userdata;
    /* The Vec is built in `disp` and copied to `position` before dispCalcZ; a single local folds the copy away. */
    Vec position;
    Vec disp = (Vec){ 0.0f, 0.0f, 0.0f };
    f32 angle;
    f32 c;
    s32 i;
    /* field_18 and field_1C are each read once into a local, which keeps them in r0 and r3;
       reading the fields directly in each test reloads them and swaps the two registers. */
    s32 fade;
    s32 frame;

    disp.x = data->x;
    disp.y = data->y;
    disp.z = data->z;
    position = disp;

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->field_18 = 30;
    }
    if (data->field_18 < 100) {
        data->field_18--;
    }
    data->field_1C++;
    fade = data->field_18;
    if (fade < 0) {
        effDelete(effect);
        return;
    }
    frame = data->field_1C;
    if (frame <= 20) {
        data->field_14 = frame * 12;
    } else if (fade < 20) {
        data->field_14 = fade * 12;
    }

    for (i = 1, data++; i < effect->count; i++, data++) {
        angle = 6.2831855f * (90.f * (data->field_2C - 70.f) / 10.f) / 360.f;
        c = (f32)cos(angle);
        data->field_14 = (s32)(255.0f * c);
        data->field_2C += data->field_30;
        if (data->field_2C > 80.f) {
            data->field_2C = 60.f;
            data->field_28 = 0.1f * (f32)(rand() % 16) + 0.4f;
            /* The literal 0.1f is rematerialised after the rand() call rather than held in a callee-saved register. */
            data->field_30 = 0.1f * (f32)(rand() % 11) + 0.1f;
            data->field_20 = (f32)(rand() % 360);
        }
    }
    dispEntry(4, dispCalcZ(&position), 2, effLensflareDisp, effect);
}

// MWCC assigns the callee-saved GPRs partly by declaration order: `cam` must be
// declared before `alpha` so cam lands in r27 and alpha in r26 (the loop counter
// then reuses cam's r27).  The Mtx/GXTexObj relative order (trans, scale, rot,
// base, texObj) fixes the stack slots, so the GPR declarations are interleaved
// among them rather than grouped.
void effLensflareDisp(CameraId cameraId, void* param) {
	f32 zero = 0.0f;
    CameraEntry* cam;
    EffectEntry* effect = param;
    Mtx trans;
    Mtx scale;
    Mtx rot;
    LensflareEntry* data;
    s32 frame;
    Mtx base;
    GXTexObj texObj;
    f32 f30;
    s32 i;
    s32 alpha;

    cam = camGetPtr(cameraId);
    data = effect->userdata;
    frame = data->field_1C;
    alpha = data->field_14;
    if (frame & 1) {
        alpha = (s32)(0.95f * (f32)alpha);
    }
    PSMTXTrans(trans, data->x, data->y, data->z);
    PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
    PSMTXConcat(trans, rot, base);
    PSMTXConcat(cam->view, base, base);
    GXSetTevColor(1, (GXColor){ 0xFF, 0x40, 0x40, alpha });
    PSMTXRotRad(rot, 'z', 0.017453292f * (f32)(frame + (frame & 1) * 2));
    PSMTXConcat(base, rot, rot);
    GXLoadPosMtxImm(rot, 0);
    GXSetCurrentMtx(0);
    GXSetCullMode(0);
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xff);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 2, 12, 8, 15);
    GXSetTevAlphaIn(0, 7, 1, 4, 7);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
    PSMTXScale(scale, 0.0625f, 0.0625f, 0.0f);
    GXLoadTexMtxImm(scale, 0x1e, 1);
    effGetTexObjN64(0x6f, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetTevColor(1, (GXColor){ 0xFF, 0x40, 0x40, alpha });
    effSetVtxDescN64(size48x48_tex32x32_vtx);
    GXBegin(0x90, 0, 6);
    tri2(0, 1, 2, 0, 0, 2, 3, 0);
    effGetTexObjN64(0x86, &texObj);
    GXLoadTexObj(&texObj, 0);
    PSMTXScale(scale, 0.03125f, 0.0078125f, 0.0f);
    GXLoadTexMtxImm(scale, 0x1e, 1);
    effSetVtxDescN64(size10x10_tex32x32_vtx);

    for (i = 1, data++; i < effect->count; i++, data++) {
        f30 = data->field_28;
        if (f30 == zero) {
            continue;
        }
        GXSetTevColor(1, (GXColor){ 0xFF, 0xFF, 0xFF, (data->field_14 * alpha) >> 8 });
        GXSetNumTevStages(1);
        GXSetTevOrder(0, 0, 0, 0xff);
        GXSetTevColorOp(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
        GXSetTevColorIn(0, 15, 15, 15, 2);
        GXSetTevAlphaIn(0, 7, 1, 4, 7);
        PSMTXRotRad(rot, 'z', 0.017453292f * data->field_20);
        PSMTXTrans(trans, data->field_2C, 0.0f, 0.0f);
        PSMTXConcat(rot, trans, trans);
        PSMTXScale(scale, f30, f30, f30);
        PSMTXConcat(trans, scale, trans);
        PSMTXConcat(base, trans, trans);
        GXLoadPosMtxImm(trans, 0);
        GXSetCurrentMtx(0);
        GXBegin(0x90, 0, 6);
        tri2(12, 13, 14, 0, 12, 14, 15, 0);

        PSMTXRotRad(rot, 'z', 0.017453292f * data->field_20);
        PSMTXTrans(trans, 5.0f + data->field_2C, 0.0f, 0.0f);
        PSMTXConcat(rot, trans, trans);
        PSMTXScale(scale, 2.3f - f30, 2.3f - f30, 2.3f - f30);
        PSMTXConcat(trans, scale, trans);
        PSMTXConcat(base, trans, trans);
        GXLoadPosMtxImm(trans, 0);
        GXSetCurrentMtx(0);
        GXBegin(0x90, 0, 6);
        tri2(8, 9, 10, 0, 8, 10, 11, 0);

        GXSetTevColor(1, (GXColor){ 0xFF, 0x00, 0xFF, (data->field_14 * alpha) >> 9 });
        GXSetTevColor(2, (GXColor){ 0xFF, 0x80, 0xFF, 0x00 });
        GXSetNumTevStages(1);
        GXSetTevOrder(0, 0, 0, 0xff);
        GXSetTevColorOp(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
        GXSetTevColorIn(0, 2, 4, 8, 15);
        GXSetTevAlphaIn(0, 7, 1, 4, 7);
        PSMTXRotRad(rot, 'z', 0.017453292f * (14.0f + data->field_20));
        PSMTXRotRad(trans, 'y', 0.017453292f * (f32)(frame + (i * 10) % 45));
        PSMTXConcat(rot, trans, rot);
        PSMTXTrans(trans, 20.f, 0.0f, 0.0f);
        PSMTXConcat(rot, trans, trans);
        {
            f32 t = 0.05f * (data->field_2C - 60.f);
            PSMTXScale(scale, 1.0f + t, 2.0f - t, 1.0f);
        }
        PSMTXConcat(trans, scale, trans);
        PSMTXConcat(base, trans, trans);
        GXLoadPosMtxImm(trans, 0);
        GXSetCurrentMtx(0);
        GXBegin(0x90, 0, 6);
        tri2(4, 5, 6, 0, 4, 6, 7, 0);

        GXSetTevColor(1, (GXColor){ 0xFF, 0x40, 0x40, (data->field_14 * alpha) >> 8 });
        {
            f32 a = 34.0f + data->field_20;
            PSMTXRotRad(rot, 'z', 0.017453292f * (a + (f32)frame));
        }
        PSMTXRotRad(trans, 'y', 0.017453292f * (f32)(i * 35));
        PSMTXConcat(rot, trans, rot);
        PSMTXTrans(trans, 15.0f, 0.0f, 0.0f);
        PSMTXConcat(rot, trans, trans);
        PSMTXScale(scale, 0.1f * (data->field_2C - 60.f) + 2.0f,
                   1.0f, 1.0f);
        PSMTXConcat(trans, scale, trans);
        PSMTXConcat(base, trans, trans);
        GXLoadPosMtxImm(trans, 0);
        GXSetCurrentMtx(0);
        GXBegin(0x90, 0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}
