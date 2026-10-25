#include "eff/eff_starpoint.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <stdlib.h>

typedef struct StarPointData {
    s32 type;       // 0x00
    s32 life;       // 0x04
    s32 frame;      // 0x08
    Vec position;   // 0x0C
    f32 velX;       // 0x18
    f32 velZ;       // 0x1C
    f32 velY;       // 0x20
    f32 gravity;    // 0x24
    u8 pad_0x28[4]; // 0x28
    s32 alpha;      // 0x2C
    s32 delay;      // 0x30
} StarPointData;


static inline f32 effStarPointSinf(f32 rad) {
    f32 v = 0.00761f;
    f32 b = 0.16605f;
    f32 one = 1.0f;

    if (rad <= 3.1415927f) {
        if (rad <= 1.5707964f) {
            f32 t = rad * rad;
            v = v * t - b;
            v = v * t + one;
            v = v * rad;
        } else {
            f32 u = 1.5707964f - (rad - 1.5707964f);
            f32 s = u * u;
            v = v * s - b;
            v = v * s + one;
            v = v * u;
        }
    } else if (rad < 4.712389f) {
        f32 t = rad - 3.1415927f;
        f32 u = t * t;
        v = v * u - b;
        v = v * u + one;
        v = v * t;
        return -v;
    } else {
        f32 u = 1.5707964f - (rad - 4.712389f);
        f32 s = u * u;
        v = v * s - b;
        v = v * s + one;
        v = v * u;
        return -v;
    }
    return v;
}

static inline f32 effStarPointCosf(f32 rad) {
    f32 v = 0.03705f;
    f32 b = 0.4967f;
    f32 one = 1.0f;

    if (rad <= 3.1415927f) {
        if (rad <= 1.5707964f) {
            f32 t = rad * rad;
            v = v * t - b;
            v = v * t + one;
        } else {
            f32 u = 1.5707964f - (rad - 1.5707964f);
            f32 s = u * u;
            v = v * s - b;
            v = v * s + one;
            return -v;
        }
    } else if (rad < 4.712389f) {
        f32 t = rad - 3.1415927f;
        f32 u = t * t;
        v = v * u - b;
        v = v * u + one;
        return -v;
    } else {
        f32 u = 1.5707964f - (rad - 4.712389f);
        f32 s = u * u;
        v = v * s - b;
        v = v * s + one;
    }
    return v;
}

EffectEntry* effStarPointEntry(s32 type, s32 count, f32 x, f32 y, f32 z) {
    s32 delay;
    EffectEntry* entry;
    StarPointData* data;
    s32 i;

    entry = effEntry();
    entry->type = "StarPoint";
    entry->count = count + 1;
    data = __memAlloc(HEAP_EFFECT, entry->count * sizeof(StarPointData));
    entry->userdata = data;
    entry->callback = effStarPointMain;
    data->type = type;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->life = 60;
    data->frame = 0;

    i = 1;
    delay = 4;
    data = &data[1];
    for (; i < entry->count; i++, data++) {
        f32 angle = rand() % 360;
        f32 rad;

        data->position.x = 0.0f;
        data->position.y = 0.0f;
        data->position.z = 0.0f;
        rad = (6.2831855f * angle) / 360.0f;
        data->velX = 2.0f * effStarPointSinf(rad);
        data->velZ = 2.0f * effStarPointCosf(rad);
        data->velY = 14.0f;
        data->gravity = 2.0f;
        data->alpha = 0;
        data->delay = delay;
        delay += 3;
    }

    return entry;
}

void effStarPointMain(EffectEntry* effect) {
    Vec position;
    StarPointData* data = effect->userdata;
    Vec disp = { 0 };
    s32 i;

    disp.x = data->position.x;
    disp.y = data->position.y;
    disp.z = data->position.z;
    position = disp;

    data->life--;
    data->frame++;
    if (data->life < 0) {
        effDelete(effect);
        return;
    }

    data++;
    for (i = 1; i < effect->count; i++, data++) {
        if (data->delay != 0) {
            data->delay--;
            data->alpha = 0;
        } else {
            data->velY -= data->gravity;
            data->position.x += data->velX;
            data->position.y += data->velY;
            data->position.z += data->velZ;
            if (data->velY < 0.0f) {
                data->alpha -= 2;
                if (data->alpha < 0) {
                    data->alpha = 0;
                }
            } else {
                data->alpha = 0xff;
            }
        }
    }
    dispEntry(CAMERA_3D, dispCalcZ(&position), 1, effStarPointDisp, effect);
}

void effStarPointDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    StarPointData* data = effect->userdata;
    StarPointData* elem;
    Mtx mtx;
    Mtx rot;
    Mtx elemMtx;
    s32 i;

    PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
    PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
    PSMTXConcat(mtx, rot, mtx);

    data++;
    for (i = 1; i < effect->count; i++, data++) {
        if (data->alpha != 0) {
            PSMTXTrans(elemMtx, data->position.x, data->position.y, data->position.z);
            PSMTXConcat(mtx, elemMtx, elemMtx);
            iconDispGx2(elemMtx, 0, 0x18C);
        }
    }
}

