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

// file-scope constants used by display_func (kept global, in original order)
const u32 dat_sys_000019b8 = 0x808080FF;
const Vec vec3_sys_000019bc = { 0.0f, 0.0f, 300.0f };
const u32 dat_sys_000019c8 = 0xFFFFFFFF;
const u32 dat_sys_000019cc = 0x000000FF;
const u32 dat_sys_000019d0 = 0x363636FF;
const u32 dat_sys_000019d4 = 0x000000FF;
const Vec vec3_sys_000019d8 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_sys_000019e4 = { 0.0f, 1.0f, 0.0f };
const Vec vec3_sys_000019f0 = { 0.0f, 0.0f, 0.0f };

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

// .rodata constants referenced by the functions below; the definitions follow
// the functions so the compiler does not fold the literals at the use sites.
extern const f32 zero_sys_00001ad8;
extern const f32 float_36_sys_00001adc;
extern const f32 float_0p5_sys_00001ae0;
extern const f32 float_40_sys_00001ae4;
extern const f32 float_90_sys_00001ae8;
extern const char str_000001EC[];
extern const char lbl_25_rodata_1F0[];
extern const f32 float_1800_sys_00001b04;
extern const f32 float_0p8_sys_00001b08;
extern const f32 float_360_sys_00001b0c;
extern const f64 double_to_int_sys_00001b10;
extern const f32 float_1_sys_00001b18;
extern const f32 float_2p5_sys_00001b1c;
extern const f32 float_180_sys_00001b20;
extern const f64 double_360_sys_00001b28;
extern const f32 float_270_sys_00001b30;
extern const f32 float_neg40_sys_00001b34;
extern const f32 float_deg2rad_sys_00001b38;
extern const f32 float_neg8p1818_sys_00001b3c;
extern const f32 float_57p5_sys_00001b40;
extern const f32 float_0p6_sys_00001b44;
extern const f32 float_44_sys_00001b48;
extern const f64 double_0p21817_sys_00001b50;
extern const f32 float_25_sys_00001b58;
extern const f32 float_640_sys_00001b5c;
extern const f32 float_22_sys_00001b60;
extern const f64 double_to_int_mask_sys_00001b68;
extern const char str_00000270[];
extern const char str_00000278[];
extern const char lbl_25_rodata_27C[];

SysWork* g_wp;

