#include "eff/eff_miss_star.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "system.h"
#include <dolphin/mtx.h>

typedef struct MissStarData {
    Vec position;   // 0x0
    f32 vx;         // 0xC
    f32 vy;         // 0x10
    f32 vz;         // 0x14
    f32 dvx;        // 0x18
    f32 dvy;        // 0x1C
    f32 dvz;        // 0x20
    s32 field_0x24; // 0x24
    s32 field_0x28; // 0x28
    f32 angle;      // 0x2C
    f32 angleVel;   // 0x30
    f32 field_0x34;  // 0x34
    f32 scale;      // 0x38
    s32 field_0x3C; // 0x3C
    f32 field_0x40; // 0x40
} MissStarData;

const Vec vec3_802f3678 = { 0.0f, 0.0f, 0.0f };
const char str_802F3684[] = "SmallStar";
const f64 double_to_int_802f3690 = 4503601774854144.0;

// .data: initial velocity vectors used to fan the stars out.
Vec spd_tbl[15] = {
    { 0.0f, 4.5f, 0.0f },  { 1.0f, 4.0f, 0.0f },  { 2.0f, 3.0f, 0.0f },
    { 3.0f, 2.0f, 0.0f },  { 3.5f, 1.0f, 0.0f },  { 4.0f, 0.0f, 0.0f },
    { 4.5f, 0.0f, 0.0f },  { 5.0f, 0.0f, 0.0f },  { 4.5f, 0.0f, 0.0f },
    { 4.0f, 0.0f, 0.0f },  { 3.5f, -1.0f, 0.0f }, { 3.0f, -2.0f, 0.0f },
    { 2.0f, -3.0f, 0.0f }, { 1.0f, -4.0f, 0.0f }, { 0.0f, -4.5f, 0.0f },
};

// .data: per-star scale multipliers, grouped three at a time.
Vec scale_tbl[15] = {
    { 1.0f, 1.0f, 1.0f },    { 0.8f, 0.8f, 0.8f },    { 0.9f, 0.9f, 0.9f },
    { 1.1f, 1.1f, 1.1f },    { 1.0f, 1.0f, 1.0f },    { 0.8f, 0.8f, 0.8f },
    { 0.9f, 0.9f, 0.9f },    { 1.1f, 1.1f, 1.1f },    { 1.0f, 1.0f, 1.0f },
    { 0.8f, 0.8f, 0.8f },    { 0.9f, 0.9f, 0.9f },    { 1.1f, 1.1f, 1.1f },
    { 1.0f, 1.0f, 1.0f },    { 0.8f, 0.8f, 0.8f },    { 0.9f, 0.9f, 0.9f },
};

// .sdata2
extern const f32 float_0_8041cb6c;
extern const f32 float_1_8041cb70;
extern const f32 float_0p8_8041cb74;
extern const f32 float_0p5_8041cb78;
extern const f32 float_0p4_8041cb7c;
extern const f32 float_0p7_8041cb80;
extern const f32 float_2_8041cb84;
extern const f32 float_255_8041cb88;

EffectEntry* effMissStarEntry(s32 type, f32 x, f32 y, f32 z, s32 lifetime, s32 angle) {
    EffectEntry* entry;
    MissStarData* data;
    s32 i;
    s32 sign;
    f32 scale;
    f32 speed;

    entry = effEntry();
    scale = float_0p8_8041cb74;
    speed = float_1_8041cb70;
    if (type <= 0) {
        type = 1;
        scale = float_0p4_8041cb7c;
        speed = float_0p7_8041cb80;
    }

    sign = 1;
    if (angle < 0) {
        angle = -angle;
        sign = -1;
    }
#ifdef MUST_MATCH
    // The original tests the bound with an explicit break; a plain `while (angle > 5)`
    // lets MWCC fold the trip count into a divwu/bdnz loop that does not match.
    while (1) {
        if (angle <= 5) {
            break;
        }
        angle -= 5;
    }
#else
    while (angle > 5) {
        angle -= 5;
    }
#endif

    entry->type = str_802F3684;
    entry->count = type;
    data = __memAlloc(HEAP_EFFECT, type * sizeof(MissStarData));
    entry->userdata = data;
    entry->callback = effMissStarMain;

    for (i = 0; i < type; i++, data++) {
        data->position.x = x;
        data->position.y = y;
        data->position.z = z;
        data->dvx = speed * ((f32)sign * (float_2_8041cb84 * spd_tbl[angle % 8].x));
        data->dvy = speed * (float_2_8041cb84 * spd_tbl[angle % 8].y);
        data->dvz = speed * (float_2_8041cb84 * spd_tbl[angle % 8].z);
        data->vx = speed * ((f32)sign * spd_tbl[angle % 8].x);
        data->vy = speed * spd_tbl[angle % 8].y;
        data->vz = speed * spd_tbl[angle % 8].z;
        // Incremented here rather than in the for header: this is where the
        // original's scheduler emits the addi (before the scale lookup).
        angle++;
        data->scale = scale * scale_tbl[i % 8].x;
        data->angle = float_0_8041cb6c;
        data->angleVel = (f32)(sign * 107);
        data->field_0x34 = -camGetPtr(CAMERA_3D)->viewYaw;
        data->field_0x24 = 14;
        data->field_0x28 = lifetime;
        data->field_0x3C = 10;
        data->field_0x40 = float_255_8041cb88;
    }

    return entry;
}

void effMissStarMain(EffectEntry* effect) {
    MissStarData* data;
    Vec position;
    Vec disp;
    f32 angleVelScale;
    s32 i;

    data = effect->userdata;

    disp = vec3_802f3678;
    disp.x = data->position.x;
    disp.y = data->position.y;
    disp.z = data->position.z;
    position = disp;
    angleVelScale = float_0p8_8041cb74;

    for (i = 0; i < effect->count; i++, data++) {
        if (data->field_0x28 != 0) {
            data->field_0x28--;
            if (data->field_0x28 == 0) {
                break;
            }
        }

        if (data->field_0x24 >= 0) {
            data->position.x += data->dvx;
            data->position.y += data->dvy;
            data->position.z += data->dvz;
        }

        data->field_0x34 = -camGetPtr(CAMERA_3D)->viewYaw;
        data->angle = data->angle + data->angleVel;
        data->angle = reviseAngle(data->angle);
        data->angleVel *= angleVelScale;

        if (data->field_0x24 < 10) {
            data->vx *= float_0p5_8041cb78;
            data->vy *= float_0p5_8041cb78;
            data->vz *= float_0p5_8041cb78;
            data->dvx = data->vx;
            data->dvy = data->vy;
            data->dvz = data->vz;
        }

        data->field_0x24--;
        if (data->field_0x24 < 0) {
            data->field_0x3C--;
            if (data->field_0x3C < 0) {
                effDelete(effect);
                return;
            }
        }
    }

    dispEntry(CAMERA_3D, dispCalcZ(&position), 1, effMissStarDisp, effect);
}
