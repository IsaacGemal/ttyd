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

// Local copy of the effect's per-instance data; the fields used by
// effMagic1Disp, effMagic1Main and effMagic1N64Entry are named.
extern const char str_802FAB08[];
extern const f32 float_0_8041f654;
extern const f32 float_deg2rad_8041f658;
extern const f32 float_9_8041f65c;
extern const f32 float_6_8041f660;
extern const f32 float_70_8041f664;
extern const f32 float_2_8041f668;
extern const f32 float_50_8041f66c;
extern const f32 float_1p2048_8041f670;
extern const f32 float_100_8041f674;
extern const f32 float_255_8041f678;
extern const f32 float_6p2832_8041f67c;
extern const f32 float_360_8041f680;
extern const f32 float_120_8041f684;
extern const f32 float_0p5_8041f688;
extern const f32 float_1_8041f68c;
extern const f32 float_0p25_8041f690;
extern const f32 float_64_8041f694;
extern const f32 float_0p0039062_8041f698;
extern const f32 float_29_8041f6a4;
extern const f32 float_6p2_8041f6a8;
extern u32 dat_8041f650;
extern GXColor unk_804231a8;
const f64 double_to_int_802fab00 = 4503601774854144.0;
EffectEntry* effMagic1N64Entry(s32 type, s32 count, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1) {
    EffectEntry* effect;
    effMagic1Data* data;
    f32 scale;

    effect = effEntry();
    effect->type = str_802FAB08;
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, 0x38);
    effect->userdata = data;
    effect->callback = effMagic1Main;
    effect->flags |= 2;
    data->field_0x2c = 0;
    data->type = type;
    data->field_0x34 = 0xff;
    data->x = x0;
    data->y = y0;
    data->z = z0;
    data->field_0x28 = float_1_8041f68c;
    switch (type) {
    case 0:
        scale = float_1_8041f68c / (f32)count;
        data->field_0x10 = float_0_8041f654;
        data->field_0x14 = float_0_8041f654;
        data->field_0x18 = float_0_8041f654;
        data->field_0x1c = (x1 - x0) * scale;
        data->field_0x20 = (y1 - y0) * scale;
        data->field_0x24 = (z1 - z0) * scale;
        data->field_0x30 = count;
        break;
    default:
        data->field_0x10 = float_29_8041f6a4;
        data->field_0x14 = float_0_8041f654;
        data->field_0x18 = float_0_8041f654;
        data->field_0x1c = float_6p2_8041f6a8;
        data->field_0x20 = float_0_8041f654;
        data->field_0x24 = float_0_8041f654;
        data->field_0x30 = 0x18;
        break;
    }
    return effect;
}
void effMagic1Main(EffectEntry* effect) {
    effMagic1Data* data = effect->userdata;
#ifdef MUST_MATCH
    /* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
       local before dispCalcZ; a single local folds the copy away (same idiom as
       effAkariSflushMain/effGlassMain). */
    Vec position;
#endif
    Vec disp = { 0 };
    f32 order;
    s32 type;

    disp.x = data->field_0x10;
    disp.y = data->field_0x14;
    disp.z = data->field_0x18;
#ifdef MUST_MATCH
    position = disp;
#endif

    if ((u32)marioStGetSystemLevel() != 0) {
#ifdef MUST_MATCH
        order = dispCalcZ(&position);
#else
        order = dispCalcZ(&disp);
#endif
        dispEntry(CAMERA_3D, order, 2, (DispCallback)effMagic1Disp, effect);
    } else {
        if (effect->flags & 4) {
            effect->flags &= ~4;
            data->field_0x30 = 0;
        }
        data->field_0x30--;
        data->field_0x2c++;
        if (data->field_0x30 < 0) {
            effDelete(effect);
            return;
        }
        type = data->type;
        if (type == 0 && data->field_0x30 == 0) {
            EffectEntry* newEffect = effMagic1N64Entry(1, 0x30, data->x + data->field_0x10,
                data->y + data->field_0x14, data->z + data->field_0x18, 0.0f, 0.0f, 0.0f);
            ((effMagic1Data*)newEffect->userdata)->field_0x28 = data->field_0x28;
        }
        if (type == 1) {
            data->field_0x1c *= float_0p83_8041f69c;
            data->field_0x34 = (s32)((f32)data->field_0x34 * float_0p9_8041f6a0);
        }
        data->field_0x10 += data->field_0x1c;
        data->field_0x14 += data->field_0x20;
        data->field_0x18 += data->field_0x24;
#ifdef MUST_MATCH
        order = dispCalcZ(&position);
#else
        order = dispCalcZ(&disp);
#endif
        dispEntry(CAMERA_3D, order, 2, (DispCallback)effMagic1Disp, effect);
    }
}

