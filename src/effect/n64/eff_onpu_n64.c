#include "eff/n64/eff_onpu_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/mtx.h>
#include <stdlib.h>

// .sdata2 float constants (named after their original addresses).
extern const f32 float_0_8041f798;
extern const f32 float_0p3_8041f7a0;
extern const f32 float_1_8041f7a4;
extern const f32 float_0p04_8041f7a8;
extern const f32 float_neg5_8041f7ac;
extern const f32 float_0p1_8041f7b0;
extern const f32 float_0p5_8041f7b4;
extern const f32 float_5_8041f7b8;

// .rodata
extern const char str_802FB218[];

// Pooled int->double conversion constant for the (f32)(rand() % 11) casts.
const f64 double_to_int_802fb210 = 4503601774854144.0;

// Local copy of system.c's inline helper so it inlines here (as in eff_rankup.c).
inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

typedef struct effOnpuData {
    s32 type;      // 0x0
    Vec position;  // 0x4
    f32 scale;     // 0x10
    s32 alpha;     // 0x14
    s32 lifetime;  // 0x18
    s32 counter;   // 0x1C
    s32 randIndex; // 0x20
    f32 velX;      // 0x24
    f32 velY;      // 0x28
    f32 targetX;   // 0x2C
    f32 targetY;   // 0x30
} effOnpuData; // 0x34

EffectEntry* effOnpuN64Entry(s32 type, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    effOnpuData* data;

    effect = effEntry();
    effect->type = str_802FB218;
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(effOnpuData));
    effect->userdata = data;
    effect->callback = effOnpuMain;

    data->type = type;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->scale = float_0_8041f798;
    data->lifetime = 0x40;
    data->alpha = 0;
    data->counter = 0;
    data->randIndex = rand() % 7;

    switch (type) {
    case 0:
        data->velX = float_0_8041f798;
        data->targetX = float_0_8041f798;
        data->velY = float_1_8041f7a4;
        data->targetY = float_1_8041f7a4;
        break;
    case 1:
        data->velX = float_0p1_8041f7b0 * (f32)(rand() % 11) + float_neg5_8041f7ac;
        data->velY = float_0p1_8041f7b0 * (f32)(rand() % 11) + float_0p5_8041f7b4;
        data->targetX = float_0_8041f798;
        data->targetY = float_0_8041f798;
        break;
    default:
        data->velX = float_5_8041f7b8 - float_0p1_8041f7b0 * (f32)(rand() % 11);
        data->velY = float_0p1_8041f7b0 * (f32)(rand() % 11) + float_1_8041f7a4;
        data->targetX = float_0_8041f798;
        data->targetY = float_0_8041f798;
        break;
    }

    return effect;
}

void effOnpuMain(EffectEntry* effect) {
    effOnpuData* data;
    Vec position;
    s32 lifetime;

    data = effect->userdata;
    position = initializeVec(&data->position);

    data->lifetime--;
    data->counter++;
    lifetime = data->lifetime;
    if (lifetime < 0) {
        effDelete(effect);
        return;
    }
    if (lifetime > 5) {
        data->alpha = (s32)(float_0p3_8041f7a0 * (f32)(255 - data->alpha) + (f32)data->alpha);
        data->scale = float_0p3_8041f7a0 * (float_1_8041f7a4 - data->scale) + data->scale;
    }
    if (lifetime < 10) {
        data->alpha = lifetime * 25;
    }
    data->position.x += data->velX;
    data->position.y += data->velY;
    data->velX += float_0p04_8041f7a8 * (data->targetX - data->velX);
    data->velY += float_0p04_8041f7a8 * (data->targetY - data->velY);
    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effOnpuDisp, effect);
}
