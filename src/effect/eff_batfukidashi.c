#include "eff/eff_batfukidashi.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "manager/fontmgr.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdio.h>


const Vec vec3_802fd498 = { 0.0f, 0.0f, 0.0f };


EffectEntry* effCoinFukidashiEntry(s32 type, f32 x, f32 y, f32 z, s32 coinNum, s32 life) {
    EffectEntry* entry;
    CoinFukidashiData* data;

    entry = effEntry();
    entry->type = "CoinFukidashi";
    entry->count = 1;
    data = __memAlloc(HEAP_EFFECT, entry->count * sizeof(CoinFukidashiData));
    entry->userdata = data;
    entry->callback = effCoinFukidashiMain;
    entry->flags |= 2;

    data->type = type;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->offsetY = 0.0f;
    data->scale = 1.0f;
    data->coinNum = coinNum;

    if (life <= 0) {
        data->life = 1000;
    } else {
        data->life = life;
    }
    data->offsetIdx = 0;
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
    Vec position;

    data = effect->userdata;
    position = (Vec){ data->position.x, data->position.y, data->position.z };

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->life = 16;
    }

    if (data->life < 1000) {
        data->life--;
    }

    if (data->life < 16) {
        data->alpha = data->life * 16;
    } else {
        data->alpha += 16;
        if (data->alpha > 255) {
            data->alpha = 255;
        }
    }

    if (data->life < 0) {
        effDelete(effect);
        return;
    }

    data->offsetY = (f32)offset_tbl[data->offsetIdx] / 100.f;
    data->offsetIdx++;
    if ((u32)data->offsetIdx >= 61) {
        data->offsetIdx = 0;
    }

    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effCoinFukidashiDisp, effect);
}

// effCoinFukidashiDisp (1140 B, matched).
void effCoinFukidashiDisp(CameraId cameraId, void* param) {
    EffectEntry* effect = param;
    CameraEntry* cam;
    CoinFukidashiData* data;
    GXTexObj tex;
    Mtx m0, m1, m2;
    f32 w, h;
    // 0.5f is held in a named local for the vertex and text maths; the literal directly would swap f0/f1.
    f32 half = 0.5f;
    f32 textW;
    s32 alpha;
    char buf[0x40];

    cam = camGetPtr(cameraId);
    data = effect->userdata;
    alpha = data->alpha;
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    effGetTexObj(0x25, &tex);
    GXLoadTexObj(&tex, GX_TEXMAP0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_CLAMP, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
    GXSetChanMatColor(GX_COLOR0A0, (GXColor){ 0xFF, 0xFF, 0xFF, alpha });

    PSMTXTrans(m0, data->position.x, data->position.y + data->offsetY, data->position.z);
    PSMTXScale(m1, data->scale, data->scale, data->scale);
    PSMTXRotRad(m2, 'y', -camGetPtr(cameraId)->viewYaw * 0.017453292f);
    PSMTXConcat(m0, m1, m0);
    PSMTXConcat(m0, m2, m0);
    PSMTXConcat(cam->view, m0, m0);
    GXLoadPosMtxImm(m0, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    w = 0.4f * (f32)GXGetTexObjWidth(&tex);
    h = 0.4f * (f32)GXGetTexObjHeight(&tex);
    GXPosition3f32(-w * half, h, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(w * half, h, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(w * half, 0.0f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(-w * half, 0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);

    sprintf(buf, "%d", data->coinNum);
    textW = data->scale * (f32)FontGetMessageWidth(buf) / 3.0f;
    PSMTXTrans(m0, -textW * half, -28.0f, 0.0f);
    PSMTXScale(m1, data->scale / 3.0f, data->scale / 3.0f, data->scale / 3.0f);
    PSMTXConcat(m0, m1, m1);
    PSMTXTrans(m0, textW * half, 28.0f, 0.0f);
    PSMTXConcat(m1, m0, m1);
    PSMTXTrans(m0, data->position.x, data->position.y, data->position.z);
    PSMTXRotRad(m2, 'y', -camGetPtr(cameraId)->viewYaw * 0.017453292f);
    PSMTXConcat(m0, m2, m2);
    PSMTXTrans(m0, 3.0f, 34.0f + data->offsetY, 0.0f);
    PSMTXConcat(m2, m0, m0);
    PSMTXConcat(m0, m1, m0);
    FontDrawStart_alpha(alpha);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
    FontDrawStringMtx(m0, buf);
}
