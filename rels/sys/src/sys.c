#include "sys.h"
#include "data/mapdata.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_itemget.h"
#include "eff/n64/eff_stardust_n64.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_fade.h"
#include "evt/evt_mario.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "manager/filemgr.h"
#include "manager/fontmgr.h"
#include "driver/msgdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "pmario_sound.h"
#include "sdk/texPalette.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <string.h>

// Provided without <math.h> so the compiler does not emit the unused sqrt/sqrtf intrinsic constants.
extern f64 fmod(f64, f64);
extern f64 tan(f64);

#pragma pool_data off

#pragma force_active on

// Work data for the sys_01 stage-select icons (size 0x104); SysWork is declared in sys.h.

typedef void (*voidfunctionptr)(void);

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

static void _unresolved(void);
static void _epilog(void);
s32 sys_01_init_evt[135];
s32 evt_start[189];

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

EVT_BEGIN(dummy_evt)
    RETURN
EVT_END()

// TODO: type
static u32 niji_tbl[2] = {
    0x00000013, 0x00000016,
};

static char* msg_tbl[9] = {
    "msg_stg_seq_prolouge", "msg_stg_seq_stg1", "msg_stg_seq_stg2", "msg_stg_seq_stg3",
    "msg_stg_seq_stg4", "msg_stg_seq_stg5", "msg_stg_seq_stg6", "msg_stg_seq_stg7",
    "msg_stg_seq_stg8",
};

SysWork* g_wp;

void _prolog(void) {
    voidfunctionptr* ctor;

    for (ctor = _ctors; *ctor != NULL; ctor++) {
        (*ctor)();
    }
    relSetEvtAddr("sys_00", dummy_evt);
    relSetEvtAddr("sys_01", sys_01_init_evt);
    if (evtGetValue(0, 0xF5DE0180) != 2 && evtGetValue(0, 0xF5DE0180) != 0x7D &&
        evtGetValue(0, 0xF5DE0180) != 0xE5 && evtGetValue(0, 0xF5DE0180) != 0x118) {
        psndStopAllFadeOut();
    }
    camGetPtr(CAMERA_2D)->flags |= 0x200;
    camGetPtr(CAMERA_2D)->flags &= ~0x100;
    camGetPtr(CAMERA_2D)->flags |= 0x400;
}

static void _epilog(void) {
    voidfunctionptr* dtor;

    if (strcmp(gp->currentMapName, "sys_01") == 0) {
        if (g_wp->handle != NULL) {
            fileFree(g_wp->handle);
        }
        g_wp = NULL;
    }
    camGetPtr(CAMERA_2D)->flags &= ~0x200;
    for (dtor = _dtors; *dtor != NULL; dtor++) {
        (*dtor)();
    }
}

static void _unresolved(void) {
    (void)0;
}

void sys_01_release(void) {
    if (g_wp->handle != NULL) {
        fileFree(g_wp->handle);
    }
    g_wp = NULL;
}

s32 getLightColor(f32 angle) {
    s32 result;
    f32 a = (f32)fmod(angle, 360.0);

    if (a >= 270.0f) {
        result = 360.0f - a;
    } else if (a >= 180.0f) {
        result = a - 180.0f;
    } else if (a >= 90.0f) {
        result = 180.0f - a;
    } else {
        result = a;
    }
    return 0xFF - result * 2;
}

