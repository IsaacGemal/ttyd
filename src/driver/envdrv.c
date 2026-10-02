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

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u32 lbl_80398960[2] = { 0xBF800000, 0x3F800000 };

extern GlobalWork* gp;
extern const f32 float_1000_8041e540;
extern const f32 float_127_8041e544;
extern const f32 float_18p85_8041e548;
extern const f32 float_0p015625_8041e54c;
extern const f32 float_10_8041e524;
extern const f32 float_0p6_8041e550;
extern const f32 float_128_8041e554;
extern const f32 float_6p2832_8041e558;
extern const f32 float_360_8041e55c;
extern const f32 float_0p5_8041e538;
extern const f32 float_0_8041e53c;
extern const f32 float_45_8041e560;
extern const f32 float_0p15_8041e564;
extern const f32 float_0p3_8041e56c;
extern const f32 float_90_8041e568;

// .sdata2
const GXColor dat_8041e510 = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor dat_8041e514 = { 0xFF, 0xFF, 0xFF, 0x00 };
const GXColor dat_8041e518 = { 0xFF, 0xFF, 0xFF, 0xA0 };
const GXColor dat_8041e51c = { 0x7F, 0x7F, 0x7F, 0x00 };

//.data
EnvironmentGlare envGlare0 = {
    GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_TEV_KCSEL_K1_A, GX_TEV_KCSEL_K0_A, 5.25f, 16,
    {
        {-1.0f, 0.0f}, {-3.0f, 0.0f}, {-5.0f, 0.0f}, {-7.0f, 0.0f},
    {1.0f, 0.0f}, {3.0f, 0.0f}, {5.0f, 0.0f}, {7.0f, 0.0f},
    {0.0f, -1.0f}, {0.0f, -3.0f}, {0.0f, -5.0f}, {0.0f, -7.0f},
    {0.0f, 1.0f}, {0.0f, 3.0f}, {0.0f, 5.0f}, {0.0f, 7.0f},
}
};

EnvironmentGlare envGlare1 = {
    GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_TEV_KCSEL_K1_A, GX_TEV_KCSEL_1, 3.5f, 8,
    {
        {-1.41419994831f, -1.41419994831f}, {0.0f, -1.0f}, {1.41419994831f, -1.41419994831f}, {-1.0f, 0.0f},
    {1.0f, 0.0f}, {-1.41419994831f, 1.41419994831f}, {0.0f, 1.0f}, {1.41419994831f, 1.41419994831f},
}
};

