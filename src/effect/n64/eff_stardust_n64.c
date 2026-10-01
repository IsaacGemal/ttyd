#include "eff/n64/eff_stardust_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "math_ext.h"

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

// .sdata byte tables, indexed by the header's particle counter.
static u8 speed_data[8] = { 0x1E, 0x3C, 0x50, 0x5F, 0x64, 0x68, 0x6A, 0x00 };
static u8 scale_data[8] = { 0x3C, 0x5A, 0x64, 0x68, 0x6A, 0x6C, 0x6D, 0x00 };

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

#pragma opt_propagation reset

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