// display_func's local initialisers (the colours, the light position and the camera vectors) pool their
// templates in .rodata between msg_tbl's strings and lang_prefix's, so the functions up to here are defined
// before the language tables. The material colours and the camera position are compound literals: MWCC gives
// each one its own temp after the call's argument copy, as in the target's stack layout.
void display_func(CameraId cameraId, void* param) {
    SysWork* work = param;
    CameraEntry* cam = camGetPtr(cameraId);
    GXLightObj light;
    GXTexObj tex;
    Mtx mtx;
    Mtx mtx2;
    Mtx mtx3;
    Mtx mtx4;
    Mtx mtx5;
    Vec pos;
    s32 j;
    s32 frame;
    const char* msg;
    s32 revealed;
    u16 cbuf[2];
    s32 i;
    s32 lightColor;
    s32 alpha;
    f32 halfW;
    f32 halfH;
    const u16* msg16;
    f32 w;
    frame = work->field_0x10;
    work->field_0x10 = frame + 1;
    if (frame > 0x500) {
        frame -= 0x500;
    }

    {
        GXColor amb = { 0x80, 0x80, 0x80, 0xFF };
        GXSetChanAmbColor(GX_COLOR0A0, amb);
    }

    {
        Vec lpos = { 0.0f, 0.0f, 300.0f };
        pos = lpos;
    }
    PSMTXMultVec((MtxPtr)&cam->view, &pos, &pos);
    GXInitLightPos(&light, pos.x, pos.y, pos.z);
    {
        GXColor white = { 0xFF, 0xFF, 0xFF, 0xFF };
        GXInitLightColor(&light, white);
    }
    GXInitLightAttn(&light, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    GXLoadLightObjImm(&light, GX_LIGHT0);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetZCompLoc(GX_TRUE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    work->field_0x8 += 2.5f;
    if (work->field_0x8 > 180.0f) {
        work->field_0x8 = 0.0f;
        work->field_0xC++;
        work->field_0xC %= 4;
    }

    lightColor = getLightColor(90.0f + work->field_0x8);
    GXSetChanMatColor(GX_COLOR0A0, (GXColor){ lightColor, lightColor, lightColor, 0xFF });

    PSMTXTrans(mtx, 0.0f, -40.0f, 0.0f);
    PSMTXRotRad(mtx3, 'y', 0.017453292f * (90.0f + work->field_0x8));
    PSMTXConcat(mtx, mtx3, mtx);
    PSMTXConcat((MtxPtr)&cam->view, mtx, mtx);
    GXLoadPosMtxImm(mtx, GX_PNMTX0);
    PSMTXInvXpose(mtx, mtx);
    GXLoadNrmMtxImm(mtx, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);

    TEXGetGXTexObjFromPalette((TEXPalette*)work->handle->data[0], &tex, (u16)work->field_0xC);
    GXLoadTexObj(&tex, GX_TEXMAP0);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    halfW = (f32)(u16)GXGetTexObjWidth(&tex) / 2.0f;
    halfH = (f32)(u16)GXGetTexObjHeight(&tex) / 2.0f;
    GXPosition3f32(-halfW / 2.0f, halfH / 2.0f, 0.0f);
    GXNormal3f32(0.0f, 0.0f, 1.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(halfW / 2.0f, halfH / 2.0f, 0.0f);
    GXNormal3f32(0.0f, 0.0f, 1.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(halfW / 2.0f, -halfH / 2.0f, 0.0f);
    GXNormal3f32(0.0f, 0.0f, 1.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(-halfW / 2.0f, -halfH / 2.0f, 0.0f);
    GXNormal3f32(0.0f, 0.0f, 1.0f);
    GXTexCoord2f32(0.0f, 1.0f);

    msg = msgSearch((const char*)msg_tbl[work->field_0x14]);
    FontDrawStart();

    revealed = work->field_0x18 / 256;
    alpha = work->field_0x18 % 256;
    msg16 = (const u16*)msg;
    for (j = 0; j < 0x2C; j += 2, msg16++) {
        if (j >= strlen(msg)) {
            break;
        }

        cbuf[0] = *msg16;
        *(u8*)&cbuf[1] = 0;
        PSMTXTrans(mtx, 0.0f, -40.0f, 0.0f);
        PSMTXRotRad(mtx3, 'z',
                    0.017453292f *
                        (-8.181818f * (f32)j + work->field_0x1C));
        PSMTXConcat(mtx, mtx3, mtx3);
        PSMTXTrans(mtx, 0.0f, 57.5f, 0.0f);
        PSMTXConcat(mtx3, mtx, mtx);
        PSMTXScale(mtx2, 0.6f, 0.6f, 0.6f);
        PSMTXConcat(mtx, mtx2, mtx);
        if (j / 2 < revealed) {
            FontDrawStart_alpha(0xFF);
            FontDrawStringMtx(mtx, (const char*)cbuf);
        } else {
            FontDrawStart_alpha(alpha);
            FontDrawStringMtx(mtx, (const char*)cbuf);
            break;
        }
    }

    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetZCompLoc(GX_TRUE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTexGens(2);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_POS, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXSetNumTevStages(4);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_COMP_RGB8_GT, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_COMP_A8_GT, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, 8, 0xf, 8, 0xf);
    GXSetTevAlphaIn(GX_TEVSTAGE0, 4, 7, 4, 7);
    GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_RED);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP1, GX_TEV_SWAP1);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE1, 0xf, 0, 8, 0xf);
    GXSetTevAlphaIn(GX_TEVSTAGE1, 7, 7, 7, 0);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE2, 2, 0, 1, 0xf);
    GXSetTevAlphaIn(GX_TEVSTAGE2, 7, 7, 7, 4);
    {
        GXColor tc = { 0x36, 0x36, 0x36, 0xFF };
        GXSetTevColor(GX_TEVREG0, tc);
    }
    GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE3, 0xf, 0xa, 0, 0xf);
    GXSetTevAlphaIn(GX_TEVSTAGE3, 7, 5, 0, 7);
    GXSetTevSwapMode(GX_TEVSTAGE3, GX_TEV_SWAP0, GX_TEV_SWAP0);

    for (i = 0; i < work->count; i++) {
        SysWorkEntry* entry = &work->entries[i];

        if (entry->field_0x14 == 0) {
            break;
        }

        TEXGetGXTexObjFromPalette((TEXPalette*)work->handle->data[0], &tex, (u16)entry->field_0x18);
        GXLoadTexObj(&tex, GX_TEXMAP0);
        TEXGetGXTexObjFromPalette((TEXPalette*)work->handle->data[0], &tex, (u16)niji_tbl[gp->language]);
        GXLoadTexObj(&tex, GX_TEXMAP1);
        lightColor = getLightColor(entry->angle);
        GXSetChanMatColor(GX_COLOR0A0, (GXColor){ lightColor, lightColor, lightColor, 0xFF });
        if ((u32)gp->language != 0) {
            PSMTXScale(mtx2, 0.8f, 0.8f, 0.8f);
        } else {
            PSMTXScale(mtx2, 1.0f, 1.0f, 1.0f);
        }
        PSMTXTrans(mtx, entry->x, entry->y, entry->z);
        PSMTXRotRad(mtx3, 'y', 0.017453292f * entry->angle);
        PSMTXConcat(mtx2, mtx, mtx);
        PSMTXConcat(mtx, mtx3, mtx);
        PSMTXConcat((MtxPtr)&cam->view, mtx, mtx);
        GXLoadPosMtxImm(mtx, GX_PNMTX0);
        PSMTXInvXpose(mtx, mtx);
        GXLoadNrmMtxImm(mtx, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        {
            Vec camPos = (Vec){ 0.0f, 0.0f, 1.0f / (f32)tan(0.21816616f) };
            Vec up = { 0.0f, 1.0f, 0.0f };
            Vec at = { 0.0f, 0.0f, 0.0f };
            C_MTXLightPerspective(mtx4, 25.0f, 640.0f, 0.5f, 0.5f, 0.5f, 0.5f);
            C_MTXLookAt(mtx5, &camPos, &up, &at);
        }
        PSMTXConcat(mtx4, mtx5, mtx4);
        if ((u32)gp->language != 0) {
            PSMTXScale(mtx2, 0.5f, 0.5f, 0.5f);
        } else {
            PSMTXScale(mtx2, 1.0f, 1.0f, 1.0f);
        }
        PSMTXTrans(mtx, entry->x, entry->y, entry->z);
        PSMTXConcat(mtx2, mtx, mtx);
        PSMTXConcat(mtx4, mtx, mtx);
        PSMTXTrans(mtx2, (f32)-frame, 0.0f, 0.0f);
        PSMTXConcat(mtx, mtx2, mtx);
        GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4);
        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        w = 44.0f;
        GXPosition3f32(-w / 2.0f, w / 2.0f, 0.0f);
        GXNormal3f32(0.0f, 0.0f, 1.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(w / 2.0f, w / 2.0f, 0.0f);
        GXNormal3f32(0.0f, 0.0f, 1.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(w / 2.0f, -w / 2.0f, 0.0f);
        GXNormal3f32(0.0f, 0.0f, 1.0f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(-w / 2.0f, -w / 2.0f, 0.0f);
        GXNormal3f32(0.0f, 0.0f, 1.0f);
        GXTexCoord2f32(0.0f, 1.0f);
    }

    GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
}



// TODO: type
u32 prologue_jp_tbl[2] = {
    0x10111106, 0x12FF0000,
};

// TODO: type
u32 stage1_jp_tbl[2] = {
    0x04050607, 0x08FF0000,
};

// TODO: type
u32 stage2_jp_tbl[2] = {
    0x04050607, 0x09FF0000,
};

// TODO: type
u32 stage3_jp_tbl[2] = {
    0x04050607, 0x0AFF0000,
};

// TODO: type
u32 stage4_jp_tbl[2] = {
    0x04050607, 0x0BFF0000,
};

// TODO: type
u32 stage5_jp_tbl[2] = {
    0x04050607, 0x0CFF0000,
};

// TODO: type
u32 stage6_jp_tbl[2] = {
    0x04050607, 0x0DFF0000,
};

// TODO: type
u32 stage7_jp_tbl[2] = {
    0x04050607, 0x0EFF0000,
};

// TODO: type
u32 stage8_jp_tbl[2] = {
    0x04050607, 0x0FFF0000,
};

// TODO: type
u32 texid_jp_tbl[9] = {
    (u32)&prologue_jp_tbl, (u32)&stage1_jp_tbl, (u32)&stage2_jp_tbl, (u32)&stage3_jp_tbl,
    (u32)&stage4_jp_tbl, (u32)&stage5_jp_tbl, (u32)&stage6_jp_tbl, (u32)&stage7_jp_tbl,
    (u32)&stage8_jp_tbl,
};

// TODO: type
u32 prologue_us_tbl[3] = {
    0x11121314, 0x13071508, 0xFF000000,
};

// TODO: type
u32 stage1_us_tbl[2] = {
    0x04050607, 0x0809FF00,
};

// TODO: type
u32 stage2_us_tbl[2] = {
    0x04050607, 0x080AFF00,
};

// TODO: type
u32 stage3_us_tbl[2] = {
    0x04050607, 0x080BFF00,
};

// TODO: type
u32 stage4_us_tbl[2] = {
    0x04050607, 0x080CFF00,
};

// TODO: type
u32 stage5_us_tbl[2] = {
    0x04050607, 0x080DFF00,
};

// TODO: type
u32 stage6_us_tbl[2] = {
    0x04050607, 0x080EFF00,
};

// TODO: type
u32 stage7_us_tbl[2] = {
    0x04050607, 0x080FFF00,
};

// TODO: type
u32 stage8_us_tbl[2] = {
    0x04050607, 0x0810FF00,
};

// TODO: type
u32 texid_us_tbl[9] = {
    (u32)&prologue_us_tbl, (u32)&stage1_us_tbl, (u32)&stage2_us_tbl, (u32)&stage3_us_tbl,
    (u32)&stage4_us_tbl, (u32)&stage5_us_tbl, (u32)&stage6_us_tbl, (u32)&stage7_us_tbl,
    (u32)&stage8_us_tbl,
};

// TODO: type
u32 lang_tbl[2] = {
    (u32)&texid_jp_tbl, (u32)&texid_us_tbl,
};

char* lang_prefix[2] = {
    "jp", "us",
};

EVT_BEGIN(sys_01_init_evt)
    CALL(init)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 0, 0, 0)
    CALL(evt_party_set_pos, 0, 0, 0, 0)
    CALL(evt_cam3d_evt_set, 0, 0, 500, 0, 0, 0, 0, 11)
    CALL(bg)
    CALL(evt_fade_entry, 32, 0, 0, 0, 0)
    CALL(evt_fade_end_wait)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), 0)
        CALL(evt_bero_get_entername, LW(1))
        IF_STR_EQUAL(LW(1), STRING("prologue"))
            SET_VALUE(LW(0), 0)
        END_IF
        IF_STR_EQUAL(LW(1), STRING("stg_1"))
            SET_VALUE(LW(0), 1)
        END_IF
        IF_STR_EQUAL(LW(1), STRING("stg_2"))
            SET_VALUE(LW(0), 2)
        END_IF
        IF_STR_EQUAL(LW(1), STRING("stg_3"))
            SET_VALUE(LW(0), 3)
        END_IF
        IF_STR_EQUAL(LW(1), STRING("stg_4"))
            SET_VALUE(LW(0), 4)
        END_IF
        IF_STR_EQUAL(LW(1), STRING("stg_5"))
            SET_VALUE(LW(0), 5)
        END_IF
        IF_STR_EQUAL(LW(1), STRING("stg_6"))
            SET_VALUE(LW(0), 6)
        END_IF
        IF_STR_EQUAL(LW(1), STRING("stg_7"))
            SET_VALUE(LW(0), 7)
        END_IF
        IF_STR_EQUAL(LW(1), STRING("stg_8"))
            SET_VALUE(LW(0), 8)
        END_IF
        CALL(display, LW(0))
    END_INLINE_EVENT
    WAIT_MS(3000)
    CALL(evt_fade_set_mapchange_type, 0, 17, 1800, 16, 1800)
    CALL(evt_fade_set_mapchange_type, 1, 17, 1800, 16, 1800)
    SPAWN_EVENT(evt_start)
    RETURN
