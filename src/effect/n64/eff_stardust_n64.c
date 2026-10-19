#include "eff/n64/eff_stardust_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "math_ext.h"

#include <stdlib.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// Work data allocated by effStardustN64Entry and stored in EffectEntry->userdata.
// Element 0 is the effect header; the particles follow it, one 0x40-byte record each.
typedef struct EffStardustN64Data {
    s32 type;       // 0x00
    f32 x;          // 0x04
    f32 y;          // 0x08
    f32 z;          // 0x0C
    f32 field_0x10; // 0x10
    f32 field_0x14; // 0x14
    f32 field_0x18; // 0x18
    f32 scale;      // 0x1C
    s32 field_0x20; // 0x20
    s32 field_0x24; // 0x24
    f32 r;          // 0x28
    f32 g;          // 0x2C
    f32 b;          // 0x30
    s32 field_0x34; // 0x34
    s32 field_0x38; // 0x38
    s32 drawCam;    // 0x3C
} EffStardustN64Data;

// .rodata zero vector and the int->float conversion doubles (named so the pool matches).
const Vec vec3_802fb770 = { 0.0f, 0.0f, 0.0f };
// MWCC pools these two conversion doubles itself; keeping them non-const makes it re-use the
// named globals instead of extra anonymous @NNN literals (same idiom as effAkariPoseN64Main).
f64 double_to_int_mask_802fb780 = 4503599627370496.0;
f64 double_to_int_802fb788 = 4503601774854144.0;

// .sdata byte table for the per-particle colour rotation; local to this unit.
static u8 color_rotation_data[36] = {
    0xFE, 0xAC, 0xAC, 0xFE, 0xAC, 0xD5, 0xFE, 0xB4, 0x9A, 0xD5, 0xB4, 0xFE,
    0xB4, 0xB4, 0xFE, 0xB4, 0xDD, 0xFE, 0xB4, 0xFE, 0xFE, 0xB4, 0xFE, 0xD5,
    0xB4, 0xFE, 0xB4, 0xD5, 0xFE, 0xB4, 0xFE, 0xFE, 0xB4, 0xFE, 0xD5, 0xAC,
};

// .sdata byte tables, indexed by the header's particle counter.
static u8 speed_data[8] = { 0x1E, 0x3C, 0x50, 0x5F, 0x64, 0x68, 0x6A, 0x00 };
static u8 scale_data[8] = { 0x3C, 0x5A, 0x64, 0x68, 0x6A, 0x6C, 0x6D, 0x00 };

// .data vertex strips (same layout as the other Stardust effect); local to this unit.
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

// .sdata2 data referenced by effStardustDisp and effStardustN64Entry.
extern const GXColor dat_8041fef0;

// .sdata2 float constants.

