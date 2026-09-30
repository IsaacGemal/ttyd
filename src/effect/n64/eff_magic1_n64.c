#include "eff/n64/eff_magic1_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "mario/mariost.h"
#include <dolphin/mtx.h>

// Local copy of the effect's per-instance data; the fields used by
// effMagic1Disp, effMagic1Main and effMagic1N64Entry are named.
typedef struct effMagic1Data {
    s32 type;         // 0x00
    f32 x;            // 0x04
    f32 y;            // 0x08
    f32 z;            // 0x0C
    f32 field_0x10;   // 0x10
    f32 field_0x14;   // 0x14
    f32 field_0x18;   // 0x18
    f32 field_0x1c;   // 0x1C
    f32 field_0x20;   // 0x20
    f32 field_0x24;   // 0x24
    f32 field_0x28;   // 0x28
    s32 field_0x2c;   // 0x2C
    s32 field_0x30;   // 0x30
    s32 field_0x34;   // 0x34
} effMagic1Data;

extern const char str_802FAB08[];
extern const f32 float_0_8041f654;
extern const f32 float_1_8041f68c;
extern const f32 float_29_8041f6a4;
extern const f32 float_6p2_8041f6a8;
const f64 double_to_int_802fab00 = 4503601774854144.0;

EffectEntry* effMagic1N64Entry(s32 type, s32 count, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1) {
    EffectEntry* effect;
    effMagic1Data* data;
    f32 scale;

    effect = effEntry();
    effect->type = str_802FAB08;
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, 0x38);
    effect->userdata = data;
    effect->callback = effMagic1Main;
    effect->flags |= 2;
    data->field_0x2c = 0;
    data->type = type;
    data->field_0x34 = 0xff;
    data->x = x0;
    data->y = y0;
    data->z = z0;
    data->field_0x28 = float_1_8041f68c;
    switch (type) {
    case 0:
        scale = float_1_8041f68c / (f32)count;
        data->field_0x10 = float_0_8041f654;
        data->field_0x14 = float_0_8041f654;
        data->field_0x18 = float_0_8041f654;
        data->field_0x1c = (x1 - x0) * scale;
        data->field_0x20 = (y1 - y0) * scale;
        data->field_0x24 = (z1 - z0) * scale;
        data->field_0x30 = count;
        break;
    default:
        data->field_0x10 = float_29_8041f6a4;
        data->field_0x14 = float_0_8041f654;
        data->field_0x18 = float_0_8041f654;
        data->field_0x1c = float_6p2_8041f6a8;
        data->field_0x20 = float_0_8041f654;
        data->field_0x24 = float_0_8041f654;
        data->field_0x30 = 0x18;
        break;
    }
    return effect;
}

void effMagic1Main(EffectEntry* effect) {
    effMagic1Data* data = effect->userdata;
#ifdef MUST_MATCH
    /* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
       local before dispCalcZ; a single local folds the copy away (same idiom as
       effAkariSflushMain/effGlassMain). */
    Vec position;
#endif
    Vec disp = { 0 };
    f32 order;
    s32 type;

    disp.x = data->field_0x10;
    disp.y = data->field_0x14;
    disp.z = data->field_0x18;
#ifdef MUST_MATCH
    position = disp;
#endif

    if ((u32)marioStGetSystemLevel() != 0) {
#ifdef MUST_MATCH
        order = dispCalcZ(&position);
#else
        order = dispCalcZ(&disp);
#endif
        dispEntry(CAMERA_3D, order, 2, (DispCallback)effMagic1Disp, effect);
    } else {
        if (effect->flags & 4) {
            effect->flags &= ~4;
            data->field_0x30 = 0;
        }
        data->field_0x30--;
        data->field_0x2c++;
        if (data->field_0x30 < 0) {
            effDelete(effect);
            return;
        }
        type = data->type;
        if (type == 0 && data->field_0x30 == 0) {
            EffectEntry* newEffect = effMagic1N64Entry(1, 0x30, data->x + data->field_0x10,
                data->y + data->field_0x14, data->z + data->field_0x18, 0.0f, 0.0f, 0.0f);
            ((effMagic1Data*)newEffect->userdata)->field_0x28 = data->field_0x28;
        }
        if (type == 1) {
            data->field_0x1c *= float_0p83_8041f69c;
            data->field_0x34 = (s32)((f32)data->field_0x34 * float_0p9_8041f6a0);
        }
        data->field_0x10 += data->field_0x1c;
        data->field_0x14 += data->field_0x20;
        data->field_0x18 += data->field_0x24;
#ifdef MUST_MATCH
        order = dispCalcZ(&position);
#else
        order = dispCalcZ(&disp);
#endif
        dispEntry(CAMERA_3D, order, 2, (DispCallback)effMagic1Disp, effect);
    }
}

static void main_dl(EffectEntry* effect, Mtx mtx);

void effMagic1Disp(CameraId camId, EffectEntry* effect) {
    effMagic1Data* data;
    CameraEntry* camera;
    s32 type;
    s32 i;
    Mtx mtxA;
    Mtx mtxB;
    Mtx mtxC;
    Mtx mtxD;
    f32 angle;

    camera = camGetPtr(camId);
    data = effect->userdata;
    type = data->type;
    switch (type) {
    case 0:
        angle = 0.0f;
        break;
    default:
        angle = -camGetPtr(CAMERA_3D)->viewYaw;
        break;
    }
    PSMTXTrans(mtxA, data->x, data->y, data->z);
    PSMTXRotRad(mtxB, 'y', 0.017453292f * angle);
    PSMTXConcat(mtxA, mtxB, mtxC);
    PSMTXConcat(camera->view, mtxC, mtxC);
    switch (type) {
    case 0:
        main_dl(effect, mtxC);
        break;
    case 1:
        for (i = 0; i < 12; i++) {
            PSMTXRotRad(mtxD, 'z', 0.017453292f * (f32)(i * 30));
            PSMTXConcat(mtxC, mtxD, mtxD);
            main_dl(effect, mtxD);
        }
        break;
    }
}