void _prolog(void) {
    voidfunctionptr* ctor;

    for (ctor = _ctors; *ctor != NULL; ctor++) {
        (*ctor)();
    }
    relSetEvtAddr(str_00000278, dummy_evt);
    relSetEvtAddr(str_00000270, sys_01_init_evt);
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

    if (strcmp(gp->currentMapName, str_00000270) == 0) {
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
    f32 a = (f32)fmod(angle, double_360_sys_00001b28);

    if (a >= float_270_sys_00001b30) {
        result = float_360_sys_00001b0c - a;
    } else if (a >= float_180_sys_00001b20) {
        result = a - float_180_sys_00001b20;
    } else if (a >= float_90_sys_00001ae8) {
        result = float_180_sys_00001b20 - a;
    } else {
        result = a;
    }
    return 0xFF - result * 2;
}

#ifdef MUST_MATCH
#define sys_str_i s
#else
#define sys_str_i i
#endif

// `sys_str_i` (a separate counter for the string loop) recovers the target's r17 counter / r20 `msg`. The
// literals that replace named constants (entry-loop `1.0f`/`0.0f`, first-quad `0.0f`/`1.0f`, the `PSMTXTrans`
// zeroes, `1.0f` in the entry-loop `tan` divide) and the `-44.0f / 2.0f` halvings are deliberate: they make
// MWCC reload/fold/colour the value where the target does instead of hoisting an extra FPR. Two `MUST_MATCH`
// volatile reads (the entry-loop `camPos`/`camUp`/`target` Vec copies and the entry-loop colour base) stop
// MWCC's loop-invariant code motion from pulling per-iteration loads into the preheader. The local declaration
// order is tuned so MWCC colours the integer registers like the target (`frame` in r19 etc.). The string-loop
// `PSMTXConcat` output matrices are the target's (write mtx3 then mtx), not the earlier guess.
// NONMATCHING (99.69%): text differs only in integer/FPR register allocation and stack-slot assignment; the
// instruction sequence, frame size, FPR saves and call structure all match.
void display_func(CameraId cameraId, void* param) {
    SysWork* work = param;
    CameraEntry* cam = camGetPtr(cameraId);
    GXLightObj light;
    GXTexObj tex;
    GXColor color;
    Mtx mtx;
    Mtx mtx2;
    Mtx mtx3;
    Mtx mtx4;
    Mtx mtx5;
    Vec pos;
    Vec camUp;
    Vec target;
#ifdef MUST_MATCH
    s32 s;
#endif
    s32 frame;
    const char* msg;
    s32 revealed;
    u16 cbuf[4];
    s32 i;
    s32 lightColor;
    s32 alpha;
    f32 halfW;
    f32 halfH;
    const u16* msg16;
    f32 w;
    s32 glresult;
    f32 gla;
#ifdef MUST_MATCH
    Vec camPosTmp;
#endif
    Vec camPos;
    frame = work->field_0x10;
    work->field_0x10 = frame + 1;
    if (frame > 0x500) {
        frame -= 0x500;
    }

    GXSetChanAmbColor(GX_COLOR0A0, *(GXColor*)&dat_sys_000019b8);

    pos = vec3_sys_000019bc;
    PSMTXMultVec((MtxPtr)&cam->view, &pos, &pos);
    GXInitLightPos(&light, pos.x, pos.y, pos.z);
    GXInitLightColor(&light, *(GXColor*)&dat_sys_000019c8);
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

    work->field_0x8 += float_2p5_sys_00001b1c;
    if (work->field_0x8 > float_180_sys_00001b20) {
        work->field_0x8 = zero_sys_00001ad8;
        work->field_0xC++;
        work->field_0xC %= 4;
    }

    // getLightColor is expanded by hand here (and in the entry loop below). MWCC's inliner hands the
    // fmod result to `frsp` via the caller's f1, while the target keeps the standalone body's f2; the
    // manual expansion reproduces the standalone allocation and matches those instructions.
    gla = (f32)fmod(float_90_sys_00001ae8 + work->field_0x8, double_360_sys_00001b28);
    if (gla >= float_270_sys_00001b30) {
        glresult = float_360_sys_00001b0c - gla;
    } else if (gla >= float_180_sys_00001b20) {
        glresult = gla - float_180_sys_00001b20;
    } else if (gla >= float_90_sys_00001ae8) {
        glresult = float_180_sys_00001b20 - gla;
    } else {
        glresult = gla;
    }
    lightColor = 0xFF - glresult * 2;
    color = *(GXColor*)&dat_sys_000019cc;
    color.r = lightColor;
    color.g = lightColor;
    color.b = lightColor;
    GXSetChanMatColor(GX_COLOR0A0, color);

    PSMTXTrans(mtx, zero_sys_00001ad8, float_neg40_sys_00001b34, zero_sys_00001ad8);
    PSMTXRotRad(mtx3, 'y', float_deg2rad_sys_00001b38 * (float_90_sys_00001ae8 + work->field_0x8));
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
    halfH = (f32)(u16)GXGetTexObjHeight(&tex) * float_0p5_sys_00001ae0;
    GXPosition3f32(-halfW * float_0p5_sys_00001ae0, halfH * float_0p5_sys_00001ae0, 0.0f);
    GXNormal3f32(0.0f, 0.0f, 1.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(halfW * float_0p5_sys_00001ae0, halfH * float_0p5_sys_00001ae0, 0.0f);
    GXNormal3f32(0.0f, 0.0f, 1.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(halfW * float_0p5_sys_00001ae0, -halfH * float_0p5_sys_00001ae0, 0.0f);
    GXNormal3f32(0.0f, 0.0f, 1.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(-halfW * float_0p5_sys_00001ae0, -halfH * float_0p5_sys_00001ae0, 0.0f);
    GXNormal3f32(0.0f, 0.0f, 1.0f);
    GXTexCoord2f32(0.0f, 1.0f);

    msg = msgSearch((const char*)msg_tbl[work->field_0x14]);
    FontDrawStart();

    revealed = work->field_0x18 / 256;
    alpha = work->field_0x18 % 256;
    msg16 = (const u16*)msg;
    for (sys_str_i = 0; sys_str_i < 0x2C; sys_str_i += 2, msg16++) {
        if (sys_str_i >= strlen(msg)) {
            break;
        }

        cbuf[0] = *msg16;
        *(u8*)&cbuf[1] = 0;
#ifdef MUST_MATCH
        // `0.0f` stops MWCC hoisting the named zero into a live FPR; the target reloads it each use.
        PSMTXTrans(mtx, 0.0f, float_neg40_sys_00001b34, 0.0f);
#else
        PSMTXTrans(mtx, zero_sys_00001ad8, float_neg40_sys_00001b34, zero_sys_00001ad8);
#endif
        PSMTXRotRad(mtx3, 'z',
                    float_deg2rad_sys_00001b38 *
                        (float_neg8p1818_sys_00001b3c * (f32)sys_str_i + work->field_0x1C));
        PSMTXConcat(mtx, mtx3, mtx3);
#ifdef MUST_MATCH
        PSMTXTrans(mtx, 0.0f, float_57p5_sys_00001b40, 0.0f);
#else
        PSMTXTrans(mtx, zero_sys_00001ad8, float_57p5_sys_00001b40, zero_sys_00001ad8);
#endif
        PSMTXConcat(mtx3, mtx, mtx);
        PSMTXScale(mtx2, 0.6f, 0.6f, 0.6f);
        PSMTXConcat(mtx, mtx2, mtx);
        if (sys_str_i / 2 < revealed) {
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
    GXSetTevColor(GX_TEVREG0, *(GXColor*)&dat_sys_000019d0);
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
        // Same hand expansion as above (see that comment).
        gla = (f32)fmod(entry->angle, double_360_sys_00001b28);
        if (gla >= float_270_sys_00001b30) {
            glresult = float_360_sys_00001b0c - gla;
        } else if (gla >= float_180_sys_00001b20) {
            glresult = gla - float_180_sys_00001b20;
        } else if (gla >= float_90_sys_00001ae8) {
            glresult = float_180_sys_00001b20 - gla;
        } else {
            glresult = gla;
        }
        lightColor = 0xFF - glresult * 2;
#ifdef MUST_MATCH
        // Volatile read keeps the loop-invariant colour base from being hoisted into the preheader (the
        // target reloads it from its own address each iteration).
        color = *(volatile GXColor*)&dat_sys_000019d4;
#else
        color = *(GXColor*)&dat_sys_000019d4;
#endif
        color.b = color.g = color.r = lightColor;
        GXSetChanMatColor(GX_COLOR0A0, color);
        if ((u32)gp->language != 0) {
            PSMTXScale(mtx2, float_0p8_sys_00001b08, float_0p8_sys_00001b08, float_0p8_sys_00001b08);
        } else {
            PSMTXScale(mtx2, float_1_sys_00001b18, float_1_sys_00001b18, float_1_sys_00001b18);
        }
        PSMTXTrans(mtx, entry->x, entry->y, entry->z);
        PSMTXRotRad(mtx3, 'y', float_deg2rad_sys_00001b38 * entry->angle);
        PSMTXConcat(mtx2, mtx, mtx);
        PSMTXConcat(mtx, mtx3, mtx);
        PSMTXConcat((MtxPtr)&cam->view, mtx, mtx);
        GXLoadPosMtxImm(mtx, GX_PNMTX0);
        PSMTXInvXpose(mtx, mtx);
        GXLoadNrmMtxImm(mtx, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
#ifdef MUST_MATCH
        // Volatile reads keep MWCC from hoisting these Vec copies into the loop preheader; the target reloads
        // them from their own storage each iteration. The local copy of camPos reproduces the target's staged
        // 12-byte copy that C_MTXLookAt receives.
        camPos = *(volatile Vec*)&vec3_sys_000019d8;
        camPos.z = 1.0f / (f32)tan(double_0p21817_sys_00001b50);
        camUp = *(volatile Vec*)&vec3_sys_000019e4;
        target = *(volatile Vec*)&vec3_sys_000019f0;
        {
            camPosTmp = camPos;
            C_MTXLightPerspective(mtx4, float_25_sys_00001b58, float_640_sys_00001b5c, 0.5f,
                                  0.5f, 0.5f, 0.5f);
            C_MTXLookAt(mtx5, &camPosTmp, &camUp, &target);
        }
#else
        camPos = vec3_sys_000019d8;
        camPos.z = float_1_sys_00001b18 / (f32)tan(double_0p21817_sys_00001b50);
        camUp = vec3_sys_000019e4;
        target = vec3_sys_000019f0;
        C_MTXLightPerspective(mtx4, float_25_sys_00001b58, float_640_sys_00001b5c, 0.5f,
                              0.5f, 0.5f, 0.5f);
        C_MTXLookAt(mtx5, &camPos, &camUp, &target);
#endif
        PSMTXConcat(mtx4, mtx5, mtx4);
        if ((u32)gp->language != 0) {
            PSMTXScale(mtx2, 0.5f, 0.5f, 0.5f);
        } else {
            PSMTXScale(mtx2, float_1_sys_00001b18, float_1_sys_00001b18, float_1_sys_00001b18);
        }
        PSMTXTrans(mtx, entry->x, entry->y, entry->z);
        PSMTXConcat(mtx2, mtx, mtx);
        PSMTXConcat(mtx4, mtx, mtx);
        PSMTXTrans(mtx2, (f32)-frame, 0.0f, 0.0f);
        PSMTXConcat(mtx, mtx2, mtx);
        GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4);
        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        // The target folds the sign of the half-width into `w` (negate 44.0 then halve it).
        w = -float_44_sys_00001b48 / 2.0f;
        GXPosition3f32(w, float_22_sys_00001b60, 0.0f);
        GXNormal3f32(0.0f, 0.0f, 1.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(float_22_sys_00001b60, float_22_sys_00001b60, 0.0f);
        GXNormal3f32(0.0f, 0.0f, 1.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(float_22_sys_00001b60, w, 0.0f);
        GXNormal3f32(0.0f, 0.0f, 1.0f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(w, w, 0.0f);
        GXNormal3f32(0.0f, 0.0f, 1.0f);
        GXTexCoord2f32(0.0f, 1.0f);
    }

    GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
}

#undef sys_str_i

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

// The int->float 2^52+2^31 double pools as an anonymous @NNN at .rodata 0x280 instead of reusing
// double_to_int_sys_00001b10 at 0x210 (reloc-only; the named constant below is unused project-wide).
// NONMATCHING (99.84%): case 2's field_0x1C update still colours field_0x1C in f0 (target f2) and
// float_360 in f2 (target f0); float_0p8 lands in f1 and the fadds result in f1, as in the target.
USER_FUNC(display) {
    s32 value = evtGetValue(event, event->args[0]);
    SysWork* work;
    s32 i;

    if (isFirstCall) {
        work = _mapAlloc(0x104);
        event->userdata[0] = (s32)work;
        memset(work, 0, 0x104);
        work->field_0x14 = value;
        work->field_0x1C = zero_sys_00001ad8;
        work->count = 0;
        while (sysLangTbl(value)[work->count] != -1) {
            work->count++;
        }
        for (i = 0; i < work->count; i++) {
#ifdef MUST_MATCH
            // `w` is only a colouring aid: with the expression written out MWCC emits `fmuls f3, f4, f3`
            // where the target has `fmuls f3, f3, f4`.
            f32 w = (f32)((work->count - 1) * 0x24);
            work->entries[i].x = float_36_sys_00001adc * (f32)i - w * float_0p5_sys_00001ae0;
#else
            work->entries[i].x = float_36_sys_00001adc * (f32)i -
                                 (f32)((work->count - 1) * 0x24) * float_0p5_sys_00001ae0;
#endif
            work->entries[i].y = float_40_sys_00001ae4;
            work->entries[i].z = zero_sys_00001ad8;
            work->entries[i].angle = float_90_sys_00001ae8;
            work->entries[i].field_0x10 = i * 0x14 + 1;
            work->entries[i].field_0x14 = 0;
            work->entries[i].field_0x18 = sysLangTbl(value)[i];
        }
    }
    work = (SysWork*)event->userdata[0];
    g_wp = work;
    switch (work->state) {
        case 0:
            if ((u32)fileAsyncf(4, 0, str_000001EC, getMarioStDvdRoot(), lang_prefix[gp->language]) != 0) {
                work->handle = fileAllocf(4, str_000001EC, getMarioStDvdRoot(), lang_prefix[gp->language]);
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
                            effStardustN64SetDrawCam(effStardustN64Entry(4, entry->x, entry->y, entry->z, float_40_sys_00001ae4), 5);
                        }
                        break;
                    case 1:
                        entry->angle = intplGetValue(5, entry->field_0x10, 0x5A, float_90_sys_00001ae8, float_1800_sys_00001b04);
                        entry->field_0x10++;
                        if (entry->field_0x10 > 0x5A) {
                            entry->field_0x14++;
                        }
                        break;
                    case 2:
                        if (i == work->count - 1) {
#ifdef MUST_MATCH
                            // `t = inc + field` reproduces the target's field_0x1C colouring and fadds
                            // operand order; the natural in-place update colours it in f0 instead.
                            f32 t = float_0p8_sys_00001b08 + work->field_0x1C;
                            work->field_0x1C = t;
                            if (work->field_0x1C > float_360_sys_00001b0c) {
                                work->field_0x1C = t - float_360_sys_00001b0c;
                            }
#else
                            work->field_0x1C += float_0p8_sys_00001b08;
                            if (work->field_0x1C > float_360_sys_00001b0c) {
                                work->field_0x1C -= float_360_sys_00001b0c;
                            }
#endif
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
        dispEntry(5, zero_sys_00001ad8, 2, display_func, work);
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
    EffectEntry* entry = effItemGetEntry(0, zero_sys_00001ad8, zero_sys_00001ad8, zero_sys_00001ad8);

    ((effItemGetData*)entry->userdata)->field_0x20 = 1;
    ((effItemGetData*)entry->userdata)->field_0x38 = 5;
    return EVT_RETURN_DONE;
}

USER_FUNC(init) {
    return EVT_RETURN_DONE;
}

const f32 zero_sys_00001ad8 = 0.0f;
const f32 float_36_sys_00001adc = 36.0f;
const f32 float_0p5_sys_00001ae0 = 0.5f;
const f32 float_40_sys_00001ae4 = 40.0f;
const f32 float_90_sys_00001ae8 = 90.0f;
const char str_000001EC[4] = { '%', 's', '/', 'e' };
const char lbl_25_rodata_1F0[] = "/%s/effect_sys.tpl";
const f32 float_1800_sys_00001b04 = 1800.0f;
const f32 float_0p8_sys_00001b08 = 0.8f;
const f32 float_360_sys_00001b0c = 360.0f;
const f64 double_to_int_sys_00001b10 = 4503601774854144.0;
const f32 float_1_sys_00001b18 = 1.0f;
const f32 float_2p5_sys_00001b1c = 2.5f;
const f32 float_180_sys_00001b20 = 180.0f;
const f64 double_360_sys_00001b28 = 360.0;
const f32 float_270_sys_00001b30 = 270.0f;
const f32 float_neg40_sys_00001b34 = -40.0f;
const f32 float_deg2rad_sys_00001b38 = 0.017453292f;
const f32 float_neg8p1818_sys_00001b3c = -8.181818f;
const f32 float_57p5_sys_00001b40 = 57.5f;
const f32 float_0p6_sys_00001b44 = 0.6f;
const f32 float_44_sys_00001b48 = 44.0f;
const f64 double_0p21817_sys_00001b50 = 0.21816615760326385;
const f32 float_25_sys_00001b58 = 25.0f;
const f32 float_640_sys_00001b5c = 640.0f;
const f32 float_22_sys_00001b60 = 22.0f;
const f64 double_to_int_mask_sys_00001b68 = 4503599627370496.0;
const char str_00000270[] = "sys_01";
const char str_00000278[4] = { 's', 'y', 's', '_' };
const char lbl_25_rodata_27C[] = "00";
// clang-format on