// Structure matches: the switch arms (incl. `case 3: case 4:` for the >=3 default), the rand/angle math, the
// `s`/`s2`/`s3`/`c`/`t` temporaries, the i/lifeA/lifeB init order, the default arm's vy-before-second-angle
// ordering, and the int->float pool double (objdiff resolves the local @NNN alias by section offset).
// opt_lifetimes off // needed: with lifetimes on the randA/randB temps coalesce into the wrong caller-saved
// FPRs and the scale/scaleB homes swap (f26/f27); off restores the original colouring.
#pragma opt_lifetimes off // randA/randB temps coalesce into the wrong caller-saved FPRs and scale/scaleB swap (f26/f27) otherwise
EffectEntry* effStardustN64Entry(s32 type, f32 x, f32 y, f32 z, f32 scale) {
    f32 angle;
    f32 t2;
    s32 lifeA;
    s32 lifeB;
    EffStardustN64Data* data;
    s32 rangeA;
    s32 rangeB;
    EffectEntry* effect;
    EffStardustN64Data* ptr;
    s32 count;
    s32 i;
    f32 scaleB;
    f32 randB;
    f32 randA;
    f32 vy;
    f32 s;
    f32 c;
    f32 t;
    f32 s3;
    f32 s2;
    s32 odd5; // explicit so the two `(i & 1) * 5` uses stay a single computation with CSE off

    effect = effEntry();
    switch (type) {
    case 0:
        count = 0x24;
        scaleB = scale;
        break;
    case 1:
        count = 4;
        scaleB = scale;
        break;
    case 2:
        count = 8;
        scaleB = scale;
        break;
    case 3:
        count = 4;
        scaleB = 8.0f;
        break;
    default:
        count = 8;
        scaleB = 8.0f;
        break;
    }
    effect->type = "StardustN64";
    effect->count = count + 1;
    data = __memAlloc(HEAP_EFFECT, (count + 1) * 0x40);
    effect->userdata = data;
    effect->callback = effStardustMain;
    data->x = x;
    data->y = y;
    data->z = z;
    data->field_0x20 = 0;
    data->type = type;
    data->field_0x24 = 0x1e;
    data->r = 1.0f;
    data->g = 1.0f;
    data->b = 1.0f;
    data->drawCam = 4;
    data->scale = 1.0f;

    rangeA = (s32)(1000.f * scale);
    rangeB = (s32)(1000.f * scaleB);

    ptr = data + 1;
    for (i = 1, lifeA = 0x20, lifeB = 0x14; i < count + 1; i++, ptr++) {
        randA = (f32)(rand() % rangeA) / 1000.f;
        randB = (f32)(rand() % rangeB) / 1000.f;
        switch (type) {
        case 0:
            ptr->field_0x18 = 0.0f;
            t = 1800.f / (f32)count;
            randB = 6.2831855f * (t * (f32)i - 90.f)
                / 360.f;
            s = (f32)sin(randB);
            t2 = 360.f / (f32)count;
            randA = 6.2831855f * (t2 * (f32)i)
                / 360.f;
            s3 = (f32)sin(randA);
            odd5 = (i & 1) * 5;
            ptr->field_0x10 = scale
                * (s3
                    * (85.0f - 15.0f * s - (f32)odd5)
                    / 100.f);
            s2 = (f32)sin(randB);
            c = (f32)cos(randA);
            ptr->field_0x14 = scale
                * (c
                    * (85.0f - 15.0f * s2 - (f32)odd5)
                    / 100.f);
            ptr->z = 0.0f;
            ptr->field_0x34 = 0;
            break;
        case 1:
        case 2:
#ifdef MUST_MATCH
            /* MUST_MATCH: the comma makes MWCC materialise the `(f32)i` conversion before the
               `(f32)count` one, which lands the two int->float scratch pairs in the original's
               slots (count 0x2c/0x28, i 0x24/0x20); the natural form below puts them the other
               way round. The result is unchanged (the first `(f32)i` is discarded). */
            angle = (360.f / (f32)count) * ((f32)i, (f32)i)
                + (f32)(rand() % 360);
#else
            angle = 360.f / (f32)count * (f32)i + (f32)(rand() % 360);
#endif
            if (i & 1) {
                vy = -((f32)(rand() % 50 / 1000) + 0.05f);
            } else {
                vy = 0.0f;
            }
            ptr->field_0x18 = vy;
            ptr->field_0x10 = 0.0f;
            ptr->field_0x14 = 0.0f;
            angle = 6.2831855f * angle / 360.f;
            ptr->x = randA * (f32)sin(angle);
            ptr->y = randB * (f32)cos(angle);
            ptr->z = 0.0f;
            ptr->field_0x34 = i % 3;
            ptr->scale = 1.0f;
            ptr->field_0x24 = lifeA;
            ptr->field_0x20 = 0;
            break;
        case 3:
        case 4:
        default:
#ifdef MUST_MATCH
            /* MUST_MATCH: same comma as the case 1/2 arm above (int->float scratch slots). */
            angle = (360.f / (f32)count) * ((f32)i, (f32)i)
                + (f32)(rand() % 360);
#else
            angle = 360.f / (f32)count * (f32)i + (f32)(rand() % 360);
#endif
            vy = -((f32)(rand() % 50 / 1000) + 0.05f);
            angle = 6.2831855f * angle / 360.f;
            ptr->field_0x18 = vy;
            ptr->field_0x10 = 0.0f;
            ptr->field_0x14 = 0.0f;
            ptr->x = randA * (f32)sin(angle);
            ptr->y = randB * (f32)cos(angle);
            ptr->z = 0.0f;
            ptr->field_0x34 = i % 3;
            ptr->scale = 1.0f;
            ptr->field_0x24 = lifeB;
            ptr->field_0x20 = 0;
            break;
        }
        lifeA += 2;
        lifeB += 2;
    }
    return effect;
}
#pragma opt_lifetimes reset

