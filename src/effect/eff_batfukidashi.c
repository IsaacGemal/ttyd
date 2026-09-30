#include "eff/eff_batfukidashi.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "manager/fontmgr.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdio.h>

extern const f32 float_0_80420478;
extern const f32 float_1_8042047c;
extern const f32 float_100_80420494;

const u32 dat_80420468 = 0xFFFFFF00;
const f32 float_deg2rad_8042046c = 0.017453292f;
const f32 float_0p4_80420470 = 0.4f;
const f32 float_0p5_80420474 = 0.5f;
const f32 float_0_80420478 = 0.0f;
const f32 float_1_8042047c = 1.0f;
const char str_80420480[] = "%d";
const f32 float_3_80420484 = 3.0f;
const f32 float_neg28_80420488 = -28.0f;
const f32 float_28_8042048c = 28.0f;
const f32 float_34_80420490 = 34.0f;
const f32 float_100_80420494 = 100.0f;

const Vec vec3_802fd498 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_mask_802fd4a8 = 4503599627370496.0;

const char str_802FD4B0[] = "CoinFukidashi";

EffectEntry* effCoinFukidashiEntry(s32 type, s32 coinNum, s32 count, f32 x, f32 y, f32 z) {
    EffectEntry* entry;
    CoinFukidashiData* data;

    entry = effEntry();
    entry->type = str_802FD4B0;
    entry->count = 1;
    data = __memAlloc(HEAP_EFFECT, entry->count * sizeof(CoinFukidashiData));
    entry->userdata = data;
    entry->callback = effCoinFukidashiMain;
    entry->flags |= 2;

    data->type = type;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->field_0x10 = float_0_80420478;
    data->scale = float_1_8042047c;
    data->field_0x14 = coinNum;

    if (count <= 0) {
        data->field_0x20 = 1000;
    } else {
        data->field_0x20 = count;
    }
    data->field_0x24 = 0;
    data->alpha = 0;

    return entry;
}

// .data table driving the coin's vertical rise (asm: .obj offset_tbl, local)
static u8 offset_tbl[] = {
    0x28, 0x23, 0x1E, 0x19, 0x14, 0x0F, 0x0A, 0x06,
    0x02, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x04,
    0x06, 0x08, 0x0A, 0x0C, 0x0F, 0x11, 0x14, 0x16,
    0x19, 0x1C, 0x1E, 0x21, 0x23, 0x25, 0x28, 0x28,
    0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28,
    0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28,
    0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28,
    0x28, 0x28, 0x28, 0x28, 0x28, 0x00, 0x00, 0x00,
};

void effCoinFukidashiMain(EffectEntry* effect) {
    CoinFukidashiData* data;
#ifdef MUST_MATCH
    /* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local before
       dispCalcZ; a single local folds the copy away. */
    Vec position;
#endif
    Vec disp = { 0.0f, 0.0f, 0.0f };

    data = effect->userdata;
    disp.x = data->position.x;
    disp.y = data->position.y;
    disp.z = data->position.z;
#ifdef MUST_MATCH
    position = disp;
#endif

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->field_0x20 = 16;
    }

    if (data->field_0x20 < 1000) {
        data->field_0x20--;
    }

    if (data->field_0x20 < 16) {
        data->alpha = data->field_0x20 * 16;
    } else {
        data->alpha += 16;
        if (data->alpha > 255) {
            data->alpha = 255;
        }
    }

    if (data->field_0x20 < 0) {
        effDelete(effect);
        return;
    }

    data->field_0x10 = (f32)offset_tbl[data->field_0x24] / float_100_80420494;
    data->field_0x24++;
    if ((u32)data->field_0x24 >= 61) {
        data->field_0x24 = 0;
    }

#ifdef MUST_MATCH
    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effCoinFukidashiDisp, effect);
#else
    dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effCoinFukidashiDisp, effect);
#endif
}

#ifdef MUST_MATCH
// MUST_MATCH: building the GXColor through an inline return puts the colour temp in the slot retail uses.
static inline GXColor effCoinFukidashiColor(u8 alpha) {
    GXColor color = *(GXColor*)&dat_80420468;
    color.a = alpha;
    return color;
}
#endif

