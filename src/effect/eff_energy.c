#include "eff/eff_energy.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"

typedef struct EnergyData {
    s32 type;       // 0x00
    f32 x;          // 0x04
    f32 y;          // 0x08
    f32 z;          // 0x0C
    f32 scale;      // 0x10
    f32 scaleAnim;  // 0x14
    f32 scaleSpeed; // 0x18
    f32 angle;      // 0x1C
    f32 angleSpeed; // 0x20
    s32 alpha;      // 0x24
    s32 lifetime;   // 0x28
    s32 alphaAnim;  // 0x2C
    s32 alphaSpeed; // 0x30
} EnergyData;

const char str_8030148C[] = "Energy";
extern const f32 float_0_80422470;

EffectEntry* effEnergyEntry(s32 a, s32 b, f32 p1, f32 p2, f32 p3, f32 p4, f32 p5, f32 p6) {
    EffectEntry* entry;
    EnergyData* data;

    entry = effEntry();
    entry->type = str_8030148C;
    entry->count = 1;
    data = __memAlloc(HEAP_EFFECT, entry->count * sizeof(EnergyData));
    entry->userdata = data;
    entry->callback = effEnergyMain;

    data->type = a;
    data->x = p1;
    data->y = p2;
    data->z = p3;
    data->angle = float_0_80422470;
    data->angleSpeed = p5;
    data->scale = p4;
    data->scaleAnim = float_0_80422470;
    data->scaleSpeed = p6;
    data->alpha = 0xFF;
    data->alphaAnim = 0;
    data->alphaSpeed = 0x20;
    if (b <= 0) {
        data->lifetime = 1000;
    } else {
        data->lifetime = b;
    }

    return entry;
}

void effEnergyMain(EffectEntry* effect) {
    EnergyData* data = effect->userdata;
#ifdef MUST_MATCH
    /* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
       local before dispCalcZ; a single local folds the copy away (same idiom as
       effLasMonMain and effGlassMain). The { 0 } literal also reproduces the
       original's scheduling of the zero-vector loads around the userdata load. */
    Vec position;
#endif
    Vec disp = { 0 };

    disp.x = data->x;
    disp.y = data->y;
    disp.z = data->z;
#ifdef MUST_MATCH
    position = disp;
#endif

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->lifetime = 16;
    }

    if (data->lifetime < 1000) {
        data->lifetime--;
    }

    if (data->lifetime < 16) {
        data->alpha = data->lifetime * 16;
    }

    if (data->lifetime < 0) {
        effDelete(effect);
        return;
    }

    data->angle += data->angleSpeed;
    data->scaleAnim += data->scaleSpeed;

    data->alphaAnim += data->alphaSpeed;
    if (data->alphaAnim > 0xFF) {
        data->alphaAnim = 0xFF;
        data->alphaSpeed = -0x20;
    }

    if (data->alphaAnim < 0) {
        data->alphaAnim = 0;
        data->alphaSpeed = 0x20;
    }

#ifdef MUST_MATCH
    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effEnergyDisp, effect);
#else
    dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effEnergyDisp, effect);
#endif
}

static GXColor color_tbl[10] = {
    { 0x38, 0xF2, 0x2C, 0xFF },
    { 0xEA, 0xFA, 0xB7, 0xFF },
    { 0x59, 0xB4, 0xFF, 0xFF },
    { 0xA2, 0xF8, 0xFD, 0xFF },
    { 0xFF, 0xA3, 0x26, 0xFF },
    { 0xFD, 0xF1, 0x89, 0xFF },
    { 0xFD, 0xFB, 0x7E, 0xFF },
    { 0xFF, 0xFF, 0xFF, 0xFF },
    { 0xF5, 0x5A, 0xC5, 0xFF },
    { 0xFD, 0xBC, 0xFB, 0xFF },
};


// Matched 100%: routing both by-value GXColor calls through the MUST_MATCH void helpers below
// makes MWCC build the colour in stack slot 0x10 and copy it to the argument temp at 0x14
// (retail's layout); the plain form put them the other way round (0x14/0x10).
#ifdef MUST_MATCH
static inline void effEnergySetMatColor(EnergyData* data) {
    GXColor color = unk_80423358;
    color.r = data->alphaAnim;
    color.g = data->alphaAnim;
    color.b = data->alphaAnim;
    color.a = data->alpha;
    GXSetChanMatColor(4, color);
}
static inline void effEnergySetTevColor(s32 id, GXColor c) {
    GXSetTevColor(id, c);
}
#endif
void effEnergyDisp(CameraId camId, void* param) {
#ifndef MUST_MATCH
    GXColor color;
#endif
    EffectEntry* effect = param;
    EnergyData* data = effect->userdata;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx mtx3;
    Mtx mtx2;
    Mtx mtx1;
    Mtx mtx0;
    s32 type;
    f32 scale;
    f32 a;
    f32 b;
    f32 c;

    cam = camGetPtr(camId);
    type = data->type;

    PSMTXTrans(mtx3, data->x, data->y, data->z);
    scale = data->scaleAnim * data->scale;
    PSMTXScale(mtx2, scale, scale, scale);
    PSMTXRotRad(mtx1, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
    PSMTXConcat(mtx3, mtx1, mtx3);
    PSMTXRotRad(mtx1, 'z', data->angle * 0.017453292f);
    PSMTXConcat(mtx3, mtx1, mtx3);
    PSMTXConcat(mtx3, mtx2, mtx0);
    PSMTXConcat(cam->view, mtx0, mtx0);

    GXLoadPosMtxImm(mtx0, 0);
    GXSetCurrentMtx(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
#ifdef MUST_MATCH
    effEnergySetMatColor(data);
#else
    color = unk_80423358;
    color.r = data->alphaAnim;
    color.g = data->alphaAnim;
    color.b = data->alphaAnim;
    color.a = data->alpha;
    GXSetChanMatColor(4, color);
#endif
#ifdef MUST_MATCH
    effEnergySetTevColor(1, color_tbl[type * 2]);
    effEnergySetTevColor(2, color_tbl[type * 2 + 1]);
#else
    GXSetTevColor(1, color_tbl[type * 2]);
    GXSetTevColor(2, color_tbl[type * 2 + 1]);
#endif
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 2, 4, 10, 15);
    GXSetTevAlphaIn(0, 7, 4, 5, 7);
    effGetTexObj(0x8D, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(13, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 13, 1, 4, 0);
    GXBegin(0x80, 0, 4);
    a = 8.0f;
    b = 4.0f;
    c = -a / 2.0f;
    GXPosition3f32(c, b, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(b, b, 0.0f);
    GXTexCoord2f32(2.0f, 0.0f);
    GXPosition3f32(b, c, 0.0f);
    GXTexCoord2f32(2.0f, 1.0f);
    GXPosition3f32(c, c, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
}