// opt_common_subs on breaks the two fmadds contractions in the particle update; the original still
// has them, so keep common subexpression merging off for this function.
#pragma opt_common_subs off
// opt_propagation off is required in the type-0 random branch: with propagation on, MWCC folds the
// int->float conversion of `rnd` into its uses and the conversion slots/register allocation drift
// (12 instruction diffs).
#pragma opt_propagation off
void effStardustMain(EffectEntry* effect) {
    EffStardustN64Data* data = effect->userdata;
#ifdef MUST_MATCH
    /* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second local before
       dispCalcZ; a single local folds the copy away (same idiom as effDogabonAttackMain). */
    Vec position;
#endif
    Vec disp = vec3_802fb770;
    s32 i;
    s32 type;
    s32 dead = 0;
    s32 counter;
    s32 camera;
    f32 angle;
    f32 radius;

    disp.x = data->x;
    disp.y = data->y;
    disp.z = data->z;
#ifdef MUST_MATCH
    position = disp;
#endif

    camera = data->drawCam;
    data->field_0x24--;
    data->field_0x20++;
    type = data->type;
    counter = data->field_0x20;
    if ((type == 0 || type == 10) && data->field_0x24 < 0) {
        effDelete(effect);
        return;
    }

    data++;
    for (i = 0; i < effect->count - 1; i++, data++) {
        data->field_0x38 = (counter + i) & 7;
        switch (type) {
        case 0:
            data->field_0x18 += -0.02f;
            if (counter - 1 < 7) {
                data->x = 0.01f
                    * (data->field_0x10 * (f32)speed_data[counter - 1]);
                data->y = 0.01f
                    * (data->field_0x14 * (f32)speed_data[counter - 1]) + data->field_0x18;
                data->scale = 0.01f * (f32)scale_data[counter - 1];
            } else {
                s32 rnd;
#ifdef MUST_MATCH
                /* MUST_MATCH: retail materialises the signed int->float conversion of `rnd` into a
                   temp before the multiply; writing the angle as one expression (the #else form)
                   makes MWCC coalesce the conversion into the loaded double's register. */
                f32 p;
#endif
                rnd = effTblRandN64(0x168, i + 0xb1);
#ifdef MUST_MATCH
                p = (f32)rnd;
                radius = 0.5f * (f32)(i & 3) + 1.0f;
                angle = 6.2831855f * p / 360.f;
#else
                angle = 6.2831855f * (f32)rnd / 360.f;
                radius = 0.5f * (f32)(i & 3) + 1.0f;
#endif
                data->type = 10;
                data->field_0x10 = radius * (f32)sin(angle);
                data->field_0x14 = radius * (f32)cos(angle);
                data->field_0x18 = -0.05f;
            }
            break;
        case 10:
            data->field_0x10 *= 0.96f;
            data->field_0x14 *= 0.96f;
            data->field_0x14 += data->field_0x18;
            data->x += data->field_0x10;
            data->y += data->field_0x14;
            data->scale = 0.1f * (0.1f - data->scale) + data->scale;
            break;
        default:
            data->field_0x10 *= 0.9f;
            data->field_0x14 += data->field_0x18;
            {
                f32 vx = data->field_0x10;
                f32 vy = data->field_0x14;
                if (data->field_0x24 < 0) {
                    dead++;
                }
                data->x += vx;
                data->y += vy;
            }
            data->field_0x24--;
            if (data->field_0x24 > 0x14 || data->field_0x24 < 0) {
                data->field_0x38 = -1;
            } else {
                data->field_0x20++;
                counter = data->field_0x20;
                if (counter - 1 < 7) {
                    data->scale = 0.01f * (f32)scale_data[counter - 1];
                } else {
                    data->scale = 0.1f * (0.1f - data->scale) + data->scale;
                }
            }
            break;
        }
    }

    if (type != 0 && type != 10 && dead >= effect->count - 1) {
        effDelete(effect);
        return;
    }

#ifdef MUST_MATCH
    dispEntry(camera, dispCalcZ(&position), 2, effStardustDisp, effect);
#else
    dispEntry(camera, dispCalcZ(&disp), 2, effStardustDisp, effect);
#endif
}

