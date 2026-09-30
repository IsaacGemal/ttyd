#include "eff/n64/eff_stamp_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <math.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .rodata
const Vec vec3_802fb6f0 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802fb700 = 4503601774854144.0;
const char str_802FB708[0xC] = "StampN64";

// .sdata2 shared float pool
extern const f32 float_deg2rad_8041fde0;
extern const f32 float_0p03125_8041fde4;
extern const f32 float_0p9_8041fdf0;
extern const f32 float_0p1_8041fdf4;
extern const f32 float_2p4_8041fdf8;
extern const f32 float_0p2_8041fdfc;
extern const f32 float_0p07_8041fe00;
extern const f32 float_100_8041fe04;
extern const f32 float_0p05_8041fe08;
extern const f32 float_7_8041fe0c;
extern const f32 float_0p6_8041fe10;
extern const f32 float_10_8041fe14;
extern const f32 float_6p2832_8041fe18;
extern const f32 float_90_8041fe1c;
extern const f32 float_360_8041fe20;
extern const f32 float_1_8041fdec;
extern const f32 float_0_8041fde8;

// .sbss2 GXColor scratch globals used to seed the TEV colours.
extern GXColor unk_80423270;
extern GXColor unk_80423274;

// .data
u32 size16x16_tex64x32_vtx[0x38 / 4] = {
    0xFFF80000, 0x00000000, 0x00000000, 0x00000008, 0x00000000, 0x08000000, 0x00000000,
    0x000A0010, 0x00000800, 0x04000000, 0x00FFFFF6, 0x00100000, 0x00000400, 0x000000FF,
};

typedef struct effStampData {
    u16 type;       // 0x0
    u8 pad_2[2];    // 0x2
    f32 x;          // 0x4
    f32 y;          // 0x8
    f32 z;          // 0xC
    f32 field_0x10; // 0x10
    f32 field_0x14; // 0x14
    f32 field_0x18; // 0x18
    f32 field_0x1C; // 0x1C
    f32 field_0x20; // 0x20
    s32 life;       // 0x24
    s32 counter;    // 0x28
    f32 angle;      // 0x2C
    f32 scale;      // 0x30
    s32 alpha;      // 0x34
    u8 field_0x38;  // 0x38
    u8 field_0x39;  // 0x39
    u8 field_0x3A;  // 0x3A
    u8 field_0x3B;  // 0x3B
    u8 field_0x3C;  // 0x3C
    u8 field_0x3D;  // 0x3D
    u8 pad_3E[2];   // 0x3E
} effStampData;

EffectEntry* effStampN64Entry(s32 type, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    effStampData* data;
    effStampData* p;
    f32 rad;
    f32 angle;
    s32 i;

    effect = effEntry();
    effect->type = str_802FB708;
    effect->count = 8;
    data = __memAlloc(HEAP_EFFECT, 0x200);
    effect->userdata = data;
    effect->callback = effStampMain;
    data->life = 30;
    data->counter = 0;
    data->type = type;
    data->x = x;
    data->y = y;
    data->z = z;
    data->alpha = 255;

    switch (type) {
    case 0:
        data->field_0x38 = data->field_0x3B = 20;
        data->field_0x39 = data->field_0x3C = 21;
        data->field_0x3A = data->field_0x3D = 242;
        break;
    case 1:
        data->field_0x38 = data->field_0x3B = 200;
        data->field_0x39 = data->field_0x3C = 21;
        data->field_0x3A = data->field_0x3D = 212;
        break;
    case 2:
        data->field_0x38 = data->field_0x3B = 255;
        data->field_0x39 = data->field_0x3C = 181;
        data->field_0x3A = data->field_0x3D = 218;
        break;
    }

    p = data + 1;
    for (i = 1; i < 8; i++, p++) {
        angle = (i - 1) * 360 / 7;
        rad = float_6p2832_8041fe18 * (float_90_8041fe1c + angle) / float_360_8041fe20;
        p->x = float_1_8041fdec * (f32)cos(rad);
        p->y = float_1_8041fdec * (f32)sin(rad);
        p->z = float_0_8041fde8;
        p->field_0x10 = float_0_8041fde8;
        p->field_0x14 = float_0_8041fde8;
        p->field_0x18 = float_0_8041fde8;
        p->field_0x1C = float_0p1_8041fdf4;
        p->field_0x20 = float_0p1_8041fdf4;
        p->scale = float_1_8041fdec;
        p->angle = angle;
    }
    return effect;
}

