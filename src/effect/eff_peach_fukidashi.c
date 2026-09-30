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
    temp.x = sin(0.017453292f * cam->viewYaw);
    temp.z = -(f32)cos(0.017453292f * cam->viewYaw);
    axis = temp;
    scale = 0.5f * (16.0f + data->radius);
    if (data->npc != NULL) {
        basePos = data->npc->position;
        y = data->npc->height;
    } else {
        mario = marioGetPtr();
        basePos = mario->position;
        y = mario->wPlayerCollisionBox.y;
    }

    PSMTXRotAxisRad(mtx, &axis, 0.017453292f * -data->angle);
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
        dispEntry(CAMERA_3D, 0.0f, 2, effPeachFukidashiDisp, effect);
    }
}

void effPeachFukidashiDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    EffPeachFukidashiData* data = effect->userdata;
    CameraEntry* cam;
    MarioWork* mario;
#ifdef MUST_MATCH
    f32 rad;
#endif
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
    static const GXColor red = { 0xFF, 0x00, 0x00, 0xFF };

    cam = camGetPtr(camId);

    cam = camGetPtr(CAMERA_3D);
    temp1 = vec3_802fdc28;
    temp1.x = sin(0.017453292f * cam->viewYaw);
    temp1.z = -(f32)cos(0.017453292f * cam->viewYaw);
    axis1 = temp1;
    scale1 = 0.5f * (16.0f + data->radius);
    if (data->npc != NULL) {
        basePos1 = data->npc->position;
        y1 = data->npc->height;
    } else {
        mario = marioGetPtr();
        basePos1 = mario->position;
        y1 = mario->wPlayerCollisionBox.y;
    }

    PSMTXRotAxisRad(mtx, &axis1, 0.017453292f * -data->angle);
    data->position.x = basePos1.x + mtx[0][1] * scale1;
    data->position.y = basePos1.y + y1 + mtx[1][1] * scale1;
    data->position.z = basePos1.z + mtx[2][1] * scale1;
    data->field_0x18 = data->angle;

    PSMTXTrans(m0, data->position.x, data->position.y, data->position.z);
    cam = camGetPtr(camId);
#ifdef MUST_MATCH
    rad = -cam->viewYaw;
    rad = 0.017453292f * rad;
    PSMTXRotRad(m1, 'y', rad);
#else
    PSMTXRotRad(m1, 'y', -cam->viewYaw * 0.017453292f);
#endif
    PSMTXConcat(m0, m1, m2);
    PSMTXScale(m3, 0.4f, 0.34f, 0.4f);
    PSMTXRotRad(m1, 'z', 0.017453292f * data->field_0x18);
    PSMTXConcat(m2, m3, m4);
    PSMTXConcat(m4, m1, m4);
    iconDispGx2(m4, 0, 0x1F2);

    cam = camGetPtr(CAMERA_3D);
    temp2 = vec3_802fdc28;
    temp2.x = sin(0.017453292f * cam->viewYaw);
    temp2.z = -(f32)cos(0.017453292f * cam->viewYaw);
    axis2 = temp2;
    scale2 = 0.5f * (40.0f + data->radius);
    if (data->npc != NULL) {
        basePos2 = data->npc->position;
        y2 = data->npc->height;
    } else {
        mario = marioGetPtr();
        basePos2 = mario->position;
        y2 = mario->wPlayerCollisionBox.y;
    }

    PSMTXRotAxisRad(mtx2, &axis2, 0.017453292f * -data->angle);
    data->position.x = basePos2.x + mtx2[0][1] * scale2;
    data->position.y = basePos2.y + y2 + mtx2[1][1] * scale2;
    data->position.z = basePos2.z + mtx2[2][1] * scale2;
    data->field_0x18 = data->angle;

    PSMTXTrans(m0, data->position.x,
               7.25f + (data->position.y - 14.0f +
                                       fabsf(1.5f * data->angle / 90.0f)),
               data->position.z);
    cam = camGetPtr(camId);
#ifdef MUST_MATCH
    rad = -cam->viewYaw;
    rad = 0.017453292f * rad;
    PSMTXRotRad(m1, 'y', rad);
#else
    PSMTXRotRad(m1, 'y', -cam->viewYaw * 0.017453292f);
#endif
    PSMTXConcat(m0, m1, m2);
    PSMTXTrans(m0, 0.0f, 0.0f, 1.0f);
    PSMTXConcat(m2, m0, m2);
    PSMTXScale(m3, 0.4f, 0.4f, 0.4f);
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

        colorTmp = red;
        PSMTXScale(m3, 0.75f, 0.75f, 0.75f);
        PSMTXTrans(m0, 6.0f + (data->position.x - 11.5f),
                   30.0f + (data->position.y - 14.0f +
                                        fabsf(1.5f * data->angle / 90.0f)) -
                       8.0f,
                   data->position.z);
        cam = camGetPtr(camId);
#ifdef MUST_MATCH
        rad = -cam->viewYaw;
        rad = 0.017453292f * rad;
        PSMTXRotRad(m1, 'y', rad);
#else
        PSMTXRotRad(m1, 'y', -cam->viewYaw * 0.017453292f);
#endif
        PSMTXConcat(m0, m1, m2);
        PSMTXTrans(m0, 0.0f, 0.0f, 1.0f);
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
