#include "eff/n64/eff_magic1_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sdata animation table used by main_dl (originally local to this unit).
static u8 anime_data[12] = {
    0x00, 0xFF, 0xD0, 0x1A, 0x78, 0x4D, 0xD0, 0xFE, 0xF0, 0xFE, 0x4C, 0x6E,
};

extern u32 dat_8041f650;
extern GXColor unk_804231a8;
EffectEntry* effMagic1N64Entry(s32 type, s32 count, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1) {
    EffectEntry* effect;
    effMagic1Data* data;
    f32 scale;

    effect = effEntry();
    effect->type = "Magic1N64\0\0";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(effMagic1Data));
    effect->userdata = data;
    effect->callback = effMagic1Main;
    effect->flags |= 2;
    data->age = 0;
    data->type = type;
    data->alpha = 0xff;
    data->x = x0;
    data->y = y0;
    data->z = z0;
    data->scale = 1.0f;
    switch (type) {
    case 0:
        scale = 1.0f / (f32)count;
        data->offsetX = 0.0f;
        data->offsetY = 0.0f;
        data->offsetZ = 0.0f;
        data->velX = (x1 - x0) * scale;
        data->velY = (y1 - y0) * scale;
        data->velZ = (z1 - z0) * scale;
        data->timer = count;
        break;
    default:
        data->offsetX = 29.0f;
        data->offsetY = 0.0f;
        data->offsetZ = 0.0f;
        data->velX = 6.2f;
        data->velY = 0.0f;
        data->velZ = 0.0f;
        data->timer = 0x18;
        break;
    }
    return effect;
}
void effMagic1Main(EffectEntry* effect) {
    effMagic1Data* data = effect->userdata;
    /* Retail builds the Vec in a scratch slot and copies it into a second
       local before dispCalcZ; a single local folds the copy away (same idiom as
       effAkariSflushMain/effGlassMain). */
    Vec position;
    Vec disp = { 0 };
    f32 order;
    s32 type;

    disp.x = data->offsetX;
    disp.y = data->offsetY;
    disp.z = data->offsetZ;
    position = disp;

    if ((u32)marioStGetSystemLevel() != 0) {
        order = dispCalcZ(&position);
        dispEntry(CAMERA_3D, order, 2, (DispCallback)effMagic1Disp, effect);
    } else {
        if (effect->flags & 4) {
            effect->flags &= ~4;
            data->timer = 0;
        }
        data->timer--;
        data->age++;
        if (data->timer < 0) {
            effDelete(effect);
            return;
        }
        type = data->type;
        if (type == 0 && data->timer == 0) {
            EffectEntry* newEffect = effMagic1N64Entry(1, 0x30, data->x + data->offsetX,
                data->y + data->offsetY, data->z + data->offsetZ, 0.0f, 0.0f, 0.0f);
            ((effMagic1Data*)newEffect->userdata)->scale = data->scale;
        }
        if (type == 1) {
            f32 k83 = 0.83f;
            f32 k9 = 0.9f;
            data->velX *= k83;
            data->alpha = (s32)((f32)data->alpha * k9);
        }
        data->offsetX += data->velX;
        data->offsetY += data->velY;
        data->offsetZ += data->velZ;
        order = dispCalcZ(&position);
        dispEntry(CAMERA_3D, order, 2, (DispCallback)effMagic1Disp, effect);
    }
}

// Per-channel animation sine used by main_dl. The original recomputes the
// phase for every channel, so this stays a macro instead of a shared local.
// Spelling the *32 as a shift and putting the +180 before the anime byte is
// what puts the phase sum's registers where the original has them (the natural form
// leaves retraceCount in r0 and the anime byte in r3, swapped).
#define MAGIC1_PHASE(phase) ((gp->retraceCount << 5) + 180 + (phase))
#define MAGIC1_SIN(phase) \
    ((f32)sin(6.2831855f * (f32)(s32)MAGIC1_PHASE(phase) / 360.0f))