// Per-channel animation sine used by main_dl. The original recomputes the
// phase for every channel, so this stays a macro instead of a shared local.
// MUST_MATCH: spelling the *32 as a shift and putting the +180 before the anime byte is
// what puts the phase sum's registers where the original has them (the natural form
// leaves retraceCount in r0 and the anime byte in r3, swapped).
#ifdef MUST_MATCH
#define MAGIC1_PHASE(phase) ((gp->retraceCount << 5) + 180 + (phase))
#else
#define MAGIC1_PHASE(phase) (gp->retraceCount * 32 + (phase) + 180)
#endif
#define MAGIC1_SIN(phase) \
    ((f32)sin(float_6p2832_8041f67c * (f32)(s32)MAGIC1_PHASE(phase) / float_360_8041f680))
// color2's sine has no +180 offset: the original computes retraceCount*32 + anime[0].
#ifdef MUST_MATCH
#define MAGIC1_PHASE2(phase) ((gp->retraceCount << 5) + (phase))
#else
#define MAGIC1_PHASE2(phase) (gp->retraceCount * 32 + (phase))
#endif
#define MAGIC1_SIN2(phase) \
    ((f32)sin(float_6p2832_8041f67c * (f32)(s32)MAGIC1_PHASE2(phase) / float_360_8041f680))

// Match-only: building each colour through an inline return puts its local and its
// return temp where the original does (a plain local lands in the other slot); the
// explicit (s32) casts keep the u8 channels on the signed int-to-float sequence
// (without them MWCC uses the unsigned constant and the pool gains a second double).
#ifdef MUST_MATCH
static inline GXColor magic1Color2(u8* anime) {
    GXColor c = *(GXColor*)&dat_8041f650;
    c.r = (u8)(s32)((s32)anime[1] + float_0p5_8041f688 * ((255 - (s32)anime[1]) * (float_1_8041f68c + MAGIC1_SIN2(anime[0]))));
    c.g = (u8)(s32)((s32)anime[2] + float_0p5_8041f688 * ((255 - (s32)anime[2]) * (float_1_8041f68c + MAGIC1_SIN2(anime[0]))));
    c.b = (u8)(s32)((s32)anime[3] + float_0p5_8041f688 * ((255 - (s32)anime[3]) * (float_1_8041f68c + MAGIC1_SIN2(anime[0]))));
    return c;
}

static inline GXColor magic1Color1(GXColor c, u8* anime, s32 alpha) {
    c.r = (u8)(s32)((s32)anime[1] + float_0p5_8041f688 * (-(s32)anime[1] * (float_1_8041f68c + MAGIC1_SIN(anime[0]))));
    c.g = (u8)(s32)((s32)anime[2] + float_0p5_8041f688 * (-(s32)anime[2] * (float_1_8041f68c + MAGIC1_SIN(anime[0]))));
    c.b = (u8)(s32)((s32)anime[3] + float_0p5_8041f688 * (-(s32)anime[3] * (float_1_8041f68c + MAGIC1_SIN(anime[0]))));
    c.a = alpha;
    return c;
}
#else
static inline GXColor magic1Color2(u8* anime) {
    GXColor c = *(GXColor*)&dat_8041f650;
    c.r = (u8)(s32)(anime[1] + float_0p5_8041f688 * (255 - anime[1]) * (float_1_8041f68c + MAGIC1_SIN2(anime[0])));
    c.g = (u8)(s32)(anime[2] + float_0p5_8041f688 * (255 - anime[2]) * (float_1_8041f68c + MAGIC1_SIN2(anime[0])));
    c.b = (u8)(s32)(anime[3] + float_0p5_8041f688 * (255 - anime[3]) * (float_1_8041f68c + MAGIC1_SIN2(anime[0])));
    return c;
}

static inline GXColor magic1Color1(GXColor c, u8* anime, s32 alpha) {
    c.r = (u8)(s32)(anime[1] - float_0p5_8041f688 * anime[1] * (float_1_8041f68c + MAGIC1_SIN(anime[0])));
    c.g = (u8)(s32)(anime[2] - float_0p5_8041f688 * anime[2] * (float_1_8041f68c + MAGIC1_SIN(anime[0])));
    c.b = (u8)(s32)(anime[3] - float_0p5_8041f688 * anime[3] * (float_1_8041f68c + MAGIC1_SIN(anime[0])));
    c.a = alpha;
    return c;
}
#endif

// Details are in REPORT.md. main_dl matches everywhere except one MWCC register
// allocation tie-break that the source shape does not move: the setup loads 1.0 into
// f28 and 0.5 into f29, while the target loads 0.5 into f28 and 1.0 into f29 (the
// colour code then agrees, only these two registers swap).
// NONMATCHING (99.69%): f28/f29 swapped.
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
    f32 angle;
#ifdef MUST_MATCH
    // MUST_MATCH: naming the (3-j)*64 product first makes MWCC load 64 into f30 and
    // 1/256 into f31 as the original does (in one expression it swaps the two).
    f32 t;
