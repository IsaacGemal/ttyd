#include "eff/n64/eff_stardust2_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "matching.h"
#include "memory.h"

#include <stdlib.h>
#define _MATH_H_ // keep the SDK math.h (sqrt inlines and their pooled doubles) out of this unit
#include "math_ext.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

extern const Vec vec3_802fb740;

// .sdata2 constants the code reads by name (named after their original addresses).
extern const GXColor dat_8041fe88;
extern const f32 float_deg2rad_8041fe8c;
extern const f32 float_0p0056818_8041fe90;
extern const f32 float_0p045455_8041fe94;
extern const f32 float_0_8041fe98;
extern const f32 float_1_8041fe9c;

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

// One Stardust2N64 particle. Element 0 is the effect header (position, age,
// life, draw camera); the particles follow it.
typedef struct EffStardust2Data {
    u16 flags;      // 0x00: set when the caller passes type 6 (stored as type 3)
    s16 type;       // 0x02
    f32 x;          // 0x04
    f32 y;          // 0x08
    f32 z;          // 0x0C
    f32 velX;       // 0x10: x step, or orbit factor for types 0/1
    f32 velY;       // 0x14: y step, or orbit factor for types 0/1
    f32 accelY;     // 0x18: y drift; added to y for types 0/1, to velY otherwise
    f32 scale;      // 0x1C
    u32 age;        // 0x20: frames since spawn; drives colour/scale tables
    s32 life;       // 0x24: frames left; deleted below 0
    s32 vtxIndex;   // 0x28: vertex strip 0..7; -1 skips drawing
    s32 drawCam;    // 0x2C
} EffStardust2Data;

static inline Vec makeVec(f32 x, f32 y, f32 z) {
    Vec temp = vec3_802fb740;
    temp.x = x;
    temp.y = y;
    temp.z = z;
    return temp;
}

