#include "eff/n64/eff_release_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"

extern const u32 dat_8041fb38;
extern const u32 unk_80423240;
extern const u32 unk_80423244;
extern const f32 float_deg2rad_8041fb3c;
extern const f32 float_0_8041fb40;
extern const f32 float_0p03125_8041fb44;
extern const f32 float_0p0039062_8041fb48;
extern const f32 float_32_8041fb4c;
extern const f32 float_64_8041fb50;
extern const f32 float_96_8041fb54;
extern const f32 float_0p015625_8041fb58;
extern const f32 float_480_8041fb5c;
extern const f32 float_608_8041fb60;
extern const f32 float_1_8041fb64;
extern const f32 float_6p2832_8041fb68;
extern const f32 float_360_8041fb6c;
extern const f32 float_0p5_8041fb70;
extern const f32 float_0p04_8041fb74;
extern const f32 float_1p2356_8041fb78;
extern const f32 float_4_8041fb7c;
extern const f32 float_8_8041fb80;
extern const f32 float_0p3_8041fb84;
extern const f32 float_0p01_8041fb88;
extern const f32 float_0p6_8041fb8c;
extern const f32 float_1p2_8041fb90;

// .rodata
// The zero Vec (vec3_802fb4f0), the two int->float magic doubles (double_to_int_802fb500,
// double_to_int_mask_802fb508) and the "ReleaseN64" string are compiler-generated here and land at
// the original's offsets; defining them by name as well only emitted duplicates after them.

// .data
// lbl_803A4C60 belongs to the previous unit (eff_recover_heart_n64); effReleaseDisp reaches
// its own .data through it as an offset base. Declared in the header.
__declspec(section ".data") static f32 negone_one_374[2] = {-1.0f, 1.0f};
__declspec(section ".data") f32 lbl_803A4C70[2] = {1.0f, -1.0f};
__declspec(section ".data") static f32 one_negone_373[2] = {1.0f, -1.0f};
__declspec(section ".data") f32 lbl_803A4C80[2] = {-1.0f, 1.0f};
__declspec(section ".data") static f32 negone_one_360[2] = {-1.0f, 1.0f};
__declspec(section ".data") f32 lbl_803A4C90[2] = {1.0f, -1.0f};
__declspec(section ".data") static f32 one_negone_359[2] = {1.0f, -1.0f};

static AnimeData anime_data[30] = {
	{0x0064, 0x00FF}, {0x00C8, 0x00FF}, {0x012C, 0x00FF}, {0x0190, 0x00FF}, {0x01F4, 0x00F5}, {0x0258, 0x00EB},
	{0x02BC, 0x00E1}, {0x0320, 0x00D7}, {0x0384, 0x00CD}, {0x03E8, 0x00C3}, {0x044C, 0x00B9}, {0x04B0, 0x00AF},
	{0x0514, 0x00A5}, {0x0578, 0x009B}, {0x05DC, 0x0091}, {0x0640, 0x0087}, {0x06A4, 0x007D}, {0x0708, 0x0073},
	{0x076C, 0x0069}, {0x07D0, 0x005F}, {0x0834, 0x0055}, {0x0898, 0x004B}, {0x08FC, 0x0041}, {0x0960, 0x0037},
	{0x09C4, 0x002D}, {0x0A28, 0x001E}, {0x0A8C, 0x0014}, {0x0AF0, 0x000A}, {0x0B54, 0x0005}, {0x0BB8, 0x0000},
};
AnimeData anime_data_2[30] = {
	{0x0BB8, 0x0000}, {0x0B54, 0x0005}, {0x0AF0, 0x000A}, {0x0A8C, 0x0014}, {0x0A28, 0x001E}, {0x09C4, 0x002D},
	{0x0960, 0x0037}, {0x08FC, 0x0041}, {0x0898, 0x004B}, {0x0834, 0x0055}, {0x07D0, 0x005F}, {0x076C, 0x0069},
	{0x0708, 0x0073}, {0x06A4, 0x007D}, {0x0640, 0x0087}, {0x05DC, 0x0091}, {0x0578, 0x009B}, {0x0514, 0x00A5},
	{0x04B0, 0x00AF}, {0x044C, 0x00B9}, {0x03E8, 0x00C3}, {0x0384, 0x00CD}, {0x0320, 0x00D7}, {0x02BC, 0x00E1},
	{0x0258, 0x00EB}, {0x01F4, 0x00F5}, {0x0190, 0x00FF}, {0x012C, 0x00FF}, {0x00C8, 0x00FF}, {0x0064, 0x00FF},
};

