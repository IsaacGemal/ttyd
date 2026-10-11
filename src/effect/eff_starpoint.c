#include "eff/eff_starpoint.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <stdlib.h>

typedef struct StarPointData {
    s32 type;       // 0x00
    s32 field_0x04; // 0x04
    s32 field_0x08; // 0x08
    Vec position;   // 0x0C
    f32 field_0x18; // 0x18
    f32 field_0x1C; // 0x1C
    f32 field_0x20; // 0x20
    f32 field_0x24; // 0x24
    s32 field_0x28; // 0x28
    s32 field_0x2C; // 0x2C
    s32 field_0x30; // 0x30
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

// opt_lifetimes off keeps `rad` and the inlines' locals as single webs (on, the webs split and the
// constants land in other FPRs); opt_propagation off keeps the inlines' `b`/`one` as hoisted locals
// instead of folding them back into literal loads at each use.
#pragma opt_lifetimes off
#pragma opt_propagation off
EffectEntry* effStarPointEntry(s32 type, s32 count, f32 x, f32 y, f32 z) {
    s32 lowAlpha;
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
    data->field_0x04 = 60;
    data->field_0x08 = 0;

    i = 1;
    lowAlpha = 4;
    data = &data[1];
    for (; i < entry->count; i++, data++) {
        f32 angle = rand() % 360;
        f32 rad;

        data->position.x = 0.0f;
        data->position.y = 0.0f;
        data->position.z = 0.0f;
        rad = (6.2831855f * angle) / 360.0f;
        data->field_0x18 = 2.0f * effStarPointSinf(rad);
        data->field_0x1C = 2.0f * effStarPointCosf(rad);
        data->field_0x20 = 14.0f;
        data->field_0x24 = 2.0f;
        data->field_0x2C = 0;
        data->field_0x30 = lowAlpha;
        lowAlpha += 3;
    }

    return entry;
}
#pragma opt_propagation reset
#pragma opt_lifetimes reset

void effStarPointMain(EffectEntry* effect) {
    Vec position;
    StarPointData* data = effect->userdata;
    Vec disp = { 0 };
    s32 i;

    disp.x = data->position.x;
    disp.y = data->position.y;
    disp.z = data->position.z;
    position = disp;

    data->field_0x04--;
    data->field_0x08++;
    if (data->field_0x04 < 0) {
        effDelete(effect);
        return;
    }

    data++;
    for (i = 1; i < effect->count; i++, data++) {
        if (data->field_0x30 != 0) {
            data->field_0x30--;
            data->field_0x2C = 0;
        } else {
            data->field_0x20 -= data->field_0x24;
            data->position.x += data->field_0x18;
            data->position.y += data->field_0x20;
            data->position.z += data->field_0x1C;
            if (data->field_0x20 < 0.0f) {
                data->field_0x2C -= 2;
                if (data->field_0x2C < 0) {
                    data->field_0x2C = 0;
                }
            } else {
                data->field_0x2C = 0xff;
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
        if (data->field_0x2C != 0) {
            PSMTXTrans(elemMtx, data->position.x, data->position.y, data->position.z);
            PSMTXConcat(mtx, elemMtx, elemMtx);
            iconDispGx2(elemMtx, 0, 0x18C);
        }
    }
}

