#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"
#include "eff/n64/eff_fire_dust_n64.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

#include <stdlib.h>
#include "math_ext.h"

// .rodata string used as the EffectEntry type name.

// .data particle scale table indexed by the per-particle frame counter (defined below).
static f32 scale_data[20];

// Local to this unit in the original (`.fn main_dl, local`); defined further down.
static void main_dl(EffectEntry* effect, Mtx matrix);

typedef struct effFireDustData {
    s32 type;       // 0x00
    f32 x;          // 0x04
    f32 y;          // 0x08
    f32 z;          // 0x0C
    f32 spreadXZ;   // 0x10, base only: scatter radius around the centre
    f32 spreadY;    // 0x14, base only: height of the scatter band
    f32 velX;       // 0x18, per-frame step, eased toward velTargetX
    f32 velY;       // 0x1C
    f32 velZ;       // 0x20
    f32 velTargetX; // 0x24
    f32 velTargetY; // 0x28
    f32 velTargetZ; // 0x2C
    s32 cycle;      // 0x30, 0..19 (indexes scale_data); negative while waiting to start
    f32 rotZ;       // 0x34, spin in degrees, +4 per frame
    s32 life;       // 0x38
    s32 frame;      // 0x3C, counts up every frame
    s32 tev1R;      // 0x40, base only: GX_TEVREG1 colour (the TEV stage never reads it)
    s32 tev1G;      // 0x44
    s32 tev1B;      // 0x48
    s32 alpha;      // 0x4C, base: fades out over the last 8 frames of life; particles: 0xFF at spawn
    s32 tev0R;      // 0x50, base only: GX_TEVREG0 colour; alpha is the particle's alpha * base alpha / 255
    s32 tev0G;      // 0x54
    s32 tev0B;      // 0x58
    s32 tev1A;      // 0x5C, GX_TEVREG1 alpha
    f32 scale;      // 0x60, base: entry scale; particles: scale_data[cycle] * 2
    f32 field_0x64; // 0x64
    f32 field_0x68; // 0x68
} effFireDustData;

// NOTE: this file's functions are emitted in reverse source order. In the original the
// order is main_dl, effFireDustDisp, effFireDustMain, effFireDustN64Entry; Entry is here,
// the not-yet-written Main belongs below it, and main_dl is last.

EffectEntry* effFireDustN64Entry(s32 type, s32 count, s32 life, f32 x, f32 y, f32 z, f32 radius, f32 height) {
    EffectEntry* effect;
    effFireDustData* data;
    effFireDustData* d;
    s32 i;
    f32 ratio;

    effect = effEntry();
    effect->type = "FireDustN64";
    effect->count = count + 2;
    data = __memAlloc(3, (count + 2) * 0x6c);
    effect->userdata = data;
    effect->callback = effFireDustMain;
    effect->flags |= 2;

    data->type = type;
    data->frame = 0;
    if (life <= 0) {
        data->life = 1000;
    } else {
        data->life = life;
    }
    d = data + 1;
    data->alpha = 0xff;
    data->x = x;
    data->y = y;
    data->z = z;
    data->scale = 1.0f;
    data->spreadXZ = radius;
    data->spreadY = height;
    data->tev1R = 0xff;
    data->tev1G = 0xff;
    data->tev1B = 0xff;
    data->tev0R = 0xff;
    data->tev0G = 0xff;
    data->tev0B = 0x7f;
    data->tev1A = 0xff;

    if (life < 0x14) {
        ratio = 20.0f / (f32)(count + 1);
    } else {
        ratio = (f32)life / (f32)(count + 1);
    }

    for (i = 1; i < count + 2; i++, d++) {
        d->cycle = (s32)(-(f32)i * ratio) - 1;
    }

    return effect;
}

