#include "eff/n64/eff_stardust2_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "matching.h"

#include <stdlib.h>
#include <math.h>
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

// .rodata constants (in the original's order, named after their addresses).
const Vec vec3_802fb740 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_mask_802fb750 = 4503599627370496.0;
const f64 double_to_int_802fb758 = 4503601774854144.0;

// .sdata colour rotation table, local to this file.
static u8 color_rotation_data[36] = {
    0xFE, 0xAC, 0xAC, 0xFE, 0xAC, 0xD5, 0xFE, 0xB4, 0x9A, 0xD5, 0xB4, 0xFE,
    0xB4, 0xB4, 0xFE, 0xB4, 0xDD, 0xFE, 0xB4, 0xFE, 0xFE, 0xB4, 0xFE, 0xD5,
    0xB4, 0xFE, 0xB4, 0xD5, 0xFE, 0xB4, 0xFE, 0xFE, 0xB4, 0xFE, 0xD5, 0xAC,
};

// .sdata speed/scale tables, local to this file.
static u8 speed_data[16] = {
    0x1E, 0x3C, 0x5A, 0x64, 0x68, 0x6A, 0x6C, 0x6E,
    0x70, 0x71, 0x71, 0x6E, 0x6B, 0x67, 0x00, 0x00,
};

static u8 scale_data[16] = {
    0x1E, 0x3C, 0x5A, 0x64, 0x68, 0x6A, 0x6C, 0x6E,
    0x70, 0x71, 0x71, 0x6E, 0x6B, 0x67, 0x00, 0x00,
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

static inline Vec makeVec(f32 x, f32 y, f32 z) {
    Vec temp = vec3_802fb740;
    temp.x = x;
    temp.y = y;
    temp.z = z;
    return temp;
}

void effStardust2Main(EffectEntry* effect) {
    EffStardust2Data* data;
    Vec position2;
    Vec position;
    s32 i;
    s16 type;
    s32 active;
    s32 colorIndex;
    s32 drawCam;
    f32 rad;
    f32 one;

    data = effect->userdata;
    position = makeVec(data->x, data->y, data->z);
    position2 = position;
    drawCam = data->drawCam;
    data->field_24--;
    data->field_20++;
    colorIndex = data->field_20;
    type = data->type;
    active = 0;
    if ((u32)type <= 1 || type == 3 || type >= 10) {
        if (data->field_24 < 0) {
            effDelete(effect);
            return;
        }
    }
    i = 0;
    data++;
    for (; i < effect->count - 1; i++, data++) {
        switch (type) {
        case 0:
        case 1:
            data->field_28 = (colorIndex + i) & 7;
            data->field_18 += float_neg0p02_8041feb4;
            if (colorIndex - 1 < 14) {
                f32 val = float_0p01_8041feb8 * speed_data[colorIndex - 1];
                data->x = data->field_10 * val;
                data->y = data->field_14 * val + data->field_18;
                data->scale = float_0p01_8041feb8 * scale_data[colorIndex - 1];
            }
            else {
                f32 angle;
                f32 ratio;
                one = float_1_8041fe9c;
                ratio = float_360_8041fec0 / (f32)(effect->count - 1);
                angle = ratio * (f32)i + float_60_8041febc;
                if (i & 1) {
                    angle -= float_120_8041fec4;
                }
                rad = float_6p2832_8041fec8 * angle / MATCH_F32(float_360_8041fec0);
                data->type = 10;
                data->field_10 = one * (f32)sin(rad);
                data->field_14 = one * (f32)cos(rad);
                data->field_18 = float_neg0p05_8041fecc;
            }
            break;
        case 30:
        case 10:
            data->field_28 = (colorIndex + i) & 7;
            data->field_10 *= float_0p96_8041fed0;
            data->field_14 *= float_0p96_8041fed0;
            data->field_14 += data->field_18;
            data->x += data->field_10;
            data->y += data->field_14;
            {
                f32 k = float_0p1_8041fea4;
                data->scale = k * (k - data->scale) + data->scale;
            }
            break;
        case 3:
            data->field_28 = (colorIndex + i + i) & 7;
            if (colorIndex - 1 < 14) {
                data->scale = float_0p01_8041feb8 * scale_data[colorIndex - 1];
            }
            else {
                data->type = 30;
            }
            break;
        case 2:
        case 4:
        case 5:
        default:
            data->field_28 = (colorIndex + i + i) & 7;
            if (data->field_24 < 0) {
                active++;
            }
            data->field_24--;
            if (data->field_24 > 30 || data->field_24 < 0) {
                data->field_28 = -1;
            }
            else {
                data->field_20++;
                colorIndex = data->field_20;
                if (colorIndex - 1 < 14) {
                    data->scale = float_0p01_8041feb8 * scale_data[colorIndex - 1];
                    if (type == 5) {
                        data->field_14 += data->field_18;
                        data->y += data->field_14;
                    }
                }
                else {
                    f32 k = float_0p1_8041fea4;
                    data->scale = k * (k - data->scale) + data->scale;
                    data->field_14 += data->field_18;
                    data->y += data->field_14;
                }
                if (type == 4) {
                    rad = float_6p2832_8041fec8 * (f32)(colorIndex * 12) / float_360_8041fec0;
                    data->x = data->field_10 * (f32)sin(rad);
                    data->z = data->field_10 * (f32)cos(rad);
                }
                else {
                    data->x = data->field_10;
                }
                data->field_14 *= float_0p96_8041fed0;
            }
            break;
        }
    }
    if (!((u32)type <= 1 || type == 3 || type >= 10)) {
        if (active >= effect->count - 1) {
            effDelete(effect);
            return;
        }
    }
    dispEntry(drawCam, dispCalcZ(&position2), 2, effStardust2Disp, effect);
}

#ifdef MUST_MATCH
static inline GXColor stardustColor(u8 r, u8 g, u8 b) {
    GXColor c = dat_8041fe88;
    c.r = r;
    c.g = g;
    c.b = b;
    return c;
}
#endif

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