void effStampMain(EffectEntry* effect) {
    effStampData* data;
    Vec position2;
    Vec position;
    s32 i;
    s32 count;

    data = effect->userdata;
    position = vec3_802fb6f0;
    position.x = data->x;
    position.y = data->y;
    position.z = data->z;
    position2 = position;
    data->counter++;
    data->life--;
    if (data->life < 0) {
        effDelete(effect);
        return;
    }
    count = data->counter;
    if (count > 10) {
        data->alpha = (s32)((f32)data->alpha * float_0p9_8041fdf0);
    }
    i = 1;
    data++;
    for (; i < effect->count; i++, data++) {
        if (count > 10) {
            data->field_0x1C += float_0p1_8041fdf4 * (float_1_8041fdec - data->field_0x1C);
            data->field_0x20 += float_0p1_8041fdf4 * (float_2p4_8041fdf8 - data->field_0x20);
            data->scale += float_0p2_8041fdfc * (float_0p07_8041fe00 * (float_100_8041fe04 - data->scale));
        }
        if (count < 10) {
            data->field_0x1C += float_0p05_8041fe08 * (float_7_8041fe0c - data->field_0x1C);
            data->field_0x20 += float_0p1_8041fdf4 * (float_1_8041fdec - data->field_0x20);
            data->scale += float_0p6_8041fe10 * (float_0p05_8041fe08 * (float_10_8041fe14 - data->scale));
        }
        data->x = data->scale * (f32)cos(float_6p2832_8041fe18 * (float_90_8041fe1c + data->angle) / float_360_8041fe20);
        data->y = data->scale * (f32)sin(float_6p2832_8041fe18 * (float_90_8041fe1c + data->angle) / float_360_8041fe20);
    }
    dispEntry(4, dispCalcZ(&position2), 2, effStampDisp, effect);
}

// The original builds each TEV colour in a helper-style local whose stack slot sits below its
// outgoing copy; writing the same code inline makes mwcc put the two slots the other way round
// (see eff_ripple_n64.c).
#ifdef MUST_MATCH
static inline void effStampSetColor1(effStampData* data) {
    GXColor color = unk_80423270;
    color.r = data->field_0x38;
    color.g = data->field_0x39;
    color.b = data->field_0x3A;
    color.a = data->alpha;
    GXSetTevColor(1, color);
}

static inline void effStampSetColor2(effStampData* data) {
    GXColor color = unk_80423274;
    color.r = data->field_0x3B;
    color.g = data->field_0x3C;
    color.b = data->field_0x3D;
    GXSetTevColor(2, color);
}
#endif

void effStampDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effStampData* data;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx mtx;
    Mtx scaleMtx;
    Mtx rotMtx;
    Mtx viewMtx;
    f32 deg2rad;
    s32 i;

    cam = camGetPtr(camId);
    data = effect->userdata;

    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 2, 4, 0xb, 0xf);
    GXSetTevAlphaIn(0, 7, 1, 4, 7);

    PSMTXTrans(mtx, data->x, data->y, data->z);
    PSMTXRotRad(rotMtx, 'y', -camGetPtr(CAMERA_3D)->viewYaw * float_deg2rad_8041fde0);
    PSMTXConcat(mtx, rotMtx, mtx);
    PSMTXConcat(cam->view, mtx, viewMtx);

    {
#ifdef MUST_MATCH
        effStampSetColor1(data);
        effStampSetColor2(data);
#else
        GXColor color;
        GXColor color2;
        GXColor tmp;

        tmp = unk_80423270;
        color = tmp;
        color.r = data->field_0x38;
        color.g = data->field_0x39;
        color.b = data->field_0x3A;
        color.a = data->alpha;
        GXSetTevColor(1, color);
        tmp = unk_80423274;
        color2 = tmp;
        color2.r = data->field_0x3B;
        color2.g = data->field_0x3C;
        color2.b = data->field_0x3D;
        GXSetTevColor(2, color2);
#endif
    }

    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
    PSMTXScale(scaleMtx, float_0p03125_8041fde4, float_0p03125_8041fde4, float_0_8041fde8);
    GXLoadTexMtxImm(scaleMtx, 0x1e, 1);
    effGetTexObjN64(0x2b, &texObj);
    GXLoadTexObj(&texObj, 0);
    data++;
    GXSetCullMode(0);
    effSetVtxDescN64((effVtxArray*)size16x16_tex64x32_vtx);

    deg2rad = float_deg2rad_8041fde0;
    for (i = 1; i < effect->count; i++, data++) {
        PSMTXTrans(mtx, data->x, data->y, data->z);
        PSMTXRotRad(rotMtx, 'z', deg2rad * data->angle);
        PSMTXScale(scaleMtx, data->field_0x20, data->field_0x1C, float_1_8041fdec);
        PSMTXConcat(mtx, rotMtx, mtx);
        PSMTXConcat(mtx, scaleMtx, mtx);
        PSMTXConcat(viewMtx, mtx, mtx);
        GXLoadPosMtxImm(mtx, 0);
        GXSetCurrentMtx(0);
        GXBegin(0x90, 0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}