// color2's sine has no +180 offset: the original computes retraceCount*32 + anime[0].
#define MAGIC1_PHASE2(phase) ((gp->retraceCount << 5) + (phase))
#define MAGIC1_SIN2(phase) \
    ((f32)sin(6.2831855f * (f32)(s32)MAGIC1_PHASE2(phase) / 360.0f))

// Building each colour through an inline return puts its local and its
// return temp where the original does (a plain local lands in the other slot); the
// explicit (s32) casts keep the u8 channels on the signed int-to-float sequence
// (without them MWCC uses the unsigned constant and the pool gains a second double).
// Naming the raw sine in `s` is what makes the setup hoist 0.5 into f28 and 1.0 into
// f29 (the target order); loading the channel byte into `a` first keeps the byte live
// across the sin call so it stays in a callee-saved GPR. Without the split the two
// constants and the f3/f4 temps land in the opposite slots.
static inline GXColor magic1Color2(u8* anime) {
    GXColor c = *(GXColor*)&dat_8041f650;
    { s32 a = anime[1]; f32 s = MAGIC1_SIN2(anime[0]);
      c.r = (u8)(s32)(a + 0.5f * ((255 - a) * (1.0f + s))); }
    { s32 a = anime[2]; f32 s = MAGIC1_SIN2(anime[0]);
      c.g = (u8)(s32)(a + 0.5f * ((255 - a) * (1.0f + s))); }
    { s32 a = anime[3]; f32 s = MAGIC1_SIN2(anime[0]);
      c.b = (u8)(s32)(a + 0.5f * ((255 - a) * (1.0f + s))); }
    return c;
}

static inline GXColor magic1Color1(GXColor c, u8* anime, s32 alpha) {
    { s32 a = anime[1]; f32 s = MAGIC1_SIN(anime[0]);
      c.r = (u8)(s32)(a + 0.5f * (-a * (1.0f + s))); }
    { s32 a = anime[2]; f32 s = MAGIC1_SIN(anime[0]);
      c.g = (u8)(s32)(a + 0.5f * (-a * (1.0f + s))); }
    { s32 a = anime[3]; f32 s = MAGIC1_SIN(anime[0]);
      c.b = (u8)(s32)(a + 0.5f * (-a * (1.0f + s))); }
    c.a = alpha;
    return c;
}

