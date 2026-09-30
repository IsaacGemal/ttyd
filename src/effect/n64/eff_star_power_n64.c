#include "eff/n64/eff_star_power_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include "math_ext.h"

extern const f32 float_0_8041fe2c;
extern const f32 float_4_8041fe3c;
extern const f32 float_0p4953_8041fe38;
extern const f32 float_0p2234_8041fe40;
extern const f32 float_6p2832_8041fe44;
extern const f32 float_7p1234_8041fe48;
extern const f32 float_360_8041fe4c;
extern const f32 float_128_8041fe50;
extern const f32 float_127_8041fe54;
extern const f32 float_1p231_8041fe58;
extern const f32 float_215_8041fe5c;
extern const f32 float_0p531_8041fe60;
extern const f32 float_3p231_8041fe64;
extern const f32 float_0p298_8041fe68;
extern const f32 float_188_8041fe6c;
extern const f32 float_0p831_8041fe70;
extern const f32 float_2p231_8041fe74;
extern const f32 float_2p044_8041fe78;
extern const f32 float_0p7_8041fe7c;
extern const f32 float_0p3_8041fe80;

const Vec vec3_802fb718 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802fb728 = 4503601774854144.0;
// Inline copy of the file-local helper: zero-fill a Vec from the rodata constant, then
// overwrite it with the supplied position; the temporary is returned by value.
inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

const char str_802FB730[] = "StarPowerN64";

EffectEntry* effStarPowerN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 scale) {
    EffectEntry* effect;
    EffStarPowerData* data;

    effect = effEntry();
    effect->type = str_802FB730;
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(EffStarPowerData));
    effect->userdata = data;
    effect->callback = effStarPowerMain;
    effect->flags |= 2;
    data->field_0x0 = 1;
    data->type = type;
    data->field_0x14 = 0;
    if (count <= 0) {
        data->lifetime = 1000;
    } else {
        data->lifetime = count;
    }
    data->x = x;
    data->y = y;
    data->z = z;
    data->field_0x38 = scale;
    data->color1R = 0xFF;
    data->color1G = 0xFF;
    data->color1B = 0x78;
    data->color2R = 0xFF;
    data->color2G = 0x78;
    data->field_0x30 = 0;
    data->color2A = 0xFF;
    data->color1A = 0xFF;
    data->field_0x3C = float_0_8041fe2c;
    data->field_0x40 = float_0_8041fe2c;
    data->field_0x44 = float_0_8041fe2c;
    return effect;
}

// The int->float conversions fold to an anonymous @216 local in our object (the original uses the
// global double_to_int_802fb728; project-wide folded-pool issue, cf. eff_rappa/win_log), MWCC hoists
// two vec3_802fb718 words into the prologue saves, and a few f-register choices differ.
// NONMATCHING (98.55%): those two compiler behaviours plus the FP allocation.
void effStarPowerMain(EffectEntry* effect) {
    EffStarPowerData* data = effect->userdata;
    Vec position = initializeVec((Vec*)&data->x);
    s32 type = data->type;
    s32 counter;
    s32 i;

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->lifetime = 0x10;
    }
    if (data->lifetime < 1000) {
        data->lifetime--;
    }
    data->field_0x14++;
    if (data->lifetime < 0) {
        effDelete(effect);
    } else {
        counter = data->field_0x14;
        if (data->lifetime < 16) {
            i = data->lifetime * 16;
            if (data->color1A > i) {
                data->color1A = i;
            }
            if (data->color2A > i) {
                data->color2A = i;
            }
        }
        if (counter < 16) {
            i = counter * 16 + 15;
            if (data->color1A < i) {
                data->color1A = i;
            }
            if (data->color2A < i) {
                data->color2A = i;
            }
        }
        *(f32*)&data->field_0x48 = data->field_0x3C;
        *(f32*)&data->field_0x4C = data->field_0x40;
        *(f32*)&data->field_0x50 = data->field_0x44;
        *(f32*)&data->field_0x54 = data->field_0x38;
        if (type == 1) {
            f32 f7;
            data->field_0x3C = (f32)(counter * 4);
            f7 = float_4_8041fe3c * (f32)counter;
            data->field_0x40 = float_0p4953_8041fe38 * f7;
            data->field_0x44 = float_0p2234_8041fe40 * f7;
            data->color1A = 0xFF;
            data->color2A = (u8)(float_127_8041fe54 * (f32)sin(float_6p2832_8041fe44 * (float_7p1234_8041fe48 * (f32)counter) / float_360_8041fe4c) + float_128_8041fe50);
            data->color1R = (u8)(float_127_8041fe54 * (f32)sin(float_6p2832_8041fe44 * (float_1p231_8041fe58 * (f32)counter) / float_360_8041fe4c) + float_215_8041fe5c);
            data->color1G = (u8)(float_127_8041fe54 * (f32)sin(float_6p2832_8041fe44 * (float_0p531_8041fe60 * (f32)counter) / float_360_8041fe4c) + float_215_8041fe5c);
            data->color1B = (u8)(float_127_8041fe54 * (f32)sin(float_6p2832_8041fe44 * (float_3p231_8041fe64 * (f32)counter) / float_360_8041fe4c) + float_215_8041fe5c);
            data->color2R = (u8)(float_127_8041fe54 * (f32)sin(float_6p2832_8041fe44 * (float_0p298_8041fe68 * (f32)counter) / float_360_8041fe4c) + float_188_8041fe6c);
            data->color2G = (u8)(float_127_8041fe54 * (f32)sin(float_6p2832_8041fe44 * (float_0p831_8041fe70 * (f32)counter) / float_360_8041fe4c) + float_188_8041fe6c);
            data->field_0x30 = (u8)(float_127_8041fe54 * (f32)sin(float_6p2832_8041fe44 * (float_2p231_8041fe74 * (f32)counter) / float_360_8041fe4c) + float_188_8041fe6c);
            data->field_0x38 = float_0p3_8041fe80 * (f32)sin(float_6p2832_8041fe44 * (float_2p044_8041fe78 * (f32)counter) / float_360_8041fe4c) + float_0p7_8041fe7c;
        }
        if (counter == 1) {
            *(f32*)&data->field_0x48 = data->field_0x3C;
            *(f32*)&data->field_0x4C = data->field_0x40;
            *(f32*)&data->field_0x50 = data->field_0x44;
            *(f32*)&data->field_0x54 = data->field_0x38;
        }
        dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effStarPowerDisp, effect);
    }
}