EffectEntry* effStardust2N64Entry(s32 type, s32 count, s32 life, f32 x, f32 y, f32 z, f32 scaleA, f32 scaleB) {
    EffectEntry* effect;
    EffStardust2Data* data;
    EffStardust2Data* ptr;
    f32 s;
    f32 c;
    f32 stepA;
    f32 baseA;
    f32 stepB;
    f32 baseB;
    f32 denom;
    f32 rad;
    f32 t;
    f32 vA;
    f32 vB;
    f32 yStep;
    s32 i;
    s32 r;

    effect = effEntry();
    effect->type = "Stardust2N64";
    effect->count = count + 1;
    data = __memAlloc(HEAP_EFFECT, (count + 1) * sizeof(EffStardust2Data));
    effect->userdata = data;
    effect->callback = effStardust2Main;

    if (type == 6) {
        type = 3;
        data->flags = 1;
    }
    else {
        data->flags = 0;
    }

    data->x = x;
    data->y = y;
    data->z = z;
    data->age = 0;
    data->type = type;
    data->life = life;
    data->drawCam = 4;

    denom = (f32)(count + 1) - float_1_8041fe9c;
    stepA = 0.1f * scaleA;
    stepB = 0.1f * scaleB;
    baseA = 0.4f * scaleA;
    baseB = 0.4f * scaleB;

    ptr = data + 1;
    for (i = 1; i < count + 1; i++, ptr++) {
        y = (360.f / denom) * (f32)i;
        if (type > 1) {
            r = rand() % 360;
        }
        else {
            r = 0;
        }
        rad = 6.2831855f * (y + (f32)r) / 360.f;
        s = (f32)sin(rad);
        c = (f32)cos(rad);
        t = (f32)sin(6.2831855f * (10.f * y - 90.f) / 360.f);
        vA = baseA - stepA * t;
        vB = baseB - stepB * t;

        ptr->life = i + 30;
        ptr->age = 0;
        ptr->scale = float_1_8041fe9c;
        yStep = ((f32)i - float_1_8041fe9c) * (scaleB / denom);

        switch (type) {
        case 0:
        case 1:
            ptr->accelY = float_0_8041fe98;
            ptr->velX = s * vA;
            ptr->velY = c * vB;
            ptr->z = float_0_8041fe98;
            break;
        case 2:
        case 5:
            ptr->velX = s * vA;
            ptr->y = yStep;
            ptr->z = c * vA;
            ptr->velY = 0.03f * (f32)(-(rand() % 10));
            ptr->accelY = 0.04f * (f32)(rand() % 10) + 0.01f;
            break;
        case 3:
            ptr->x = 0.005f * (s * scaleA * (f32)(rand() % 100));
            ptr->y = 0.005f * (c * scaleB * (f32)(rand() % 100));
            ptr->z = float_0_8041fe98;
            ptr->velY = float_0_8041fe98;
            ptr->velX = float_0_8041fe98;
            ptr->accelY = float_0_8041fe98;
            ptr->life += i + i;
            break;
        default:
            ptr->velX = s * vA;
            ptr->y = yStep;
            ptr->z = c * vA;
            ptr->velY = 0.03f * (f32)(-(rand() % 10));
            ptr->accelY = 0.04f * (f32)(rand() % 10) + 0.01f;
            break;
        }
    }
    return effect;
}
void effStardust2Main(EffectEntry* effect) {
    EffStardust2Data* data;
    Vec dispPosition;
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
    dispPosition = position;
    drawCam = data->drawCam;
    data->life--;
    data->age++;
    colorIndex = data->age;
    type = data->type;
    active = 0;
    if ((u32)type <= 1 || type == 3 || type >= 10) {
        if (data->life < 0) {
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
            data->vtxIndex = (colorIndex + i) & 7;
            data->accelY += -0.02f;
            // speed_data and scale_data have 14 usable entries
            if (colorIndex - 1 < 14) {
                f32 val = 0.01f * speed_data[colorIndex - 1];
                data->x = data->velX * val;
                data->y = data->velY * val + data->accelY;
                data->scale = 0.01f * scale_data[colorIndex - 1];
            }
            else {
                f32 angle;
                f32 ratio;
                one = float_1_8041fe9c;
                ratio = 360.f / (f32)(effect->count - 1);
                angle = ratio * (f32)i + 60.f;
                if (i & 1) {
                    angle -= 120.f;
                }
                rad = 6.2831855f * angle / 360.f;
                data->type = 10;
                data->velX = one * (f32)sin(rad);
                data->velY = one * (f32)cos(rad);
                data->accelY = -0.05f;
            }
            break;
        case 30:
        case 10:
            data->vtxIndex = (colorIndex + i) & 7;
            data->velX *= 0.96f;
            data->velY *= 0.96f;
            data->velY += data->accelY;
            data->x += data->velX;
            data->y += data->velY;
            data->scale += 0.1f * (0.1f - data->scale);
            break;
        case 3:
            data->vtxIndex = (colorIndex + i + i) & 7;
            if (colorIndex - 1 < 14) {
                data->scale = 0.01f * scale_data[colorIndex - 1];
            }
            else {
                data->type = 30;
            }
            break;
        case 2:
        case 4:
        case 5:
        default:
            data->vtxIndex = (colorIndex + i + i) & 7;
            if (data->life < 0) {
                active++;
            }
            data->life--;
            if (data->life > 30 || data->life < 0) {
                data->vtxIndex = -1;
            }
            else {
                data->age++;
                colorIndex = data->age;
                if (colorIndex - 1 < 14) {
                    data->scale = 0.01f * scale_data[colorIndex - 1];
                    if (type == 5) {
                        data->velY += data->accelY;
                        data->y += data->velY;
                    }
                }
                else {
                    data->scale += 0.1f * (0.1f - data->scale);
                    data->velY += data->accelY;
                    data->y += data->velY;
                }
                if (type == 4) {
                    rad = 6.2831855f * (f32)(colorIndex * 12) / 360.f;
                    data->x = data->velX * (f32)sin(rad);
                    data->z = data->velX * (f32)cos(rad);
                }
                else {
                    data->x = data->velX;
                }
                data->velY *= 0.96f;
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
    dispEntry(drawCam, dispCalcZ(&dispPosition), 2, effStardust2Disp, effect);
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
    s32 vtxIndex;
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
    colorIdx = (data->age - 1) * 3;
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
        colorG = 0.5f;
        colorB = 0.1f;
        break;
    case 3:
    case 30:
        colorR = float_1_8041fe9c;
        colorG = 0.8f;
        colorB = 0.7f;
        break;
    case 5:
        colorR = 1.4f;
        colorB = 0.7f;
        colorG = colorR;
        break;
    default:
        colorG = 0.7f;
        colorR = 1.4f;
        colorB = colorG;
        break;
    }

    for (i = 0; i < entry->count - 1; i++, data++) {
        vtxIndex = data->vtxIndex;
        if (vtxIndex < 0) {
            continue;
        }
        PSMTXTrans(mtx, data->x, data->y, data->z);
        mtx[0][0] = mtx[1][1] = mtx[2][2] = data->scale;
        PSMTXConcat(base, mtx, mtx);
        GXLoadPosMtxImm(mtx, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);

        // wrap to the start of the 36-byte colour rotation table
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

        switch (vtxIndex & 7) {
        case 0:
            vtx = &((effVtxArray*)size8x8_tex22x22_vtx)[3];
            break;
        case 1:
            vtx = &((effVtxArray*)size8x8_tex22x22_vtx)[4];
            break;
        case 2:
            vtx = &((effVtxArray*)size8x8_tex22x22_vtx)[5];
            break;
        case 3:
            vtx = &((effVtxArray*)size8x8_tex22x22_vtx)[6];
            break;
        case 4:
            vtx = &((effVtxArray*)size8x8_tex22x22_vtx)[7];
            break;
        case 5:
            vtx = &((effVtxArray*)size8x8_tex22x22_vtx)[0];
            break;
        case 6:
            vtx = &((effVtxArray*)size8x8_tex22x22_vtx)[1];
            break;
        case 7:
            vtx = &((effVtxArray*)size8x8_tex22x22_vtx)[2];
            break;
        }
        effSetVtxDescN64(vtx);
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}
void effStardust2N64SetDrawCam(EffectEntry* entry, s32 cam) {
    ((EffStardust2Data*)entry->userdata)->drawCam = cam;
}

// The constants the code above loads by name, defined after it (defined first, the compiler folds them).
const Vec vec3_802fb740 = { 0.0f, 0.0f, 0.0f };
const GXColor dat_8041fe88 = { 0x00, 0x00, 0x00, 0xFF };
const f32 float_deg2rad_8041fe8c = 0.017453292f;
const f32 float_0p0056818_8041fe90 = 0.0056818184f;
const f32 float_0p045455_8041fe94 = 0.045454547f;
const f32 float_0_8041fe98 = 0.0f;
const f32 float_1_8041fe9c = 1.0f;
