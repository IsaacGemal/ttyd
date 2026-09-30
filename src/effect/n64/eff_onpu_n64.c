#include "eff/n64/eff_onpu_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>

// .sdata2 float constants (named after their original addresses).
extern const f32 float_0p0625_8041f790;
extern const f32 float_0p0044643_8041f794;
extern const f32 float_0_8041f798;
extern const f32 float_deg2rad_8041f79c;
extern const f32 float_0p3_8041f7a0;
extern const f32 float_1_8041f7a4;
extern const f32 float_0p04_8041f7a8;
extern const f32 float_neg5_8041f7ac;
extern const f32 float_0p1_8041f7b0;
extern const f32 float_0p5_8041f7b4;
extern const f32 float_5_8041f7b8;

// .rodata
extern const char str_802FB218[];

// .data: one effVtxArray (0x38 bytes) for the textured quad used by effOnpuDisp.
extern u32 size16x32_tex16x32_vtx[];

// .sdata: byte-indexed color rotation table (36 bytes).
static u8 color_rotation_data[36] = {
    0xFE, 0xAC, 0xAC, 0xFE, 0xAC, 0xD5, 0xFE, 0xB4, 0x9A, 0xD5, 0xB4, 0xFE,
    0xB4, 0xB4, 0xFE, 0xB4, 0xDD, 0xFE, 0xB4, 0xFE, 0xFE, 0xB4, 0xFE, 0xD5,
    0xB4, 0xFE, 0xB4, 0xD5, 0xFE, 0xB4, 0xFE, 0xFE, 0xB4, 0xFE, 0xD5, 0xAC,
};

// .sbss2: shared scratch GXColor, only its bytes are overwritten (see eff_small_star_n64.c).
extern GXColor unk_804231c0;

// Pooled int->double conversion constant for the (f32)(rand() % 11) casts. Non-const so the compiler
// references it by name instead of folding it to an anonymous @NNN literal (as in eff_akari_pose_n64.c).
f64 double_to_int_802fb210 = 4503601774854144.0;

// Local copy of system.c's inline helper so it inlines here (as in eff_rankup.c).
inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

// Building the TEV colour in an inlined helper allocates it at r1+0x8 (the original's slot);
// building it directly in effOnpuDisp puts it at r1+0xc (the known GXColor temp/pass swap).
static inline void onpuSetColor(s32 offset, s32 alpha) {
    GXColor color = unk_804231c0;

    color.r = color_rotation_data[offset];
    color.g = color_rotation_data[offset + 1];
    color.b = color_rotation_data[offset + 2];
    color.a = alpha;
    GXSetTevColor(GX_TEVREG0, color);
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

void effOnpuDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    CameraEntry* cam;
    effOnpuData* data;
    Mtx mtx;
    Mtx rotMtx;
    Mtx scaleMtx;
    GXTexObj texObj;
    s32 randIndex;
    s32 offset;

    cam = camGetPtr(camId);
    data = effect->userdata;
    randIndex = data->randIndex;
    // Split keeps the multiply result in r29 (freeing r28 for randIndex), as the original schedules it.
    offset = data->counter * 3;
    offset %= 36;

    GXSetNumChans(0);
    GXSetNumTexGens(2);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);

    PSMTXScale(scaleMtx, float_0p0625_8041f790, float_0p0044643_8041f794, float_0_8041f798);
    PSMTXTrans(mtx, float_0_8041f798, (f32)(randIndex * 32), float_0_8041f798);
    PSMTXConcat(scaleMtx, mtx, scaleMtx);
    GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);

    PSMTXScale(scaleMtx, float_0p0625_8041f790, float_0p0044643_8041f794, float_0_8041f798);
    PSMTXTrans(mtx, float_0_8041f798, (f32)(randIndex * 32), float_0_8041f798);
    PSMTXConcat(scaleMtx, mtx, scaleMtx);
    GXLoadTexMtxImm(scaleMtx, GX_TEXMTX1, GX_MTX2x4);

    GXSetNumTevStages(2);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);

    onpuSetColor(offset, data->alpha);

    effGetTexObjN64(0x28, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    effGetTexObjN64(0x29, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP1);

    PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
    PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041f79c * -camGetPtr(CAMERA_3D)->viewYaw);
    PSMTXConcat(mtx, rotMtx, mtx);
    PSMTXScale(scaleMtx, data->scale, data->scale, float_0_8041f798);
    PSMTXConcat(mtx, scaleMtx, mtx);
    PSMTXConcat(cam->view, mtx, mtx);
    GXLoadPosMtxImm(mtx, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GXSetCullMode(GX_CULL_NONE);
    effSetVtxDescN64((effVtxArray*)size16x32_tex16x32_vtx);
    GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
    tri2(0, 1, 2, 0, 0, 2, 3, 0);
}
