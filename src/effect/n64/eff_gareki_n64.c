#include "eff/n64/eff_gareki_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "system.h"
#include <dolphin/mtx.h>
#include <stdlib.h>

// .sdata2 float constants (named after their original addresses).
extern const f32 float_neg0p1_8041f054;
extern const f32 float_0p8_8041f058;
extern const f32 float_neg0p2_8041f05c;
extern const f32 float_0p9_8041f060;
extern const f32 float_10_8041f064;
extern const f32 float_20_8041f068;
extern const f32 float_neg2_8041f06c;
extern const f32 float_0_8041f048;
extern const f32 float_0p25_8041f070;
extern const f32 float_2_8041f074;
extern const f32 float_1_8041f078;
extern const f32 float_0p1_8041f07c;

// .rodata / .data constants (named after their original addresses).
const f64 double_to_int_802fa6e0 = 4503601774854144.0;
extern const char str_802FA6E8[];

// trans_data is a run of (offsetX, offsetY) pairs; typed as a pair struct so the
// loop's induction variable strength-reduces to a pre-biased pointer.
typedef struct GarekiOffset {
    f32 x; // 0x0
    f32 y; // 0x4
} GarekiOffset;

extern GarekiOffset trans_data[];
static f32 vx_data[8] = { -1.0f, -0.5f, 0.5f, 1.0f, -1.0f, -0.5f, 0.5f, 1.0f };

typedef struct GarekiChild {
    f32 x;          // 0x00
    f32 y;          // 0x04
    f32 z;          // 0x08
    u8 pad_0C[4];   // 0x0C
    f32 vx;         // 0x10
    f32 vy;         // 0x14
    f32 offsetX;    // 0x18
    f32 offsetY;    // 0x1C
    u8 pad_20[0xC]; // 0x20
    f32 angleX;     // 0x2C
    u8 pad_30[4];   // 0x30
    f32 angleZ;     // 0x34
    u8 pad_38[8];   // 0x38
} GarekiChild; // 0x40

typedef struct GarekiData {
    Vec position;            // 0x00
    f32 groundY;             // 0x0C
    u8 pad_10[4];            // 0x10
    f32 vy;                  // 0x14
    u8 pad_18[8];            // 0x18
    s32 timer;               // 0x20
    s32 state;               // 0x24
    s32 alpha;               // 0x28
    f32 angleX;              // 0x2C
    f32 angleZ;              // 0x30
    f32 field_34;            // 0x34
    f32 angleVelX;           // 0x38
    f32 angleVelZ;           // 0x3C
    GarekiChild children[];  // 0x40
} GarekiData;

// Local copy of system.c's inline helper so it inlines here.
inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

// NONMATCHING (99.86%): only the two loop-preheader pointer setups are scheduled in the
// opposite order (target: trans = &trans_data[1], then p = data->children; MWCC swaps them).
EffectEntry* effGarekiN64Entry(s32 type, f32 x, f32 y, f32 z, f32 groundY) {
    EffectEntry* effect;
    GarekiData* data;
    GarekiChild* p;
    GarekiOffset* trans;
    s32 i;

    effect = effEntry();
    effect->type = str_802FA6E8;
    effect->count = 9;
    data = __memAlloc(HEAP_EFFECT, 0x240);
    effect->userdata = data;
    effect->callback = effGarekiMain;
    data->state = 0;
    data->position.x = x;
    data->position.y = y;
    data->groundY = groundY;
    data->position.z = z;
    data->vy = float_neg2_8041f06c;
    data->angleX = float_0_8041f048;
    data->angleZ = float_0_8041f048;
    data->field_34 = (f32)(rand() % 60 - 30);
    data->alpha = 0xff;
    data->angleVelX = (f32)(rand() % 60 - 30);
    data->angleVelZ = float_0p25_8041f070 * data->angleVelX;
    data->timer = 20;

    p = data->children;
    trans = trans_data;
    for (i = 1; i < 9; i++, p++) {
        p->offsetX = trans[i - 1].x;
        p->offsetY = trans[i - 1].y;
        p->x = float_0_8041f048;
        p->y = float_0_8041f048;
        p->z = float_0_8041f048;
        p->vx = float_2_8041f074 * vx_data[i - 1];
        p->vy = float_2_8041f074 * ((f32)(rand() % 10) * float_0p1_8041f07c) + float_1_8041f078;
        p->angleX = float_0_8041f048;
        p->angleZ = float_0_8041f048;
    }
    return effect;
}

void effGarekiMain(EffectEntry* effect) {
    GarekiData* data = effect->userdata;
    Vec position;
    s32 i;

    position = initializeVec(&data->position);

    if (data->state == 0) {
        data->vy += float_neg0p1_8041f054;
        data->position.y += data->vy;
        data->angleX += data->angleVelX;
        data->angleZ += data->angleVelZ;
        if (data->position.y <= data->groundY) {
            data->state = 1;
        }
    }

    if (data->state == 1) {
        data->timer--;
        if (data->timer < 0) {
            effDelete(effect);
            return;
        }
        if (data->timer < 10) {
            data->alpha = (s32)((f32)data->alpha * float_0p8_8041f058);
        }
        data++;
        for (i = 1; i < effect->count; i++, data++) {
            ((GarekiChild*)data)->vy += float_neg0p2_8041f05c;
            ((GarekiChild*)data)->vx *= float_0p9_8041f060;
            ((GarekiChild*)data)->x += ((GarekiChild*)data)->vx;
            ((GarekiChild*)data)->y += ((GarekiChild*)data)->vy;
            ((GarekiChild*)data)->angleX += float_10_8041f064;
            ((GarekiChild*)data)->angleZ += float_20_8041f068;
        }
    }

    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effGarekiDisp, effect);
}

