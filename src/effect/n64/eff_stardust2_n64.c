#include "eff/n64/eff_stardust2_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sdata2 constants (named after their original addresses; defined by the
// not-yet-written Entry function of this unit).
extern const GXColor dat_8041fe88;
extern const f32 float_deg2rad_8041fe8c;
extern const f32 float_0p0056818_8041fe90;
extern const f32 float_0p045455_8041fe94;
extern const f32 float_0_8041fe98;
extern const f32 float_1_8041fe9c;
extern const f32 float_0p5_8041fea0;
extern const f32 float_0p1_8041fea4;
extern const f32 float_0p8_8041fea8;
extern const f32 float_0p7_8041feac;
extern const f32 float_1p4_8041feb0;

// .rodata constant (named after its original address).
const f64 double_to_int_mask_802fb750 = 4503599627370496.0;

// .sdata colour rotation table, local to this file.
static u8 color_rotation_data[36] = {
    0xFE, 0xAC, 0xAC, 0xFE, 0xAC, 0xD5, 0xFE, 0xB4, 0x9A, 0xD5, 0xB4, 0xFE,
    0xB4, 0xB4, 0xFE, 0xB4, 0xDD, 0xFE, 0xB4, 0xFE, 0xFE, 0xB4, 0xFE, 0xD5,
    0xB4, 0xFE, 0xB4, 0xD5, 0xFE, 0xB4, 0xFE, 0xFE, 0xB4, 0xFE, 0xD5, 0xAC,
};

// .data vertex strips, local to this file.
static u32 size8x8_tex22x22_vtx[0x70] = {
    0xFFF8FFF8, 0x00000000, 0x00000000, 0x00FF0008, 0xFFF80000, 0x02C00000,
    0x000000FF, 0x00080008, 0x000002C0, 0x02C00000, 0x00FFFFF8, 0x00080000,
    0x000002C0, 0x000000FF, 0xFFF8FFF8, 0x000002C0, 0x00000000, 0x00FF0008,
    0xFFF80000, 0x05800000, 0x000000FF, 0x00080008, 0x00000580, 0x02C00000,
    0x00FFFFF8, 0x00080000, 0x02C002C0, 0x000000FF, 0xFFF8FFF8, 0x00000580,
    0x00000000, 0x00FF0008, 0xFFF80000, 0x08400000, 0x000000FF, 0x00080008,
    0x00000840, 0x02C00000, 0x00FFFFF8, 0x00080000, 0x058002C0, 0x000000FF,
    0xFFF8FFF8, 0x00000840, 0x00000000, 0x00FF0008, 0xFFF80000, 0x0B000000,
    0x000000FF, 0x00080008, 0x00000B00, 0x02C00000, 0x00FFFFF8, 0x00080000,
    0x084002C0, 0x000000FF, 0xFFF8FFF8, 0x00000B00, 0x00000000, 0x00FF0008,
    0xFFF80000, 0x0DC00000, 0x000000FF, 0x00080008, 0x00000DC0, 0x02C00000,
    0x00FFFFF8, 0x00080000, 0x0B0002C0, 0x000000FF, 0xFFF8FFF8, 0x00000DC0,
    0x00000000, 0x00FF0008, 0xFFF80000, 0x10800000, 0x000000FF, 0x00080008,
    0x00001080, 0x02C00000, 0x00FFFFF8, 0x00080000, 0x0DC002C0, 0x000000FF,
    0xFFF8FFF8, 0x00001080, 0x00000000, 0x00FF0008, 0xFFF80000, 0x13400000,
    0x000000FF, 0x00080008, 0x00001340, 0x02C00000, 0x00FFFFF8, 0x00080000,
    0x108002C0, 0x000000FF, 0xFFF8FFF8, 0x00001340, 0x00000000, 0x00FF0008,
    0xFFF80000, 0x16000000, 0x000000FF, 0x00080008, 0x00001600, 0x02C00000,
    0x00FFFFF8, 0x00080000, 0x134002C0, 0x000000FF,
};

// One Stardust2N64 particle; the entry at index 0 also carries the shared
// colour/rotation state that effStardust2Disp caches before the loop.
typedef struct EffStardust2Data {
    u16 flags;      // 0x00
    s16 type;       // 0x02
    f32 x;          // 0x04
    f32 y;          // 0x08
    f32 z;          // 0x0C
    f32 field_10;   // 0x10
    f32 field_14;   // 0x14
    f32 field_18;   // 0x18
    f32 scale;      // 0x1C
    u32 field_20;   // 0x20
    s32 field_24;   // 0x24
    s32 field_28;   // 0x28
    s32 drawCam;    // 0x2C
} EffStardust2Data;

