#include "eff/eff_break.h"
#include "driver/dispdrv.h"
#include "eff/n64/eff_stardust_n64.h"
#include "mario/mariost.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/types.h>
#include "evt/evt_cmd.h"

typedef struct BreakEffect {
    s32 type;       // 0x00
    f32 x;          // 0x04
    f32 y;          // 0x08
    f32 z;          // 0x0C
    s32 timer;      // 0x10
    f32 texOffsetY; // 0x14
    s32 field_0x18; // 0x18
    s32 state;      // 0x1C
    f32 scale;      // 0x20
} BreakEffect;

static u32 tex_jp[6] = { 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2 };
static u32 tex_us[6] = { 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6 };
static u32* texid_tbl[2] = { tex_jp, tex_us };

extern const char str_Break[];
extern const f32 float_0_804210b0;
extern const f32 float_2_804210cc;
extern const f32 float_10_804210d0;
extern const f32 float_20_804210c0;
extern const Vec vec3_802fe610;
extern f32 ty_data[21];
extern f32 ty_data2[21];

// clang-format off
EffectEntry* effBreakEntry(s32 type, s32 maxTimer, f32 x, f32 y, f32 z) {
    EffectEntry* entry;
    BreakEffect* data;
    BreakEffect* elem;
    s32 count;
    s32 i;

    entry = effEntry();
#ifdef MUST_MATCH
    // The original emits a redundant language test that stores 7 on both paths.
    count = 7;
    if ((u32)gp->language != 0) {
        count = 7;
    }
#else
    count = 7;
#endif
    entry->type = str_Break;
    entry->count = count;
    data = __memAlloc(HEAP_EFFECT, count * sizeof(BreakEffect));
    entry->userdata = data;
    entry->callback = effBreakMain;
    data->type = type;
    data->x = x;
    data->y = y;
    data->z = z;
    data->timer = maxTimer;
    data->scale = float_2_804210cc;

    for (i = 1; i < entry->count; i++) {
        elem = &data[i];
        // Layout mirror of the original: (i-1)*0x24 + 0x12 - ((count-2)*0x24 + 0x18) / 2.
        elem->x = float_2_804210cc * (f32)((i - 1) * 0x24 + 0x12 - ((entry->count - 2) * 0x24 + 0x18) / 2);
        elem->y = float_0_804210b0;
        elem->z = float_0_804210b0;
        elem->texOffsetY = float_10_804210d0;
        elem->field_0x18 = 0;
        elem->timer = (i - 1) * 0x15;
        elem->state = 0;
    }
    return entry;
}


#pragma opt_propagation off // without it MWCC rematerialises the scale multiply and swaps f0/f1, losing the target's fmuls operand order
void effBreakMain(EffectEntry* effect) {
    BreakEffect* data = effect->userdata;
    Vec position;
    Vec disp;
    f32 scale;
    f32 t;
    s32 max;
    s32 i;
    s32 done;

    disp = vec3_802fe610;
    disp.x = data->x;
    disp.y = data->y;
    disp.z = data->z;
    position = disp;
    // The target loads scale into f1, then the constant into f0, and multiplies f0*f1. Keeping the field in a
    // temp and writing the constant first gives that allocation (the pragma above is required for it to hold).
    t = data->scale;
    scale = float_20_804210c0 * t;
    max = data->timer;

    done = 0;
    data = &data[1];
    for (i = 1; i < effect->count; i++, data++) {
        switch (data->state) {
        case 0:
            data->timer--;
            if (data->timer <= 0) {
                data->timer = 0;
                data->state++;
            }
            break;
        case 1:
            if ((u32)data->timer < 0x15) {
                data->texOffsetY = ty_data[data->timer];
                data->timer++;
                if (float_0_804210b0 == data->texOffsetY) {
                    effStardustN64SetDrawCam(
                        effStardustN64Entry(1, position.x + data->x, position.y + data->y, position.z + data->z, scale), 8);
                }
            } else {
                data->timer = (effect->count - 1 - i) * 0x15;
                data->state++;
            }
            break;
        case 2:
            data->timer--;
            if (data->timer <= 0) {
                data->timer = 0;
                data->state++;
            }
            break;
        case 3:
            data->timer++;
            if (data->timer > max) {
                data->timer = 0;
                data->state++;
            }
            break;
        case 4:
            if ((u32)data->timer < 0x15) {
                data->texOffsetY = ty_data2[data->timer];
                if ((i & 1) == 0) {
                    data->texOffsetY = -data->texOffsetY;
                }
                data->timer++;
            } else {
                data->state++;
            }
            break;
        case 5:
            done++;
            break;
        }
    }

    if (done >= effect->count - 1) {
        effDelete(effect);
    } else {
        dispEntry(8, dispCalcZ(&position), 2, effBreakDisp, effect);
    }
}


#pragma opt_propagation reset

