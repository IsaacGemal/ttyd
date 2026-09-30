#include "eff/n64/eff_pturn_n64.h"
#include "memory.h"

#include "driver/dispdrv.h"
#include "math_ext.h"

// .rodata and .sdata2 constants (named after their original addresses in the asm).
extern const char str_802FB468[];
extern const f32 float_1_8041fa64;
extern const f32 float_0_8041fa68;
extern const f32 float_6p2832_8041fa74;
extern const f32 float_0p05_8041fa78;
extern const f32 float_360_8041fa7c;
extern const f32 float_4_8041fa80;
extern const f32 float_neg2_8041fa84;
extern const f32 float_neg0p5_8041fa88;
extern const f32 float_10_8041fa8c;
f32 scale_dat[9] = { 5.0f, 4.5f, 4.0f, 3.0f, 2.0f, 1.0f, 0.7f, 1.2f, 1.0f };

// .rodata: zero vector copied into the local position before the particle fields are set
const Vec vec3_802fb450 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802fb460 = 4503601774854144.0;

EffectEntry* effPturnN64Entry(s32 type, s32 a1, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    effPturnN64Data* data;

    effect = effEntry();
    effect->type = str_802FB468;
    effect->count = 2;
    data = __memAlloc(HEAP_EFFECT, 0x90);
    effect->userdata = data;
    effect->callback = effPturnMain;
    effect->flags |= 2;
    data->field_0x0 = type;
    data->field_0x34 = float_1_8041fa64;
    if (type == 1) {
        data->field_0x38 = 1;
        data->field_0x3C = 0;
    } else {
        data->field_0x38 = 0;
        data->field_0x3C = a1;
    }
    data->x = x;
    data->y = y;
    data->z = z;
    switch (type) {
    case 0:
        data->life = 100;
        break;
    case 1:
        data->life = 100;
        break;
    case 10:
        data->life = 0x15;
        break;
    }
    data->field_0x2C = 0;
    data->field_0x44 = 0;
    data->field_0x4 = 1;
    data->field_0x58 = float_0_8041fa68;
    data->field_0x54 = float_0_8041fa68;
    data->field_0x50 = float_0_8041fa68;
    data->field_0x68 = float_0_8041fa68;
    data->field_0x64 = float_0_8041fa68;
    data->field_0x60 = float_0_8041fa68;
    data->field_0x5C = float_0_8041fa68;
    data->field_0x88 = float_0_8041fa68;
    data->field_0x70 = 0;
    data->field_0x6C = 0xFF;
    return effect;
}

void effPturnMain(EffectEntry* effect) {
    s32 life25;
    PturnN64Elem* data;
    s32 i;
    s32 life;
    s32 type;
    s32 counter;
    s32 field3C;
    s32 field38;
    f32 f31;
    f32 f30;
    f32 scaleFactor;
#ifdef MUST_MATCH
    /* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
       local before dispCalcZ; a single local folds the copy away (same idiom as
       eff_bush_n64.c). */
    Vec position;
#endif
    Vec disp;

    data = effect->userdata;
    disp = vec3_802fb450;
    disp.x = data->x;
    disp.y = data->y;
    disp.z = data->z;
#ifdef MUST_MATCH
    position = disp;
#endif

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->life = 0x15;
    }
    if (data->life < 100) {
        data->life--;
    }
    if (data->life < 0) {
        effDelete(effect);
        return;
    }
    data->field_0x2C++;
    if (data->field_0x2C > 0x4F1A0) {
        data->field_0x2C = 0x100;
    }

    type = data->field_0x0;
    life = data->life;
    counter = data->field_0x2C;
    field3C = data->field_0x3C;
    field38 = data->field_0x38;
    scaleFactor = data->field_0x34;
    if (type == 0) {
        if (data->field_0x44 > 0) {
            data->field_0x44--;
        } else if (field3C != field38) {
            if (field3C < 0) {
                field3C = 0;
            }
            if (field3C > field38) {
                data->life = (type == 0) ? 100 : 60;
                data->field_0x2C = 1;
                data->field_0x38++;
                data[1].life = 0;
            } else if (field3C < field38) {
                effPturnN64Entry(10, 0, data->x, data->y + data[1].field_0x20, data->z);
                data->field_0x38--;
            }
            data->field_0x44 = 8;
        }
    }

    f31 = float_6p2832_8041fa74 * (f32)(counter * 12);
    f30 = float_0p05_8041fa78 * (f32)life;
    life25 = life * 25;

    data++;
    for (i = 1; i < effect->count; i++, data++) {
        s32 elemLife = data->life;
        f32 scale;

        scale = (elemLife >= 9 || type == 10) ? float_1_8041fa64 : scale_dat[elemLife];
        data->field_0x30 = scale;
        data->field_0x30 *= scaleFactor;
        data->field_0x20 = (type == 0 || type == 1) ? float_4_8041fa80 * (f32)cos(f31 / float_360_8041fa7c) : float_0_8041fa68;
        data->x += data->field_0x14;
        data->y += data->field_0x18;
        data->z += data->field_0x1C;
        if (life == 20) {
            data->field_0x18 = float_4_8041fa80;
            data->field_0x14 = float_neg2_8041fa84;
        }
        if (life < 20) {
            data->field_0x20 *= f30;
            data->field_0x18 += float_neg0p5_8041fa88;
            data->field_0x40 += float_10_8041fa8c;
        }
        if (life < 10) {
            data->field_0x24 = life25;
        }
        data->life++;
    }

#ifdef MUST_MATCH
    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effPturnDisp, effect);
#else
    dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effPturnDisp, effect);
#endif
}

void effPturnDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effPturnN64Data* data = effect->userdata;

    if (data->field_0x4 == 0) {
        return;
    }
    pturn_main_dispp(camId, param);
    if (data->field_0x0 == 1) {
        return;
    }
    pturn_num_dispp(camId, param);
}