static u8 size16x16_tex64x64_vtx[] = {
	0xFF, 0xF8, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08,
	0xFF, 0xF8, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x08,
	0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0xFF,
};
u8 size16x16_tex256x32_vtx[] = {
	0xFF, 0xF8, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08,
	0xFF, 0xF8, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0xFF, 0xFA,
	0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0xFF, 0xFA, 0x00, 0x00,
	0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0xFF, 0xFA, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0xFF, 0xFA, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0xFF, 0xFC, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xF8, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0xFF, 0xF8, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08,
	0xFF, 0xFC, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0xFF, 0xFE,
	0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0xFF, 0xFE, 0x00, 0x00,
	0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0xFF, 0xFE, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x02,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x02, 0x00, 0x00,
	0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x02, 0x00, 0x00, 0x20, 0x00,
	0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x04, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0xFF, 0xF8, 0x00, 0x04, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08,
	0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x06,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x06, 0x00, 0x00,
	0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x06, 0x00, 0x00, 0x20, 0x00,
	0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x08, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
};
u8 size16x16_tex128x128_vtx[] = {
	0xFF, 0xF8, 0x00, 0x00, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08,
	0x00, 0x00, 0xFF, 0xF8, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x00,
	0x00, 0x08, 0x10, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x08,
	0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0xFF,
};
u8 size16x16_tex64x64_0_vtx[] = {
	0xFF, 0xF8, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08,
	0xFF, 0xF8, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x08, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
	0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
};
u8 rectangle_vtx[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x80, 0x01, 0xE0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
};

EffectEntry* effReleaseN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
    EffectEntry* effect;
    effReleaseN64Data* data;
    s32 typeLo;

    effect = effEntry();
    effect->type = "ReleaseN64";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, 0x78);
    effect->userdata = data;
    effect->callback = effReleaseMain;
    data->field_0x0 = type & 0xFF00;
    typeLo = type & 0xFF;
    data->field_0x4 = typeLo;
    data->field_0x18 = 0;
    if (duration <= 0) {
        data->field_0x14 = 1000;
    } else {
        data->field_0x14 = duration;
    }
    data->field_0x28 = 0xFF;
    data->x = x;
    data->y = y;
    data->z = z;
    data->field_0x3C = scale;
    data->field_0x1C = 0xFF;
    data->field_0x20 = 0xFF;
    data->field_0x24 = 0xFF;
    data->field_0x2C = 0xFF;
    data->field_0x30 = 0xFF;
    data->field_0x34 = 0xFF;
    data->field_0x38 = 0xFF;
    data->field_0x60 = 0;
    data->field_0x64 = 0;
    data->field_0x68 = 0;
    data->field_0x6C = 0;
    data->scales[0] = float_1_8041fb64;
    data->scales[1] = float_0_8041fb40;
    data->scales[2] = float_0_8041fb40;
    data->scales[3] = float_0_8041fb40;
    data->alphas[0] = 0xFF;
    data->alphas[1] = 0;
    data->alphas[2] = 0;
    data->alphas[3] = 0;
    if (typeLo == 3) {
        data->field_0x70 = 1;
    } else {
        data->field_0x70 = 0;
    }
    data->field_0x74 = 0;
    return effect;
}

// Match-only helper: builds the initial Vec from an anonymous zero literal so the
// compiler schedules the loads the way the original does (see the prologue).
static inline Vec releaseMakeVec(f32 x, f32 y, f32 z) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = x;
    temp.y = y;
    temp.z = z;
    return temp;
}

