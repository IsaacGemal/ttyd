#include "driver/envdrv.h"
#include "driver/arcdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "manager/dvdmgr.h"
#include "system.h"
#include "texPalette.h"
#include <dolphin/dvd.h>
#include <string.h>
#include "driver/fadedrv.h"
#include <stdio.h>
#include <stdlib.h>
#include "math_ext.h"

// The unit's .data opens with the eight {-1, 1} / {1, -1} pairs every unit that includes this header
// set has; the four named pairs are local ($) in the original, so they are static here and
// force_active keeps them emitted (same layout as battle_break_slot.c).
#pragma force_active on
__declspec(section ".data") u32 lbl_80398960[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") static u32 negone_one_374[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") u32 lbl_80398970[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") static u32 one_negone_373[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") u32 lbl_80398980[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") static u32 negone_one_360[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") u32 lbl_80398990[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") static u32 one_negone_359[2] = { 0x3F800000, 0xBF800000 };
#pragma force_active reset

extern GlobalWork* gp;

// Texture-matrix ids and the five vectors the matrix cases rotate against (target .rodata at 0x802f9338).
const EnvTevMtxData dat_802f9338 = {
    { 30, 33, 36, 39, 42, 45, 48, 51, 54, 57 },
    { 0.0f, 0.0f, -1.0f },
    { 0.0f, 0.0f, 1.0f },
    { 0.0f, 100.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, -1.0f },
};

// .sdata2
const GXColor dat_8041e510 = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor dat_8041e514 = { 0xFF, 0xFF, 0xFF, 0x00 };
const GXColor dat_8041e518 = { 0xFF, 0xFF, 0xFF, 0xA0 };

//.data
static EnvironmentGlare envGlare0 = {
    GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_TEV_KCSEL_7_8, GX_TEV_KCSEL_1, 5.25f, 16,
    {
        {-1.0f, 0.0f}, {-3.0f, 0.0f}, {-5.0f, 0.0f}, {-7.0f, 0.0f},
    {1.0f, 0.0f}, {3.0f, 0.0f}, {5.0f, 0.0f}, {7.0f, 0.0f},
    {0.0f, -1.0f}, {0.0f, -3.0f}, {0.0f, -5.0f}, {0.0f, -7.0f},
    {0.0f, 1.0f}, {0.0f, 3.0f}, {0.0f, 5.0f}, {0.0f, 7.0f},
}
};

static EnvironmentGlare envGlare1 = {
    GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_TEV_KCSEL_7_8, GX_TEV_KCSEL_1_2, 3.5f, 8,
    {
        {-1.41419994831f, -1.41419994831f}, {0.0f, -1.0f}, {1.41419994831f, -1.41419994831f}, {-1.0f, 0.0f},
    {1.0f, 0.0f}, {-1.41419994831f, 1.41419994831f}, {0.0f, 1.0f}, {1.41419994831f, 1.41419994831f},
}
};

static EnvironmentGlare envGlare2 = {
    GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_TEV_KCSEL_7_8, GX_TEV_KCSEL_1_4, 3.5f, 52,
    {
        {0.0f, -5.0f}, {-1.0f, -4.0f}, {0.0f, -4.0f}, {1.0f, -4.0f},
    {-2.0f, -3.0f}, {-1.0f, -3.0f}, {0.0f, -3.0f}, {1.0f, -3.0f},
    {2.0f, -3.0f}, {-3.0f, -2.0f}, {-2.0f, -2.0f}, {-1.0f, -2.0f},
    {0.0f, -2.0f}, {1.0f, -2.0f}, {2.0f, -2.0f}, {3.0f, -2.0f},
    {-3.0f, -1.0f}, {-2.0f, -1.0f}, {-1.0f, -1.0f}, {0.0f, -1.0f},
    {1.0f, -1.0f}, {2.0f, -1.0f}, {3.0f, -1.0f}, {-3.0f, 0.0f},
    {-2.0f, 0.0f}, {-1.0f, 0.0f}, {3.0f, 0.0f}, {2.0f, 0.0f},
    {1.0f, 0.0f}, {-3.0f, 1.0f}, {-2.0f, 1.0f}, {-1.0f, 1.0f},
    {0.0f, 1.0f}, {1.0f, 1.0f}, {2.0f, 1.0f}, {3.0f, 1.0f},
    {-3.0f, 2.0f}, {-2.0f, 2.0f}, {-1.0f, 2.0f}, {0.0f, 2.0f},
    {1.0f, 2.0f}, {2.0f, 2.0f}, {3.0f, 2.0f}, {-2.0f, 3.0f},
    {-1.0f, 3.0f}, {0.0f, 3.0f}, {1.0f, 3.0f}, {2.0f, 3.0f},
    {-1.0f, 4.0f}, {0.0f, 4.0f}, {1.0f, 4.0f}, {0.0f, 5.0f},
}
};

static EnvironmentGlare envGlare3 = {
    GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_TEV_KCSEL_7_8, GX_TEV_KCSEL_1_2, 3.5f, 16,
    {
        {0.0f, -4.0f}, {-1.0f, -3.0f}, {1.0f, -3.0f}, {-2.0f, -2.0f},
    {2.0f, -2.0f}, {-3.0f, -1.0f}, {3.0f, -1.0f}, {-3.0f, 0.0f},
    {3.0f, 0.0f}, {-3.0f, 1.0f}, {3.0f, 1.0f}, {-2.0f, 2.0f},
    {2.0f, 2.0f}, {-1.0f, 3.0f}, {1.0f, 3.0f}, {0.0f, 4.0f},
}
};

//.bss
static EnvironmentWork work[2];

//.sbss (MWCC lays these out last-declared-first: declared in the reverse of the original's order)
s32 v_count;
f32 u2;
f32 u1;
f32 a;
f32 frame_cnt;
s32 tpl_ok;
TEXPalette* env_tpl;

//local prototypes

//there is no function so we'll make a macro
#define envGetWork() (gp->inBattle ? &work[1] : &work[0])

#define PI 3.141592653589793f
#define TWO_PI 6.283185307179586f
#define NAN (*(f32*)__float_nan)
#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5
#define fpclassify(x) __fpclassifyf(x)

static void _callback(s32 result, DVDFileInfo* fileInfo) {
    DVDEntry* handle = fileInfo->cb.userData;

    UnpackTexPalette(env_tpl);
    DVDMgrClose(handle);
    tpl_ok = TRUE;
}

void envInit(void) {
    memset(&work[0], 0, sizeof(EnvironmentWork));
    memset(&work[1], 0, sizeof(EnvironmentWork));
    env_tpl = NULL;
    tpl_ok = FALSE;
}

void envReInit(void) {
    EnvironmentWork* wp = envGetWork();
    memset(wp, 0, sizeof(EnvironmentWork));
}

void envTexSetup(void) { //1:1
    DVDEntry* entry;
    TEXPalette* tpl;
    char path[128];
    u32 size;

    env_tpl = NULL;
    tpl = arcOpen("env.tpl", 0, 0);
    env_tpl = tpl;
    if (!tpl) {
        sprintf(path, "%s/env.tpl", getMarioStDvdRoot());
        entry = DVDMgrOpen(path, 2, 0);
        size = DVDMgrGetLength(entry);
        env_tpl = __memAlloc(HEAP_DEFAULT, OSRoundUp32B(size));
        entry->info.cb.userData = entry;
        DVDMgrReadAsync(entry, env_tpl, size, 0, _callback);
    }
    else {
        UnpackTexPalette(tpl);
        tpl_ok = 1;
    }
}

void envCapture(CameraId camId, void* param) { //1:1
    EnvironmentWork* wp = envGetWork();
    u16 var0, var1, var2, var3;
    u32 size;
    
    var0 = gp->fbWidth;
    var1 = gp->efbHeight;
    var2 = (u16)(((u32)var0 & 0xFFFF) / 2);
    var3 = (u16)(((u32)var1 & 0xFFFF) / 2);
    if (wp->flags & 1) {
        GXSetTexCopySrc(0, 0, gp->fbWidth, gp->efbHeight);
        GXSetTexCopyDst(var2, var3, GX_TF_RGBA8, GX_TRUE);
        size = GXGetTexBufferSize(var2, var3, GX_TF_RGBA8, GX_FALSE, 0);
        wp->capture = smartAlloc(size, 3);
        GXCopyTex(wp->capture->address, GX_FALSE);
        GXPixModeSync();
        GXInitTexObj(&wp->captureObj, wp->capture->address, var2, var3, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
        GXInitTexObjLOD(&wp->captureObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    }
    if (wp->flags & 2) {
        GXSetTexCopySrc(0, 0, var0, var1);
        GXSetTexCopyDst(var2, var3, GX_CTF_Z8M, GX_TRUE);
        size = GXGetTexBufferSize(var2, var3, GX_CTF_Z8M, GX_FALSE, 0);
        wp->capture2 = smartAlloc(size, 3);
        GXCopyTex(wp->capture2->address, 0);
        GXPixModeSync();
        GXInitTexObj(&wp->captureObj2, wp->capture2->address, var2, var3, GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);
        GXInitTexObjLOD(&wp->captureObj2, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    }
}

void envCapture2(CameraId camId, void* param) { //1:1
    EnvironmentWork* wp = envGetWork();
    u16 var0, var1;

    var0 = (u16)(((u32)gp->fbWidth & 0xFFFF) / 2);
    var1 = (u16)(((u32)gp->efbHeight & 0xFFFF) / 2);
    if (wp->flags & 4) {
        GXSetTexCopySrc(0, 0, gp->fbWidth, gp->efbHeight);
        GXSetTexCopyDst(var0, var1, GX_TF_RGB565, GX_TRUE);
        GXCopyTex(wp->envBlur, GX_FALSE);
        GXPixModeSync();
        GXInitTexObj(&wp->envBlurObj, wp->envBlur, var0, var1, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
        GXInitTexObjLOD(&wp->envBlurObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    }
    if (wp->flags & 0x40000000) {
        if ((wp->unk98 & 1) && (wp->flags & 8)) {
            wp->flags &= ~4;
        }
        wp->flags |= 8;
    }
}

void envDisp(CameraId camId, void *param) {
    f32 vp[GX_VIEWPORT_SZ];
    f32 pm[GX_PROJECTION_SZ];

    GXGetViewportv(vp);
    GXGetProjectionv(pm);
    envDisp_DepthOfField();
    envDisp_Blur();
    envDisp_FF();
    GXSetViewport(vp[0], vp[1], vp[2], vp[3], vp[4], vp[5]);
    GXSetProjectionv(pm);
}

void envDisp2(CameraId camId, void *param) {
    f32 vp[GX_VIEWPORT_SZ];
    f32 pm[GX_PROJECTION_SZ];
    EnvironmentWork* wp;
    EnvironmentGlareFilter* filter;

    GXGetViewportv(vp);
    GXGetProjectionv(pm);
    wp = envGetWork();
    filter = &wp->filter;
    if (wp->flags & 0x20000000) {
        switch (filter->type) {
            case 0:
                break;
            case 1:
                envGlare(&envGlare0);
                break;
            case 2:
                envGlare(&envGlare1);
                break;
            case 3:
                envGlare(&envGlare2);
                break;
            case 4:
                envGlare(&envGlare3);
                break;
        }
    }
    GXSetViewport(vp[0], vp[1], vp[2], vp[3], vp[4], vp[5]);
    GXSetProjectionv(pm);
}

void envMain(void) {
    dispEntry(CAMERA_3D, 20.0f, 7, envCapture, NULL);
    dispEntry(CAMERA_3D, 10.0f, 7, envDisp, NULL);
    dispEntry(CAMERA_3D, 0.0f, 7, envCapture2, NULL);
    dispEntry(CAMERA_3D, 5000.0f, 8, envDisp2, NULL);
}

// The stage that blends the blurred capture (texmap 1) over the frame by the konst alpha.
static inline void envDofBlendStage(s32 tevStage, s32 texCoord, s32 colorC, s32 alphaC) {
    GXSetTevKColorSel(tevStage, 0xC);
    GXSetTevKAlphaSel(tevStage, 0x1C);
    GXSetTevOrder(tevStage, texCoord, 1, 0xFF);
    GXSetTevColorOp(tevStage, 8, 0, 0, 1, 0);
    GXSetTevAlphaOp(tevStage, 8, 0, 0, 1, 0);
    GXSetTevColorIn(tevStage, 8, 0xE, colorC, 0xF);
    GXSetTevAlphaIn(tevStage, 4, 6, alphaC, 7);
    GXSetTevSwapMode(tevStage, 0, 0);
}

// The same blend stage weighted by a register colour (TEVREG1 white) instead of the frame.
static inline void envDofBlendStageReg(s32 tevStage, s32 texCoord) {
    GXSetTevColor(2, dat_8041e510);
    envDofBlendStage(tevStage, texCoord, 0xC, 2);
}

// `opt_propagation off` keeps the polynomial coefficients in variables that MWCC hoists out of the
// loop (as in envDisp_FF; with propagation on they are folded into every branch), and
// `opt_lifetimes off` keeps the 0.00761/0.03705 seed loads inside the loop.
#pragma opt_propagation off
#pragma opt_lifetimes off
#pragma inline_max_size(2000)
static inline void envDofSinCos(f32 angle, f32* s, f32* c) {
    f32 cb;
    f32 cv;
    f32 sb;
    f32 one;
    f32 sv;
    f32 t, u;
    f32 cu;

    sb = 0.16605f;
    sv = 0.00761f;
    one = 1.0f;
    if (angle <= PI) {
        if (angle <= 1.5707964f) {
            u = angle * angle;
            sv = sv * u - sb;
            sv = sv * u + one;
            sv = sv * angle;
        } else {
            t = 1.5707964f - (angle - 1.5707964f);
            u = t * t;
            sv = sv * u - sb;
            sv = sv * u + one;
            sv = sv * t;
        }
    } else if (angle < 4.712389f) {
        t = angle - PI;
        u = t * t;
        sv = sv * u - sb;
        sv = sv * u + one;
        sv = sv * t;
        sv = -sv;
    } else {
        t = 1.5707964f - (angle - 4.712389f);
        u = t * t;
        sv = sv * u - sb;
        sv = sv * u + one;
        sv = sv * t;
        sv = -sv;
    }
    *s = sv;

    cb = 0.4967f;
    cv = 0.03705f;
    if (angle <= PI) {
        if (angle <= 1.5707964f) {
            cu = angle * angle;
            cv = cv * cu - cb;
            cv = cv * cu + one;
        } else {
            cu = 1.5707964f - (angle - 1.5707964f);
            cu = cu * cu;
            cv = cv * cu - cb;
            cv = cv * cu + one;
            cv = -cv;
        }
    } else if (angle < 4.712389f) {
        cu = angle - PI;
        cu = cu * cu;
        cv = cv * cu - cb;
        cv = cv * cu + one;
        cv = -cv;
    } else {
        cu = 1.5707964f - (angle - 4.712389f);
        cu = cu * cu;
        cv = cv * cu - cb;
        cv = cv * cu + one;
    }
    *c = cv;
}

// One blur tap per texture matrix: each offsets the capture by `amount` around a circle.
static inline void envDofTaps(const f32* amount, s32* texCoord, GXTevStageID* tevStage) {
    Mtx mtx;
    s32 first;
    s32 i;
    const s32* id = dat_802f9338.tbl;
    GXTevColorArg colorD;
    GXTevAlphaArg alphaD;

    for (i = 0; i < 4; i++) {
        f32 s, c;

        envDofSinCos(TWO_PI * i / 4, &s, &c);

        MTXTrans(mtx, *amount * s / 100.0f, *amount * c / 100.0f, 0.0f);
        GXLoadTexMtxImm(mtx, *id, 1);
        GXSetTexCoordGen2(*texCoord, 1, 4, *id, 0, 0x7D);
        GXSetTevKColorSel(*tevStage, 6);
        GXSetTevKAlphaSel(*tevStage, 6);
        GXSetTevOrder(*tevStage, *texCoord, 0, 0xFF);
        GXSetTevColorOp(*tevStage, 0, 0, 0, 1, i == 3);
        GXSetTevAlphaOp(*tevStage, 0, 0, 0, 1, i == 3);
        first = !i;
        colorD = GX_CC_CPREV;
        if (first) {
            colorD = GX_CC_ZERO;
        }
        GXSetTevColorIn(*tevStage, 0xF, 8, 0xE, colorD);
        if (first) {
            alphaD = GX_CA_ZERO;
        } else {
            alphaD = GX_CA_APREV;
        }
        GXSetTevAlphaIn(*tevStage, 7, 4, 6, alphaD);
        GXSetTevSwapMode(*tevStage, 0, 0);
        (*texCoord)++;
        id++;
        (*tevStage)++;
    }
}

// Under propagation off the inline GXPosition3f32 keeps x in a parameter variable, which colours after
// the y conversion (f5 for the first vertex). The original stores x straight from its computation after
// y has been evaluated into its parameter, which is what this vertex macro does.
#undef GXPosition3f32
#define GXPosition3f32(x, y, z)                                                                            \
    do {                                                                                                   \
        f32 _y = (y);                                                                                      \
        GXWGFifo.f32 = (x);                                                                                \
        GXWGFifo.f32 = _y;                                                                                 \
        GXWGFifo.f32 = (z);                                                                                \
    } while (0)

static inline void envDofQuad(void) {
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(-gp->fbWidth / 2.0f, gp->efbHeight / 2.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(gp->fbWidth / 2.0f, gp->efbHeight / 2.0f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(gp->fbWidth / 2.0f, -gp->efbHeight / 2.0f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(-gp->fbWidth / 2.0f, -gp->efbHeight / 2.0f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
}

// The far pass: build its blur taps, blend against white, then composite with the frame.
static inline void envDofFar(EnvironmentUnk88* u, GXColor* color) {
    GXTevStageID tevStage;
    s32 texCoord;
    Mtx mtx;
    s32 first;
    s32 i;
    GXTevStageID next;
    const s32* id = dat_802f9338.tbl;
    GXTevColorArg colorD;
    GXTevAlphaArg alphaD;

    texCoord = 0;
    tevStage = 0;
    for (i = 0; i < 4; i++) {
        f32 s, c;

        envDofSinCos(TWO_PI * i / 4, &s, &c);

        MTXTrans(mtx, u->unk8 * s / 100.0f, u->unk8 * c / 100.0f, 0.0f);
        GXLoadTexMtxImm(mtx, *id, 1);
        GXSetTexCoordGen2(texCoord, 1, 4, *id, 0, 0x7D);
        GXSetTevKColorSel(tevStage, 6);
        GXSetTevKAlphaSel(tevStage, 6);
        GXSetTevOrder(tevStage, texCoord, 0, 0xFF);
        GXSetTevColorOp(tevStage, 0, 0, 0, 1, i == 3);
        GXSetTevAlphaOp(tevStage, 0, 0, 0, 1, i == 3);
        first = !i;
        colorD = GX_CC_CPREV;
        if (first) {
            colorD = GX_CC_ZERO;
        }
        GXSetTevColorIn(tevStage, 0xF, 8, 0xE, colorD);
        if (first) {
            alphaD = GX_CA_ZERO;
        } else {
            alphaD = GX_CA_APREV;
        }
        GXSetTevAlphaIn(tevStage, 7, 4, 6, alphaD);
        GXSetTevSwapMode(tevStage, 0, 0);
        texCoord++;
        id++;
        tevStage++;
    }
    color->r = u->unk3;
    color->a = 0xFF;
    GXSetTevKColor(0, *color);
    GXSetTexCoordGen2(texCoord, 1, 4, 0x3C, 0, 0x7D);
    envDofBlendStageReg(tevStage, texCoord++);
    next = tevStage + 1;
    GXSetTevOrder(next, 0xFF, 0xFF, 0xFF);
    GXSetTevColorOp(next, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(next, 0, 0, 0, 1, 0);
    GXSetTevColorIn(next, 2, 0xF, 1, 0xF);
    GXSetTevAlphaIn(next, 1, 7, 0, 7);
    GXSetTevSwapMode(next, 0, 0);
    tevStage += 2;
    GXSetNumTexGens(texCoord);
    GXSetNumTevStages(tevStage);
    envDofQuad();
}

// tevStage is a GXTevStageID so its increments stay in place
// (an int counter gets folded into the GXSetNumTevStages argument by the propagation pass).
// texCoord and tevStage only take the original's registers when three of their interference
// neighbours are named locals declared after them: with lower vregs those leave the allocation
// graph first, which keeps the two counters out of the late priority level that the loop's own
// counters fall into.
void envDisp_DepthOfField(void) {
    EnvironmentWork* wp = envGetWork();
    EnvironmentUnk88* u = &wp->unk88;
    CameraEntry* cam;
    GXTexObj* texobj;
    s32 texCoord;
    GXTevStageID tevStage;
    GXColor color;
    // The colour channels and the texcoord copy live in named locals; assigning them in place makes the
    // two counters and the loop's i/id colour the other way round.
    s32 tc;
    u8 alpha;
    u8 red;

    if (!(wp->flags & 0x80000000)) {
        return;
    }

    cam = camGetPtr(CAMERA_2D);
    GXSetProjection(camGetPtr(CAMERA_2D)->projection, cam->projType);
    GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
    GXSetCurrentMtx(0);
    texobj = smartTexObj(&wp->captureObj, (void**)wp->capture);
    GXLoadTexObj(texobj, 0);
    texobj = smartTexObj(&wp->captureObj2, (void**)wp->capture2);
    GXLoadTexObj(texobj, 1);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
    GXSetBlendMode(0, 1, 0, 7);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 3, 0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXSetCullMode(0);

    if ((u->unk0 & 1) && (u->unk4)) {
        texCoord = 0;
        tevStage = 0;
        envDofTaps(&u->unk4, &texCoord, &tevStage);
        red = u->unk2;
        color.r = red;
        alpha = 0xFF;
        color.a = alpha;
        GXSetTevKColor(0, color);
        GXSetTexCoordGen2(texCoord, 1, 4, 0x3C, 0, 0x7D);
        tc = texCoord++;
        envDofBlendStage(tevStage, tc, 2, 1);
        tevStage++;
        GXSetNumTexGens(texCoord);
        GXSetNumTevStages(tevStage);
        envDofQuad();
    }

    if ((u->unk0 & 2) && (u->unk8)) {
        envDofFar(u, &color);
    }
}
#pragma inline_max_size reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#undef GXPosition3f32

// A by-value GXColor built as `color = dat; color.a = a;` and then passed lands its local at the higher
// slot (sp+0x10 here) and the argument copy at sp+0xC, the wrong way round. Building the colour in an
// inline that returns it puts the return temporary at sp+0xC and the argument at sp+0x10. The default
// (duration == 0) colour goes through a void helper too, or its argument temp takes sp+0x8 and pushes
// the if-branch temporaries back up.
static inline GXColor envDisp_MakeBlurColor(u8 alpha) {
    GXColor color = dat_8041e514;
    color.a = alpha;
    return color;
}
static inline void envDisp_SetBlurColorDefault(void) {
    GXSetTevKColor(GX_KCOLOR0, dat_8041e518);
}
void envDisp_Blur(void) {
    EnvironmentWork* wp = envGetWork();
    EnvironmentBlur* blur = (EnvironmentBlur*)&wp->unk98;
    CameraEntry* cam;
    s32 alpha;
    f32 ratio;
    u64 elapsed;
    s32 duration;

    if ((wp->flags & 0x40000000) && (wp->flags & 8)) {
        cam = camGetPtr(CAMERA_2D);
        GXSetProjection(camGetPtr(CAMERA_2D)->projection, cam->projType);
        GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        GXSetNumChans(0);
        GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
        GXSetNumTevStages(1);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_KONST, GX_CA_TEXA, GX_CA_ZERO);
        GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);

        duration = blur->unk10;
        if (duration != 0) {
            elapsed = gp->renderTime - blur->renderTime;
            ratio = (f32)(elapsed / (OS_TIMER_CLOCK / 1000)) / (f32)duration;
            if (ratio > 1.0f) {
                ratio = 1.0f;
            }
            if (blur->flags & 1) {
                alpha = (s32)(255.0f * (1.0f - ratio));
            } else {
                alpha = (s32)(120.0f * (1.0f - ratio) + 128.0f);
            }
            GXSetTevKColor(GX_KCOLOR0, envDisp_MakeBlurColor(alpha));
        } else {
            envDisp_SetBlurColorDefault();
        }

        GXLoadTexObj(&wp->envBlurObj, GX_TEXMAP0);
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
        GXSetZCompLoc(GX_TRUE);
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
        GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
        GXClearVtxDesc();
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        // MWCC puts 0.5f in f3 and the int->float mask double in f4 only when the
        // factor is a named local; with the literal the two swap and ~14 fmuls/fsubs differ.
        {
            f32 half = 0.5f;
            GXPosition3f32(-gp->fbWidth * half, gp->efbHeight * half, 0.0f);
            GXTexCoord2f32(0.0f, 0.0f);
            GXPosition3f32(gp->fbWidth * half, gp->efbHeight * half, 0.0f);
            GXTexCoord2f32(1.0f, 0.0f);
            GXPosition3f32(gp->fbWidth * half, -gp->efbHeight * half, 0.0f);
            GXTexCoord2f32(1.0f, 1.0f);
            GXPosition3f32(-gp->fbWidth * half, -gp->efbHeight * half, 0.0f);
            GXTexCoord2f32(0.0f, 1.0f);
        }
    }
}

#define isnan(x) (fpclassify(x) == FP_NAN)

void envBlurOn(s32 flag, s32 value) {
    EnvironmentWork* wp = envGetWork();
    EnvironmentBlur* blur = (EnvironmentBlur*)&wp->unk98;

    wp->flags |= 0x40000000;
    wp->flags |= 4;
    wp->flags &= ~8;
    memset(blur, 0, sizeof(EnvironmentBlur));
    blur->renderTime = gp->renderTime;
    blur->unk10 = value;
    if (flag != 0) {
        blur->flags |= 1;
    }
    wp->envBlur = _mapAlloc(GXGetTexBufferSize((u16)(gp->fbWidth / 2), (u16)(gp->efbHeight / 2), GX_TF_RGB565, GX_FALSE, 0));
}

void envBlurOff(void) {
    EnvironmentWork* wp = envGetWork();

    wp->flags &= ~4;
    wp->flags &= ~0x40000000;
    _mapFree(wp->envBlur);
}

// envGlare was matched with opt_lifetimes off (needed: its register colouring regresses with the default pass)
#pragma opt_lifetimes off
void envGlare(EnvironmentGlare* glare) {
    GXTevStageID var_r23;
    u16 temp_r8;
    GXTexObj sp8;
    GXTevStageID var_r22;
    u32 var_r22_2;
    Mtx44 sp88;
    EnvironmentGlareFilter* temp_r30;
    EnvironmentWork* var_r4 = envGetWork();
    s32 temp_r30_2;
    GXTevKColorSel temp_r21_2;
    EnvironmentGlare* var_r21;
    GXTevKColorSel temp_r20_2;
    // The target keeps the sign-extended filter x0 in r20 and the texture-matrix index in r23;
    // caching the cast in x0s and declaring var_r22_3 ahead of var_r23_2 gives those colours
    // (an inline cast and a late declaration colour them differently).
    s16 x0s;
    EnvironmentGlare* var_r22_3;
    s32 temp_r29;
    s32 var_r28;
    u32 var_r23_2;
    u16 temp_r27;
    s32 temp_r31;
    u32 temp_r20;
    u32 temp_r21;
    u32 size;
    u16 temp_r26;
    Mtx sp58;
    Mtx sp28;
    s32 var_r19_3;
    void* temp_r19;
    int i;
    u16 temp_r0;

    frame_cnt += 1.0f;
    temp_r30 = &var_r4->filter;
    temp_r27 = var_r4->filter.y0;
    temp_r26 = var_r4->filter.x0;
    temp_r8 = (u16)(var_r4->filter.x1 - temp_r26);
    temp_r0 = (u16)(var_r4->filter.y1 - temp_r27);
    temp_r21 = (u32)temp_r8 / 2;
    temp_r20 = (u32)temp_r0 / 2;
    size = GXGetTexBufferSize((u16)temp_r21, (u16)temp_r20, GX_TF_RGB565, GX_FALSE, 0);
    temp_r19 = smartAlloc(size, 3)->address;
    GXSetTexCopySrc(temp_r26, temp_r27, temp_r8, temp_r0);
    GXSetTexCopyDst((u16)temp_r21, (u16)temp_r20, GX_TF_RGB565, GX_TRUE);
    GXCopyTex(temp_r19, GX_FALSE);
    GXPixModeSync();
    GXInitTexObj(&sp8, temp_r19, (u16)temp_r21, (u16)temp_r20, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GXInitTexObjLOD(&sp8, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&sp8, GX_TEXMAP0);
    temp_r29 = glare->count;
    var_r28 = 0;
    temp_r20_2 = glare->unkC;
    temp_r21_2 = glare->unk10;
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_KONST, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
    GXSetTevKColorSel(GX_TEVSTAGE0, temp_r20_2);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C1, GX_CC_KONST, GX_CC_ZERO);
    GXSetTevKColorSel(GX_TEVSTAGE1, temp_r21_2);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);

    i = 0;
    var_r23 = 2;
    var_r22 = 3;
    for (; i < 7; i++) {
        GXSetTevOrder(var_r23, i + 1, 0, 0xFF);
        GXSetTevColorOp(var_r23, 1, 0, 0, 1, 2);
        GXSetTevColorIn(var_r23, 0xE, 0xF, 0xF, 8);
        GXSetTevKColorSel(var_r23, temp_r20_2);
        GXSetTevSwapMode(var_r23, 0, 0);
        GXSetTevOrder(var_r22, 0xFF, 0xFF, 0xFF);
        GXSetTevColorOp(var_r22, 0, 0, 0, 1, 0);
        GXSetTevColorIn(var_r22, 0xF, 4, 0xE, 0);
        GXSetTevKColorSel(var_r22, temp_r21_2);
        GXSetTevSwapMode(var_r22, 0, 0);
        var_r23 += 2;
        var_r22 += 2;
    }

    for (i = 0, var_r22_2 = GX_TEXMTX0; i < 8; i++, var_r22_2 += 3) {
        GXSetTexCoordGen(i, GX_TG_MTX2x4, GX_TG_TEX0, var_r22_2);
    }

    if (temp_r30->flags & 1) {
        GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
    } else {
        GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    }
    GXSetBlendMode(glare->mode, glare->srcFactor, glare->dstFactor, GX_LO_CLEAR);
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    GXSetViewport(0.0f, 0.0f, (f32)gp->fbWidth, (f32)gp->efbHeight, 0.0f, 1.0f);
    GXSetScissor(0, 0, gp->fbWidth, gp->efbHeight);
    MTXOrtho(sp88, 0.0f, (f32)gp->efbHeight, 0.0f, (f32)gp->fbWidth, 0.0f, -32767.0f);
    GXSetProjection(sp88, GX_ORTHOGRAPHIC);
    PSMTXIdentity(sp58);
    GXLoadPosMtxImm(sp58, 0);
    GXSetCurrentMtx(0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT7, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
    GXSetVtxAttrFmt(GX_VTXFMT7, GX_VA_TEX0, GX_TEX_ST, GX_U16, 0);
    GXSetCullMode(GX_CULL_BACK);
    GXSetZScaleOffset(1.0f, temp_r30->zscale);
    temp_r30_2 = temp_r27 + temp_r0;
    temp_r31 = temp_r26 + temp_r8;
    var_r21 = glare;
    x0s = (s16)temp_r26;
    while (var_r28 < temp_r29) {
        var_r22_3 = var_r21;
        var_r19_3 = 0;
        var_r23_2 = 0x1E;
        while ((var_r19_3 < 8) && (var_r28 < temp_r29)) {
            MTXTrans(sp28, (glare->unk14 * var_r22_3->coords[0].unk0) / (f32) gp->fbWidth, (glare->unk14 * var_r22_3->coords[0].unk4) / (f32) gp->efbHeight, 0.0f); //temp_r4, temp_r5_3, as 2 3 and temp_f4 as last
            GXLoadTexMtxImm(sp28, var_r23_2, 1);
            var_r23_2 += 3;
            var_r19_3 += 1;
            var_r22_3 = (void*)((u32)var_r22_3 + 8);
            var_r21 = (void*)((u32)var_r21 + 8);
            var_r28 += 1;
        }
        GXSetNumTevStages((u8)(var_r19_3 * 2));
        GXSetNumTexGens((u8)var_r19_3);
        GXBegin(GX_QUADS, GX_VTXFMT7, 4);
        GXPosition3s16(x0s, (s16)temp_r27, 0);
        GXTexCoord2u16(0, 0);
        GXPosition3s16((s16)temp_r31, (s16)temp_r27, 0);
        GXTexCoord2u16(1, 0);
        GXPosition3s16((s16)temp_r31, (s16)temp_r30_2, 0);
        GXTexCoord2u16(1, 1);
        GXPosition3s16(x0s, (s16)temp_r30_2, 0);
        GXTexCoord2u16(0, 1);
    }
}
#pragma opt_lifetimes reset

// 100%: the type store must follow the clamps (stores the type only after the sign/clamp checks) and
// the four s16 temps need the original's declaration order for the register allocation to line up.
void envGlareFilter(s32 type, s32 y0, s32 x0, s32 y1, s32 x1) {
    EnvironmentWork* wp = envGetWork();
    EnvironmentGlareFilter* filter = &wp->filter;

    if (type == 0) {
        filter->type = 0;
        filter->x1 = 0;
        filter->y1 = 0;
        filter->x0 = 0;
        filter->y0 = 0;
        wp->flags &= ~0x20000000;
    }
    else {
        s16 tx1, ty1, tx0, ty0; // declaration order controls the register allocation of the four temps

        tx0 = x0 - (x0 & 1);
        ty0 = y0 - (y0 & 1);
        tx1 = x1 + (x1 & 1);
        ty1 = y1 + (y1 & 1);
        if (tx0 < 0) {
            tx0 = 0;
        }
        if (ty0 < 0) {
            ty0 = 0;
        }
        if (tx1 > gp->fbWidth) {
            tx1 = gp->fbWidth;
        }
        if (ty1 > gp->efbHeight) {
            ty1 = gp->efbHeight;
        }
        filter->type = type;
        filter->y0 = ty0;
        filter->x0 = tx0;
        filter->y1 = ty1;
        filter->x1 = tx1;
        filter->zscale = 0.0f;
        filter->flags &= ~1;
        wp->flags |= 0x20000000;
    }
}

void envGlareFilterZ(f32 z) {
    EnvironmentWork* wp = envGetWork();
    EnvironmentGlareFilter* filter = &wp->filter;

    filter->zscale = z;
    filter->flags |= 1;
}

int __float_nan[];


static inline s32 __fpclassifyf(f32 x) {
    switch (*(s32*)&x & 0x7F800000) {
        case 0x7F800000:
            if (*(s32*)&x & 0x007FFFFF) {
                return FP_NAN;
            } else {
                return FP_INFINITE;
            }
            break;
        case 0:
            if (*(s32*)&x & 0x007FFFFF) {
                return FP_SUBNORMAL;
            } else {
                return FP_ZERO;
            }
            break;
        default:
            return FP_NORMAL;
    }
}


static inline f32 sqrtf(f32 x) {
    const f64 _half = .5;
    const f64 _three = 3.0;

    if (x > 0.0f) {
        f64 guess = __frsqrte(x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        return x * guess;
    } else if (x < 0.0) {
        return NAN;
    } else if (isnan(x)) {
        return NAN;
    }
    return x;
}

// envDisp_FF's copy of the unit's sqrtf: MWCC colours the last Newton step's `guess` into
// f0 (matching the target) only when the last iteration is written as a returned expression
// instead of a third assignment followed by `return x * guess;`. The shared sqrtf above is
// left as it was (now unused); this is only for envDisp_FF.
static inline f32 envFF_Sqrtf(f32 x) {
    const f64 _half = .5;
    const f64 _three = 3.0;

    if (x > 0.0f) {
        f64 guess = __frsqrte(x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        return x * (_half * guess * (_three - guess * guess * x));
    } else if (x < 0.0) {
        return NAN;
    } else if (isnan(x)) {
        return NAN;
    }
    return x;
}

// `opt_propagation off` is required or MWCC rematerialises the polynomial coefficients into
// every branch instead of hoisting them (same as evt_sub.c).
// `opt_lifetimes off` keeps the two seed loads (0.03705/0.00761) inside the loop; with the
// default lifetime pass they are hoisted into the preheader, keeping two extra FPRs live and
// shifting the whole loop allocation by one register.
#pragma opt_propagation off
#pragma opt_lifetimes off
// The five loop FPRs match the target (sinv=f20, sinC1=f21, cosv=f22, cosC1=f23,
// one=f24, pi=f29). Getting them there needs this declaration order (one, cosC1, cosv,
// sinC1, sinv) and the seed assignments in the order cosC1, one, cosv, with sinC1 assigned
// down beside sinv (that swap moves pi into f29 and fixes the preheader load order).
// The halfpi constant is assigned to the `f1` local in each branch before the nested
// compare (the envDisp_DepthOfField shape); that is what puts it in f1 as the target does.
// The two `angle * angle` squares are written inline so MWCC keeps them in f0. The tail
// uses `v / 2.0f + 0.5f` (not `v * 0.5f`) so the fused fmadds takes the variable as its
// first operand. All of this is needed. The .rodata double pool is emitted signed, mask, 0.5, 3.0, 0.0
// by the immediately generated `envdrv_pool_i2f` helper above (see its comment).
void envDisp_FF(void) {
    EnvironmentWork* wp = envGetWork();
    CameraEntry* cam;
    f32 scale, f0, f1, rnd;
    f32 one;
    f32 cosC1;
    f32 cosv;
    f32 sinC1;
    f32 sinv;
    s32 i;

    if (wp->flags & 0x10000000) {
        GXSetNumChans(GX_FALSE);
        GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
        GXSetNumTevStages(1);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
        GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXLoadTexObj(&wp->captureObj, GX_TEXMAP0);
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
        GXSetZCompLoc(GX_TRUE);
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
        GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
        GXSetLineWidth(0x10, GX_TO_ZERO);
        cam = camGetPtr(CAMERA_2D);
        GXLoadPosMtxImm(cam->view, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        GXClearVtxDesc();
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

        scale = envFF_Sqrtf(gp->fbWidth / 2 * gp->fbWidth / 2 + gp->efbHeight / 2 * gp->efbHeight / 2);

        GXBegin(GX_LINES, GX_VTXFMT0, 2000);
        for (i = 0; i < 1000; i++) {
            f32 angle = (TWO_PI * (f32)i) / 1000.0f;

            cosv = 0.03705f;
            cosC1 = 0.4967f;
            one = 1.0f;
            if (angle <= 3.1415927f) {
                f1 = 1.5707964f;
                if (angle <= f1) {
                    cosv = cosv * (angle * angle) - cosC1;
                    cosv = cosv * (angle * angle) + one;
                } else {
                    f0 = angle - f1;
                    f0 = f1 - f0;
                    f0 = f0 * f0;
                    cosv = cosv * f0 - cosC1;
                    cosv = cosv * f0 + one;
                    cosv = -cosv;
                }
            } else if (angle < 4.712389f) {
                f0 = angle - 3.1415927f;
                f0 = f0 * f0;
                cosv = cosv * f0 - cosC1;
                cosv = cosv * f0 + one;
                cosv = -cosv;
            } else {
                f0 = angle - 4.712389f;
                f1 = 1.5707964f;
                f0 = f1 - f0;
                f0 = f0 * f0;
                cosv = cosv * f0 - cosC1;
                cosv = cosv * f0 + one;
            }
            sinv = 0.00761f;
            sinC1 = 0.16605f;
            if (angle <= 3.1415927f) {
                f1 = 1.5707964f;
                if (angle <= f1) {
                    sinv = sinv * (angle * angle) - sinC1;
                    sinv = sinv * (angle * angle) + one;
                    sinv = sinv * angle;
                } else {
                    f0 = angle - f1;
                    f1 = f1 - f0;
                    f0 = f1 * f1;
                    sinv = sinv * f0 - sinC1;
                    sinv = sinv * f0 + one;
                    sinv = sinv * f1;
                }
            } else if (angle < 4.712389f) {
                f1 = angle - 3.1415927f;
                f0 = f1 * f1;
                sinv = sinv * f0 - sinC1;
                sinv = sinv * f0 + one;
                sinv = sinv * f1;
                sinv = -sinv;
            } else {
                f0 = angle - 4.712389f;
                f1 = 1.5707964f;
                f1 = f1 - f0;
                f0 = f1 * f1;
                sinv = sinv * f0 - sinC1;
                sinv = sinv * f0 + one;
                sinv = sinv * f1;
                sinv = -sinv;
            }
            rnd = (f32)(rand() % 10) / 100.0f;

            GXPosition3f32(sinv * scale, cosv * scale, 0.0f);
            GXTexCoord2f32(sinv / 2.0f + 0.5f + rnd, -cosv / 2.0f + 0.5f + rnd);
            GXPosition3f32(0.0f, 0.0f, 0.0f);
            GXTexCoord2f32(0.5f, 0.5f);
        }
    }
}
#pragma opt_lifetimes reset
#pragma opt_propagation reset

// 100% (0 instruction diffs): frame/local order matters (frustumE must be Mtx44 to leave the 0x10 gap the
// target has before scaleD), and the case-2 angle/scale temporaries need the target's declaration order.
void envTevLoadTexMtxImm(s32 type, u32 texMtx, u32 texMtx2, GXTexMtxType mtxType) {
    const EnvTevMtxData* data = &dat_802f9338;
    EnvironmentWork* wp = envGetWork();
    Mtx mtx;
    Mtx scaleA;
    Mtx rotA;
    Mtx scaleB;
    Mtx mtxB;
    Mtx invC;
    Mtx rotC;
    Mtx scaleD;
    // the target reserves 0x40 here (a 4x4 matrix), which leaves the 0x10 stack gap before scaleD
    Mtx44 frustumE;
    Mtx invE;
    Mtx concatE;
    Mtx lookE;
    Vec vecA;
    Vec vecB;
    Vec vecC;
    Vec vecD;
    Vec vecE;
    Vec vecF;
    Vec vecG;
    Vec vecH;
    Vec vecI;
    Vec vecJ;
    Vec vecK;
    Vec vecL;

    switch (type) {
    // EnvironmentWork::unk70 is a byte blob in the header; the original fields inside it are
    // +0x00 (0x70) MtxPtr, +0x08 (0x78) MtxPtr, +0x0C (0x7C) f32, +0x10 (0x80) f32, +0x14 (0x84) f32.
    case 0:
        PSMTXRotRad(mtx, 'Y', 0.017453292f * *(f32*)&wp->unk70[0x10]);
        PSMTXRotRad(rotA, 'Z', 0.017453292f * -*(f32*)&wp->unk70[0x14]);
        PSMTXConcat(mtx, rotA, mtx);
        PSMTXInvXpose(mtx, mtx);
        PSVECNormalize((Vec*)&mtx[0], (Vec*)&mtx[0]);
        PSVECNormalize((Vec*)&mtx[1], (Vec*)&mtx[1]);
        PSVECNormalize((Vec*)&mtx[2], (Vec*)&mtx[2]);
        PSMTXScale(scaleA, 0.5f, 0.5f, 1.0f);
        PSMTXConcat(scaleA, mtx, mtx);
        PSMTXTrans(scaleA, 0.5f, 0.5f, 0.0f);
        PSMTXConcat(scaleA, mtx, mtx);
        GXLoadTexMtxImm(mtx, texMtx, mtxType);
        break;
    case 1:
        PSMTXRotRad(mtx, 'Y', -1.5707964f);
        PSMTXConcat(mtx, *(MtxPtr*)&wp->unk70[8], mtx);
        PSMTXInvXpose(mtx, mtx);
        PSVECNormalize((Vec*)&mtx[0], (Vec*)&mtx[0]);
        PSVECNormalize((Vec*)&mtx[1], (Vec*)&mtx[1]);
        PSVECNormalize((Vec*)&mtx[2], (Vec*)&mtx[2]);
        PSMTXScale(scaleB, 0.5f - 0.25f * *(f32*)&wp->unk70[0xc], 0.5f, 1.0f);
        PSMTXConcat(scaleB, mtx, mtx);
        PSMTXTrans(scaleB, 0.5f, 0.5f, 0.0f);
        PSMTXConcat(scaleB, mtx, mtx);
        GXLoadTexMtxImm(mtx, texMtx, mtxType);
        break;
    case 2: {
        f32 tmp;
        f32 angX = 3.1415927f * *(f32*)&wp->unk70[0x10] / 180.0f;
        f32 angY = 3.1415927f * *(f32*)&wp->unk70[0x14] / 180.0f;
        f32 scale;
        // a named temp for cos(angY) makes f0 the multiply accumulator, matching the target's operand order
        f32 c;

        vecA = data->vec28;
        tmp = (f32)sin(angX);
        c = (f32)cos(angY);
        vecB.x = -c * tmp;
        vecB.y = -(f32)sin(angY);
        tmp = (f32)cos(angX);
        c = (f32)cos(angY);
        vecB.z = -c * tmp;
        PSMTXMultVecSR(*(MtxPtr*)wp->unk70, &vecB, &vecB);
        C_VECHalfAngle(&vecB, &vecA, &vecC);
        PSMTXInvXpose(*(MtxPtr*)&wp->unk70[8], mtx);
        PSVECNormalize((Vec*)&mtx[0], (Vec*)&mtx[0]);
        PSVECNormalize((Vec*)&mtx[1], (Vec*)&mtx[1]);
        PSVECNormalize((Vec*)&mtx[2], (Vec*)&mtx[2]);
        PSMTXIdentity(mtxB);
        mtxB[0][0] = vecC.x;
        mtxB[0][1] = vecC.y;
        mtxB[0][2] = vecC.z;
        PSMTXConcat(mtxB, mtx, mtx);
        scale = 20.0f * *(f32*)&wp->unk70[0xc];
        PSMTXScale(mtxB, scale, 1.0f, 1.0f);
        PSMTXConcat(mtxB, mtx, mtx);
        PSMTXTrans(mtxB, 1.0f - scale, 0.0f, 0.0f);
        PSMTXConcat(mtxB, mtx, mtx);
        GXLoadTexMtxImm(mtx, texMtx, mtxType);
        break;
    }
    case 3: {
        f32 angX = 3.1415927f * *(f32*)&wp->unk70[0x10] / 180.0f;
        f32 angY = 3.1415927f * *(f32*)&wp->unk70[0x14] / 180.0f;
        f32 tmp;

        vecG = data->vec34;
        vecD.x = (f32)cos(angY) * (f32)sin(angX);
        vecD.y = (f32)sin(angY);
        vecD.z = (f32)cos(angY) * (f32)cos(angX);
        PSMTXMultVecSR(*(MtxPtr*)wp->unk70, &vecD, &vecD);
        tmp = PSVECDotProduct(&vecD, &vecG);
        if (tmp == -1.0f) {
            PSMTXScale(mtx, 0.0f, 0.0f, 0.0f);
            GXLoadTexMtxImm(mtx, texMtx, GX_MTX2x4);
        } else {
            C_VECHalfAngle(&vecD, &vecG, &vecE);
            vecE.x = -vecE.x;
            vecE.y = -vecE.y;
            vecE.z = -vecE.z;
            PSMTXInvXpose(*(MtxPtr*)&wp->unk70[8], invC);
            PSVECNormalize((Vec*)&invC[0], (Vec*)&invC[0]);
            PSVECNormalize((Vec*)&invC[1], (Vec*)&invC[1]);
            PSVECNormalize((Vec*)&invC[2], (Vec*)&invC[2]);
            if (tmp == 1.0f) {
                PSMTXIdentity(rotC);
            } else {
                PSVECCrossProduct(&vecE, &vecG, &vecF);
                PSMTXRotAxisRad(rotC, &vecF, (f32)acos(PSVECDotProduct(&vecG, &vecE)));
            }
            PSMTXConcat(rotC, invC, mtx);
            angX = 2.0f * *(f32*)&wp->unk70[0xc];
            PSMTXScale(rotC, angX, angX, 1.0f);
            PSMTXConcat(rotC, mtx, mtx);
            PSMTXTrans(rotC, 0.5f, 0.5f, 0.0f);
            PSMTXConcat(rotC, mtx, mtx);
            GXLoadTexMtxImm(mtx, texMtx, mtxType);
        }
        break;
    }
    case 4:
        PSMTXInvXpose(*(MtxPtr*)&wp->unk70[8], mtx);
        PSVECNormalize((Vec*)&mtx[0], (Vec*)&mtx[0]);
        PSVECNormalize((Vec*)&mtx[1], (Vec*)&mtx[1]);
        PSVECNormalize((Vec*)&mtx[2], (Vec*)&mtx[2]);
        GXLoadTexMtxImm(mtx, texMtx, GX_MTX2x4);
        PSMTXScale(scaleD, 0.5f, 0.5f, 0.0f);
        PSMTXTrans(mtx, 0.5f, 0.5f, 1.0f);
        PSMTXConcat(mtx, scaleD, mtx);
        GXLoadTexMtxImm(mtx, texMtx2, mtxType);
        break;
    case 5:
        a += 0.0001f;
        vecK = data->vec40;
        vecK.x = 10.0f * (f32)sin(a);
        vecK.z = 10.0f * (f32)cos(a);
        vecH = vecK;
        vecL = data->vec4C;
        vecL.x = 10.0f * (f32)sin(a);
        vecL.z = 10.0f * (f32)cos(a);
        vecI = vecL;
        vecJ = data->vec58;
        C_MTXLookAt(lookE, &vecH, &vecJ, &vecI);
        C_MTXLightFrustum(frustumE, -32.0f, 32.0f, -32.0f, 32.0f, 300.0f, 0.5f, 0.5f, 0.5f, 0.5f);
        PSMTXInverse(*(MtxPtr*)wp->unk70, invE);
        PSMTXConcat(lookE, invE, concatE);
        PSMTXConcat(frustumE, concatE, mtx);
        GXLoadTexMtxImm(mtx, texMtx2, mtxType);
        break;
    }
}

// The weather/overlay texture setup the game keeps next to texPalette's: TEXGetGXTexObjFromPalette and its CI sibling
// with clamped wraps and no TLUT name, picking the CI form when the texture has a palette. Inlined at every use.
static inline void envLoadTevTexture(u32 id, u32 map, GXTexObj* obj, GXTlutObj* tlut) {
    TEXDescriptor* tdp;
    GXBool mipMap;

    if (TEXGet(env_tpl, id)->CLUTHeader != NULL) {
        tdp = TEXGet(env_tpl, id);
        if (tdp->textureHeader->minLOD == tdp->textureHeader->maxLOD) {
            mipMap = GX_FALSE;
        } else {
            mipMap = GX_TRUE;
        }
        GXInitTlutObj(tlut, tdp->CLUTHeader->data, tdp->CLUTHeader->format, tdp->CLUTHeader->numEntries);
        GXInitTexObjCI(obj, tdp->textureHeader->data, tdp->textureHeader->width, tdp->textureHeader->height,
                       tdp->textureHeader->format, GX_CLAMP, GX_CLAMP, mipMap, 0);
        GXInitTexObjLOD(obj, tdp->textureHeader->minFilter, tdp->textureHeader->magFilter,
                        tdp->textureHeader->minLOD, tdp->textureHeader->maxLOD, tdp->textureHeader->LODBias,
                        GX_DISABLE, tdp->textureHeader->edgeLODEnable, GX_ANISO_1);
        GXLoadTlut(tlut, 0);
        GXLoadTexObj(obj, map);
    } else {
        tdp = TEXGet(env_tpl, id);
        if (tdp->textureHeader->minLOD == tdp->textureHeader->maxLOD) {
            mipMap = GX_FALSE;
        } else {
            mipMap = GX_TRUE;
        }
        GXInitTexObj(obj, tdp->textureHeader->data, tdp->textureHeader->width, tdp->textureHeader->height,
                     tdp->textureHeader->format, GX_CLAMP, GX_CLAMP, mipMap);
        GXInitTexObjLOD(obj, tdp->textureHeader->minFilter, tdp->textureHeader->magFilter,
                        tdp->textureHeader->minLOD, tdp->textureHeader->maxLOD, tdp->textureHeader->LODBias,
                        GX_DISABLE, tdp->textureHeader->edgeLODEnable, GX_ANISO_1);
        GXLoadTexObj(obj, map);
    }
}

// Adds one tev stage for the overlay effect `type` (1..8, 10) and loads its texture, texgen and texture matrix.
// `param` is the shared stage cursor (EnvironmentYami's first fields: stage, texmap, texcoord).
// The three cursors must be declared as their GX enum types: MWCC's callee-saved allocator ranks them by
// type, and only with these types does it colour tevStage/texMap/texCoord to r30/r29/r28
// (plain s32 gives tevStage/texMap/texCoord = r29/r28/r30, i.e. 99.58%).
void envAddTev(s32 type, void* param) {
    EnvironmentYami* yami = param;
    EnvironmentWork* wp = envGetWork();
    GXTevStageID tevStage = yami->tevStage;
    GXTexMapID texMap = yami->texMap;
    GXTexCoordID texCoord = yami->texCoord;
    s32 unk18 = yami->unk18;
    Mtx mtx;
    GXTexObj texObj1;
    GXTexObj texObj2;
    GXTexObj texObj3;
    GXTexObj texObj4;
    GXTexObj texObj5;
    GXTexObj texObj6;
    GXTexObj texObj7;
    GXTexObj texObj8;
    GXTexObj texObj9;
    GXTlutObj tlut1;
    GXTlutObj tlut2;
    GXTlutObj tlut3;
    GXTlutObj tlut4;
    GXTlutObj tlut5;
    GXTlutObj tlut6;
    GXTlutObj tlut7;
    GXTlutObj tlut8;
    GXTlutObj tlut9;
    GXColor kColor;

    if (type != 0) {
        PSMTXConcat(camGetCurPtr()->view, yami->unk4, mtx);
        *(MtxPtr*)&wp->unk70[0] = camGetCurPtr()->view;
        *(MtxPtr*)&wp->unk70[4] = yami->unk4;
        *(MtxPtr*)&wp->unk70[8] = mtx;
        GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
        GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
        switch (type) {
        case 1:
            GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
            GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
            *(f32*)&wp->unk70[0x10] = 10.0f;
            *(f32*)&wp->unk70[0x14] = (f32)(gp->retraceCount + 45);
            *(f32*)&wp->unk70[0xC] = 0.5f;
            envLoadTevTexture(6, texMap, &texObj1, &tlut1);
            GXSetTexCoordGen2(texCoord, GX_TG_MTX2x4, GX_TG_NRM, dat_802f9338.tbl[texMap], GX_FALSE, GX_PTIDENTITY);
            envTevLoadTexMtxImm(0, dat_802f9338.tbl[texMap], 0x40, GX_MTX2x4);
            tevStage += 1;
            texCoord += 1;
            texMap += 1;
            break;
        case 2:
            GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_TEXA);
            GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
            *(f32*)&wp->unk70[0x10] = 10.0f;
            *(f32*)&wp->unk70[0x14] = (f32)(gp->retraceCount + 45);
            *(f32*)&wp->unk70[0xC] = 0.5f;
            envLoadTevTexture(8, texMap, &texObj2, &tlut2);
            GXSetTexCoordGen2(texCoord, GX_TG_MTX2x4, GX_TG_NRM, dat_802f9338.tbl[texMap], GX_FALSE, GX_PTIDENTITY);
            envTevLoadTexMtxImm(0, dat_802f9338.tbl[texMap], 0x40, GX_MTX2x4);
            tevStage += 1;
            texCoord += 1;
            texMap += 1;
            break;
        case 3:
            GXSetTevColorIn(tevStage, GX_CC_CPREV, GX_CC_ZERO, GX_CC_TEXC, GX_CC_ZERO);
            GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
            *(f32*)&wp->unk70[0xC] = 0.5f;
            envLoadTevTexture(0, texMap, &texObj3, &tlut3);
            GXSetTexCoordGen2(texCoord, GX_TG_MTX2x4, GX_TG_NRM, dat_802f9338.tbl[texMap], GX_FALSE, GX_PTIDENTITY);
            envTevLoadTexMtxImm(1, dat_802f9338.tbl[texMap], 0x40, GX_MTX2x4);
            tevStage += 1;
            texMap += 1;
            texCoord += 1;
            break;
        case 4:
            GXSetTevColorIn(tevStage, GX_CC_CPREV, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
            GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
            *(f32*)&wp->unk70[0xC] = 0.5f;
            *(f32*)&wp->unk70[0x10] = 45.0f;
            *(f32*)&wp->unk70[0x14] = 45.0f;
            envLoadTevTexture(1, texMap, &texObj4, &tlut4);
            GXSetTexCoordGen2(texCoord, GX_TG_MTX2x4, GX_TG_NRM, dat_802f9338.tbl[texMap], GX_FALSE, GX_PTIDENTITY);
            envTevLoadTexMtxImm(2, dat_802f9338.tbl[texMap], 0x40, GX_MTX2x4);
            tevStage += 1;
            texMap += 1;
            texCoord += 1;
            break;
        case 5:
            GXSetTevColorIn(tevStage, GX_CC_CPREV, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
            GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
            *(f32*)&wp->unk70[0xC] = 0.5f;
            *(f32*)&wp->unk70[0x10] = 45.0f;
            *(f32*)&wp->unk70[0x14] = 45.0f;
            envLoadTevTexture(2, texMap, &texObj5, &tlut5);
            GXSetTexCoordGen2(texCoord, GX_TG_MTX2x4, GX_TG_NRM, dat_802f9338.tbl[texMap], GX_FALSE, GX_PTIDENTITY);
            envTevLoadTexMtxImm(3, dat_802f9338.tbl[texMap], 0x40, GX_MTX2x4);
            tevStage += 1;
            texMap += 1;
            texCoord += 1;
            break;
        case 6:
            GXSetTevColorIn(tevStage, GX_CC_CPREV, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
            GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
            *(f32*)&wp->unk70[0xC] = 0.15f;
            *(f32*)&wp->unk70[0x10] = 0.0f;
            *(f32*)&wp->unk70[0x14] = 90.0f;
            envLoadTevTexture(4, texMap, &texObj6, &tlut6);
            GXSetTexCoordGen2(texCoord, GX_TG_MTX2x4, GX_TG_NRM, dat_802f9338.tbl[texMap], GX_FALSE, GX_PTIDENTITY);
            envTevLoadTexMtxImm(2, dat_802f9338.tbl[texMap], 0x40, GX_MTX2x4);
            tevStage += 1;
            texMap += 1;
            texCoord += 1;
            break;
        case 7:
            GXSetTevColorIn(tevStage, GX_CC_CPREV, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
            GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
            *(f32*)&wp->unk70[0xC] = 0.3f;
            *(f32*)&wp->unk70[0x10] = 0.0f;
            *(f32*)&wp->unk70[0x14] = 90.0f;
            envLoadTevTexture(5, texMap, &texObj7, &tlut7);
            GXSetTexCoordGen2(texCoord, GX_TG_MTX2x4, GX_TG_NRM, dat_802f9338.tbl[texMap], GX_FALSE, GX_PTIDENTITY);
            envTevLoadTexMtxImm(3, dat_802f9338.tbl[texMap], 0x40, GX_MTX2x4);
            tevStage += 1;
            texMap += 1;
            texCoord += 1;
            break;
        case 8:
            GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
            GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
            *(f32*)&wp->unk70[0xC] = 0.5f;
            *(f32*)&wp->unk70[0x10] = 10.0f;
            *(f32*)&wp->unk70[0x14] = 45.0f;
            envLoadTevTexture(10, texMap, &texObj8, &tlut8);
            GXSetTexCoordGen2(texCoord, GX_TG_MTX2x4, GX_TG_NRM, dat_802f9338.tbl[texMap], GX_FALSE, GX_PTIDENTITY);
            envTevLoadTexMtxImm(0, dat_802f9338.tbl[texMap], 0x40, GX_MTX2x4);
            tevStage += 1;
            texMap += 1;
            texCoord += 1;
            break;
        case 10:
            GXSetTevColorIn(tevStage, GX_CC_CPREV, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
            GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
            GXSetTevKColorSel(tevStage, GX_TEV_KCSEL_K1);
            kColor = (GXColor){ 0x7F, 0x7F, 0x7F, 0x00 };
            GXSetTevKColor(GX_KCOLOR1, kColor);
            envLoadTevTexture(9, texMap, &texObj9, &tlut9);
            GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_NRM, dat_802f9338.tbl[texMap], GX_FALSE, GX_PTTEXMTX0);
            envTevLoadTexMtxImm(4, dat_802f9338.tbl[texMap], 0x40, GX_MTX3x4);
            tevStage += 1;
            texMap += 1;
            texCoord += 1;
            break;
        }
        yami->tevStage = tevStage;
        yami->texCoord = texCoord;
        yami->unk18 = unk18;
        yami->texMap = texMap;
    }
}

// The map-object flag logic matches the target's block layout: `flags` is a local, the entry tests
// `flags & 0x20` and branches into the loop (`while ((obj = obj->parent) != NULL)`), with the
// set-flag path in the else arm. The bbox `lhz` loads sit in both arms (inside the walk before the
// `break`, and in the else), as in the original, so the walk running out falls through to the flag
// comparisons with whatever those registers hold. The initial `flags` load is written through
// `((MapObject*)yami->unk0)` rather than `obj`; that makes MWCC spill `yami` first (`mr r17, r3`)
// and load obj with `lwz r3, 0x0(r3)` as the target does.
#pragma opt_propagation off // stops MWCC re-evaluating yami->unk0, so obj/flags colour as the target
#pragma opt_lifetimes off // needed: without it envSetWater 99.58%
// `texMap` is an `int` so its enum-typed `texMap + 1` arguments are not merged into one temporary; the
// water texel offset is built in two steps from `i / 4` and `i % 4`; the frustum's near distance is an
// argument expression, so its int->float temporary is created after the four edge conversions.
void envSetWater(s32 arg0, void* arg1) {
    EnvironmentYami* yami = (EnvironmentYami*)arg0;
    s32 tevStage;
    int texMap;
    void* capture;
    s32 texCoord;
    s32 unk18;
    u8* water;
    MapObject* obj;
    u32 size;
    u32 flags;
    s32 i;
    s32 width;
    s32 j;
    s32 x0;
    s32 y0;
    s32 x1;
    s32 y1;
    s32 height;
    GXTexObj captureObj;
    GXTexObj waterObj;
    Vec pos;
    Mtx mtx;
    f32 indMtx[2][3];
    f32 fovTan;
    f32 scale;
#ifndef MUST_MATCH
    x0 = y0 = x1 = y1 = 0;
#endif

    obj = (MapObject*)yami->unk0;
    tevStage = yami->tevStage;
    flags = ((MapObject*)yami->unk0)->flags;
    texMap = yami->texMap;
    texCoord = yami->texCoord;
    unk18 = yami->unk18;

    if (flags & 0x20) {
        while ((obj = obj->parent) != NULL) {
            flags = obj->flags;
            if ((flags & 0x10) == 0) {
                continue;
            }
            if ((flags & 0x20000000) == 0) {
                obj->flags = flags | 0x20000000;
                return;
            }
            x0 = *(u16*)(obj->field_0xEC + 0);
            y0 = *(u16*)(obj->field_0xEC + 2);
            x1 = *(u16*)(obj->field_0xEC + 4);
            y1 = *(u16*)(obj->field_0xEC + 6);
            break;
        }
    }
    else {
        if ((flags & 0x20000000) == 0) {
            obj->flags = flags | 0x20000000;
            return;
        }
        x0 = *(u16*)(obj->field_0xEC + 0);
        y0 = *(u16*)(obj->field_0xEC + 2);
        x1 = *(u16*)(obj->field_0xEC + 4);
        y1 = *(u16*)(obj->field_0xEC + 6);
    }

    if (x0 == 0x3FF || y0 == 0x3FF || x1 == 0x3FF || y1 == 0x3FF) {
        return;
    }
    if (x0 == 0 && y0 == 0 && x1 == 0 && y1 == 0) {
        return;
    }

    x0 -= x0 & 1;
    y0 -= y0 & 1;
    x1 += x1 & 1;
    y1 += y1 & 1;
    if (x0 < 0) {
        x0 = 0;
    }
    if (y0 < 0) {
        y0 = 0;
    }
    if (x1 > gp->fbWidth) {
        x1 = gp->fbWidth;
    }
    if (y1 > gp->efbHeight) {
        y1 = gp->efbHeight;
    }

    width = x1 - x0;
    height = y1 - y0;

    size = GXGetTexBufferSize((u16)(width / 2), (u16)(height / 2), GX_TF_RGB565, GX_FALSE, 0);
    capture = smartAlloc(size, HEAP_EFFECT)->address;
    GXSetTexCopySrc((u16)x0, (u16)y0, (u16)width, (u16)height);
    GXSetTexCopyDst((u16)(width / 2), (u16)(height / 2), GX_TF_RGB565, GX_TRUE);
    GXCopyTex(capture, GX_FALSE);
    GXPixModeSync();
    GXTexModeSync();
    GXInitTexObj(&captureObj, capture, (u16)(width / 2), (u16)(height / 2), GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GXInitTexObjLOD(&captureObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);

    PSMTXMultVec(yami->unk4, &((MapObject*)yami->unk0)->center, &pos);
    scale = 1000.0f / PSVECDistance(&camGetCurPtr()->cameraPos, &pos);
    if (scale < 0.0f) {
        scale = 0.0f;
    }
    if (scale > 127.0f) {
        scale = 127.0f;
    }

    size = GXGetTexBufferSize(0x40, 0x40, GX_TF_IA8, GX_FALSE, 0);
    water = smartAlloc(size, HEAP_EFFECT)->address;

    for (i = 0; i < 0x40; i++) {
        for (j = 0; j < 0x40; j++) {
            f32 s = (f32)sin(18.849552154541016f * (f32)j / 64.0f + (f32)gp->retraceCount / 10.0f);
            s32 v = (s32)(0.6f + (scale * s + 128.0f));
            s32 base, col, idx;
            u8* p;

            base = i / 4;
            col = i % 4;
            idx = base * 32 + (j / 4) * 512;
            idx += (col + (j % 4) * 4) * 2;
            p = &water[idx];
            p[0] = v;
            p[1] = 0x80;
        }
    }

    DCFlushRange(water, 0x2000);
    GXInitTexObj(&waterObj, water, 0x40, 0x40, GX_TF_IA8, GX_REPEAT, GX_REPEAT, GX_FALSE);
    GXInitTexObjLOD(&waterObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);

    fovTan = (f32)tan(6.2831855f * (camGetPtr(CAMERA_3D)->fovY / 2.0f) / 360.0f);
    C_MTXLightFrustum(mtx,
        (f32)(gp->efbHeight / 2 - y1),
        (f32)(gp->efbHeight / 2 - y0),
        (f32)(x0 - gp->fbWidth / 2),
        (f32)(x1 - gp->fbWidth / 2),
        (f32)gp->efbHeight / 2.0f / fovTan,
        0.5f, 0.5f, 0.5f, 0.5f);
    PSMTXConcat(mtx, camGetCurPtr()->view, mtx);
    PSMTXConcat(mtx, yami->unk4, mtx);

    GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, dat_802f9338.tbl[texMap], GX_FALSE, GX_PTIDENTITY);
    GXLoadTexMtxImm(mtx, dat_802f9338.tbl[texMap], GX_MTX3x4);
    GXLoadTexObj(&captureObj, texMap);
    GXLoadTexObj(&waterObj, texMap + 1);
    GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
    GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(tevStage, GX_CC_TEXC, GX_CC_CPREV, GX_CC_APREV, GX_CC_ZERO);
    GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
    indMtx[0][0] = 0.5f;
    indMtx[0][1] = 0.0f;
    indMtx[0][2] = 0.0f;
    indMtx[1][0] = 0.0f;
    indMtx[1][1] = 0.5f;
    indMtx[1][2] = 0.0f;
    GXSetIndTexMtx(GX_ITM_0, indMtx, 1);
    GXSetIndTexOrder(unk18, texCoord, texMap + 1);
    GXSetIndTexCoordScale(unk18, GX_ITS_1, GX_ITS_1);
    GXSetTevIndWarp(tevStage, unk18, GX_TRUE, GX_FALSE, GX_ITM_0);

    tevStage += 1;
    texCoord += 1;
    yami->tevStage = tevStage;
    unk18 += 1;
    texMap += 2;
    yami->texCoord = texCoord;
    yami->unk18 = unk18;
    yami->texMap = texMap;
}
#pragma opt_lifetimes reset
#pragma opt_propagation reset

void envSetYamiView(Mtx mtx) {
    EnvironmentWork* wp = envGetWork();

    PSMTXCopy(mtx, (MtxPtr)wp->unkC0);
}

#pragma opt_common_subs off // MWCC otherwise CSE-hoists tevStage-1/+1/+2 across the GX calls into callee-saved regs
#pragma opt_propagation off  // keep texMap*4 and the loop counters in the target's callee-saved colouring
void envSetYami(EnvironmentYami* yami) {
    GlobalWork* g = gp;
    EnvironmentWork* wp = g->inBattle ? &work[1] : &work[0];
    u8* texMtx;
    // texMap*4 kept in a callee-saved register (r29); declared before the counters so it colours after them.
    s32 texMtxOff;
    s32 tevStage = yami->tevStage;
    s32 texMap = yami->texMap;
    s32 texCoord = yami->texCoord;
    s32 unk18 = yami->unk18;
    Mtx mtx;
    GXTexObj texObj;

    u32 oldCount = v_count;
    u32 newCount = g->retraceCount;
    if (oldCount != newCount) {
        v_count = newCount;

        u1 += 0.004f;
        u2 -= 0.004f;
        if (u1 > 10.0f) {
            u1 -= 10.0f;
        }
        if (u2 < 10.0f) {
            u2 += 10.0f;
        }
    }

    GXSetTevColorOp(tevStage - 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
    GXSetTevAlphaOp(tevStage - 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);

    C_MTXLightOrtho(mtx, 72.0f, -72.0f, -96.0f, 96.0f, 0.5f, 0.5f, u1, 0.5f);
    PSMTXConcat(mtx, (MtxPtr)wp->unkC0, mtx);
    PSMTXConcat(mtx, yami->unk4, mtx);
    texMtx = (u8*)dat_802f9338.tbl;
    texMtxOff = texMap * 4;
    GXLoadTexMtxImm(mtx, *(s32*)(texMtx + texMtxOff), GX_MTX3x4);
    GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, *(s32*)(texMtx + texMtxOff), GX_FALSE, GX_PTIDENTITY);

    C_MTXLightOrtho(mtx, 72.0f, -72.0f, -96.0f, 96.0f, 0.5f, 0.5f, u2, 0.5f);
    PSMTXConcat(mtx, (MtxPtr)wp->unkC0, mtx);
    PSMTXConcat(mtx, yami->unk4, mtx);
    {
        s32* p = (s32*)((u8*)dat_802f9338.tbl + texMtxOff);
        GXLoadTexMtxImm(mtx, *++p, GX_MTX3x4);
        GXSetTexCoordGen2(texCoord + 1, GX_TG_MTX3x4, GX_TG_POS, *p, GX_FALSE, GX_PTIDENTITY);
    }

    TEXGetGXTexObjFromPalette(env_tpl, &texObj, 9);
    GXLoadTexObj(&texObj, texMap);
    TEXGetGXTexObjFromPalette(env_tpl, &texObj, 10);
    GXLoadTexObj(&texObj, texMap + 1);

    GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
    GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
    GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);

    GXSetTevOrder(tevStage + 1, texCoord + 1, texMap + 1, GX_COLOR_NULL);
    GXSetTevColorOp(tevStage + 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(tevStage + 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(tevStage + 1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXA, GX_CC_ZERO);
    GXSetTevAlphaIn(tevStage + 1, GX_CA_ZERO, GX_CA_APREV, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevSwapMode(tevStage + 1, GX_TEV_SWAP0, GX_TEV_SWAP0);

    GXSetTevOrder(tevStage + 2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevColorOp(tevStage + 2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(tevStage + 2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(tevStage + 2, GX_CC_C0, GX_CC_ZERO, GX_CC_APREV, GX_CC_ZERO);
    GXSetTevAlphaIn(tevStage + 2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
    GXSetTevSwapMode(tevStage + 2, GX_TEV_SWAP0, GX_TEV_SWAP0);

    tevStage += 3;
    texCoord += 2;
    yami->tevStage = tevStage;
    texMap += 2;
    yami->texCoord = texCoord;
    yami->unk18 = unk18;
    yami->texMap = texMap;
}
#pragma opt_common_subs reset
#pragma opt_propagation reset