// Details are in REPORT.md.
static void main_dl(EffectEntry* effect, Mtx mtx) {
    u8* anime;
    s32 size;
    effMagic1Data* data;
    s32 type;
    s32 alpha;
    s32 i;
    s32 j;
    s32 count;
    GXTexObj texObj;
    Mtx mtxA;
    Mtx mtxC;
    Mtx mtxB;
    Mtx mtxD;
    Mtx mtxE;
    GXColor color2;
    GXColor color;
    f32 rotAngle;
    f32 scale;
    f32 ringAngle;
    f32 radius;
    f32 x;
    f32 y;
    f32 z;
    f32 cosAngle;
    f32 theta;
    f32 angle;
    // Naming the (3-j)*64 product first makes MWCC load 64 into f30 and
    // 1/256 into f31 as the original does (in one expression it swaps the two).
    f32 t;

    data = effect->userdata;
    type = data->type;
    scale = data->scale;
    count = data->timer;
    alpha = data->alpha;
    x = data->offsetX;
    y = data->offsetY;
    z = data->offsetZ;

    switch (type) {
    case 0:
        radius = 9.0f;
        ringAngle = (f32)(count * 35);
        angle = -camGetPtr(CAMERA_3D)->viewYaw;
        break;
    default:
        x = x * scale;
        radius = 6.0f;
        ringAngle = (f32)(count * 25);
        angle = 0.0f;
        break;
    }

    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C0, GX_CC_C1, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    effGetTexObjN64(0x10, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    rotAngle = 0.017453292f * angle;

    for (i = 0; i < 3; i++) {
        if (i > 0) {
            switch (type) {
            case 0:
                ringAngle -= 70.0f;
                x -= 2.0f * data->velX;
                y -= 2.0f * data->velY;
                z -= 2.0f * data->velZ;
                break;
            default:
                ringAngle -= 50.0f;
                x -= 2.0f * (1.2048193f * data->velX);
                break;
            }
            alpha = (s32)(100.0f * (f32)data->alpha / 255.0f);
        }

        GXSetNumTexGens(1);
        GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
        PSMTXTrans(mtxA, x, y, z);
        PSMTXScale(mtxB, scale, scale, scale);
        PSMTXRotRad(mtxC, 'y', rotAngle);
        PSMTXConcat(mtxC, mtxA, mtxA);
        PSMTXConcat(mtxA, mtxB, mtxD);

        for (j = 0, anime = anime_data; j < 3; j++, anime += 4) {
            theta = 6.2831855f * ringAngle / 360.0f;
            cosAngle = (f32)cos(theta);
            PSMTXTrans(mtxE, radius * (f32)sin(theta), radius * cosAngle, 0.0f);
            ringAngle += 120.0f;
            PSMTXConcat(mtxD, mtxE, mtxE);
            PSMTXConcat(mtx, mtxE, mtxE);
            GXLoadPosMtxImm(mtxE, 0);

            GXSetTevColor(GX_TEVREG0, magic1Color1(unk_804231a8, anime, alpha));
            GXSetTevColor(GX_TEVREG1, magic1Color2(anime));

            size = (type == 0) ? 15 : 10;
            PSMTXScale(mtxB, 2.0f, 0.25f, 1.0f);
            {
                f32 k64 = 64.0f;
                f32 k256 = 0.00390625f;
                t = k64 * (f32)(3 - j);
                PSMTXTrans(mtxA, 0.0f, t * k256, 0.0f);
            }
            PSMTXConcat(mtxA, mtxB, mtxA);
            GXLoadTexMtxImm(mtxA, GX_TEXMTX0, 1);

            GXBegin(GX_QUADS, GX_VTXFMT0, 4);
            GXPosition3s16(-(s16)size, size, 0);
            GXTexCoord2f32(0.0f, 0.0f);
            GXPosition3s16(size, size, 0);
            GXTexCoord2f32(1.0f, 0.0f);
            GXPosition3s16(size, -(s16)size, 0);
            GXTexCoord2f32(1.0f, 1.0f);
            GXPosition3s16(-(s16)size, -(s16)size, 0);
            GXTexCoord2f32(0.0f, 1.0f);
        }
    }
}
void effMagic1Disp(CameraId camId, EffectEntry* effect) {
    effMagic1Data* data;
    CameraEntry* camera;
    s32 type;
    s32 i;
    Mtx mtxA;
    Mtx mtxB;
    Mtx mtxC;
    Mtx mtxD;
    f32 angle;

    camera = camGetPtr(camId);
    data = effect->userdata;
    type = data->type;
    switch (type) {
    case 0:
        angle = 0.0f;
        break;
    default:
        angle = -camGetPtr(CAMERA_3D)->viewYaw;
        break;
    }
    PSMTXTrans(mtxA, data->x, data->y, data->z);
    PSMTXRotRad(mtxB, 'y', 0.017453292f * angle);
    PSMTXConcat(mtxA, mtxB, mtxC);
    PSMTXConcat(camera->view, mtxC, mtxC);
    switch (type) {
    case 0:
        main_dl(effect, mtxC);
        break;
    case 1:
        for (i = 0; i < 12; i++) {
            PSMTXRotRad(mtxD, 'z', 0.017453292f * (f32)(i * 30));
            PSMTXConcat(mtxC, mtxD, mtxD);
            main_dl(effect, mtxD);
        }
        break;
    }
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
__declspec(section ".sdata2") u32 dat_8041f650 = 0x000000FF;


// .sbss2 scratch colour (declared in reverse: MWCC lays uninitialised globals out last-declared-first).
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor gap_10_804231AC_sbss2;
__declspec(section ".sdata") GXColor unk_804231a8;
#pragma section ".sdata" ".sbss"