#pragma opt_common_subs off // without it MWCC CSEs the 0.01*t/0.3*idx product across the conversion and picks different FPRs (94.0%)
// Only the case-100 `field_0x74 < 30` block's scratch registers differ: the 0x4330/index pair uses
// r7/r6 instead of r6/r9 and the mask/double address pair uses r8/r9 instead of r7/r8. The 0x70
// dispatch also omits the target's redundant `bge default` before its `b default` (one instruction).
// NONMATCHING (99.66%): see above.
void effReleaseMain(EffectEntry* effect) {
    effReleaseN64Data* data = effect->userdata;
    Vec position2;
    Vec position;
    s32 counter;
    s32 type;
    s32 i;

    position = releaseMakeVec(data->x, data->y, data->z);
    position2 = position;
    type = data->field_0x4;

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->field_0x14 = 16;
    }
    if (data->field_0x14 < 1000) {
        data->field_0x14--;
    }
    data->field_0x18++;
    if (data->field_0x14 < 0) {
        effDelete(effect);
        return;
    }
    counter = data->field_0x18;
    if (data->field_0x14 < 16) {
        data->field_0x28 = data->field_0x14 << 4;
    }

    switch (data->field_0x70) {
        case 0:
            data->field_0x74++;
            data->alphas[0] = 200 + (counter & 3) * 30;
            data->scales[0] = float_0p04_8041fb74 * (f32)sin(float_6p2832_8041fb68 * (f32)(counter * 20) / float_360_8041fb6c) + float_0p5_8041fb70;
            if ((s32)data->field_0x0 != 0) {
                data->x += (f32)sin(float_6p2832_8041fb68 * (f32)counter / float_360_8041fb6c);
                data->y += (f32)cos(float_6p2832_8041fb68 * (float_1p2356_8041fb78 * (f32)counter) / float_360_8041fb6c);
                if (data->field_0x74 > 50) {
                    data->field_0x74 = 0;
                    if (type == 3) {
                        data->field_0x70 = 100;
                    } else {
                        data->field_0x70 = 1;
                    }
                }
            }
            break;
        case 100: {
            u16 t;
            u8 b;
            f32 s;
            switch (data->field_0x74) {
                case 0:
                    data->scales[3] = float_4_8041fb7c;
                    data->alphas[3] = 0xFF;
                    break;
                case 1:
                    data->scales[3] = float_8_8041fb80;
                    data->alphas[3] = 0xFF;
                    break;
                default:
                    data->scales[3] = float_0_8041fb40;
                    data->alphas[3] = 0;
                    break;
            }
            if (data->field_0x74 < 30) {
                t = anime_data[data->field_0x74].field_0;
                s = float_0p3_8041fb84 * (f32)data->field_0x74 + float_0p01_8041fb88 * (f32)t;
                b = anime_data[data->field_0x74].field_2;
            } else {
                t = anime_data[29].field_0;
                s = float_0p01_8041fb88 * (f32)t + float_0p3_8041fb84 * (f32)data->field_0x74;
                b = anime_data[29].field_2;
            }
            data->alphas[0] = 0;
            data->scales[0] = float_0_8041fb40;
            data->alphas[1] = b & 0xFF;
            data->alphas[2] = b & 0xFF;
            data->scales[1] = s;
            data->scales[2] = float_0p6_8041fb8c + s;
            data->field_0x74++;
            break;
        }
        case 1: {
            u16 t;
            u8 a;
            f32 s;
            switch (data->field_0x74) {
                case 0:
                    data->scales[3] = float_4_8041fb7c;
                    data->alphas[3] = 0xFF;
                    break;
                case 1:
                    data->scales[3] = float_8_8041fb80;
                    data->alphas[3] = 0xFF;
                    break;
                default:
                    data->scales[3] = float_0_8041fb40;
                    data->alphas[3] = 0;
                    break;
            }
            t = anime_data[data->field_0x74].field_0;
            s = float_0p01_8041fb88 * (f32)t;
            a = anime_data[data->field_0x74].field_2;
            data->alphas[0] = 0;
            data->alphas[1] = a;
            data->alphas[2] = a;
            data->scales[0] = float_0_8041fb40;
            data->scales[1] = s;
            data->scales[2] = float_1p2_8041fb90 * s + float_4_8041fb7c;
            data->field_0x74++;
            if (data->field_0x74 >= 30) {
                data->field_0x74 = 0;
                data->field_0x70 = 4;
            }
            break;
        }
        case 4: {
            u16 t;
            u8 a;
            f32 s;
            switch (29 - data->field_0x74) {
                case 0:
                    data->scales[3] = float_4_8041fb7c;
                    data->alphas[3] = 0xFF;
                    break;
                case 1:
                    data->scales[3] = float_8_8041fb80;
                    data->alphas[3] = 0xFF;
                    break;
                default:
                    data->scales[3] = float_0_8041fb40;
                    data->alphas[3] = 0;
                    break;
            }
            t = anime_data_2[data->field_0x74].field_0;
            s = float_0p01_8041fb88 * (f32)t;
            a = anime_data_2[data->field_0x74].field_2;
            data->alphas[0] = 0;
            data->scales[0] = float_0_8041fb40;
            data->alphas[1] = a;
            data->alphas[2] = 0;
            data->scales[1] = s;
            data->scales[2] = float_0_8041fb40;
            data->field_0x74++;
            if (data->field_0x74 >= 30) {
                data->field_0x74 = 0;
                data->field_0x70 = 5;
            }
            break;
        }
        default:
            data->scales[0] = float_0_8041fb40;
            data->scales[1] = float_0_8041fb40;
            data->scales[2] = float_0_8041fb40;
            data->alphas[0] = 0;
            data->alphas[1] = 0;
            data->alphas[2] = 0;
            data->scales[3] = float_0_8041fb40;
            data->alphas[3] = 0;
            data->scales[3] = float_0_8041fb40;
            data->field_0x74++;
            break;
    }

    for (i = 0; i < 4; i++) {
        if (data->alphas[i] > 0xFF) {
            data->alphas[i] = 0xFF;
        }
        if (data->alphas[i] < 0) {
            data->alphas[i] = 0;
        }
        if (data->scales[i] < float_0_8041fb40) {
            data->scales[i] = float_0_8041fb40;
        }
    }

    dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effReleaseDisp, effect);
}
#pragma opt_common_subs reset