#ifdef MUST_MATCH
static inline GXColor stardustColor(u8 r, u8 g, u8 b) {
    GXColor c = dat_8041fe88;
    c.r = r;
    c.g = g;
    c.b = b;
    return c;
}
#endif

// NONMATCHING (99.97%): the unsigned-conversion magic and the render-type switch jump table are
// emitted as anonymous pool entries (@243 / @249) instead of the original's named symbols, and the
// by-value GXSetTevColor GXColor temp lands at r1+0x8/r1+0xC the opposite way round.
void effStardust2Disp(CameraId camId, void* param) {
    EffectEntry* entry = param;
    CameraEntry* cam;
    EffStardust2Data* data;
    s32 type;
    s32 i;
    s32 renderType;
    s32 colorIdx;
    effVtxArray* vtx;
    GXTexObj texObj;
    GXColor color;
    Mtx mtx;
    Mtx scale;
    Mtx rot;
    Mtx base;
    s32 colorRByte;
    s32 colorGByte;
    s32 colorBByte;
    f32 colorR;
    f32 colorG;
    f32 colorB;

    cam = camGetPtr(camId);
    data = entry->userdata;
    type = data->type;
    colorIdx = (data->field_20 - 1) * 3;
    PSMTXTrans(mtx, data->x, data->y, data->z);
    PSMTXRotRad(rot, 'y', -camGetPtr(CAMERA_3D)->viewYaw * float_deg2rad_8041fe8c);
    PSMTXConcat(mtx, rot, base);
    PSMTXConcat(cam->view, base, base);

    data++;
    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    PSMTXScale(scale, float_0p0056818_8041fe90, float_0p045455_8041fe94, float_0_8041fe98);
    GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
    effGetTexObjN64(0x20, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C0, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetCullMode(GX_CULL_NONE);

    switch (type) {
    case 0:
    case 1:
    case 2:
    case 10:
        colorR = float_1_8041fe9c;
        colorG = float_0p5_8041fea0;
        colorB = float_0p1_8041fea4;
        break;
    case 3:
    case 30:
        colorR = float_1_8041fe9c;
        colorG = float_0p8_8041fea8;
        colorB = float_0p7_8041feac;
        break;
    case 5:
        colorR = float_1p4_8041feb0;
        colorB = float_0p7_8041feac;
        colorG = colorR;
        break;
    default:
        colorG = float_0p7_8041feac;
        colorR = float_1p4_8041feb0;
        colorB = colorG;
        break;
    }

    for (i = 0; i < entry->count - 1; i++, data++) {
        renderType = data->field_28;
        if (renderType < 0) {
            continue;
        }
        PSMTXTrans(mtx, data->x, data->y, data->z);
        mtx[0][0] = mtx[1][1] = mtx[2][2] = data->scale;
        PSMTXConcat(base, mtx, mtx);
        GXLoadPosMtxImm(mtx, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);

        if (colorIdx >= 0x24) {
            colorIdx = 0;
        }
        colorRByte = (s32)(color_rotation_data[colorIdx++] * colorR);
        colorGByte = (s32)(color_rotation_data[colorIdx++] * colorG);
        colorBByte = (s32)(color_rotation_data[colorIdx++] * colorB);
        if (colorRByte > 0xFF) {
            colorRByte = 0xFF;
        }
        if (colorGByte > 0xFF) {
            colorGByte = 0xFF;
        }
        if (colorBByte > 0xFF) {
            colorBByte = 0xFF;
        }
#ifdef MUST_MATCH
        color = stardustColor(colorRByte, colorGByte, colorBByte);
#else
        color = dat_8041fe88;
        color.r = colorRByte;
        color.g = colorGByte;
        color.b = colorBByte;
#endif
        GXSetTevColor(GX_TEVREG0, color);

        switch (renderType & 7) {
        case 0:
            vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0xA8);
            break;
        case 1:
            vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0xE0);
            break;
        case 2:
            vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x118);
            break;
        case 3:
            vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x150);
            break;
        case 4:
            vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x188);
            break;
        case 5:
            vtx = (effVtxArray*)size8x8_tex22x22_vtx;
            break;
        case 6:
            vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x38);
            break;
        case 7:
            vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x70);
            break;
        }
        effSetVtxDescN64(vtx);
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}

void effStardust2N64SetDrawCam(EffectEntry* entry, s32 cam) {
    *(s32*)((u8*)entry->userdata + 0x2C) = cam;
}