// effCoinFukidashiDisp (1140 B, matched).
void effCoinFukidashiDisp(CameraId cameraId, void* param) {
    EffectEntry* effect = param;
    CameraEntry* cam;
    CoinFukidashiData* data;
    GXTexObj tex;
    GXColor color;
    Mtx m0, m1, m2;
    f32 w, h;
#ifdef MUST_MATCH
    // MUST_MATCH: a named 0.5f local reproduces retail's FP register allocation of the vertex
    // and text maths; using the literal directly swaps f0/f1 in eight instructions.
    f32 half = float_0p5_80420474;
#endif
    f32 textW;
    s32 alpha;
    char buf[0x40];

    cam = camGetPtr(cameraId);
    data = effect->userdata;
    alpha = data->alpha;
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x3c, 0, 0x7d);
    effGetTexObj(0x25, &tex);
    GXLoadTexObj(&tex, 0);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevOp(0, 0);
#ifdef MUST_MATCH
    color = effCoinFukidashiColor(alpha);
#else
    color = *(GXColor*)&dat_80420468;
    color.a = alpha;
#endif
    GXSetChanMatColor(4, color);

    PSMTXTrans(m0, data->position.x, data->position.y + data->field_0x10, data->position.z);
    PSMTXScale(m1, data->scale, data->scale, data->scale);
    PSMTXRotRad(m2, 'y', -camGetPtr(cameraId)->viewYaw * float_deg2rad_8042046c);
    PSMTXConcat(m0, m1, m0);
    PSMTXConcat(m0, m2, m0);
    PSMTXConcat(cam->view, m0, m0);
    GXLoadPosMtxImm(m0, 0);
    GXSetCurrentMtx(0);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xd, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xd, 1, 4, 0);

    GXBegin(0x80, 0, 4);
    w = float_0p4_80420470 * (f32)GXGetTexObjWidth(&tex);
    h = float_0p4_80420470 * (f32)GXGetTexObjHeight(&tex);
#ifdef MUST_MATCH
    GXPosition3f32(-w * half, h, float_0_80420478);
#else
    GXPosition3f32(-w * float_0p5_80420474, h, float_0_80420478);
#endif
    GXTexCoord2f32(float_0_80420478, float_0_80420478);
#ifdef MUST_MATCH
    GXPosition3f32(w * half, h, float_0_80420478);
#else
    GXPosition3f32(w * float_0p5_80420474, h, float_0_80420478);
#endif
    GXTexCoord2f32(float_1_8042047c, float_0_80420478);
#ifdef MUST_MATCH
    GXPosition3f32(w * half, float_0_80420478, float_0_80420478);
#else
    GXPosition3f32(w * float_0p5_80420474, float_0_80420478, float_0_80420478);
#endif
    GXTexCoord2f32(float_1_8042047c, float_1_8042047c);
#ifdef MUST_MATCH
    GXPosition3f32(-w * half, float_0_80420478, float_0_80420478);
#else
    GXPosition3f32(-w * float_0p5_80420474, float_0_80420478, float_0_80420478);
#endif
    GXTexCoord2f32(float_0_80420478, float_1_8042047c);

    sprintf(buf, str_80420480, data->field_0x14);
    textW = data->scale * (f32)FontGetMessageWidth(buf) / float_3_80420484;
#ifdef MUST_MATCH
    PSMTXTrans(m0, -textW * half, float_neg28_80420488, float_0_80420478);
#else
    PSMTXTrans(m0, -textW * float_0p5_80420474, float_neg28_80420488, float_0_80420478);
#endif
    PSMTXScale(m1, data->scale / float_3_80420484, data->scale / float_3_80420484, data->scale / float_3_80420484);
    PSMTXConcat(m0, m1, m1);
#ifdef MUST_MATCH
    PSMTXTrans(m0, textW * half, float_28_8042048c, float_0_80420478);
#else
    PSMTXTrans(m0, textW * float_0p5_80420474, float_28_8042048c, float_0_80420478);
#endif
    PSMTXConcat(m1, m0, m1);
    PSMTXTrans(m0, data->position.x, data->position.y, data->position.z);
    PSMTXRotRad(m2, 'y', -camGetPtr(cameraId)->viewYaw * float_deg2rad_8042046c);
    PSMTXConcat(m0, m2, m2);
    PSMTXTrans(m0, float_3_80420484, float_34_80420490 + data->field_0x10, float_0_80420478);
    PSMTXConcat(m2, m0, m0);
    PSMTXConcat(m0, m1, m0);
    FontDrawStart_alpha(alpha);
    GXSetZMode(1, 3, 0);
    FontDrawStringMtx(m0, buf);
}