// .text

#ifdef MUST_MATCH
// The three GXColors are built inside inline setters so each call gets its own
// local-plus-by-value-copy stack pair, as in the original.
static inline void releaseSetColor1(effReleaseN64Data* data) {
    GXColor c = *(GXColor*)&unk_80423240;
    c.r = data->field_0x2C;
    c.g = data->field_0x30;
    c.b = data->field_0x34;
    c.a = data->field_0x38;
    GXSetTevColor(GX_TEVREG1, c);
}

static inline void releaseSetColor2(effReleaseN64Data* data, s32 i) {
    GXColor c = *(GXColor*)&unk_80423244;
    c.r = data->field_0x1C;
    c.g = data->field_0x20;
    c.b = data->field_0x24;
    c.a = data->alphas[i];
    GXSetTevColor(GX_TEVREG0, c);
}

static inline void releaseSetColor3(s32 alpha) {
    GXColor c = *(GXColor*)&dat_8041fb38;
    c.a = alpha;
    GXSetChanMatColor(GX_COLOR0A0, c);
}
#endif

void effReleaseDisp(CameraId camId, void* param) {
#ifdef MUST_MATCH
    u8* vtxBase = (u8*)lbl_803A4C60;
#endif
    EffectEntry* effect = param;
    effReleaseN64Data* data;
    CameraEntry* cam = camGetPtr(camId);
    s32 type;
    Mtx trans;
    Mtx scale;
    Mtx rot;
    Mtx mtx;
    Mtx44 ortho;
    GXTexObj texObj;
    f32 projv[7];
    s32 i;
    s32 alpha;

    data = effect->userdata;
    type = data->field_0x4;

    PSMTXTrans(trans, data->x, data->y, data->z);
    PSMTXRotRad(rot, 'y', -camGetPtr(CAMERA_3D)->viewYaw * float_deg2rad_8041fb3c);
    PSMTXScale(scale, data->field_0x3C, data->field_0x3C, data->field_0x3C);
    PSMTXConcat(trans, rot, trans);
    PSMTXConcat(trans, scale, mtx);

    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);

    {
#ifdef MUST_MATCH
    releaseSetColor1(data);
#else
    GXColor color;
    color = *(GXColor*)&unk_80423240;
    color.r = data->field_0x2C;
    color.g = data->field_0x30;
    color.b = data->field_0x34;
    color.a = data->field_0x38;
    GXSetTevColor(GX_TEVREG1, color);
#endif
    }
    GXSetCullMode(GX_CULL_NONE);

    for (i = 0; i < 4; i++) {
#ifndef MUST_MATCH
        GXColor color2;
#endif
        if (type == 1 && i == 2) {
            continue;
        }
        if (float_0_8041fb40 == data->scales[i]) {
            continue;
        }
        if (data->alphas[i] == 0) {
            continue;
        }
        PSMTXScale(scale, data->scales[i], data->scales[i], data->scales[i]);
        PSMTXConcat(mtx, scale, scale);
        PSMTXConcat(cam->view, scale, scale);
        GXLoadPosMtxImm(scale, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
#ifdef MUST_MATCH
        releaseSetColor2(data, i);
#else
        color2 = *(GXColor*)&unk_80423244;
        color2.r = data->field_0x1C;
        color2.g = data->field_0x20;
        color2.b = data->field_0x24;
        color2.a = data->alphas[i];
        GXSetTevColor(GX_TEVREG0, color2);
#endif

        switch (i) {
            case 0:
                effGetTexObjN64(0x44, &texObj);
                GXLoadTexObj(&texObj, GX_TEXMAP0);
                PSMTXScale(scale, float_0p03125_8041fb44, float_0p03125_8041fb44, float_0_8041fb40);
                GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
#ifdef MUST_MATCH
                effSetVtxDescN64((effVtxArray*)(vtxBase + 0x130));
#else
                effSetVtxDescN64((effVtxArray*)size16x16_tex64x64_vtx);
#endif
                GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
                tri2(0, 1, 2, 0, 0, 2, 3, 0);
                break;
            case 1:
                effGetTexObjN64(0x45, &texObj);
                GXLoadTexObj(&texObj, GX_TEXMAP0);
#ifdef MUST_MATCH
                effSetVtxDescN64((effVtxArray*)(vtxBase + 0x168));
#else
                effSetVtxDescN64((effVtxArray*)size16x16_tex256x32_vtx);
#endif
                PSMTXScale(scale, float_0p0039062_8041fb48, float_0p0039062_8041fb48, float_0_8041fb40);
                PSMTXTrans(trans, float_0_8041fb40, float_0_8041fb40, float_0_8041fb40);
                PSMTXConcat(scale, trans, scale);
                GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
                GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0xC);
                tri2(0, 1, 2, 0, 0, 2, 3, 0);
                tri2(0x1C, 0x1D, 0x1E, 0x1C, 0x1C, 0x1E, 0x1F, 0x1C);
                PSMTXScale(scale, float_0p0039062_8041fb48, float_0p0039062_8041fb48, float_0_8041fb40);
                PSMTXTrans(trans, float_0_8041fb40, float_32_8041fb4c, float_0_8041fb40);
                PSMTXConcat(scale, trans, scale);
                GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
                GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0xC);
                tri2(4, 5, 6, 4, 4, 6, 7, 4);
                tri2(0x18, 0x19, 0x1A, 0x18, 0x18, 0x1A, 0x1B, 0x18);
                PSMTXScale(scale, float_0p0039062_8041fb48, float_0p0039062_8041fb48, float_0_8041fb40);
                PSMTXTrans(trans, float_0_8041fb40, float_64_8041fb50, float_0_8041fb40);
                PSMTXConcat(scale, trans, scale);
                GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
                GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0xC);
                tri2(8, 9, 10, 8, 8, 10, 11, 8);
                tri2(0x14, 0x15, 0x16, 0x14, 0x14, 0x16, 0x17, 0x14);
                PSMTXScale(scale, float_0p0039062_8041fb48, float_0p0039062_8041fb48, float_0_8041fb40);
                PSMTXTrans(trans, float_0_8041fb40, float_96_8041fb54, float_0_8041fb40);
                PSMTXConcat(scale, trans, scale);
                GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
                GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0xC);
                tri2(0xC, 0xD, 0xE, 0xC, 0xC, 0xE, 0xF, 0xC);
                tri2(0x10, 0x11, 0x12, 0x10, 0x10, 0x12, 0x13, 0x10);
                break;
            case 2:
                effGetTexObjN64(0x46, &texObj);
                GXLoadTexObj(&texObj, GX_TEXMAP0);
                PSMTXScale(scale, float_0p015625_8041fb58, float_0p015625_8041fb58, float_0_8041fb40);
                GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