void effFireDustMain(EffectEntry* effect) {
    s32 ival;
    effFireDustData* data;
    s32 i;
    Vec position;
    Vec disp = (Vec){ 0.0f, 0.0f, 0.0f };
    f32 half;
    f32 radius;
    f32 angle;

    data = effect->userdata;
    disp.x = data->x;
    disp.y = data->y;
    disp.z = data->z;
    position = disp;

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->life = 16;
    }
    if (data->life < 1000) {
        data->life--;
    }
    data->frame++;
    if (data->life < 0) {
        effDelete(effect);
        return;
    }
    if (data->life < 8) {
        data->alpha = data->life << 5;
    }

    i = 1;
    radius = data->spreadXZ;
    ival = (s32)data->spreadY;
    half = 0.5f * data->spreadY;
    data++;
    for (; i < effect->count; i++, data++) {
        data->cycle++;
        if (data->cycle >= 20) {
            data->cycle = 0;
        }
        if (data->cycle < 0) {
            continue;
        }
        if (data->cycle == 0) {
            angle = (f32)(rand() % 360);
            /* The original discards a rand() result here; the RNG state feeds later particles. */
            (void)rand();
            angle = 6.2831855f * angle / 360.0f;
            data->x = radius * (f32)sin(angle);
            data->y = (f32)(rand() % ival) - half;
            data->z = radius * (f32)cos(angle);
            data->velX = 15.0f * (f32)sin(angle);
            data->velY = 0.4f * data->y;
            data->velZ = 15.0f * (f32)cos(angle);
            data->velTargetX = 0.5f * data->velX;
            data->velTargetY = 0.5f * data->velY;
            data->velTargetZ = 0.5f * data->velZ;
            data->field_0x64 = (f32)(rand() % 15);
            data->field_0x68 = (f32)(rand() % 15);
            data->rotZ = 0.0f;
            data->alpha = 0xff;
        }
        data->scale = 2.0f * scale_data[data->cycle];
        data->rotZ += 4.0f;
        data->x += data->velX;
        data->y += data->velY;
        data->z += data->velZ;
        data->field_0x64 += 0.2f;
        data->field_0x68 += 0.6f;
        data->velX += 0.04f * (data->velTargetX - data->velX);
        data->velY += 0.04f * (data->velTargetY - data->velY);
        data->velZ += 0.04f * (data->velTargetZ - data->velZ);
    }
    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effFireDustDisp, effect);
}

