#include "eff/eff_peach_fukidashi.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "mario/mario.h"
#include "manager/fontmgr.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math.h>
#include <string.h>

extern const GXColor dat_80420a28;
extern const f32 float_deg2rad_80420a2c;
extern const f32 float_0p5_80420a30;
extern const f32 float_16_80420a34;
extern const f32 float_0p4_80420a38;
extern const f32 float_0p34_80420a3c;
extern const f32 float_40_80420a40;
extern const f32 float_7p25_80420a44;
extern const f32 float_14_80420a48;
extern const f32 float_1p5_80420a4c;
extern const f32 float_90_80420a50;
extern const f32 float_0_80420a54;
extern const f32 float_1_80420a58;
extern const f32 float_0p75_80420a5c;
extern const f32 float_6_80420a60;
extern const f32 float_11p5_80420a64;
extern const f32 float_30_80420a68;
extern const f32 float_8_80420a6c;

const Vec vec3_802fdc28 = { 0.0f, 0.0f, 0.0f };
const char dat_802fdc34[0x20] = { 0x81, 0x7E };
const char str_802FDC54[] = "PeachFukidashi";

EffectEntry* effPeachFukidashiEntry(s32 type, NpcEntry* npc, s32 lifetime, f32 a, f32 b, f32 c, f32 radius, f32 angle) {
    EffectEntry* entry;
    EffPeachFukidashiData* data;
    CameraEntry* cam;
    MarioWork* mario;
    Vec temp;
    Vec basePos;
    Vec axis;
    Mtx mtx;
    f32 scale;
    f32 y;

    entry = effEntry();
    entry->type = str_802FDC54;
    entry->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(EffPeachFukidashiData) * entry->count);
    entry->userdata = data;
    entry->callback = effPeachFukidashiMain;

    data->flags = type;
    data->radius = radius;
    data->angle = angle;
    data->lifetime = lifetime;
    data->npc = npc;

    cam = camGetPtr(CAMERA_3D);
    temp = vec3_802fdc28;
    temp.x = sin(float_deg2rad_80420a2c * cam->viewYaw);
    temp.z = -(f32)cos(float_deg2rad_80420a2c * cam->viewYaw);
    axis = temp;
    scale = float_0p5_80420a30 * (float_16_80420a34 + data->radius);
    if (data->npc != NULL) {
        basePos = data->npc->position;
        y = data->npc->height;
    } else {
        mario = marioGetPtr();
        basePos = mario->position;
        y = mario->wPlayerCollisionBox.y;
    }

    PSMTXRotAxisRad(mtx, &axis, float_deg2rad_80420a2c * -data->angle);
    data->position.x = basePos.x + mtx[0][1] * scale;
    data->position.y = basePos.y + y + mtx[1][1] * scale;
    data->position.z = basePos.z + mtx[2][1] * scale;
    data->field_0x18 = data->angle;

    return entry;
}

void effPeachFukidashiMain(EffectEntry* effect) {
    EffPeachFukidashiData* data = effect->userdata;

    if (data->lifetime < 1000) {
        data->lifetime--;
    }
    if (data->lifetime < 0) {
        effDelete(effect);
    } else {
        dispEntry(CAMERA_3D, float_0_80420a54, 2, effPeachFukidashiDisp, effect);
    }
}

void effPeachFukidashiDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    EffPeachFukidashiData* data = effect->userdata;
    CameraEntry* cam;
    MarioWork* mario;
    f32 rad;
    Vec temp1;
    Vec basePos1;
    Vec axis1;
    Vec temp2;
    Vec basePos2;
    Vec axis2;
    Mtx m0, m3, m1, m2, m4;
    Mtx mtx, mtx2;
    f32 scale1;
    f32 y1;
    f32 scale2;
    f32 y2;
    GXColor colorTmp;
    GXColor color;

    cam = camGetPtr(camId);

    cam = camGetPtr(CAMERA_3D);
    temp1 = vec3_802fdc28;
    temp1.x = sin(float_deg2rad_80420a2c * cam->viewYaw);
    temp1.z = -(f32)cos(float_deg2rad_80420a2c * cam->viewYaw);
    axis1 = temp1;
    scale1 = float_0p5_80420a30 * (float_16_80420a34 + data->radius);
    if (data->npc != NULL) {
        basePos1 = data->npc->position;
        y1 = data->npc->height;
    } else {
        mario = marioGetPtr();
        basePos1 = mario->position;
        y1 = mario->wPlayerCollisionBox.y;
    }

    PSMTXRotAxisRad(mtx, &axis1, float_deg2rad_80420a2c * -data->angle);
    data->position.x = basePos1.x + mtx[0][1] * scale1;
    data->position.y = basePos1.y + y1 + mtx[1][1] * scale1;
    data->position.z = basePos1.z + mtx[2][1] * scale1;
    data->field_0x18 = data->angle;

    PSMTXTrans(m0, data->position.x, data->position.y, data->position.z);
    cam = camGetPtr(camId);
    rad = -cam->viewYaw;
    rad = float_deg2rad_80420a2c * rad;
    PSMTXRotRad(m1, 'y', rad);
    PSMTXConcat(m0, m1, m2);
    PSMTXScale(m3, float_0p4_80420a38, float_0p34_80420a3c, float_0p4_80420a38);
    PSMTXRotRad(m1, 'z', float_deg2rad_80420a2c * data->field_0x18);
    PSMTXConcat(m2, m3, m4);
    PSMTXConcat(m4, m1, m4);
    iconDispGx2(m4, 0, 0x1F2);

    cam = camGetPtr(CAMERA_3D);
    temp2 = vec3_802fdc28;
    temp2.x = sin(float_deg2rad_80420a2c * cam->viewYaw);
    temp2.z = -(f32)cos(float_deg2rad_80420a2c * cam->viewYaw);
    axis2 = temp2;
    scale2 = float_0p5_80420a30 * (float_40_80420a40 + data->radius);
    if (data->npc != NULL) {
        basePos2 = data->npc->position;
        y2 = data->npc->height;
    } else {
        mario = marioGetPtr();
        basePos2 = mario->position;
        y2 = mario->wPlayerCollisionBox.y;
    }

    PSMTXRotAxisRad(mtx2, &axis2, float_deg2rad_80420a2c * -data->angle);
    data->position.x = basePos2.x + mtx2[0][1] * scale2;
    data->position.y = basePos2.y + y2 + mtx2[1][1] * scale2;
    data->position.z = basePos2.z + mtx2[2][1] * scale2;
    data->field_0x18 = data->angle;

    PSMTXTrans(m0, data->position.x,
               float_7p25_80420a44 + (data->position.y - float_14_80420a48 +
                                       fabsf(float_1p5_80420a4c * data->angle / float_90_80420a50)),
               data->position.z);
    cam = camGetPtr(camId);
    rad = -cam->viewYaw;
    rad = float_deg2rad_80420a2c * rad;
    PSMTXRotRad(m1, 'y', rad);
    PSMTXConcat(m0, m1, m2);
    PSMTXTrans(m0, float_0_80420a54, float_0_80420a54, float_1_80420a58);
    PSMTXConcat(m2, m0, m2);
    PSMTXScale(m3, float_0p4_80420a38, float_0p4_80420a38, float_0p4_80420a38);
    PSMTXConcat(m2, m3, m2);

    switch (data->flags & 3) {
        case 0:
            iconDispGx2(m2, 0, 0x1F5);
            break;
        case 1:
            iconDispGx2(m2, 0, 0x1F3);
            break;
#ifdef MUST_MATCH
        /* MUST_MATCH: the empty case 2 forces MWCC's switch decision tree to emit the
           duplicated branch to the default body that retail has. */
        case 2:
#endif
        default:
            iconDispGx2(m2, 0, 0x1F4);
            break;
    }

    if (data->flags & 0x80) {
#ifdef MUST_MATCH
        /* MUST_MATCH: retail copies the 0x20-byte banner string as eight word moves; a
           memcpy call or a plain initialiser does not reproduce those instructions. */
        char buf[0x20];

        *(u32*)&buf[0x00] = *(u32*)&dat_802fdc34[0x00];
        *(u32*)&buf[0x04] = *(u32*)&dat_802fdc34[0x04];
        *(u32*)&buf[0x08] = *(u32*)&dat_802fdc34[0x08];
        *(u32*)&buf[0x0C] = *(u32*)&dat_802fdc34[0x0C];
        *(u32*)&buf[0x10] = *(u32*)&dat_802fdc34[0x10];
        *(u32*)&buf[0x14] = *(u32*)&dat_802fdc34[0x14];
        *(u32*)&buf[0x18] = *(u32*)&dat_802fdc34[0x18];
        *(u32*)&buf[0x1C] = *(u32*)&dat_802fdc34[0x1C];
#else
        char buf[0x20];

        memcpy(buf, dat_802fdc34, sizeof(buf));
#endif

        colorTmp = dat_80420a28;
        PSMTXScale(m3, float_0p75_80420a5c, float_0p75_80420a5c, float_0p75_80420a5c);
        PSMTXTrans(m0, float_6_80420a60 + (data->position.x - float_11p5_80420a64),
                   float_30_80420a68 + (data->position.y - float_14_80420a48 +
                                        fabsf(float_1p5_80420a4c * data->angle / float_90_80420a50)) -
                       float_8_80420a6c,
                   data->position.z);
        cam = camGetPtr(camId);
        rad = -cam->viewYaw;
        rad = float_deg2rad_80420a2c * rad;
        PSMTXRotRad(m1, 'y', rad);
        PSMTXConcat(m0, m1, m2);
        PSMTXTrans(m0, float_0_80420a54, float_0_80420a54, float_1_80420a58);
        PSMTXConcat(m2, m0, m2);
        PSMTXConcat(m2, m3, m2);
        FontDrawStart();
        color = colorTmp;
        FontDrawColor(&color);
        FontDrawStringMtx(m2, buf);
    }
}

void effSetPeachFukiBatu(const char* name) {
    EffectEntry* entry = effNameToPtr(name);
    ((EffPeachFukidashiData*)entry->userdata)->flags |= 0x80;
}

// Defined after their uses, so the functions load them by symbol instead of folding the values.
const GXColor dat_80420a28 = { 0xFF, 0x00, 0x00, 0xFF };
const f32 float_deg2rad_80420a2c = 0.017453292f;
const f32 float_0p5_80420a30 = 0.5f;
const f32 float_16_80420a34 = 16.0f;
const f32 float_0p4_80420a38 = 0.4f;
const f32 float_0p34_80420a3c = 0.34f;
const f32 float_40_80420a40 = 40.0f;
const f32 float_7p25_80420a44 = 7.25f;
const f32 float_14_80420a48 = 14.0f;
const f32 float_1p5_80420a4c = 1.5f;
const f32 float_90_80420a50 = 90.0f;
const f32 float_0_80420a54 = 0.0f;
const f32 float_1_80420a58 = 1.0f;
const f32 float_0p75_80420a5c = 0.75f;
const f32 float_6_80420a60 = 6.0f;
const f32 float_11p5_80420a64 = 11.5f;
const f32 float_30_80420a68 = 30.0f;
const f32 float_8_80420a6c = 8.0f;