#ifdef MUST_MATCH
/* MUST_MATCH: the original builds the TEV colour through a helper, so GXSetTevColor's argument
   temp is allocated next to the helper's local (0x8) instead of above it (the plain #else form
   swaps the two scratch slots). Same idiom as effFireSparkDisp. */
static inline void stardustN64SetTevColor(s32 r, s32 g, s32 b) {
    GXColor color = dat_8041fef0;
    color.r = r;
    color.g = g;
    color.b = b;
    GXSetTevColor(GX_TEVREG0, color);
}
#endif

void effStardustDisp(CameraId camId, void* param) {
    EffectEntry* entry = param;
    CameraEntry* cam;
    EffStardustN64Data* data;
    s32 i;
    s32 renderType;
    s32 colorIdx;
    effVtxArray* vtx;
    GXTexObj texObj;
    GXColor color;
    Mtx mtx;
    Mtx scale;
    Mtx rot;
    Mtx base;
    s32 colorRByte;
    s32 colorGByte;
    s32 colorBByte;
    f32 colorR;
    f32 colorG;
    f32 colorB;

    cam = camGetPtr(camId);
    data = entry->userdata;
    colorIdx = (data->field_0x20 - 1) * 3;
    colorR = data->r;
    colorG = data->g;
    colorB = data->b;
    PSMTXTrans(mtx, data->x, data->y, data->z);
    PSMTXRotRad(rot, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
    PSMTXScale(scale, data->scale, data->scale, data->scale);
    PSMTXConcat(mtx, rot, base);
    PSMTXConcat(base, scale, base);
    PSMTXConcat(cam->view, base, base);

    data++;
    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    PSMTXScale(scale, 0.0056818184f, 0.045454547f, 0.0f);
    GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
    effGetTexObjN64(0x20, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C0, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetCullMode(GX_CULL_NONE);

    for (i = 0; i < entry->count - 1; i++, data++) {
        renderType = data->field_0x38;
        if (renderType < 0) {
            continue;
        }
        PSMTXTrans(mtx, data->x, data->y, data->z);
        mtx[0][0] = data->scale;
        mtx[1][1] = data->scale;
        mtx[2][2] = data->scale;
        PSMTXConcat(base, mtx, mtx);
        GXLoadPosMtxImm(mtx, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);

        colorIdx += 3;
        colorIdx %= 36;
        colorRByte = (s32)(color_rotation_data[colorIdx++] * colorR);
        colorGByte = (s32)(color_rotation_data[colorIdx++] * colorG);
        colorBByte = (s32)(color_rotation_data[colorIdx++] * colorB);
        if (colorRByte > 0xFF) {
            colorRByte = 0xFF;
        }
        if (colorGByte > 0xFF) {
            colorGByte = 0xFF;
        }
        if (colorBByte > 0xFF) {
            colorBByte = 0xFF;
        }
#ifdef MUST_MATCH
        stardustN64SetTevColor(colorRByte, colorGByte, colorBByte);
#else
        color = dat_8041fef0;
        color.r = colorRByte;
        color.g = colorGByte;
        color.b = colorBByte;
        GXSetTevColor(GX_TEVREG0, color);
#endif

        switch (renderType & 7) {
        case 0:
            vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0xA8);
            break;
        case 1:
            vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0xE0);
            break;
        case 2:
            vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x118);
            break;
        case 3:
            vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x150);
            break;
        case 4:
            vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x188);
            break;
        case 5:
            vtx = (effVtxArray*)size8x8_tex22x22_vtx;
            break;
        case 6:
            vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x38);
            break;
        case 7:
            vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x70);
            break;
        }
        effSetVtxDescN64(vtx);
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}

void effStardustN64SetColor(EffectEntry* entry, f32 r, f32 g, f32 b) {
    EffStardustN64Data* data = (EffStardustN64Data*)entry->userdata;
    data->r = r;
    data->g = g;
    data->b = b;
}

void effStardustN64SetDrawCam(EffectEntry* entry, s32 cam) {
    ((EffStardustN64Data*)entry->userdata)->drawCam = cam;
}

void effStardustN64SetScale(EffectEntry* entry, f32 scale) {
    ((EffStardustN64Data*)entry->userdata)->scale = scale;
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const GXColor dat_8041fef0 = { 0x00, 0x00, 0x00, 0xFF };