EnvironmentGlare envGlare2 = {
    GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_TEV_KCSEL_K1_A, GX_TEV_KCSEL_3_4, 3.5f, 52,
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

EnvironmentGlare envGlare3 = {
    GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_TEV_KCSEL_K1_A, GX_TEV_KCSEL_1, 3.5f, 16,
    {
        {0.0f, -4.0f}, {-1.0f, -3.0f}, {1.0f, -3.0f}, {-2.0f, -2.0f},
    {2.0f, -2.0f}, {-3.0f, -1.0f}, {3.0f, -1.0f}, {-3.0f, 0.0f},
    {3.0f, 0.0f}, {-3.0f, 1.0f}, {3.0f, 1.0f}, {-2.0f, 2.0f},
    {2.0f, 2.0f}, {-1.0f, 3.0f}, {1.0f, 3.0f}, {0.0f, 4.0f},
}
};

//.bss
static EnvironmentWork work[2];

//.sbss
TEXPalette* env_tpl;
s32 tpl_ok;
f32 frame_cnt;
f32 a;
f32 u1;
f32 u2;
s32 v_count;

//local prototypes

//there is no function so we'll make a macro
#define envGetWork() (gp->inBattle ? &work[1] : &work[0])

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
#ifdef MUST_MATCH
    u8* base = (u8*)lbl_80398960;
#endif

    GXGetViewportv(vp);
    GXGetProjectionv(pm);
    wp = envGetWork();
    filter = &wp->filter;
    if (wp->flags & 0x20000000) {
        switch (filter->type) {
            case 0:
                break;
#ifdef MUST_MATCH
            // retail addresses envGlare0..3 as offsets from lbl_80398960 (0x40/0xdc/0x138/0x2f4)
            case 1:
                envGlare((EnvironmentGlare*)(base + 0x40));
                break;
            case 2:
                envGlare((EnvironmentGlare*)(base + 0xdc));
                break;
            case 3:
                envGlare((EnvironmentGlare*)(base + 0x138));
                break;
            case 4:
                envGlare((EnvironmentGlare*)(base + 0x2f4));
                break;
#else
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
#endif
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







#define PI 3.141592653589793f
#define TWO_PI 6.283185307179586f
s32 texmtx_tbl[] = {30, 33, 36, 39, 42, 45, 48, 51, 54, 57, 4, 5, 6, 14, 0, 1, 2, 3, 8, 9, 10}; //TODO: put in right spot

// NONMATCHING (55.26%): 33 instructions differ, 239 missing, 104 extra, plus register differences.
void envDisp_DepthOfField(void) {
    EnvironmentWork* wp = envGetWork();
    Mtx sp48;
    Mtx sp18;
    s8 sp17;
    u8 sp14;
    s32 sp10;
    s32 spC;
    s32 sp8;
    f32 temp_f0;
    f32 temp_f0_10;
    f32 temp_f0_11;
    f32 temp_f0_12;
    f32 temp_f0_13;
    f32 temp_f0_14;
    f32 temp_f0_15;
    f32 temp_f0_16;
    f32 temp_f0_17;
    f32 temp_f0_18;
    f32 temp_f0_19;
    f32 temp_f0_20;
    f32 temp_f0_21;
    f32 temp_f0_22;
    f32 temp_f0_23;
    f32 temp_f0_24;
    f32 temp_f0_25;
    f32 temp_f0_26;
    f32 temp_f0_27;
    f32 temp_f0_28;
    f32 temp_f0_2;
    f32 temp_f0_3;
    f32 temp_f0_4;
    f32 temp_f0_5;
    f32 temp_f0_6;
    f32 temp_f0_7;
    f32 temp_f0_8;
    f32 temp_f0_9;
    f32 temp_f1;
    f32 temp_f1_10;
    f32 temp_f1_2;
    f32 temp_f1_3;
    f32 temp_f1_4;
    f32 temp_f1_5;
    f32 temp_f1_6;
    f32 temp_f1_7;
    f32 temp_f1_8;
    f32 temp_f1_9;
    f32 temp_f24;
    f32 temp_f24_2;
    f32 temp_f25;
    f32 temp_f25_2;
    f32 temp_f26;
    f32 temp_f26_2;
    f32 temp_f27;
    f32 temp_f27_2;
    f32 temp_f28;
    f32 temp_f28_2;
    f32 temp_f29;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f30;
    f32 temp_f31;
    f32 temp_f31_2;
    f32 temp_f3;
    f32 temp_f3_2;
    f32 temp_f3_3;
    f32 temp_f3_4;
    f32 temp_f4;
    f32 temp_f4_2;
    f32 var_f2;
    f32 var_f2_2;
    f32 var_f4;
    f32 var_f4_2;
    s32 temp_r26;
    s32 temp_r27_2;
    s32 temp_r28;
    s32 var_r25;
    s32 var_r25_2;
    s32 var_r26;
    s32 var_r30_2;
    //s32 var_r31;
    s32 var_r31_2;
    s32* var_r26_2;
    s32* var_r30;
    u8 temp_r5;
    u8 temp_r5_2;
    CameraEntry* temp_r27;
    EnvironmentUnk88* temp_r29;
    GlobalWork* temp_r8;
    GlobalWork* temp_r8_2;

    GXTexObj* texobj;
    GXTevColorArg v33; // r7
    GXTevAlphaArg v34; // r7
    GXTevColorArg v59; // r7
    GXTevAlphaArg v60; // r7
    int i;

    f32 v15; // fp31
    u32 *v16; // r30
    GXTexCoordID v17; // r25
    f32 v18;
    GXTexCoordID v19;
    f32 v20;
    f32 v22;
    f32 v23;
    f32 v24;
    f32 v25;
    f32 v26;
    f32 v27;
    f32 v28;
    f32 v29;
    f32 v30;
    f32 v31;
    f32 v32;
    f32 v81;

    s32 r25, r26, r31;
    s32* r30;
    f32 f0, f1, f2, f3, f4, f24, f25, f26, f27, f28, f29, f31;

    temp_r29 = &wp->unk88;
    if (wp->flags & 0x80000000) {
        temp_r27 = camGetPtr(CAMERA_2D);
        GXSetProjection(camGetPtr(CAMERA_2D)->projection, temp_r27->projType);
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
        if ((temp_r29->unk0 & 1) && (temp_r29->unk4)) {
            f31 = TWO_PI;
            r30 = texmtx_tbl;
            r25 = 0;
            f29 = 0.25f;
            r26 = 0;
            f26 = 0.16605f;
            r31 = 0;
            f27 = 1.0f;
            f28 = PI;
            f25 = 0.4967f;
            f24 = 100.0f;
            for (;r31 < 4;) {
                f2 = 0.00761f;
                f0 = (f32)r31;
                f0 = (f31 * f0);
                f3 = f0 * f29;
                if (f3 <= f28) {
                    f1 = MTXDegToRad(90.0f);
                    if (f3 <= f1) {
                        f0 = f3 * f3;
                        f2 = (f2 * f0) - f26;
                        f2 = (f2 * f0) + f27;
                        f2 - f2 * f3;
                    }
                    else {
                        f0 = f3 - f1;
                        f1 = f1 - f0;
                        f0 = f1 * f1;
                        f2 = (f2 * f0) - f26;
                        f2 = (f2 * f0) + f27;
                        f2 - f2 * f1;
                    }
                }
                else {
                    f0 = MTXDegToRad(270.0f);
                    if (f3 <= f0) {
                        f0 = f3 - f0;
                        f1 = MTXDegToRad(90.0f);
                        f1 = f1 - f0;
                        f0 = f1 * f1;
                        f2 = (f2 * f0) - f26;
                        f2 = (f2 * f0) + f27;
                        f2 = f2 * f1;
                        f2 = -f2;
                    }
                    else {
                        f1 = f3 - f28;
                        f0 = f1 * f1;
                        f2 = (f2 * f0) - f26;
                        f2 = (f2 * f0) + f27;
                        f2 = f2 * f1;
                        f2 = -f2;
                    }
                }
                f4 = 0.03705f;
                if (f3 > f28) {
                    f0 = MTXDegToRad(270.0f);
                    if (f3 >= f1) {
                        f0 = f3 - f0;
                        f1 = MTXDegToRad(90.0f);
                        f0 = f1 - f0;
                        f0 = f0 * f0;
                        f4 = (f4 * f0) - f25;
                        f4 = (f4 * f0) + f27;
                    }
                    else {
                        f0 = f3 - f28;
                        f0 = f0 * f0;
                        f4 = (f4 * f0) - f25;
                        f4 = (f4 * f0) + f27;
                        f4 = -f4;
                    }

                }
                else {
                    f1 = MTXDegToRad(90.0f);
                    if (f3 > f1) {
                        f0 = f3 - f1;
                        f0 = f1 - f0;
                        f0 = f0 * f0;
                        f4 = (f4 * f0) - f25;
                        f4 = (f4 * f0) + f27;
                        f4 = -f4;
                    }
                    else {
                        f0 = f3 * f3;
                        f4 = (f4 * f0) - f25;
                        f4 = (f4 * f0) + f27;
                    }
                }

                MTXTrans(sp48, (temp_r29->unk4 * f2) / f24, (temp_r29->unk4 * f4) / f24, 0.0f);
                r31++;
            }
            /*v15 = TWO_PI;
            v16 = texmtx_tbl;
            v17 = GX_TEXCOORD0;
            v18 = 0.25f;
            v19 = GX_TEVSTAGE0;
            v20 = 0.16605f;
            i = 0;
            v22 = 1.0f;
            v23 = PI;
            v24 = 0.4967f;
            v25 = 100.0f;
            for (i = 0; i < 4; i++) {
            v81 = (f32)i;
            v26 = (v15 * v81) * v18;
            if (v26 <= v23) {
            if (v26 <= MTXDegToRad(90.0f)) {
            v27 = (((((0.00761f * (v26 * v26)) - v20) * (v26 * v26)) + v22) * v26);
            }
            else {
            v28 = (MTXDegToRad(90.0f) - (((v15 * v81) * v18) - MTXDegToRad(90.0f)));
            v27 = (((((0.00761f * (v28 * v28)) - v20) * (v28 * v28)) + v22) * v28);
            }
            }
            else {
            if (v26 < MTXDegToRad(270.0f)) {
            v27 = -(((((0.00761f * ((v26 - v23) * (v26 - v23))) - v20) * ((v26 - v23) * (v26 - v23))) + v22) * (v26 - v23));
            }
            else {
            v29 = (MTXDegToRad(90.0f) - (((v15 * v81) * v18) - MTXDegToRad(270.0f)));
            v27 = -(((((0.00761f * (v29 * v29)) - v20) * (v29 * v29)) + v22) * v29);
            }
            }
            if (v26 <= v23) {
            if (v26 <= MTXDegToRad(90.0f)) {
            v30 = ((((0.03705f * (v26 * v26)) - v24) * (v26 * v26)) + v22);
            }
            else {
            v31 = (MTXDegToRad(90.0f) - (((v15 * v81) * v18) - MTXDegToRad(90.0f)));
            v30 = -((((0.03705f * (v31 * v31)) - v24) * (v31 * v31)) + v22);
            }
            }
            else {
            if (v26 < MTXDegToRad(270.0f)) {
            v30 = -((((0.03705f * ((v26 - v23) * (v26 - v23))) - v24) * ((v26 - v23) * (v26 - v23))) + v22);
            }
            else {
            v32 = (MTXDegToRad(90.0f) - (((v15 * v81) * v18) - MTXDegToRad(270.0f)));
            v30 = ((((0.03705f * (v32 * v32)) - v24) * (v32 * v32)) + v22);
            }
            }
            PSMTXTrans(
            sp48,
            ((temp_r29->unk4 * v27) / v25),
            ((temp_r29->unk4 * v30) / v25),
            0.0f);
            GXLoadTexMtxImm(sp48, *v16, GX_MTX2x4);
            GXSetTexCoordGen2(v17, GX_TG_MTX2x4, GX_TG_TEX0, *v16, GX_FALSE, GX_PTIDENTITY);
            GXSetTevKColorSel(v19, GX_TEV_KCSEL_1_4);
            GXSetTevKAlphaSel(v19, 6);
            GXSetTevOrder(v19, v17, GX_TEXMAP0, GX_COLOR_NULL);
            GXSetTevColorOp(v19, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, (GXTevRegID)(i == 3));
            GXSetTevAlphaOp(v19, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, (GXTevRegID)(i == 3));
            v33 = GX_CC_CPREV;
            if ( !i )
            v33 = GX_CC_ZERO;
            GXSetTevColorIn(v19, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, v33);
            v34 = GX_CA_APREV;
            if ( !i )
            v34 = GX_CA_ZERO;
            GXSetTevAlphaIn(v19, GX_CA_ZERO, GX_CA_TEXA, GX_CA_KONST, v34);
            GXSetTevSwapMode(v19, GX_TEV_SWAP0, GX_TEV_SWAP0);
            ++i;
            ++v16;
            ++v19;
            ++v17;
            }*/
            /*temp_f31 = TWO_PI;
            var_r30 = texmtx_tbl;
            var_r25 = 0;
            temp_f29 = 0.25f;
            var_r26 = 0;
            temp_f26 = 0.16605f;
            //var_r31 = 0;
            temp_f27 = 1.0f;
            temp_f28 = PI;
            temp_f25 = 0.4967f;
            temp_f24 = 100.0f;
            for (i = 0; i < 4; i++) {
            //do {
            temp_f2 = 0.00761f;
            temp_f3 = temp_f31 * (f32) i * temp_f29;
            if (temp_f3 <= temp_f28) {
            temp_f1 = MTXDegToRad(-90.0f);
            if (temp_f3 <= temp_f1) {
            temp_f0 = temp_f3 * temp_f3;
            var_f2 = ((((temp_f2 * temp_f0) - temp_f26) * temp_f0) + temp_f27) * temp_f3;
            } else {
            temp_f1_2 = temp_f1 - (temp_f3 - temp_f1);
            temp_f0_2 = temp_f1_2 * temp_f1_2;
            var_f2 = ((((temp_f2 * temp_f0_2) - temp_f26) * temp_f0_2) + temp_f27) * temp_f1_2;
            }
            } else {
            temp_f0_3 = MTXDegToRad(270.0f);
            if (temp_f3 < temp_f0_3) {
            temp_f1_3 = temp_f3 - temp_f28;
            temp_f0_4 = temp_f1_3 * temp_f1_3;
            var_f2 = -(((((temp_f2 * temp_f0_4) - temp_f26) * temp_f0_4) + temp_f27) * temp_f1_3);
            } else {
            temp_f1_4 = MTXDegToRad(-90.0f) - (temp_f3 - temp_f0_3);
            temp_f0_5 = temp_f1_4 * temp_f1_4;
            var_f2 = -(((((temp_f2 * temp_f0_5) - temp_f26) * temp_f0_5) + temp_f27) * temp_f1_4);
            }
            }
            temp_f4 = 0.03705f;
            if (temp_f3 <= temp_f28) {
            temp_f1_5 = MTXDegToRad(-90.0f);
            if (temp_f3 <= temp_f1_5) {
            temp_f0_6 = temp_f3 * temp_f3;
            var_f4 = (((temp_f4 * temp_f0_6) - temp_f25) * temp_f0_6) + temp_f27;
            } else {
            temp_f0_7 = temp_f1_5 - (temp_f3 - temp_f1_5);
            temp_f0_8 = temp_f0_7 * temp_f0_7;
            var_f4 = -((((temp_f4 * temp_f0_8) - temp_f25) * temp_f0_8) + temp_f27);
            }
            } else {
            temp_f0_9 = MTXDegToRad(270.0f);
            if (temp_f3 < temp_f0_9) {
            temp_f0_10 = temp_f3 - temp_f28;
            temp_f0_11 = temp_f0_10 * temp_f0_10;
            var_f4 = -((((temp_f4 * temp_f0_11) - temp_f25) * temp_f0_11) + temp_f27);
            } else {
            temp_f0_12 = MTXDegToRad(-90.0f) - (temp_f3 - temp_f0_9);
            temp_f0_13 = temp_f0_12 * temp_f0_12;
            var_f4 = (((temp_f4 * temp_f0_13) - temp_f25) * temp_f0_13) + temp_f27;
            }
            }
            temp_f0_14 = temp_r29->unk4;
            PSMTXTrans(sp48, (temp_f0_14 * var_f2) / temp_f24, (temp_f0_14 * var_f4) / temp_f24, 0.0f);
            GXLoadTexMtxImm(sp48, *var_r30, 1);
            GXSetTexCoordGen2(var_r25, 1, 4, *var_r30, 0, 0x7D);
            GXSetTevKColorSel(var_r26, 6);
            GXSetTevKAlphaSel(var_r26, 6);
            GXSetTevOrder(var_r26, var_r25, 0, 0xFF);
            GXSetTevColorOp(var_r26, 0, 0, 0, 1, i == 3);
            GXSetTevAlphaOp(var_r26, 0, 0, 0, 1, i == 3);
            v33 = GX_CC_CPREV;
            if ( !i )
            v33 = GX_CC_ZERO;
            GXSetTevColorIn(var_r26, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, v33);
            v34 = GX_CA_APREV;
            if ( !i )
            v34 = GX_CA_ZERO;
            GXSetTevAlphaIn(var_r26, GX_CA_ZERO, GX_CA_TEXA, GX_CA_KONST, v34);
            GXSetTevSwapMode(var_r26, GX_TEV_SWAP0, GX_TEV_SWAP0);
            temp_r27_2 = var_r31 == 0;
            if (temp_r27_2 != 0) {

            }
            GXSetTevColorIn(var_r26, 0xF, 8, 0xE);
            if (temp_r27_2 != 0) {

            }
            GXSetTevAlphaIn();
            GXSetTevSwapMode(var_r26, 0, 0);
            //var_r31 += 1;
            var_r30 += 4;
            var_r26 += 1;
            var_r25 += 1;
            }// while (var_r31 < 4);*/
            /*GXSetTevKColor(GX_KCOLOR0, (GXColor){temp_r29->unk2, 0, 0, 0xFF});
            GXSetTexCoordGen2(v17, 1, 4, 0x3C, 0, 0x7D);
            GXSetTevKColorSel(v19, 0xC);
            GXSetTevKAlphaSel(v19, 0x1C);
            GXSetTevOrder(v19, v17, 1, 0xFF);
            GXSetTevColorOp(v19, 8, 0, 0, 1, 0);
            GXSetTevAlphaOp(v19, 8, 0, 0, 1, 0);
            GXSetTevColorIn(v19, 8, 0xE, 2, 0xF);
            GXSetTevAlphaIn(v19, 4, 6, 1, 7);
            GXSetTevSwapMode(v19, 0, 0);
            GXSetNumTexGens((s8) (v17 + 1));
            GXSetNumTevStages((s8) (v19 + 1));*/
            GXBegin(0x80, 0, 4);
            temp_r8 = gp;
            temp_f3_2 = 0.5f;
            *(f32* )0xCC010000 = (f32) -(s32) temp_r8->fbWidth * temp_f3_2;
            *(f32* )0xCC008000 = (f32) temp_r8->efbHeight * temp_f3_2;
            *(f32* )0xCC008000 = 0.0f;
            *(f32* )0xCC008000 = 0.0f;
            *(f32* )0xCC008000 = 0.0f;
            *(f32* )0xCC008000 = (f32) temp_r8->fbWidth * temp_f3_2;
            *(f32* )0xCC008000 = (f32) temp_r8->efbHeight * temp_f3_2;
            *(f32* )0xCC008000 = 0.0f;
            *(f32* )0xCC008000 = 1.0f;
            *(f32* )0xCC008000 = 0.0f;
            *(f32* )0xCC008000 = (f32) temp_r8->fbWidth * temp_f3_2;
            *(f32* )0xCC008000 = (f32) -(s32) temp_r8->efbHeight * temp_f3_2;
            *(f32* )0xCC008000 = 0.0f;
            *(f32* )0xCC008000 = 1.0f;
            *(f32* )0xCC008000 = 1.0f;
            *(f32* )0xCC008000 = (f32) -(s32) temp_r8->fbWidth * temp_f3_2;
            *(f32* )0xCC008000 = (f32) -(s32) temp_r8->efbHeight * temp_f3_2;
            *(f32* )0xCC008000 = 0.0f;
            *(f32* )0xCC008000 = 0.0f;
            *(f32* )0xCC008000 = 1.0f;
        }
        if ((temp_r29->unk0 & 2) && (temp_r29->unk8 != 0.0f)) {
            var_r31_2 = 0;
            temp_f28_2 = TWO_PI;
            var_r26_2 = texmtx_tbl;
            temp_f30 = 0.25f;
            var_r30_2 = 0;
            temp_f26_2 = 0.16605f;
            var_r25_2 = 0;
            temp_f25_2 = 1.0f;
            temp_f31_2 = PI;
            temp_f27_2 = 0.4967f;
            temp_f24_2 = 100.0f;
            do {
                temp_f2_2 = 0.00761f;
                temp_f3_3 = temp_f28_2 * (f32) var_r25_2 * temp_f30;
                if (temp_f3_3 <= temp_f31_2) {
                    temp_f1_6 = MTXDegToRad(-90.0f);
                    if (temp_f3_3 <= temp_f1_6) {
                        temp_f0_15 = temp_f3_3 * temp_f3_3;
                        var_f2_2 = ((((temp_f2_2 * temp_f0_15) - temp_f26_2) * temp_f0_15) + temp_f25_2) * temp_f3_3;
                    } else {
                        temp_f1_7 = temp_f1_6 - (temp_f3_3 - temp_f1_6);
                        temp_f0_16 = temp_f1_7 * temp_f1_7;
                        var_f2_2 = ((((temp_f2_2 * temp_f0_16) - temp_f26_2) * temp_f0_16) + temp_f25_2) * temp_f1_7;
                    }
                } else {
                    temp_f0_17 = MTXDegToRad(270.0f);
                    if (temp_f3_3 < temp_f0_17) {
                        temp_f1_8 = temp_f3_3 - temp_f31_2;
                        temp_f0_18 = temp_f1_8 * temp_f1_8;
                        var_f2_2 = -(((((temp_f2_2 * temp_f0_18) - temp_f26_2) * temp_f0_18) + temp_f25_2) * temp_f1_8);
                    } else {
                        temp_f1_9 = MTXDegToRad(-90.0f) - (temp_f3_3 - temp_f0_17);
                        temp_f0_19 = temp_f1_9 * temp_f1_9;
                        var_f2_2 = -(((((temp_f2_2 * temp_f0_19) - temp_f26_2) * temp_f0_19) + temp_f25_2) * temp_f1_9);
                    }
                }
                temp_f4_2 = 0.03705f;
                if (temp_f3_3 <= temp_f31_2) {
                    temp_f1_10 = MTXDegToRad(-90.0f);
                    if (temp_f3_3 <= temp_f1_10) {
                        temp_f0_20 = temp_f3_3 * temp_f3_3;
                        var_f4_2 = (((temp_f4_2 * temp_f0_20) - temp_f27_2) * temp_f0_20) + temp_f25_2;
                    } else {
                        temp_f0_21 = temp_f1_10 - (temp_f3_3 - temp_f1_10);
                        temp_f0_22 = temp_f0_21 * temp_f0_21;
                        var_f4_2 = -((((temp_f4_2 * temp_f0_22) - temp_f27_2) * temp_f0_22) + temp_f25_2);
                    }
                } else {
                    temp_f0_23 = MTXDegToRad(270.0f);
                    if (temp_f3_3 < temp_f0_23) {
                        temp_f0_24 = temp_f3_3 - temp_f31_2;
                        temp_f0_25 = temp_f0_24 * temp_f0_24;
                        var_f4_2 = -((((temp_f4_2 * temp_f0_25) - temp_f27_2) * temp_f0_25) + temp_f25_2);
                    } else {
                        temp_f0_26 = MTXDegToRad(-90.0f) - (temp_f3_3 - temp_f0_23);
                        temp_f0_27 = temp_f0_26 * temp_f0_26;
                        var_f4_2 = (((temp_f4_2 * temp_f0_27) - temp_f27_2) * temp_f0_27) + temp_f25_2;
                    }
                }
                temp_f0_28 = temp_r29->unk8;
                PSMTXTrans(sp18, (temp_f0_28 * var_f2_2) / temp_f24_2, (temp_f0_28 * var_f4_2) / temp_f24_2, 0.0f);
                GXLoadTexMtxImm(sp18, *var_r26_2, 1);
                GXSetTexCoordGen2(var_r31_2, 1, 4, *var_r26_2, 0, 0x7D);
                GXSetTevKColorSel(var_r30_2, 6);
                GXSetTevKAlphaSel(var_r30_2, 6);
                GXSetTevOrder(var_r30_2, var_r31_2, 0, 0xFF);
                GXSetTevColorOp(var_r30_2, 0, 0, 0, 1, var_r25_2 == 3);
                GXSetTevAlphaOp(var_r30_2, 0, 0, 0, 1, var_r25_2 == 3);
                v59 = GX_CC_CPREV;
                if ( !var_r25_2 )
                    v59 = GX_CC_ZERO;
                GXSetTevColorIn(var_r30_2, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, v59);
                v60 = GX_CA_APREV;
                if ( !var_r25_2 )
                    v60 = GX_CA_ZERO;
                GXSetTevAlphaIn(var_r30_2, GX_CA_ZERO, GX_CA_TEXA, GX_CA_KONST, v60);
                GXSetTevSwapMode(var_r30_2, GX_TEV_SWAP0, GX_TEV_SWAP0);
                /*temp_r28 = var_r25_2 == 0;
                if (temp_r28 != 0) {

                }
                GXSetTevColorIn(var_r30_2, 0xF, 8, 0xE);
                if (temp_r28 != 0) {

                }
                GXSetTevAlphaIn();
                GXSetTevSwapMode(var_r30_2, 0, 0);*/
                var_r25_2 += 1;
                var_r26_2 += 4;
                var_r30_2 += 1;
                var_r31_2 += 1;
            } while (var_r25_2 < 4);
            GXSetTevKColor(GX_KCOLOR0, (GXColor){temp_r29->unk3, 0, 0, 0xFF});
            /*temp_r5_2 = temp_r29->unk3;
            sp17 = 0xFF;
            sp14 = temp_r5_2;
            spC = (s32) sp14;
            GXSetTevKColor(0, &spC, temp_r5_2);*/
            GXSetTexCoordGen2(var_r31_2, 1, 4, 0x3C, 0, 0x7D);
            GXSetTevColor(GX_TEVREG1, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
            //sp8 = lbl_8041E510;
            //GXSetTevColor(2, &sp8);
            GXSetTevKColorSel(var_r30_2, 0xC);
            GXSetTevKAlphaSel(var_r30_2, 0x1C);
            GXSetTevOrder(var_r30_2, var_r31_2, 1, 0xFF);
            GXSetTevColorOp(var_r30_2, 8, 0, 0, 1, 0);
            GXSetTevAlphaOp(var_r30_2, 8, 0, 0, 1, 0);
            GXSetTevColorIn(var_r30_2, 8, 0xE, 0xC, 0xF);
            GXSetTevAlphaIn(var_r30_2, 4, 6, 2, 7);
            GXSetTevSwapMode(var_r30_2, 0, 0);
            temp_r26 = var_r30_2 + 1;
            GXSetTevOrder(temp_r26, 0xFF, 0xFF, 0xFF);
            GXSetTevColorOp(temp_r26, 0, 0, 0, 1, 0);
            GXSetTevAlphaOp(temp_r26, 0, 0, 0, 1, 0);
            GXSetTevColorIn(temp_r26, 2, 0xF, 1, 0xF);
            GXSetTevAlphaIn(temp_r26, 1, 7, 0, 7);
            GXSetTevSwapMode(temp_r26, 0, 0);
            GXSetNumTexGens((s8) (var_r31_2 + 1));
            GXSetNumTevStages((s8) (var_r30_2 + 2));
            GXBegin(0x80, 0, 4);
            temp_r8_2 = gp;
            temp_f3_4 = 0.5f;
            *(f32*)0xCC010000 = (f32) ((f32) -(s32) temp_r8_2->fbWidth * temp_f3_4);
            *(f32*)0xCC008000 = (f32) ((f32) temp_r8_2->efbHeight * temp_f3_4);
            *(f32*)0xCC008000 = (f32) 0.0f;
            *(f32*)0xCC008000 = (f32) 0.0f;
            *(f32*)0xCC008000 = (f32) 0.0f;
            *(f32*)0xCC008000 = (f32) ((f32) temp_r8_2->fbWidth * temp_f3_4);
            *(f32*)0xCC008000 = (f32) ((f32) temp_r8_2->efbHeight * temp_f3_4);
            *(f32*)0xCC008000 = (f32) 0.0f;
            *(f32*)0xCC008000 = (f32) 1.0f;
            *(f32*)0xCC008000 = (f32) 0.0f;
            *(f32*)0xCC008000 = (f32) ((f32) temp_r8_2->fbWidth * temp_f3_4);
            *(f32*)0xCC008000 = (f32) ((f32) -(s32) temp_r8_2->efbHeight * temp_f3_4);
            *(f32*)0xCC008000 = (f32) 0.0f;
            *(f32*)0xCC008000 = (f32) 1.0f;
            *(f32*)0xCC008000 = (f32) 1.0f;
            *(f32*)0xCC008000 = (f32) ((f32) -(s32) temp_r8_2->fbWidth * temp_f3_4);
            *(f32*)0xCC008000 = (f32) ((f32) -(s32) temp_r8_2->efbHeight * temp_f3_4);
            *(f32*)0xCC008000 = (f32) 0.0f;
            *(f32*)0xCC008000 = (f32) 0.0f;
            *(f32*)0xCC008000 = (f32) 1.0f;
        }
    }
}

// MUST_MATCH: a by-value GXColor built as `color = dat; color.a = a;` and then passed lands its local at
// the higher slot (sp+0x10 here) and the argument copy at sp+0xC, the wrong way round. Building the colour
// in an inline that returns it puts the return temporary at sp+0xC and the argument at sp+0x10, as the
// original does. The default (duration == 0) colour must go through a void helper too, or its argument
// temp takes sp+0x8 and pushes the if-branch temporaries back up. The plain form is under #else.
#ifdef MUST_MATCH
static inline GXColor envDisp_MakeBlurColor(u8 alpha) {
    GXColor color = dat_8041e514;
    color.a = alpha;
    return color;
}
static inline void envDisp_SetBlurColorDefault(void) {
    GXSetTevKColor(GX_KCOLOR0, dat_8041e518);
}
#endif
void envDisp_Blur(void) {
    EnvironmentWork* wp = envGetWork();
    EnvironmentBlur* blur = (EnvironmentBlur*)&wp->unk98;
#ifndef MUST_MATCH
    GXColor color;
#endif
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
#ifdef MUST_MATCH
            GXSetTevKColor(GX_KCOLOR0, envDisp_MakeBlurColor(alpha));
#else
            color = dat_8041e514;
            color.a = alpha;
            GXSetTevKColor(GX_KCOLOR0, color);
#endif
        } else {
#ifdef MUST_MATCH
            envDisp_SetBlurColorDefault();
#else
            GXSetTevKColor(GX_KCOLOR0, dat_8041e518);
#endif
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
#ifdef MUST_MATCH
        // MWCC puts 0.5f in f3 and the int->float mask double in f4 only when the
        // factor is a named local; with the literal the two swap and ~14
        // fmuls/fsubs differ. The natural form is under #else.
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
#else
        GXPosition3f32(-gp->fbWidth * 0.5f, gp->efbHeight * 0.5f, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(gp->fbWidth * 0.5f, gp->efbHeight * 0.5f, 0.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(gp->fbWidth * 0.5f, -gp->efbHeight * 0.5f, 0.0f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(-gp->fbWidth * 0.5f, -gp->efbHeight * 0.5f, 0.0f);
        GXTexCoord2f32(0.0f, 1.0f);
#endif
    }
}


// The map-object flag tests and the texture loop match; what still differs is global register allocation.
// The target keeps the map object in r3 and its flags in r4 (mine swaps them), so the bbox lands
// in r21-r18 (mine r24-r21) and the loop counters/base/fp constants shift by a few registers
// (i: r23/r19, j: r22/r18, base r24/r20). The entry block also lays out the wrong way (target:
// `beq F4; b E4`, mine: `bne E4` + fall-through); constant-pool symbols show as @NNN.
// NONMATCHING (91.19%): register allocation / block layout only.
void envSetWater(s32 arg0, void* arg1) {
    EnvironmentYami* yami = (EnvironmentYami*)arg0;
    MapObject* obj;
    s32 tevStage;
    s32 texMap;
    s32 texCoord;
    s32 unk18;
    s32 x0, y0, x1, y1;
    s32 width, height;
    u32 size;
    void* capture;
    u8* water;
    GXTexObj captureObj;
    GXTexObj waterObj;
    Vec pos;
    Mtx mtx;
    f32 indMtx[2][3];
    f32 dist;
    f32 scale;
    s32 i, j;

    obj = (MapObject*)yami->unk0;
    tevStage = yami->tevStage;
    texMap = yami->texMap;
    texCoord = yami->texCoord;
    unk18 = yami->unk18;

    if ((*(u32*)obj->field_0x0 & 0x20) == 0) {
        if ((*(u32*)obj->field_0x0 & 0x20000000) == 0) {
            *(u32*)obj->field_0x0 |= 0x20000000;
            return;
        }
    }
    else {
        while (TRUE) {
            obj = obj->parent;
            if (obj == NULL) {
                break;
            }
            if ((*(u32*)obj->field_0x0 & 0x10) == 0) {
                continue;
            }
            if ((*(u32*)obj->field_0x0 & 0x20000000) == 0) {
                *(u32*)obj->field_0x0 |= 0x20000000;
                return;
            }
            break;
        }
    }

    x0 = *(u16*)(obj->field_0xEC + 0);
    y0 = *(u16*)(obj->field_0xEC + 2);
    x1 = *(u16*)(obj->field_0xEC + 4);
    y1 = *(u16*)(obj->field_0xEC + 6);

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
    GXInitTexObjLOD(&captureObj, GX_NEAR, GX_NEAR, float_0_8041e53c, float_0_8041e53c, float_0_8041e53c, GX_FALSE, GX_FALSE, GX_ANISO_1);

    PSMTXMultVec(yami->unk4, (Vec*)(((MapObject*)yami->unk0)->field_0xC + 4), &pos);
    scale = float_1000_8041e540 / PSVECDistance(&camGetCurPtr()->cameraPos, &pos);
    if (scale < float_0_8041e53c) {
        scale = float_0_8041e53c;
    }
    if (scale > float_127_8041e544) {
        scale = float_127_8041e544;
    }

    size = GXGetTexBufferSize(0x40, 0x40, GX_TF_IA8, GX_FALSE, 0);
    water = smartAlloc(size, HEAP_EFFECT)->address;

    for (i = 0; i < 0x40; i++) {
        for (j = 0; j < 0x40; j++) {
            f32 s = (f32)sin(float_18p85_8041e548 * (f32)j * float_0p015625_8041e54c + (f32)gp->retraceCount / float_10_8041e524);
            s32 v = (s32)(float_0p6_8041e550 + (scale * s + float_128_8041e554));
            u8* p = &water[((i % 4) + (j % 4) * 4) * 2 + (i / 4) * 32 + (j / 4) * 512];

            p[0] = v;
            p[1] = 0x80;
        }
    }

    DCFlushRange(water, 0x2000);
    GXInitTexObj(&waterObj, water, 0x40, 0x40, GX_TF_IA8, GX_REPEAT, GX_REPEAT, GX_FALSE);
    GXInitTexObjLOD(&waterObj, GX_LINEAR, GX_LINEAR, float_0_8041e53c, float_0_8041e53c, float_0_8041e53c, GX_FALSE, GX_FALSE, GX_ANISO_1);

    dist = (f32)gp->efbHeight * float_0p5_8041e538 / (f32)tan(float_6p2832_8041e558 * (camGetPtr(CAMERA_3D)->fovY * float_0p5_8041e538) / float_360_8041e55c);
    C_MTXLightFrustum(mtx,
        (f32)(gp->efbHeight / 2 - y1),
        (f32)(gp->efbHeight / 2 - y0),
        (f32)(x0 - gp->fbWidth / 2),
        (f32)(x1 - gp->fbWidth / 2),
        dist,
        float_0p5_8041e538, float_0p5_8041e538, float_0p5_8041e538, float_0p5_8041e538);
    PSMTXConcat(mtx, camGetCurPtr()->view, mtx);
    PSMTXConcat(mtx, yami->unk4, mtx);

    GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, texmtx_tbl[texMap], GX_FALSE, GX_PTIDENTITY);
    GXLoadTexMtxImm(mtx, texmtx_tbl[texMap], GX_MTX3x4);
    GXLoadTexObj(&captureObj, texMap);
    GXLoadTexObj(&waterObj, texMap + 1);
    GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
    GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(tevStage, GX_CC_TEXC, GX_CC_CPREV, GX_CC_APREV, GX_CC_ZERO);
    GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
    indMtx[0][0] = float_0p5_8041e538;
    indMtx[0][1] = float_0_8041e53c;
    indMtx[0][2] = float_0_8041e53c;
    indMtx[1][0] = float_0_8041e53c;
    indMtx[1][1] = float_0p5_8041e538;
    indMtx[1][2] = float_0_8041e53c;
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

// NONMATCHING (88.71%): full logic; callee-saved locals land in r25-r31 instead of the original r24-r30 and a few scheduler moves differ.
void envSetYami(EnvironmentYami* yami) {
    EnvironmentWork* wp = envGetWork();
    s32 tevStage = yami->tevStage;
    s32 texMap = yami->texMap;
    s32 texCoord = yami->texCoord;
    s32 unk18 = yami->unk18;
    Mtx mtx;
    GXTexObj texObj;

    if (v_count != gp->retraceCount) {
        f32 newu1 = u1 + 0.004f;
        f32 newu2 = u2 - 0.004f;

        v_count = gp->retraceCount;
        u1 = newu1;
        u2 = newu2;
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
    GXLoadTexMtxImm(mtx, texmtx_tbl[texMap], GX_MTX3x4);
    GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, texmtx_tbl[texMap], GX_FALSE, GX_PTIDENTITY);

    C_MTXLightOrtho(mtx, 72.0f, -72.0f, -96.0f, 96.0f, 0.5f, 0.5f, u2, 0.5f);
    PSMTXConcat(mtx, (MtxPtr)wp->unkC0, mtx);
    PSMTXConcat(mtx, yami->unk4, mtx);
    GXLoadTexMtxImm(mtx, texmtx_tbl[texMap + 1], GX_MTX3x4);
    GXSetTexCoordGen2(texCoord + 1, GX_TG_MTX3x4, GX_TG_POS, texmtx_tbl[texMap + 1], GX_FALSE, GX_PTIDENTITY);

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

void envSetYamiView(Mtx mtx) {
    EnvironmentWork* wp = envGetWork();

    PSMTXCopy(mtx, (MtxPtr)wp->unkC0);
}

void envGlareFilterZ(f32 z) {
    EnvironmentWork* wp = envGetWork();
    EnvironmentGlareFilter* filter = &wp->filter;

    filter->zscale = z;
    filter->flags |= 1;
}

int __float_nan[];
#define NAN (*(f32*)__float_nan)

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

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

#define fpclassify(x) __fpclassifyf(x)
#define isnan(x) (fpclassify(x) == FP_NAN)

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

#ifdef MUST_MATCH
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
#endif

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
// first operand. All of this is needed; the remaining diff is only which pool slot each
// float constant loads from (the unit's .sdata2/.rodata is still missing the objects the
// other functions in this file define), so objdiff still reports 99.99% here.
// NONMATCHING (99.99%): only pool slot offsets differ (unit data incomplete), no instr diffs.
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

#ifdef MUST_MATCH
        scale = envFF_Sqrtf(gp->fbWidth / 2 * gp->fbWidth / 2 + gp->efbHeight / 2 * gp->efbHeight / 2);
#else
        scale = sqrtf(gp->fbWidth / 2 * gp->fbWidth / 2 + gp->efbHeight / 2 * gp->efbHeight / 2);
#endif

        GXBegin(GX_LINES, GX_VTXFMT0, 2000);
        for (i = 0; i < 1000; i++) {
            f32 angle = (TWO_PI * (f32)i) / 1000.0f;

            cosC1 = 0.4967f;
            one = 1.0f;
            cosv = 0.03705f;
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

// The texture-matrix data the original keeps in .rodata at dat_802f9338: ten texture-matrix ids followed
// by five vectors. The target accesses the vectors as dat_802f9338+0x28..+0x60, so model the whole blob
// as one object (the header has no name for it).
typedef struct EnvTevMtxData {
    s32 tbl[10]; // 0x00
    Vec vec28;   // 0x28
    Vec vec34;   // 0x34
    Vec vec40;   // 0x40
    Vec vec4C;   // 0x4C
    Vec vec58;   // 0x58
} EnvTevMtxData;

// Texture-matrix ids and the five vectors the matrix cases rotate against (target .rodata at 0x802f9338).
const EnvTevMtxData dat_802f9338 = {
    { 30, 33, 36, 39, 42, 45, 48, 51, 54, 57 },
    { 0.0f, 0.0f, -1.0f },
    { 0.0f, 0.0f, 1.0f },
    { 0.0f, 100.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, -1.0f },
};

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
#ifdef MUST_MATCH
    // the target reserves 0x40 here (a 4x4 matrix), which leaves the 0x10 stack gap before scaleD
    Mtx44 frustumE;
#else
    Mtx frustumE;
#endif
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
#ifdef MUST_MATCH
        // a named temp for cos(angY) makes f0 the multiply accumulator, matching the target's operand order
        f32 c;
#endif

        vecA = data->vec28;
        tmp = (f32)sin(angX);
#ifdef MUST_MATCH
        c = (f32)cos(angY);
        vecB.x = -c * tmp;
#else
        vecB.x = -(f32)cos(angY) * tmp;
#endif
        vecB.y = -(f32)sin(angY);
        tmp = (f32)cos(angX);
#ifdef MUST_MATCH
        c = (f32)cos(angY);
        vecB.z = -c * tmp;
#else
        vecB.z = -(f32)cos(angY) * tmp;
#endif
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

// NONMATCHING (99.58%): the stage/texmap/texcoord cursors land in r30/r29/r28 in the target and r29/r28/r30 here
// (register allocation only; declaration and use order did not move them).
// Adds one tev stage for the overlay effect `type` (1..8, 10) and loads its texture, texgen and texture matrix.
// `param` is the shared stage cursor (EnvironmentYami's first fields: stage, texmap, texcoord).
void envAddTev(s32 type, void* param) {
    EnvironmentYami* yami = param;
    EnvironmentWork* wp = envGetWork();
    s32 tevStage = yami->tevStage;
    s32 texMap = yami->texMap;
    s32 texCoord = yami->texCoord;
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
            *(f32*)&wp->unk70[0x10] = float_10_8041e524;
            *(f32*)&wp->unk70[0x14] = (f32)(gp->retraceCount + 45);
            *(f32*)&wp->unk70[0xC] = float_0p5_8041e538;
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
            *(f32*)&wp->unk70[0x10] = float_10_8041e524;
            *(f32*)&wp->unk70[0x14] = (f32)(gp->retraceCount + 45);
            *(f32*)&wp->unk70[0xC] = float_0p5_8041e538;
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
            *(f32*)&wp->unk70[0xC] = float_0p5_8041e538;
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
            *(f32*)&wp->unk70[0xC] = float_0p5_8041e538;
            *(f32*)&wp->unk70[0x10] = float_45_8041e560;
            *(f32*)&wp->unk70[0x14] = float_45_8041e560;
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
            *(f32*)&wp->unk70[0xC] = float_0p5_8041e538;
            *(f32*)&wp->unk70[0x10] = float_45_8041e560;
            *(f32*)&wp->unk70[0x14] = float_45_8041e560;
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
            *(f32*)&wp->unk70[0xC] = float_0p15_8041e564;
            *(f32*)&wp->unk70[0x10] = float_0_8041e53c;
            *(f32*)&wp->unk70[0x14] = float_90_8041e568;
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
            *(f32*)&wp->unk70[0xC] = float_0p3_8041e56c;
            *(f32*)&wp->unk70[0x10] = float_0_8041e53c;
            *(f32*)&wp->unk70[0x14] = float_90_8041e568;
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
            *(f32*)&wp->unk70[0xC] = float_0p5_8041e538;
            *(f32*)&wp->unk70[0x10] = float_10_8041e524;
            *(f32*)&wp->unk70[0x14] = float_45_8041e560;
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
            kColor = dat_8041e51c;
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
#ifdef MUST_MATCH
        s16 tx1, ty1, tx0, ty0; // declaration order controls the register allocation of the four temps
#else
        s16 tx0, ty0, tx1, ty1;
#endif

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
#ifdef MUST_MATCH
    // The target keeps the sign-extended filter x0 in r20 and the texture-matrix index in r23;
    // caching the cast in x0s and declaring var_r22_3 ahead of var_r23_2 gives those colours
    // (the natural inline cast and late declaration are under #else).
    s16 x0s;
    EnvironmentGlare* var_r22_3;
#endif
    s32 temp_r29;
    s32 var_r28;
    u32 var_r23_2;
    u16 temp_r27;
    s32 temp_r31;
    u32 temp_r20;
    u32 temp_r21;
    u32 size;
    u16 temp_r26;
#ifndef MUST_MATCH
    EnvironmentGlare* var_r22_3;
#endif
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
#ifdef MUST_MATCH
    x0s = (s16)temp_r26;
#endif
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
#ifdef MUST_MATCH
        GXPosition3s16(x0s, (s16)temp_r27, 0);
#else
        GXPosition3s16((s16)temp_r26, (s16)temp_r27, 0);
#endif
        GXTexCoord2u16(0, 0);
        GXPosition3s16((s16)temp_r31, (s16)temp_r27, 0);
        GXTexCoord2u16(1, 0);
        GXPosition3s16((s16)temp_r31, (s16)temp_r30_2, 0);
        GXTexCoord2u16(1, 1);
#ifdef MUST_MATCH
        GXPosition3s16(x0s, (s16)temp_r30_2, 0);
#else
        GXPosition3s16((s16)temp_r26, (s16)temp_r30_2, 0);
#endif
        GXTexCoord2u16(0, 1);
    }
}
#pragma opt_lifetimes reset

void envBlurOff(void) {
    EnvironmentWork* wp = envGetWork();

    wp->flags &= ~4;
    wp->flags &= ~0x40000000;
    _mapFree(wp->envBlur);
}

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