#ifdef MUST_MATCH
                effSetVtxDescN64((effVtxArray*)(vtxBase + 0x328));
#else
                effSetVtxDescN64((effVtxArray*)size16x16_tex128x128_vtx);
#endif
                GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
                tri2(0, 1, 2, 0, 0, 2, 3, 0);
                /* no break: case 2 falls through into case 3 (matches the original) */
            case 3:
                effGetTexObjN64(0x47, &texObj);
                GXLoadTexObj(&texObj, GX_TEXMAP0);
                PSMTXScale(scale, float_0p015625_8041fb58, float_0p03125_8041fb44, float_0_8041fb40);
                GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
#ifdef MUST_MATCH
                effSetVtxDescN64((effVtxArray*)(vtxBase + 0x360));
#else
                effSetVtxDescN64((effVtxArray*)size16x16_tex64x64_0_vtx);
#endif
                GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0xC);
                tri2(0, 1, 2, 0, 0, 2, 3, 0);
                tri2(4, 5, 6, 4, 4, 6, 7, 4);
                break;
            case 4:
                break;
            default:
                break;
        }
    }

    if (type == 2 || type == 3) {
        alpha = 0;
        if (data->field_0x70 == 1 || data->field_0x70 == 0x64) {
            if (data->field_0x74 > 2 && data->field_0x74 <= 10) {
                alpha = 0xFF - (data->field_0x74 - 2) * 30;
            }
        }
        if (data->field_0x70 == 5) {
            if (data->field_0x74 > 0 && data->field_0x74 <= 8) {
                alpha = 0xFF - data->field_0x74 * 30;
            }
        }
        if (alpha > 0) {
#ifndef MUST_MATCH
            GXColor color3;
#endif
            GXGetProjectionv(projv);
            C_MTXOrtho(ortho, float_0_8041fb40, float_480_8041fb5c, float_0_8041fb40, float_608_8041fb60, float_0_8041fb40, float_1_8041fb64);
            GXSetProjection(ortho, GX_ORTHOGRAPHIC);
            GXSetNumChans(1);
            GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
#ifdef MUST_MATCH
            releaseSetColor3(alpha);
#else
            color3 = *(GXColor*)&dat_8041fb38;
            color3.a = alpha;
            GXSetChanMatColor(GX_COLOR0A0, color3);
#endif
            GXSetNumTevStages(1);
            GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
            GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ONE);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
            PSMTXIdentity(trans);
            GXLoadPosMtxImm(trans, GX_PNMTX0);
            GXSetCurrentMtx(GX_PNMTX0);
#ifdef MUST_MATCH
            effSetVtxDescN64((effVtxArray*)(vtxBase + 0x3d0));
#else
            effSetVtxDescN64((effVtxArray*)rectangle_vtx);
#endif
            GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
            tri2(0, 1, 2, 0, 0, 2, 3, 0);
            GXSetProjectionv(projv);
        }
    }
}