void effFireDustDisp(CameraId camId, void* param) {
    effFireDustData* data;
    EffectEntry* effect;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx transMtx;
    Mtx scaleMtx;
    Mtx rotMtx;
    Mtx outMtx;
    Mtx dispMtx;

    effect = param;
    cam = camGetPtr(camId);
    data = effect->userdata;

    PSMTXTrans(transMtx, data->x, data->y, data->z);
    PSMTXScale(scaleMtx, data->scale, data->scale, data->scale);
    PSMTXConcat(transMtx, scaleMtx, outMtx);

    GXSetTevColor(GX_TEVREG1, (GXColor){ data->tev1R, data->tev1G, data->tev1B, data->tev1A });

    effGetTexObjN64(0x20, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    PSMTXScale(scaleMtx, 0.0056818184f, 0.045454547f, 0.0f);
    GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C0, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetCullMode(GX_CULL_NONE);

    PSMTXConcat(cam->view, outMtx, dispMtx);
    main_dl(effect, dispMtx);

    PSMTXRotRad(rotMtx, 'y', 2.0943952f);
    PSMTXConcat(outMtx, rotMtx, dispMtx);
    PSMTXConcat(cam->view, dispMtx, dispMtx);
    main_dl(effect, dispMtx);

    PSMTXRotRad(rotMtx, 'y', -2.0943952f);
    PSMTXConcat(outMtx, rotMtx, dispMtx);
    PSMTXConcat(cam->view, dispMtx, dispMtx);
    main_dl(effect, dispMtx);
}

// .data particle scale table indexed by the per-particle frame counter.
static f32 scale_data[20] = {
    0.2f, 0.4f, 0.8f, 1.3f, 1.5f, 1.3f, 0.9f, 0.6f, 0.4f, 0.3f,
    0.2f, 0.4f, 0.8f, 1.3f, 1.5f, 1.3f, 0.9f, 0.6f, 0.4f, 0.3f,
};

// .data vertex strip; the original is a raw 0x1C0-byte object and main_dl
// passes 0x38-byte slices of it to effSetVtxDescN64.
static u32 size8x8_tex22x22_vtx[0x70] = {
    0xFFF8FFF8, 0x00000000, 0x00000000, 0x00FF0008, 0xFFF80000, 0x02C00000,
    0x000000FF, 0x00080008, 0x000002C0, 0x02C00000, 0x00FFFFF8, 0x00080000,
    0x000002C0, 0x000000FF, 0xFFF8FFF8, 0x000002C0, 0x00000000, 0x00FF0008,
    0xFFF80000, 0x05800000, 0x000000FF, 0x00080008, 0x00000580, 0x02C00000,
    0x00FFFFF8, 0x00080000, 0x02C002C0, 0x000000FF, 0xFFF8FFF8, 0x00000580,
    0x00000000, 0x00FF0008, 0xFFF80000, 0x08400000, 0x000000FF, 0x00080008,
    0x00000840, 0x02C00000, 0x00FFFFF8, 0x00080000, 0x058002C0, 0x000000FF,
    0xFFF8FFF8, 0x00000840, 0x00000000, 0x00FF0008, 0xFFF80000, 0x0B000000,
    0x000000FF, 0x00080008, 0x00000B00, 0x02C00000, 0x00FFFFF8, 0x00080000,
    0x084002C0, 0x000000FF, 0xFFF8FFF8, 0x00000B00, 0x00000000, 0x00FF0008,
    0xFFF80000, 0x0DC00000, 0x000000FF, 0x00080008, 0x00000DC0, 0x02C00000,
    0x00FFFFF8, 0x00080000, 0x0B0002C0, 0x000000FF, 0xFFF8FFF8, 0x00000DC0,
    0x00000000, 0x00FF0008, 0xFFF80000, 0x10800000, 0x000000FF, 0x00080008,
    0x00001080, 0x02C00000, 0x00FFFFF8, 0x00080000, 0x0DC002C0, 0x000000FF,
    0xFFF8FFF8, 0x00001080, 0x00000000, 0x00FF0008, 0xFFF80000, 0x13400000,
    0x000000FF, 0x00080008, 0x00001340, 0x02C00000, 0x00FFFFF8, 0x00080000,
    0x108002C0, 0x000000FF, 0xFFF8FFF8, 0x00001340, 0x00000000, 0x00FF0008,
    0xFFF80000, 0x16000000, 0x000000FF, 0x00080008, 0x00001600, 0x02C00000,
    0x00FFFFF8, 0x00080000, 0x134002C0, 0x000000FF,
};

static void main_dl(EffectEntry* effect, Mtx matrix) {
    effFireDustData* data;
    Mtx mtx, scale, rot;
    s32 c0, c1, c2;
    s32 i;
    u32* vtx;
    f32 alphaScale;
    f32 v;

    data = effect->userdata;
    c0 = data->tev0R;
    c1 = data->tev0G;
    c2 = data->tev0B;
    alphaScale = (f32)data->alpha / 255.0f;
    data++;
    for (i = 1; i < effect->count; i++, data++) {
        if (data->cycle < 0) {
            continue;
        }
        PSMTXTrans(mtx, data->x, data->y, data->z);
        PSMTXRotRad(rot, 'z', 0.017453292f * data->rotZ);
        v = data->scale * alphaScale;
        PSMTXScale(scale, v, v, v);
        PSMTXConcat(mtx, rot, mtx);
        PSMTXConcat(mtx, scale, mtx);
        PSMTXConcat(matrix, mtx, mtx);
        GXLoadPosMtxImm(mtx, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        GXSetTevColor(GX_TEVREG0, (GXColor){ c0, c1, c2, (u8)((f32)data->alpha * alphaScale) });
        switch (i & 7) {
            case 0:
                vtx = &size8x8_tex22x22_vtx[3 * 14];
                break;
            case 1:
                vtx = &size8x8_tex22x22_vtx[4 * 14];
                break;
            case 2:
                vtx = &size8x8_tex22x22_vtx[5 * 14];
                break;
            case 3:
                vtx = &size8x8_tex22x22_vtx[6 * 14];
                break;
            case 4:
                vtx = &size8x8_tex22x22_vtx[7 * 14];
                break;
            case 5:
                vtx = size8x8_tex22x22_vtx;
                break;
            case 6:
                vtx = &size8x8_tex22x22_vtx[1 * 14];
                break;
            case 7:
                vtx = &size8x8_tex22x22_vtx[2 * 14];
                break;
        }
        effSetVtxDescN64(vtx);
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}