#endif

    data = effect->userdata;
    type = data->type;
    scale = data->field_0x28;
    count = data->field_0x30;
    alpha = data->field_0x34;
    x = data->field_0x10;
    y = data->field_0x14;
    z = data->field_0x18;

    switch (type) {
    case 0:
        radius = float_9_8041f65c;
        ringAngle = (f32)(count * 35);
        angle = -camGetPtr(CAMERA_3D)->viewYaw;
        break;
    default:
        x = x * scale;
        radius = float_6_8041f660;
        ringAngle = (f32)(count * 25);
        angle = float_0_8041f654;
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

    rotAngle = float_deg2rad_8041f658 * angle;

    for (i = 0; i < 3; i++) {
        if (i > 0) {
            switch (type) {
            case 0:
                ringAngle -= float_70_8041f664;
                x -= float_2_8041f668 * data->field_0x1c;
                y -= float_2_8041f668 * data->field_0x20;
                z -= float_2_8041f668 * data->field_0x24;
                break;
            default:
                ringAngle -= float_50_8041f66c;
                x -= float_2_8041f668 * (float_1p2048_8041f670 * data->field_0x1c);
                break;
            }
            alpha = (s32)(float_100_8041f674 * (f32)data->field_0x34 / float_255_8041f678);
        }

        GXSetNumTexGens(1);
        GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
        PSMTXTrans(mtxA, x, y, z);
        PSMTXScale(mtxB, scale, scale, scale);
        PSMTXRotRad(mtxC, 'y', rotAngle);
        PSMTXConcat(mtxC, mtxA, mtxA);
        PSMTXConcat(mtxA, mtxB, mtxD);

        for (j = 0, anime = anime_data; j < 3; j++, anime += 4) {
#ifdef MUST_MATCH
            /* MUST_MATCH: naming the loop angle `cosAngle` and the cosine `angle` is what
               puts them in the target's f15/f16; the natural assignment swaps the two. */
            cosAngle = float_6p2832_8041f67c * ringAngle / float_360_8041f680;
            angle = (f32)cos(cosAngle);
            PSMTXTrans(mtxE, radius * (f32)sin(cosAngle), radius * angle, 0.0f);
#else
            angle = float_6p2832_8041f67c * ringAngle / float_360_8041f680;
            cosAngle = (f32)cos(angle);
            PSMTXTrans(mtxE, radius * (f32)sin(angle), radius * cosAngle, 0.0f);
#endif
            ringAngle += float_120_8041f684;
            PSMTXConcat(mtxD, mtxE, mtxE);
            PSMTXConcat(mtx, mtxE, mtxE);
            GXLoadPosMtxImm(mtxE, 0);

            GXSetTevColor(GX_TEVREG0, magic1Color1(unk_804231a8, anime, alpha));
            GXSetTevColor(GX_TEVREG1, magic1Color2(anime));

            size = (type == 0) ? 15 : 10;
            PSMTXScale(mtxB, float_2_8041f668, float_0p25_8041f690, float_1_8041f68c);
#ifdef MUST_MATCH
            t = float_64_8041f694 * (f32)(3 - j);
            PSMTXTrans(mtxA, 0.0f,
                       t * float_0p0039062_8041f698, 0.0f);
#else
            PSMTXTrans(mtxA, 0.0f,
                       (f32)(3 - j) * float_64_8041f694 * float_0p0039062_8041f698, 0.0f);
#endif
            PSMTXConcat(mtxA, mtxB, mtxA);
            GXLoadTexMtxImm(mtxA, 0x1E, 1);

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
const f32 float_0_8041f654 = 0.0f;
const f32 float_deg2rad_8041f658 = 0.017453292f;
const f32 float_9_8041f65c = 9.0f;
const f32 float_6_8041f660 = 6.0f;
const f32 float_70_8041f664 = 70.0f;
const f32 float_2_8041f668 = 2.0f;
const f32 float_50_8041f66c = 50.0f;
const f32 float_1p2048_8041f670 = 1.2048193f;
const f32 float_100_8041f674 = 100.0f;
const f32 float_255_8041f678 = 255.0f;
const f32 float_6p2832_8041f67c = 6.2831855f;
const f32 float_360_8041f680 = 360.0f;
const f32 float_120_8041f684 = 120.0f;
const f32 float_0p5_8041f688 = 0.5f;
const f32 float_1_8041f68c = 1.0f;
const f32 float_0p25_8041f690 = 0.25f;
const f32 float_64_8041f694 = 64.0f;
const f32 float_0p0039062_8041f698 = 0.00390625f;
const f32 float_0p83_8041f69c = 0.83f;
const f32 float_0p9_8041f6a0 = 0.9f;
const f32 float_29_8041f6a4 = 29.0f;
const f32 float_6p2_8041f6a8 = 6.2f;