// The default common-subexpression pass reorders the two independent loop-latch induction
// increments (i before the i-1 texid index); disabling it keeps the target's order.
#pragma opt_common_subs off // keeps the i-1 texid induction increment before the i increment in the loop latch
void effBreakDisp(CameraId cameraId, void* param) {
    EffectEntry* effect = param;
    BreakEffect* data = effect->userdata;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx mtxB8;
    Mtx mtx88;
    Mtx mtx58;
    Mtx mtx28;
    f32 scale;
    s32 i;

    cam = camGetPtr(cameraId);
    scale = data->scale;
    PSMTXTrans(mtxB8, data->x, data->y, data->z);
    PSMTXRotRad(mtx88, 'y', -camGetPtr(cameraId)->viewYaw * float_deg2rad_804210a8);
    PSMTXConcat(mtxB8, mtx88, mtxB8);
    PSMTXConcat(cam->view, mtxB8, mtx28);

    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xff);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevOp(0, 3);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xd, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xd, 1, 4, 0);

    data++;
    for (i = 1; i < effect->count; i++, data++) {
        effGetTexObj(texid_tbl[gp->language != 0][i - 1], &texObj);
        GXLoadTexObj(&texObj, 0);
#ifdef MUST_MATCH
        {
            // One volatile read of the zero word, reused for both arguments (the original loads it once and fmr's).
            f32 zero = *(volatile f32*)&float_0_804210b0;
            PSMTXTrans(mtxB8, zero, data->texOffsetY, zero);
        }
#else
        PSMTXTrans(mtxB8, 0.0f, data->texOffsetY, 0.0f);
#endif
        GXLoadTexMtxImm(mtxB8, 0x1e, 1);
        PSMTXTrans(mtxB8, data->x, data->y, data->z);
        PSMTXScale(mtx58, scale, scale, scale);
        PSMTXConcat(mtx28, mtxB8, mtxB8);
        PSMTXConcat(mtxB8, mtx58, mtxB8);
        GXLoadPosMtxImm(mtxB8, 0);
        GXSetCurrentMtx(0);
        GXBegin(0x80, 0, 4);
#ifdef MUST_MATCH
        // volatile reads stop MWCC from hoisting the zero constant into a callee-saved FPR; the original reloads it
        // from float_0_804210b0 before each store.
        GXWGFifo.f32 = -float_48_804210ac * float_0p5_804210b4;
        GXWGFifo.f32 = float_24_804210b8;
        GXWGFifo.f32 = *(volatile f32*)&float_0_804210b0;
        GXWGFifo.f32 = *(volatile f32*)&float_0_804210b0;
        GXWGFifo.f32 = *(volatile f32*)&float_0_804210b0;
        GXWGFifo.f32 = float_24_804210b8;
        GXWGFifo.f32 = float_24_804210b8;
        GXWGFifo.f32 = *(volatile f32*)&float_0_804210b0;
        GXWGFifo.f32 = float_1_804210bc;
        GXWGFifo.f32 = *(volatile f32*)&float_0_804210b0;
        GXWGFifo.f32 = float_24_804210b8;
        GXWGFifo.f32 = -float_48_804210ac * float_0p5_804210b4;
        GXWGFifo.f32 = *(volatile f32*)&float_0_804210b0;
        GXWGFifo.f32 = float_1_804210bc;
        GXWGFifo.f32 = float_1_804210bc;
        GXWGFifo.f32 = -float_48_804210ac * float_0p5_804210b4;
        GXWGFifo.f32 = -float_48_804210ac * float_0p5_804210b4;
        GXWGFifo.f32 = *(volatile f32*)&float_0_804210b0;
        GXWGFifo.f32 = *(volatile f32*)&float_0_804210b0;
        GXWGFifo.f32 = float_1_804210bc;
#else
        GXPosition3f32(-float_48_804210ac * float_0p5_804210b4, float_24_804210b8, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(float_24_804210b8, float_24_804210b8, 0.0f);
        GXTexCoord2f32(float_1_804210bc, 0.0f);
        GXPosition3f32(float_24_804210b8, -float_48_804210ac * float_0p5_804210b4, 0.0f);
        GXTexCoord2f32(float_1_804210bc, float_1_804210bc);
        GXPosition3f32(-float_48_804210ac * float_0p5_804210b4, -float_48_804210ac * float_0p5_804210b4, 0.0f);
        GXTexCoord2f32(0.0f, float_1_804210bc);
#endif
    }
}

f32 ty_data[21] = {
    3.0f, 3.0f, 2.7857144f, 2.5714285f,
    2.357143f, 2.142857f, 1.9285715f, 1.7142857f,
    1.5f, 1.2857143f, 1.0714285f, 0.85714287f,
    0.64285713f, 0.42857143f, 0.21428572f, 0.0f,
    0.083333336f, 0.16666667f, 0.25f, 0.125f,
    0.0f,
};

f32 ty_data2[21] = {
    0.0f, 0.15f, 0.3f, 0.45f,
    0.6f, 0.75f, 0.9f, 1.05f,
    1.2f, 1.35f, 1.5f, 1.65f,
    1.8f, 1.95f, 2.1f, 2.25f,
    2.4f, 2.55f, 2.7f, 2.85f,
    3.0f,
};
// clang-format on

#pragma opt_common_subs reset

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const Vec vec3_802fe610 = { 0.0f, 0.0f, 0.0f };
const f32 float_deg2rad_804210a8 = 0.017453292f;
const f32 float_48_804210ac = 48.0f;
const f32 float_0_804210b0 = 0.0f;
const f32 float_0p5_804210b4 = 0.5f;
const f32 float_24_804210b8 = 24.0f;
#ifdef MUST_MATCH
const volatile f32 float_1_804210bc = 1.0f;
#else
const f32 float_1_804210bc = 1.0f;
#endif
const f32 float_20_804210c0 = 20.0f;
const char str_Break[] = "Break";
const f32 float_2_804210cc = 2.0f;
const f32 float_10_804210d0 = 10.0f;

// 4-byte zero word that ends the unit's .sdata2: all-zero objects need explicit_zero_data to be emitted and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata2") const u32 gap_09_804210D4_sdata2 = 0;
#pragma force_active reset
#pragma explicit_zero_data reset
