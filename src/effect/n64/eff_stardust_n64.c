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
extern const f32 float_deg2rad_8041fef4;
extern const f32 float_0p0056818_8041fef8;
extern const f32 float_0p045455_8041fefc;
extern const f32 float_0_8041ff00;

// .sdata2 float constants.
extern const f32 float_neg0p02_8041ff04;
extern const f32 float_0p01_8041ff08;
extern const f32 float_1_8041ff0c;
extern const f32 float_0p5_8041ff10;
extern const f32 float_6p2832_8041ff14;
extern const f32 float_360_8041ff18;
extern const f32 float_neg0p05_8041ff1c;
extern const f32 float_0p96_8041ff20;
extern const f32 float_0p1_8041ff24;
extern const f32 float_0p9_8041ff28;
extern const f32 float_8_8041ff2c;
extern const f32 float_1000_8041ff30;
extern const f32 float_1800_8041ff34;
extern const f32 float_90_8041ff38;
extern const f32 float_85_8041ff3c;
extern const f32 float_15_8041ff40;
extern const f32 float_100_8041ff44;
extern const f32 float_0p05_8041ff48;

// Structure (switch arms incl. `case 3: case 4:` for the >=3 default, rand/angle math, the `s`/`s2`/`s3`/`c`/`t`
// temporaries, and the i/lifeA/lifeB init order) now matches, as does the whole type-0 arm. What still differs is
// compiler scratch-register/stack-slot choice: type-0 angle2's `360` and `6.2832` land in f3/f4 swapped; the
// type-1/2 and default int->float conversion slots are mirrored (retail 0x2c/0x28 vs here 0x24/0x20); the type-1/2
// angle divide is hoisted above the `vy` branch; and the int->float pool double is a local @NNN alias instead of
// named double_to_int_802fb788 (objdiff resolves that by section offset, so it does not score).
// NONMATCHING (95.78%): see above.
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
        scaleB = float_8_8041ff2c;
        break;
    default:
        count = 8;
        scaleB = float_8_8041ff2c;
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
    data->r = float_1_8041ff0c;
    data->g = float_1_8041ff0c;
    data->b = float_1_8041ff0c;
    data->drawCam = 4;
    data->scale = float_1_8041ff0c;

    rangeA = (s32)(float_1000_8041ff30 * scale);
    rangeB = (s32)(float_1000_8041ff30 * scaleB);

    ptr = data + 1;
    for (i = 1, lifeA = 0x20, lifeB = 0x14; i < count + 1; i++, ptr++) {
        randA = (f32)(rand() % rangeA) / float_1000_8041ff30;
        randB = (f32)(rand() % rangeB) / float_1000_8041ff30;
        switch (type) {
        case 0:
            ptr->field_0x18 = 0.0f;
            t = float_1800_8041ff34 / (f32)count;
            randB = float_6p2832_8041ff14 * (t * (f32)i - float_90_8041ff38)
                / float_360_8041ff18;
            s = (f32)sin(randB);
            t2 = float_360_8041ff18 / (f32)count;
            randA = float_6p2832_8041ff14 * (t2 * (f32)i)
                / float_360_8041ff18;
            s3 = (f32)sin(randA);
            ptr->field_0x10 = scale
                * (s3
                    * (float_85_8041ff3c - float_15_8041ff40 * s - (f32)((i & 1) * 5))
                    / float_100_8041ff44);
            s2 = (f32)sin(randB);
            c = (f32)cos(randA);
            ptr->field_0x14 = scale
                * (c
                    * (float_85_8041ff3c - float_15_8041ff40 * s2 - (f32)((i & 1) * 5))
                    / float_100_8041ff44);
            ptr->z = 0.0f;
            ptr->field_0x34 = 0;
            break;
        case 1:
        case 2:
            angle = float_360_8041ff18 / (f32)count * (f32)i + (f32)(rand() % 360);
            if (i & 1) {
                vy = -((f32)(rand() % 50 / 1000) + float_0p05_8041ff48);
            } else {
                vy = 0.0f;
            }
            ptr->field_0x18 = vy;
            ptr->field_0x10 = 0.0f;
            ptr->field_0x14 = 0.0f;
            angle = float_6p2832_8041ff14 * angle / float_360_8041ff18;
            ptr->x = randA * (f32)sin(angle);
            ptr->y = randB * (f32)cos(angle);
            ptr->z = 0.0f;
            ptr->field_0x34 = i % 3;
            ptr->scale = float_1_8041ff0c;
            ptr->field_0x24 = lifeA;
            ptr->field_0x20 = 0;
            break;
        case 3:
        case 4:
        default:
            angle = float_360_8041ff18 / (f32)count * (f32)i + (f32)(rand() % 360);
            angle = float_6p2832_8041ff14 * angle / float_360_8041ff18;
            vy = -((f32)(rand() % 50 / 1000) + float_0p05_8041ff48);
            ptr->field_0x18 = vy;
            ptr->field_0x10 = 0.0f;
            ptr->field_0x14 = 0.0f;
            ptr->x = randA * (f32)sin(angle);
            ptr->y = randB * (f32)cos(angle);
            ptr->z = 0.0f;
            ptr->field_0x34 = i % 3;
            ptr->scale = float_1_8041ff0c;
            ptr->field_0x24 = lifeB;
            ptr->field_0x20 = 0;
            break;
        }
        lifeA += 2;
        lifeB += 2;
    }
    return effect;
}

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
            data->field_0x18 += float_neg0p02_8041ff04;
            if (counter - 1 < 7) {
                data->x = float_0p01_8041ff08
                    * (data->field_0x10 * (f32)speed_data[counter - 1]);
                data->y = float_0p01_8041ff08
                    * (data->field_0x14 * (f32)speed_data[counter - 1]) + data->field_0x18;
                data->scale = float_0p01_8041ff08 * (f32)scale_data[counter - 1];
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
                radius = float_0p5_8041ff10 * (f32)(i & 3) + float_1_8041ff0c;
                angle = float_6p2832_8041ff14 * p / float_360_8041ff18;
#else
                angle = float_6p2832_8041ff14 * (f32)rnd / float_360_8041ff18;
                radius = float_0p5_8041ff10 * (f32)(i & 3) + float_1_8041ff0c;
#endif
                data->type = 10;
                data->field_0x10 = radius * (f32)sin(angle);
                data->field_0x14 = radius * (f32)cos(angle);
                data->field_0x18 = float_neg0p05_8041ff1c;
            }
            break;
        case 10:
            data->field_0x10 *= float_0p96_8041ff20;
            data->field_0x14 *= float_0p96_8041ff20;
            data->field_0x14 += data->field_0x18;
            data->x += data->field_0x10;
            data->y += data->field_0x14;
            data->scale = float_0p1_8041ff24 * (float_0p1_8041ff24 - data->scale) + data->scale;
            break;
        default:
            data->field_0x10 *= float_0p9_8041ff28;
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
                    data->scale = float_0p01_8041ff08 * (f32)scale_data[counter - 1];
                } else {
                    data->scale = float_0p1_8041ff24 * (float_0p1_8041ff24 - data->scale) + data->scale;
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
static inline GXColor stardustN64Color(u8 r, u8 g, u8 b) {
    GXColor c = dat_8041fef0;
    c.r = r;
    c.g = g;
    c.b = b;
    return c;
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
    PSMTXRotRad(rot, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_8041fef4);
    PSMTXScale(scale, data->scale, data->scale, data->scale);
    PSMTXConcat(mtx, rot, base);
    PSMTXConcat(base, scale, base);
    PSMTXConcat(cam->view, base, base);

    data++;
    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    PSMTXScale(scale, float_0p0056818_8041fef8, float_0p045455_8041fefc, float_0_8041ff00);
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
        color = stardustN64Color(colorRByte, colorGByte, colorBByte);
#else
        color = dat_8041fef0;
        color.r = colorRByte;
        color.g = colorGByte;
        color.b = colorBByte;
#endif
        GXSetTevColor(GX_TEVREG0, color);

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