EVT_END()

EVT_BEGIN(evt_start)
    CALL(evt_fade_end_wait)
    WAIT_MS(2000)
    CALL(evt_bero_get_entername, LW(1))
    IF_STR_NOT_EQUAL(LW(1), STRING("stg_3"))
        IF_STR_NOT_EQUAL(LW(1), STRING("stg_5"))
            IF_STR_NOT_EQUAL(LW(1), STRING("stg_6"))
                CALL(evt_snd_bgmon, 512, STRING("BGM_FF_STG_START1"))
            END_IF
        END_IF
    END_IF
    CALL(evt_fade_entry, 35, 0, 0, 0, 0)
    CALL(evt_fade_end_wait)
    CALL(start)
    WAIT_MS(5000)
    LOOP(0)
        CALL(evt_key_get_buttontrg, 0, LW(0))
        IF_BITS_SET(LW(0), 4864)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_fade_set_mapchange_type, 0, 19, 500, 18, 500)
    CALL(evt_fade_set_mapchange_type, 1, 19, 500, 18, 500)
    CALL(evt_bero_get_entername, LW(1))
    IF_STR_EQUAL(LW(1), STRING("prologue"))
        SET_VALUE(LW(0), 0)
    END_IF
    IF_STR_EQUAL(LW(1), STRING("stg_1"))
        SET_VALUE(LW(0), 1)
    END_IF
    IF_STR_EQUAL(LW(1), STRING("stg_2"))
        SET_VALUE(LW(0), 2)
    END_IF
    IF_STR_EQUAL(LW(1), STRING("stg_3"))
        SET_VALUE(LW(0), 3)
    END_IF
    IF_STR_EQUAL(LW(1), STRING("stg_4"))
        SET_VALUE(LW(0), 4)
    END_IF
    IF_STR_EQUAL(LW(1), STRING("stg_5"))
        SET_VALUE(LW(0), 5)
    END_IF
    IF_STR_EQUAL(LW(1), STRING("stg_6"))
        SET_VALUE(LW(0), 6)
    END_IF
    IF_STR_EQUAL(LW(1), STRING("stg_7"))
        SET_VALUE(LW(0), 7)
    END_IF
    IF_STR_EQUAL(LW(1), STRING("stg_8"))
        SET_VALUE(LW(0), 8)
    END_IF
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(evt_bero_mapchange, STRING("gor_10"), 0)
        CASE_EQUAL(1)
        CALL(evt_bero_mapchange, STRING("hei_00"), STRING("dokan_2"))
        CASE_EQUAL(2)
        CALL(evt_bero_mapchange, STRING("win_06"), STRING("dokan1"))
        CASE_EQUAL(3)
        CALL(evt_bero_mapchange, STRING("tou_20"), 0)
        CASE_EQUAL(4)
        CALL(evt_bero_mapchange, STRING("usu_00"), STRING("dokan_1"))
        CASE_EQUAL(5)
        CALL(evt_bero_mapchange, STRING("muj_20"), 0)
        CASE_EQUAL(6)
        CALL(evt_bero_mapchange, STRING("rsh_02_a"), 0)
        CASE_EQUAL(7)
        CALL(evt_bero_mapchange, STRING("bom_00"), STRING("dokan"))
        CASE_EQUAL(8)
        CALL(evt_bero_mapchange, STRING("las_00"), STRING("w_bero"))
    END_SWITCH
    RETURN
