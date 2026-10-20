#include "dmo_00.h"
#include "evt/evt_bg.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_img.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_urouro.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/imgdrv.h"
#include "driver/msgdrv.h"
#include "manager/evtmgr_cmd.h"
#include "manager/filemgr.h"
#include "manager/fontmgr.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "pmario_sound.h"
#include "statuswindow.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdio.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on
static USER_FUNC(next);

typedef struct DemoWork {
    u32 field_0x00;
    f32 bookAngle;        // book_r; the book is turned by -bookAngle about y
    s32 page;             // -1 = none; next() counts up, demo_cap captures at 4
    s32 captured;         // set once demo_cap has copied the frame
    f32 fade;             // adjust's angle; the picture alpha is 255 * (1 - fade)
    f32 scale;            // demo_scale ramps 0.6 -> 1.5
    f32 posX;
    f32 posY;             // adjust: -105 * sin(pi/2 * fade)
    void* captureBuf;     // GXCopyTex destination: *(void**)captureBuf is the buffer
    FileEntry* tplFile;   // demo.tpl
    s32 winOpen;          // demo_winonoff argument, nonzero = window opening
    s32 winAlpha;         // 0..0xCC, steps of 10 per frame
} DemoWork;
static DemoWork work;
u32 buf[128];

// clang-format off
extend_data(0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(2,
     f64, _three, 3,
     f64, _half, 0.5)

// TODO: type
static DemoWork* wp = &work;

// Functions are written in the reverse of their .text order (deferred codegen).
USER_FUNC(demo_init) {
    if (isFirstCall) {
        memset(wp, 0, sizeof(DemoWork));
        wp->bookAngle = 0.0f;
        wp->page = -1;
        wp->captured = 0;
        wp->fade = 0.0f;
        wp->scale = 1.0f;
        wp->posX = 0.0f;
        wp->posY = 0.0f;
        statusWinDispOff();
        psndSetFlag(4);
    }
    if ((u32)fileAsyncf(FILE_TYPE_TPL, 0, "%s/demo.tpl", getMarioStDvdRoot()) != 0) {
        wp->tplFile = fileAllocf(FILE_TYPE_TPL, "%s/demo.tpl", getMarioStDvdRoot());
        gp->areaFadeOutType = 9;
        gp->areaFadeOutDuration = 500;
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

void demo_exit(void) {
    CameraEntry* cam = camGetPtr(8);
    if (wp->tplFile != 0) {
        fileFree(wp->tplFile);
    }
    psndClearFlag(4);
    cam = camGetPtr(4);
    cam->viewport[0] = 0.0f;
    cam->viewport[1] = 0.0f;
    cam->viewport[2] = 608.0f;
    cam->viewport[3] = 480.0f;
    statusWinDispOn();
}

void demo_cap(void) {
    if (wp->page == 4) {
        if (wp->captured == 0) {
            GXSetTexCopySrc(0, 0, 608, 480);
            GXSetTexCopyDst(608, 480, 6, 0);
            GXCopyTex(*(void**)wp->captureBuf, 0);
            GXTexModeSync();
            wp->captured = 1;
        } else if (wp->fade != 0.0f) {
            GXSetTexCopySrc(0, 0, 608, 480);
            GXSetTexCopyDst(608, 480, 6, 0);
            GXCopyTex(*(void**)wp->captureBuf, 0);
            GXTexModeSync();
        }
    }
}

void demo_desk(CameraId cameraId, void* param) {
    CameraEntry* cam = camGetPtr(8);
    GXTexObj texObj;
    Mtx m1;
    Mtx m2;
#ifdef MUST_MATCH
    // MUST_MATCH: these temporaries pin MWCC's FPR assignment and commutative-operand order; the
    // #else branch is the natural form and does not match.
    f32 w = 608.0f;
    f32 h = 480.0f;
    f32 top = 240.0f;
    f32 right = 304.0f;
    f32 half = 0.5f;
#endif

    GXSetProjection(cam->projection, cam->projType);
    GXSetViewport(0.0f, 0.0f, 608.0f, 480.0f, 0.0f, 1.0f);
    GXSetBlendMode(1, 4, 5, 0);
    GXSetZCompLoc(1);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetZMode(0, 3, 0);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetChanMatColor(4, (GXColor) { 0xFF, 0xFF, 0xFF, (u8)(255.0f * (1.0f - wp->fade)) });
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    TEXGetGXTexObjFromPalette((TEXPalette*)wp->tplFile->data[0], &texObj, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevOp(0, 0);
    GXSetTevSwapMode(0, 0, 0);
    PSMTXTrans(m1, wp->posX, wp->posY, 0.0f);
    PSMTXScale(m2, wp->scale, wp->scale, wp->scale);
    PSMTXConcat(m1, m2, m1);
    PSMTXConcat(cam->view, m1, m1);
    GXLoadPosMtxImm(m1, 0);
    GXSetCurrentMtx(0);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXBegin(0x80, 0, 4);
#ifdef MUST_MATCH
    GXPosition3f32(-w * half, top, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(right, top, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(right, -h * half, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(-w * half, -h * half, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
#else
    GXPosition3f32(-608.0f * 0.5f, 240.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(304.0f, 240.0f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(304.0f, -480.0f * 0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(-608.0f * 0.5f, -480.0f * 0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
#endif
}

// demo_img projects the captured frame and duplicates it into two other viewports.
void demo_img(CameraId cameraId, void* param) {
    CameraEntry* cam = camGetPtr(8);
    GXTexObj texObj;
    Mtx m1;
    Mtx m2;
    f32 proj[7];
    f32 vp[6];
    Vec p0, p1, p2, p3;
    f32 b;
    f32 a;
    f32 right = 101.333336f;
    f32 top = 115.0f;
#ifdef MUST_MATCH
    // MUST_MATCH: these locals keep MWCC from folding the edge expressions into a single pool literal;
    // the original loads 202.66667/0.5 (and 160/0.5/35) and emits fneg/fmuls/fmadds. The #else form is natural.
    f32 edge_w = 202.66667f;
    f32 edge_h = 160.0f;
    f32 half = 0.5f;
    f32 topoff = 35.0f;
#endif

    if (wp->page == -1) {
        return;
    }
    GXSetProjection(cam->projection, cam->projType);
    GXSetViewport(0.0f, 0.0f, 608.0f, 480.0f, 0.0f, 1.0f);
    GXSetBlendMode(1, 4, 5, 0);
    GXSetZCompLoc(1);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetZMode(0, 3, 0);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetChanMatColor(4, (GXColor) { 0xFF, 0xFF, 0xFF, (u8)(255.0f * (1.0f - wp->fade)) });
    TEXGetGXTexObjFromPalette((TEXPalette*)wp->tplFile->data[0], &texObj, wp->page + 8);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevOp(0, 0);
    GXSetTevAlphaIn(0, 7, 7, 7, 5);
    GXSetTevSwapMode(0, 0, 0);
    PSMTXTrans(m1, wp->posX, wp->posY, 0.0f);
    PSMTXScale(m2, wp->scale, wp->scale, wp->scale);
    PSMTXConcat(m1, m2, m1);
    PSMTXConcat(cam->view, m1, m1);
    GXLoadPosMtxImm(m1, 0);
    GXSetCurrentMtx(0);
    GXSetCullMode(2);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXGetViewportv(vp);
    GXGetProjectionv(proj);
#ifdef MUST_MATCH
    a = -edge_w * half;
#else
    a = -202.66667f * 0.5f;
#endif
    GXProject(a, 115.0f, 0.0f, m1, proj, vp, &p0.x, &p0.y, &p0.z);
    GXProject(101.333336f, 115.0f, 0.0f, m1, proj, vp, &p1.x, &p1.y, &p1.z);
#ifdef MUST_MATCH
    // MUST_MATCH: assigning b inside the call (instead of on its own line) makes MWCC schedule the call's
    // 101.333336 load and &p2.y setup the way the original does; the #else form is the natural one.
    GXProject(101.333336f, (b = -edge_h * half + topoff), 0.0f, m1, proj, vp, &p2.x, &p2.y, &p2.z);
#else
    b = -160.0f * 0.5f + 35.0f;
    GXProject(101.333336f, b, 0.0f, m1, proj, vp, &p2.x, &p2.y, &p2.z);
#endif
    GXProject(a, b, 0.0f, m1, proj, vp, &p3.x, &p3.y, &p3.z);
    cam = camGetPtr(4);
    cam->viewport[0] = p0.x;
    cam->viewport[1] = p0.y;
    cam->viewport[2] = p2.x - p0.x;
    cam->viewport[3] = p2.y - p0.y;
    cam = camGetPtr(6);
    cam->viewport[0] = 0.0f;
    cam->viewport[1] = 0.0f;
    cam->viewport[2] = 608.0f;
    cam->viewport[3] = 480.0f;
    GXBegin(0x80, 0, 4);
    GXPosition3f32(a, top, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(right, top, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(right, b, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(a, b, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
}

void demo_book(CameraId cameraId, void* param) {
    CameraEntry* cam = camGetPtr(8);
    GXTexObj texObj;
    Mtx m1;
    Mtx m2;
    Mtx m3;
    f32 w;
    f32 h;
    f32 w95;
    f32 h95;

    GXSetProjection(cam->projection, cam->projType);
    GXSetViewport(0.0f, 0.0f, 608.0f, 480.0f, 0.0f, 1.0f);
    TEXGetGXTexObjFromPalette((TEXPalette*)wp->tplFile->data[0], &texObj, 2);
    w = (f32)(u16)GXGetTexObjWidth(&texObj);
    PSMTXTrans(m1, wp->posX, wp->posY, 0.0f);
    PSMTXScale(m2, wp->scale, wp->scale, wp->scale);
    PSMTXConcat(m1, m2, m1);
    PSMTXConcat(cam->view, m1, m2);
    PSMTXTrans(m1, -w / 2.0f + 8.0f, 0.0f, 0.0f);
    PSMTXConcat(m2, m1, m1);
    GXSetBlendMode(1, 4, 5, 0);
    GXSetZCompLoc(1);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetZMode(0, 3, 0);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetChanMatColor(4, (GXColor) { 0xFF, 0xFF, 0xFF, (u8)(255.0f * (1.0f - wp->fade)) });
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    TEXGetGXTexObjFromPalette((TEXPalette*)wp->tplFile->data[0], &texObj, 1);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevOp(0, 0);
    GXSetTevAlphaIn(0, 7, 7, 7, 5);
    GXSetTevSwapMode(0, 0, 0);
    GXSetCullMode(2);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    TEXGetGXTexObjFromPalette((TEXPalette*)wp->tplFile->data[0], &texObj, 2);
    GXLoadTexObj(&texObj, 0);
    w = (f32)(u16)GXGetTexObjWidth(&texObj);
    h = (f32)(u16)GXGetTexObjHeight(&texObj);
    GXLoadPosMtxImm(m1, 0);
    GXSetCurrentMtx(0);
    GXBegin(0x80, 0, 4);
    GXPosition3f32(0.0f, h / 2.0f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(w, h / 2.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(w, -h / 2.0f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(0.0f, -h / 2.0f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    w = (f32)(u16)GXGetTexObjWidth(&texObj);
    h = (f32)(u16)GXGetTexObjHeight(&texObj);
    GXLoadPosMtxImm(m1, 0);
    GXSetCurrentMtx(0);
    if (wp->page >= 0) {
        TEXGetGXTexObjFromPalette((TEXPalette*)wp->tplFile->data[0], &texObj, wp->page + 3);
        GXLoadTexObj(&texObj, 0);
        GXBegin(0x80, 0, 4);
        h95 = h * 0.95f;
        w95 = w * 0.95f;
        GXPosition3f32(0.0f, h95 / 2.0f, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(w95, h95 / 2.0f, 0.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(w95, -h95 / 2.0f, 0.0f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(0.0f, -h95 / 2.0f, 0.0f);
        GXTexCoord2f32(0.0f, 1.0f);
        demo_img(cameraId, param);
    }
    GXSetBlendMode(0, 1, 0, 7);
    GXSetZCompLoc(1);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetZMode(0, 3, 0);
    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetTevSwapMode(0, 0, 0);
    PSMTXRotRad(m3, 'y', 0.017453292f * -wp->bookAngle);
    PSMTXConcat(m1, m3, m3);
    GXLoadPosMtxImm(m3, 0);
    GXSetCurrentMtx(0);
    TEXGetGXTexObjFromPalette((TEXPalette*)wp->tplFile->data[0], &texObj, 1);
    GXLoadTexObj(&texObj, 0);
    GXBegin(0x80, 0, 4);
    GXPosition3f32(0.0f, h / 2.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(w, h / 2.0f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(w, -h / 2.0f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(0.0f, -h / 2.0f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXSetCullMode(1);
    TEXGetGXTexObjFromPalette((TEXPalette*)wp->tplFile->data[0], &texObj, 2);
    GXLoadTexObj(&texObj, 0);
    GXBegin(0x80, 0, 4);
    GXPosition3f32(0.0f, h / 2.0f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(w, h / 2.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(w, -h / 2.0f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(0.0f, -h / 2.0f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
}

void demo_window(CameraId cameraId, void* param) {
    CameraEntry* cam = camGetPtr(cameraId);
    Mtx m1;
    GXTexObj texObj;
    f32 w;
    f32 h;
    f32 left;
    f32 bottom;
    f32 right;
    f32 top;

    if (wp->winOpen != 0) {
        wp->winAlpha += 10;
    } else {
        wp->winAlpha -= 10;
    }
    if (wp->winAlpha > 0xCC) {
        wp->winAlpha = 0xCC;
    }
    if (wp->winAlpha < 0) {
        wp->winAlpha = 0;
    }
    GXSetBlendMode(1, 4, 5, 0);
    GXSetZCompLoc(1);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetZMode(0, 3, 0);
    TEXGetGXTexObjFromPalette((TEXPalette*)wp->tplFile->data[0], &texObj, 0xD);
    GXLoadTexObj(&texObj, 0);
    w = (f32)(u16)GXGetTexObjWidth(&texObj);
    h = (f32)(u16)GXGetTexObjHeight(&texObj);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetChanMatColor(4, (GXColor) { 0xFF, 0xFF, 0xFF, (u8)(wp->winAlpha * 255 / 255) });
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevOp(0, 0);
    GXSetTevSwapMode(0, 0, 0);
    GXSetCullMode(2);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    PSMTXTrans(m1, 0.0f, -130.0f, 0.0f);
    PSMTXConcat(cam->view, m1, m1);
    GXLoadPosMtxImm(m1, 0);
    GXSetCurrentMtx(0);
    GXBegin(0x80, 0, 4);
    right = w / 2.0f;
    top = h / 2.0f;
    left = -right;
    bottom = -top;
    GXPosition3f32(left, top, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(right, top, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(right, bottom, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(left, bottom, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
}

USER_FUNC(demo_draw) {
    dispEntry(4, 9.0f, 7, demo_desk, 0);
    dispEntry(4, 8.0f, 7, demo_book, 0);
    if (wp->fade != 0.0f) {
        dispEntry(7, 0.0f, 7, demo_img, 0);
    }
    dispEntry(8, 0.0f, 3, demo_window, 0);
    return EVT_RETURN_BLOCK;
}

USER_FUNC(demo_winonoff) {
    wp->winOpen = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(demo_scale) {
    OSTime time = event->lifetime;
    f32 t;
    if (isFirstCall) {
        event->savedTime = time;
    }
    t = (f32)(s32)((u64)((s32)time - (s32)event->savedTime) / (OS_TIMER_CLOCK / 1000)) / 12000.0f;
    wp->scale = 600.0f * (1.0f - t) + 1500.0f * t;
    wp->scale /= 1000.0f;
    if (wp->scale >= 1.5f) {
        wp->scale = 1.5f;
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(book_r) {
    wp->bookAngle = evtGetFloat(event, event->args[0]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(next) {
    wp->page++;
    return EVT_RETURN_DONE;
}

USER_FUNC(adjust) {
    f32 angle = evtGetFloat(event, event->args[0]);
    ImageEntry* img = imgNameToPtr("img4", gp->inBattle != 0);
    Mtx m1;
    Mtx m2;

    wp->fade = angle;
    {
        f32 s = (f32)sin(1.5707964f * wp->fade);
        wp->scale = 1.5f * s + 1.5f;
    }
    wp->posY = -105.0f * (f32)sin(1.5707964f * wp->fade);
    PSMTXTrans(m1, wp->posX / 10.0f, wp->posY / 10.0f, 0.0f);
    PSMTXScale(m2, wp->scale / 1.5f, wp->scale / 1.5f, wp->scale / 1.5f);
    PSMTXConcat(m1, m2, img->field_0x120);
    return EVT_RETURN_DONE;
}

USER_FUNC(eff_cam) {
    *(s32*)((u8*)effNameToPtr("eff0")->userdata + 0x38) = 5;
    *(s32*)((u8*)effNameToPtr("eff1")->userdata + 0x38) = 5;
    return EVT_RETURN_DONE;
}

USER_FUNC(eff_cam1) {
    *(s32*)((u8*)effNameToPtr("eff2")->userdata + 0x98) = 5;
    return EVT_RETURN_DONE;
}

// width is u16 in the original (the compiler emits `clrlwi r3, r3, 16` before the `neg`).
USER_FUNC(make_msg) {
    u16 lines;
    u16 width = FontGetMessageWidthLine(msgSearch((const char*)evtGetValue(event, event->args[0])), &lines);
    sprintf((char*)buf, "<clear><wpos %d %d %d %d><once_stop>", -width / 2 - 0x3e, lines * 0x1c / 2 - 0x54, 0x190,
            0xc8);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(buttonCheck)
    CALL(evt_fade_end_wait)
    LOOP(0)
        CALL(evt_key_get_buttontrg, 0, LW(0))
        IF_BITS_SET(LW(0), 4352)
            SET_VALUE(GF(10), 1)
            CALL(evt_fade_set_mapchange_type, 1, 17, 500, -1, -1)
            CALL(evt_seq_set_seq, 3, STRING("title"), 0)
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(demo_evt)
    CALL(evt_seq_wait, 2)
    SET_VALUE(GF(10), 0)
    SPAWN_EVENT_GET_ID(PTR(buttonCheck), GW(10))
    CALL(evt_snd_bgmon, 513, STRING("BGM_DEMO1"))
    CALL(evt_snd_bgm_scope, 0, 1)
    BEGIN_INLINE_EVENT_GET_ID(LW(9))
        WAIT_MS(4000)
        CALL(demo_winonoff, 1)
        CALL(make_msg, STRING("op_book_01"))
        CALL(evt_msg_print, 1, PTR(buf), 0, 0)
        CALL(evt_msg_print_add, 0, STRING("op_book_01"))
        CALL(make_msg, STRING("op_book_02"))
        CALL(evt_msg_print, 1, PTR(buf), 0, 0)
        CALL(evt_msg_print_add, 0, STRING("op_book_02"))
        CALL(demo_winonoff, 0)
    END_INLINE_EVENT
    CALL(demo_scale, LW(0))
    WAIT_MS(1000)
    CALL(demo_winonoff, 1)
    CALL(make_msg, STRING("op_book_03"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_03"))
    CALL(make_msg, STRING("op_book_04"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_04"))
    CALL(demo_winonoff, 0)
    LOOP(0)
        IS_EVENT_RUNNING(LW(9), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(next)
    CALL(evt_snd_sfxon_, STRING("SFX_EVT_OPN_BOOK_OPEN1"), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, 1800, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(book_r, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(1000)
    CALL(demo_winonoff, 1)
    CALL(make_msg, STRING("op_book_05"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_05"))
    CALL(make_msg, STRING("op_book_06"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_06"))
    CALL(make_msg, STRING("op_book_07"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_07"))
    CALL(make_msg, STRING("op_book_08"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_08"))
    CALL(demo_winonoff, 0)
    CALL(evt_img_entry, STRING("img1"))
    CALL(evt_img_entry, STRING("img2"))
    CALL(evt_img_entry, STRING("img3"))
    CALL(evt_img_entry, STRING("img4"))
    CALL(N_evt_img_set_z, STRING("img1"), FLOAT(4.0f))
    CALL(N_evt_img_set_z, STRING("img2"), FLOAT(3.0f))
    CALL(N_evt_img_set_z, STRING("img3"), FLOAT(2.0f))
    CALL(N_evt_img_set_z, STRING("img4"), FLOAT(1.0f))
    CALL(evt_img_set_paper, STRING("img1"), STRING("OFF_d_meku_right"))
    CALL(evt_img_clear_virtual_point, STRING("img1"))
    CALL(evt_img_alloc_capture, STRING("img1"), 0, 0, 1, 124, 0, 386, 480)
    CALL(evt_img_onoff, STRING("img1"), 1)
    WAIT_FRAMES(1)
    CALL(next)
    CALL(evt_snd_sfxon_, STRING("SFX_EVT_OPN_PAGE_OPEN1"), 0)
    CALL(evt_img_set_paper_anim, STRING("img1"), STRING("A_1"))
    CALL(evt_img_wait_animend, STRING("img1"))
    WAIT_MS(1000)
    CALL(demo_winonoff, 1)
    CALL(make_msg, STRING("op_book_09"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_09"))
    CALL(make_msg, STRING("op_book_10"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_10"))
    CALL(make_msg, STRING("op_book_11"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_11"))
    CALL(make_msg, STRING("op_book_12"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_12"))
    CALL(demo_winonoff, 0)
    CALL(evt_img_set_paper, STRING("img2"), STRING("OFF_d_meku_right"))
    CALL(evt_img_clear_virtual_point, STRING("img2"))
    CALL(evt_img_alloc_capture, STRING("img2"), 0, 0, 1, 124, 0, 386, 480)
    CALL(evt_img_onoff, STRING("img2"), 1)
    WAIT_FRAMES(1)
    CALL(next)
    CALL(evt_snd_sfxon_, STRING("SFX_EVT_OPN_PAGE_OPEN1"), 0)
    CALL(evt_img_set_paper_anim, STRING("img2"), STRING("A_1"))
    CALL(evt_img_wait_animend, STRING("img2"))
    CALL(evt_img_free_capture, STRING("img1"), 0)
    CALL(evt_img_release, STRING("img1"))
    WAIT_MS(1000)
    CALL(demo_winonoff, 1)
    CALL(make_msg, STRING("op_book_13"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_13"))
    CALL(make_msg, STRING("op_book_14"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_14"))
    CALL(make_msg, STRING("op_book_15"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_15"))
    CALL(make_msg, STRING("op_book_16"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_16"))
    CALL(demo_winonoff, 0)
    CALL(evt_img_set_paper, STRING("img3"), STRING("OFF_d_meku_right"))
    CALL(evt_img_clear_virtual_point, STRING("img3"))
    CALL(evt_img_alloc_capture, STRING("img3"), 0, 0, 1, 124, 0, 386, 480)
    CALL(evt_img_onoff, STRING("img3"), 1)
    WAIT_FRAMES(1)
    CALL(next)
    CALL(evt_snd_sfxon_, STRING("SFX_EVT_OPN_PAGE_OPEN1"), 0)
    CALL(evt_img_set_paper_anim, STRING("img3"), STRING("A_1"))
    CALL(evt_img_wait_animend, STRING("img3"))
    CALL(evt_img_free_capture, STRING("img2"), 0)
    CALL(evt_img_release, STRING("img2"))
    WAIT_MS(1000)
    CALL(demo_winonoff, 1)
    CALL(make_msg, STRING("op_book_17"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_17"))
    CALL(make_msg, STRING("op_book_18"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_18"))
    CALL(make_msg, STRING("op_book_19"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_19"))
    CALL(make_msg, STRING("op_book_20"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_20"))
    CALL(make_msg, STRING("op_book_21"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_21"))
    CALL(demo_winonoff, 0)
    CALL(evt_img_set_paper, STRING("img4"), STRING("OFF_d_meku_right"))
    CALL(evt_img_clear_virtual_point, STRING("img4"))
    CALL(evt_img_alloc_capture, STRING("img4"), 0, 0, 1, 124, 0, 386, 480)
    CALL(evt_img_onoff, STRING("img4"), 1)
    WAIT_FRAMES(1)
    CALL(next)
    CALL(evt_snd_sfxon_, STRING("SFX_EVT_OPN_PAGE_OPEN1"), 0)
    CALL(evt_img_set_paper_anim, STRING("img4"), STRING("A_1"))
    CALL(evt_img_wait_animend, STRING("img4"))
    CALL(evt_img_free_capture, STRING("img3"), 0)
    CALL(evt_img_release, STRING("img3"))
    WAIT_MS(1000)
    CALL(demo_winonoff, 1)
    CALL(make_msg, STRING("op_book_22"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_22"))
    CALL(make_msg, STRING("op_book_23"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_23"))
    CALL(make_msg, STRING("op_book_24"))
    CALL(evt_msg_print, 1, PTR(buf), 0, 0)
    CALL(evt_msg_print_add, 0, STRING("op_book_24"))
    CALL(demo_winonoff, 0)
    CALL(evt_bg_disp_onoff, 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_01_naka"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_02_f1_naka"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_02_f2_naka"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_03_naka"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_04_naka"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_gai_kabe_k"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_gai_kabe_k_01"), 1)
    CALL(evt_npc_flag_onoff, 0, STRING("peach"), 128)
    CALL(evt_npc_flag_onoff, 0, STRING("nazo"), 128)
    CALL(evt_npc_flag_onoff, 0, STRING("hako"), 128)
    CALL(evt_npc_flag_onoff, 0, STRING("ロテン１"), 128)
    CALL(evt_npc_flag_onoff, 0, STRING("クリボー１"), 128)
    CALL(evt_npc_flag_onoff, 0, STRING("チューさん１"), 128)
    CALL(evt_mobj_flag_onoff, 0, STRING("hako"), 16384)
    CALL(evt_snd_bgmoff_f, 513, 6000)
    CALL(evt_sub_intpl_msec_init, 0, 0, 1000, 3000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
        CALL(adjust, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_img_free_capture, STRING("img4"), 0)
    CALL(evt_img_release, STRING("img4"))
    DELETE_EVENT(GW(0))
    CALL(evt_paper_delete, STRING("OFF_d_meku_right"))
    LABEL(100)
    WAIT_MS(1000)
    CALL(evt_snd_bgmon, 513, STRING("BGM_DEMO2"))
    CALL(evt_npc_move_position, STRING("peach"), 0, 450, 0, FLOAT(60.0f), 1)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("op_00"), 0, 0)
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 0, 100, 834, 0, 50, 437, 800, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("op_01"), 0, STRING("peach"))
    CALL(evt_npc_set_tribe, STRING("peach"), STRING("デモピーチ標準"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 0, 200, 837, 0, 200, 437, 4000, 11)
    WAIT_MS(300)
    LOOP(4)
        CALL(evt_npc_reverse_ry, STRING("peach"))
        WAIT_MS(500)
    END_LOOP
    CALL(evt_msg_print, 0, STRING("op_02"), 0, 0)
    WAIT_MS(300)
    WAIT_MS(1000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("op_03"), 0, STRING("nazo"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 0, 89, 739, 0, 44, 442, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("peach"), STRING("P_B_1"))
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("peach"), STRING("P_B_2"))
    WAIT_MS(300)
    CALL(evt_npc_reverse_ry, STRING("peach"))
    WAIT_MS(800)
    CALL(evt_npc_set_autotalkpose, STRING("peach"), STRING("P_B_2"), STRING("P_T_7"))
    CALL(evt_msg_print, 0, STRING("op_04"), 0, STRING("peach"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -100, 89, 739, -100, 44, 442, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_npc_set_tribe, STRING("peach"), STRING("デモピーチ標準"))
    CALL(evt_npc_move_position, STRING("peach"), -55, 450, 0, FLOAT(60.0f), 1)
    CALL(evt_msg_print, 0, STRING("op_05"), 0, STRING("nazo"))
    WAIT_MS(300)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("op_06"), 0, STRING("peach"))
    WAIT_MS(300)
    CALL(evt_npc_set_tribe, STRING("peach"), STRING("デモピーチ不安"))
    WAIT_MS(1000)
    CALL(evt_npc_set_anim, STRING("peach"), STRING("P_H_1"))
    CALL(evt_eff_fukidashi, 1, 0, STRING("peach"), 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1500)
    CALL(evt_npc_set_anim, STRING("peach"), STRING("P_S_7"))
    CALL(evt_npc_set_tribe, STRING("peach"), STRING("デモピーチ標準2"))
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, -120, 89, 739, -120, 44, 442, 1000, 11)
    CALL(evt_npc_move_position, STRING("peach"), -80, 450, 0, FLOAT(60.0f), 1)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("op_07"), 0, STRING("peach"))
    WAIT_MS(300)
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("hako"), LW(7), LW(8), LW(9))
    CALL(evt_sub_intpl_msec_init, 11, 0, 100, 1000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_npc_set_position, STRING("hako"), LW(7), LW(0), LW(9))
        SUBTRACT_FLOAT(LW(7), FLOAT(0.1f))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_msg_print, 0, STRING("op_08"), 0, STRING("nazo"))
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("nazo"), STRING("A_1"))
    WAIT_MS(700)
    CALL(evt_npc_set_anim, STRING("nazo"), STRING("S_1"))
    CALL(evt_npc_set_tribe, STRING("peach"), STRING("デモピーチ不安"))
    CALL(evt_msg_print, 0, STRING("op_09"), 0, STRING("peach"))
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, -450, 250, 1030, -135, 100, 482, 0, 11)
    CALL(evt_npc_flag_onoff, 1, STRING("peach"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("nazo"), 32768)
    CALL(evt_npc_set_tribe, STRING("peach"), STRING("デモピーチ標準2"))
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_gai_kabe_k"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_gai_kabe_k_01"), 1)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("op_10"), 0, STRING("nazo"))
    WAIT_MS(300)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("op_11"), 0, STRING("peach"))
    WAIT_MS(300)
    CALL(evt_npc_status_onoff, 0, STRING("peach"), 2)
    CALL(evt_npc_status_onoff, 0, STRING("nazo"), 2)
    SET_VALUE(GF(0), 0)
    SET_VALUE(GF(1), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_npc_get_position, STRING("peach"), LW(0), LW(1), LW(2))
        SUBTRACT(LW(0), 20)
        CALL(evt_npc_move_position, STRING("peach"), LW(0), LW(2), 0, FLOAT(40.0f), 1)
        SET_VALUE(GF(0), 1)
        LOOP(60)
            CALL(evt_npc_get_position, STRING("hako"), LW(0), LW(1), LW(2))
            ADD_FLOAT(LW(0), FLOAT(0.08301f))
            ADD_FLOAT(LW(1), FLOAT(0.16602f))
            CALL(evt_npc_set_position, STRING("hako"), LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(GF(1), 1)
        CALL(evt_npc_set_tribe, STRING("peach"), STRING("デモピーチ物持ち"))
        WAIT_MS(300)
        CALL(evt_npc_get_position, STRING("peach"), LW(0), LW(1), LW(2))
        ADD(LW(0), 20)
        CALL(evt_npc_move_position, STRING("peach"), LW(0), LW(2), 0, FLOAT(40.0f), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("hako"), LW(0), LW(1), LW(2))
        CALL(evt_npc_get_position, STRING("nazo"), LW(3), LW(4), LW(5))
        SUBTRACT_FLOAT(LW(0), LW(3))
        LOOP(0)
            CALL(evt_npc_get_position, STRING("nazo"), LW(10), LW(11), LW(12))
            ADD_FLOAT(LW(10), LW(0))
            CALL(evt_npc_set_position, STRING("hako"), LW(10), LW(1), LW(2))
            IF_INT_EQUAL(GF(0), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(0)
            IF_INT_EQUAL(GF(1), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_npc_get_position, STRING("hako"), LW(0), LW(1), LW(2))
        CALL(evt_npc_get_position, STRING("peach"), LW(3), LW(4), LW(5))
        SUBTRACT_FLOAT(LW(0), LW(3))
        LOOP(0)
            CALL(evt_npc_get_position, STRING("peach"), LW(10), LW(11), LW(12))
            ADD_FLOAT(LW(10), LW(0))
            CALL(evt_npc_set_position, STRING("hako"), LW(10), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_npc_get_position, STRING("nazo"), LW(0), LW(1), LW(2))
    ADD(LW(0), 20)
    CALL(evt_npc_move_position, STRING("nazo"), LW(0), LW(2), 0, FLOAT(40.0f), 1)
    WAIT_MS(300)
    WAIT_MS(300)
    CALL(evt_npc_get_position, STRING("nazo"), LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 20)
    CALL(evt_npc_move_position, STRING("nazo"), LW(0), LW(2), 0, FLOAT(40.0f), 1)
    WAIT_MS(300)
    CALL(evt_npc_set_ry, STRING("hako"), 225)
    CALL(evt_cam3d_evt_set, -90, 89, 739, -90, 44, 442, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("op_12"), 0, STRING("peach"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -85, 50, 590, -85, 28, 442, 15000, 11)
    WAIT_MS(1000)
    CALL(evt_npc_set_camid, STRING("hako"), 5)
    CALL(evt_npc_set_anim, STRING("hako"), STRING("A_1"))
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("hako"), LW(0), LW(1), LW(2))
        ADD(LW(1), 10)
        CALL(evt_eff64, STRING("eff0"), STRING("crystal_n64"), 2, LW(0), LW(1), LW(2), FLOAT(0.5f), 0, 0, 0, 0, 0, 0, 0)
        CALL(evt_eff64, STRING("eff1"), STRING("crystal_n64"), 6, LW(0), LW(1), LW(2), FLOAT(0.5f), 60, 0, 0, 0, 0, 0, 0)
        CALL(eff_cam)
        WAIT_MS(500)
        CALL(evt_eff64, STRING("eff2"), STRING("radiation_n64"), 3, LW(0), LW(1), LW(2), FLOAT(0.5f), 0, 0, 0, 0, 0, 0, 0)
        CALL(eff_cam1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_npc_set_anim, STRING("peach"), STRING("P_B_3"))
        WAIT_MS(300)
        CALL(evt_npc_set_anim, STRING("peach"), STRING("P_B_4"))
    END_INLINE_EVENT
    WAIT_MS(2000)
    LOOP(0)
        IF_INT_EQUAL(GF(10), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    DELETE_EVENT(GW(10))
    CALL(evt_fade_set_mapchange_type, 1, 17, 2000, -1, -1)
    CALL(evt_seq_set_seq, 3, STRING("title"), 0)
    RETURN
EVT_END()

static EVT_BEGIN(koushukei_dai)
    LOOP(0)
        CALL(evt_sub_intpl_msec_init, 11, 10, -10, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_mapobj_rotate, 0, STRING("S_rop"), LW(0), 0, 0)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_FRAMES(1)
        CALL(evt_sub_intpl_msec_init, 11, -10, 10, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_mapobj_rotate, 0, STRING("S_rop"), LW(0), 0, 0)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(roten_move)
    LOOP(0)
        CALL(evt_npc_move_position, STRING("ロテン１"), 450, 150, 0, FLOAT(32.0f), 0)
        WAIT_MS(1000)
        CALL(evt_npc_move_position, STRING("ロテン１"), -350, 150, 0, FLOAT(32.0f), 0)
        WAIT_MS(1000)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(kuribo_move)
    CALL(evt_npc_set_position, STRING("クリボー１"), 0, 0, 0)
    CALL(evt_npc_get_position, STRING("クリボー１"), LW(0), LW(1), LW(2))
    CALL(urouro_init_func, STRING("クリボー１"), LW(0), LW(2), FLOAT(100.0f), FLOAT(30.0f), 0)
    CALL(urouro_main_func, STRING("クリボー１"))
    RETURN
EVT_END()

EVT_BEGIN(chusan_move)
    CALL(evt_npc_set_position, STRING("チューさん１"), -175, 0, 100)
    CALL(evt_npc_get_position, STRING("チューさん１"), LW(0), LW(1), LW(2))
    CALL(urouro_init_func, STRING("チューさん１"), LW(0), LW(2), FLOAT(100.0f), FLOAT(30.0f), 0)
    CALL(urouro_main_func, STRING("チューさん１"))
    RETURN
EVT_END()

EVT_BEGIN(dmo_00_init_evt)
    CALL(demo_init)
    CALL(evt_npc_entry, STRING("peach"), STRING("c_peach"))
    CALL(evt_npc_set_tribe, STRING("peach"), STRING("デモピーチ標準2"))
    CALL(evt_npc_set_ry, STRING("peach"), 90)
    CALL(evt_npc_set_position, STRING("peach"), -2, 0, 550)
    CALL(evt_npc_status_onoff, 1, STRING("peach"), 2)
    CALL(evt_npc_flag_onoff, 1, STRING("peach"), 1074267648)
    CALL(evt_npc_entry, STRING("nazo"), STRING("c_nazo"))
    CALL(evt_npc_set_tribe, STRING("nazo"), STRING("謎の人"))
    CALL(evt_npc_set_ry, STRING("nazo"), 90)
    CALL(evt_npc_set_position, STRING("nazo"), -145, 0, 450)
    CALL(evt_npc_status_onoff, 1, STRING("nazo"), 2)
    CALL(evt_npc_flag_onoff, 1, STRING("nazo"), 1074266112)
    CALL(evt_npc_set_height, STRING("nazo"), 40)
    CALL(evt_npc_entry, STRING("hako"), STRING("MOBJ_GoldenTreasureBox"))
    CALL(evt_npc_set_anim, STRING("hako"), STRING("S_1"))
    CALL(evt_npc_set_position, STRING("hako"), -125, 0, 450)
    CALL(evt_npc_flag_onoff, 1, STRING("hako"), 1073741824)
    CALL(evt_npc_pera_onoff, STRING("hako"), 0)
    CALL(evt_mobj_itembox, STRING("hako"), 340, 110, -20, 1, 0, 0, 0)
    CALL(evt_mobj_flag_onoff, 1, STRING("hako"), 16384)
    CALL(evt_npc_entry, STRING("ロテン１"), STRING("c_roten"))
    CALL(evt_npc_set_tribe, STRING("ロテン１"), STRING("ロテン"))
    CALL(evt_npc_flag_onoff, 1, STRING("ロテン１"), 1074267648)
    CALL(evt_npc_status_onoff, 1, STRING("ロテン１"), 2)
    CALL(evt_npc_change_interrupt, STRING("ロテン１"), 1, PTR(roten_move))
    CALL(evt_npc_entry, STRING("クリボー１"), STRING("c_kuribo_n"))
    CALL(evt_npc_set_tribe, STRING("クリボー１"), STRING("NPCクリボー"))
    CALL(evt_npc_flag_onoff, 1, STRING("クリボー１"), 1074267648)
    CALL(evt_npc_status_onoff, 1, STRING("クリボー１"), 2)
    CALL(evt_npc_change_interrupt, STRING("クリボー１"), 1, PTR(kuribo_move))
    CALL(evt_npc_entry, STRING("チューさん１"), STRING("c_tyusan"))
    CALL(evt_npc_set_tribe, STRING("チューさん１"), STRING("チューさん"))
    CALL(evt_npc_flag_onoff, 1, STRING("チューさん１"), 1074267648)
    CALL(evt_npc_status_onoff, 1, STRING("チューさん１"), 2)
    CALL(evt_npc_change_interrupt, STRING("チューさん１"), 1, PTR(chusan_move))
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_paper_entry, STRING("OFF_d_meku_right"))
    CALL(evt_cam3d_evt_set, 0, 385, 1473, 0, 100, 0, 0, 11)
    CALL(evt_cam_letter_box_onoff, 1, 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S"), 1)
    CALL(evt_bg_disp_onoff, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("peach"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("nazo"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("hako"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("ロテン１"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("クリボー１"), 128)
    CALL(evt_npc_flag_onoff, 1, STRING("チューさん１"), 128)
    BEGIN_INLINE_EVENT_GET_ID(GW(0))
        SET_TYPE(0)
        CALL(demo_draw)
    END_INLINE_EVENT
    SPAWN_EVENT(demo_evt)
    SPAWN_EVENT(koushukei_dai)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_animgroup_async, STRING("OFF_d_maku_center"))
    END_INLINE_EVENT
    RETURN
EVT_END()


// clang-format on