EVT_END()



// The outer switch on work->state is a switch over {0, 2, 3} (state 3 increments on past 2 in start());
// MWCC 2.6 then emits the target's 2-first tree and the dead `b default` leaf at 0x278. Writing it as
// {0, 2} instead makes the compiler emit a 3-boundary range scan that does not match, so the empty
// `case 3` is real, not padding.
// sysLangTbl exists to reproduce the target's colouring of the string-table lookup: with the index
// expression written out, MWCC keeps the lang_tbl base in r4 (target r5) and its count temp in r5/r7
// (target r4), which also swaps r6/r7 in the entry loop. Through the inline helper both loops match.
static inline s8* sysLangTbl(s32 v) {
    return (s8*)((u32*)lang_tbl[gp->language])[v];
}

USER_FUNC(display) {
    s32 value = evtGetValue(event, event->args[0]);
    SysWork* work;
    s32 i;

    if (isFirstCall) {
        work = _mapAlloc(0x104);
        event->userdata[0] = (s32)work;
        memset(work, 0, 0x104);
        work->field_0x14 = value;
        work->field_0x1C = 0.0f;
        work->count = 0;
        while (sysLangTbl(value)[work->count] != -1) {
            work->count++;
        }
        for (i = 0; i < work->count; i++) {
            work->entries[i].x = 36.0f * (f32)i - (f32)((work->count - 1) * 0x24) / 2.0f;
            work->entries[i].y = 40.0f;
            work->entries[i].z = 0.0f;
            work->entries[i].angle = 90.0f;
            work->entries[i].field_0x10 = i * 0x14 + 1;
            work->entries[i].field_0x14 = 0;
            work->entries[i].field_0x18 = sysLangTbl(value)[i];
        }
    }
    work = (SysWork*)event->userdata[0];
    g_wp = work;
    switch (work->state) {
        case 0:
            if ((u32)fileAsyncf(FILE_TYPE_TPL, 0, "%s/e/%s/effect_sys.tpl", getMarioStDvdRoot(), lang_prefix[gp->language]) != 0) {
                work->handle = fileAllocf(FILE_TYPE_TPL, "%s/e/%s/effect_sys.tpl", getMarioStDvdRoot(), lang_prefix[gp->language]);
                work->state++;
            }
            break;
        case 2:
            for (i = 0; i < work->count; i++) {
                SysWorkEntry* entry = &work->entries[i];

                switch (entry->field_0x14) {
                    case 0:
                        entry->field_0x10--;
                        if (entry->field_0x10 < 0) {
                            entry->field_0x10 = 0;
                            entry->field_0x14++;
                            effStardustN64SetDrawCam(effStardustN64Entry(4, entry->x, entry->y, entry->z, 40.0f), 5);
                        }
                        break;
                    case 1:
                        entry->angle = intplGetValue(5, entry->field_0x10, 0x5A, 90.0f, 1800.0f);
                        entry->field_0x10++;
                        if (entry->field_0x10 > 0x5A) {
                            entry->field_0x14++;
                        }
                        break;
                    case 2:
                        if (i == work->count - 1) {
                            work->field_0x1C += 0.8f;
                            if (work->field_0x1C > 360.0f) {
                                work->field_0x1C -= 360.0f;
                            }
                            work->field_0x18 += 0x30;
                            if (work->field_0x18 > 0x2710) {
                                work->field_0x18 = 0x2710;
                            }
                        }
                        break;
                }
            }
            break;
        case 3:
            break;
    }
    if (work->state != 0) {
        dispEntry(5, 0.0f, 2, display_func, work);
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(start) {
    SysWork* work = g_wp;

    gp->flags |= 0x10;
    if (work->state < 1) {
        return EVT_RETURN_BLOCK;
    }
    work->state++;
    return EVT_RETURN_DONE;
}

USER_FUNC(bg) {
    EffectEntry* entry = effItemGetEntry(0, 0.0f, 0.0f, 0.0f);

    ((effItemGetData*)entry->userdata)->field_0x20 = 1;
    ((effItemGetData*)entry->userdata)->field_0x38 = 5;
    return EVT_RETURN_DONE;
}

USER_FUNC(init) {
    return EVT_RETURN_DONE;
}

// clang-format on
