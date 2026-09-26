#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/swdrv.h"
#include "driver/msgdrv.h"
#include "manager/fontmgr.h"
#include "mario/mariost.h"
#include "system.h"
#include <string.h>
#include <stdlib.h>

// .rodata / .sdata2 constants this translation unit owns (named by their original address).
extern const char str_802FA0CC[];
extern const GXColor dat_8041e7b8;
extern const GXColor dat_8041e7bc;
extern const GXColor dat_8041e7c0;
extern const GXColorS10 zero_802fa0c4;
#ifdef MUST_MATCH
// volatile: telop scores higher with it (95.5% vs 91.9%); see telop's comment.
extern const volatile f32 float_0_8041e7c4;
extern const volatile f32 float_0p2_8041e7c8;
extern const volatile f32 float_1_8041e7cc;
#else
extern const f32 float_0_8041e7c4;
extern const f32 float_0p2_8041e7c8;
extern const f32 float_1_8041e7cc;
#endif
extern const f32 float_neg15_8041e7d0;
extern const f32 float_neg105_8041e7d4;
extern const f32 float_100_8041e7d8;
extern const f32 float_0p5_8041e7dc;
extern const f32 float_24_8041e7e0;
extern const f32 float_neg256_8041e7e4;
extern const f32 float_256_8041e7e8;
extern const f32 float_300_8041e7ec;
const f64 double_to_int_mask_802fa0d8 = 4503599627370496.0;

// The telop's private work. Only the fields this file uses are named.
typedef struct TelopWork {
    s32 msgId;    // 0x0, index into msg_tbl
    s32 alpha;    // 0x4
    OSTime start; // 0x8, time the current phase began
} TelopWork;

static TelopWork work;
static TelopWork* wp = &work;

// Indexed by wp->msgId; the slots point at the per-flag messages.
static const char* msg_tbl[] = {
    "menu_monosiri_gor", "menu_monosiri_hei", "menu_monosiri_nok", "menu_monosiri_gon",
    "menu_monosiri_win", "menu_monosiri_mri", "menu_monosiri_tou", "menu_monosiri_usu",
    "menu_monosiri_jin", "menu_monosiri_muj", "menu_monosiri_dou", "menu_monosiri_hom",
    "menu_monosiri_pik", "menu_monosiri_bom", "menu_monosiri_moo", NULL,
    "menu_monosiri_aji", "menu_monosiri_las",
};

// All telop logic matches, but:
// (1) every aggregate local is allocated 4 bytes lower than the original (texObj at 0x24 vs
//     0x28, the matrices likewise);
// (2) the int->float conversions use compiler-generated pool constants (@288, @291) where the
//     original uses double_to_int_mask_802fa0d8 and double_to_int_802fa0e0;
// (3) float_1_8041e7cc and float_0_8041e7c4 are reloaded where the original reuses a register
//     (fmr). Declaring them volatile (under MUST_MATCH) gets closer than not.
// NONMATCHING (95.53%): the three points above.
void telop(CameraId cameraId, void* param) {
    CameraEntry* cam = camGetPtr(cameraId);
    GXColor color;
    GXColorS10 colorS10;
    GXTexObj texObj;
    Mtx texTrans;
    Mtx texScale;
    Mtx fontTrans;
    Mtx fontScale;
    const char* msg;
    f32 width;
    f32 height;
    f32 scale;
    s32 y;
    s32 i;

    effGetTexObj(0x81, &texObj);
    width = (f32)GXGetTexObjWidth(&texObj);
    height = (f32)GXGetTexObjHeight(&texObj);
    GXSetZCompLoc(GX_TRUE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_OR);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTexGens(2);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);

    {
        f32 ty = (f32)(rand() % 64) / height;
        f32 tx = (f32)(rand() % 256) / width;

        PSMTXTrans(texTrans, tx, ty, float_0_8041e7c4);
    }
    PSMTXScale(texScale, float_0p2_8041e7c8, float_0p2_8041e7c8, float_1_8041e7cc);
    PSMTXConcat(texTrans, texScale, texTrans);
    GXLoadTexMtxImm(texTrans, GX_TEXMTX0, GX_MTX2x4);

    GXSetNumTevStages(3);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_TEXA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_C1, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_A1, GX_CA_ZERO);

    colorS10 = zero_802fa0c4;
    colorS10.b = wp->alpha;
    GXSetTevColorS10(GX_TEVREG0, colorS10);

    effGetTexObj(0x81, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    effGetTexObj(0x80, &texObj);
    GXInitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, float_0_8041e7c4, float_0_8041e7c4, float_0_8041e7c4, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, GX_TEXMAP1);

    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    i = 1;
    y = 5;
    while (i >= 0) {
        if (i != 0) {
            GXSetTevColor(GX_TEVREG1, dat_8041e7b8);
        } else {
            GXSetTevColor(GX_TEVREG1, dat_8041e7bc);
        }

        PSMTXTrans(texTrans, float_neg15_8041e7d0 + (f32)y, float_neg105_8041e7d4 - (f32)y, float_0_8041e7c4);
        PSMTXConcat(cam->view, texTrans, texTrans);
        GXLoadPosMtxImm(texTrans, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        GXPosition3f32(float_0_8041e7c4, float_0_8041e7c4, float_0_8041e7c4);
        GXTexCoord2f32(float_0_8041e7c4, float_0_8041e7c4);
        GXPosition3f32(width, float_0_8041e7c4, float_0_8041e7c4);
        GXTexCoord2f32(float_1_8041e7cc, float_0_8041e7c4);
        GXPosition3f32(width, -height, float_0_8041e7c4);
        GXTexCoord2f32(float_1_8041e7cc, float_1_8041e7cc);
        GXPosition3f32(float_0_8041e7c4, -height, float_0_8041e7c4);
        GXTexCoord2f32(float_0_8041e7c4, float_1_8041e7cc);
        GXEnd();

        i--;
        y -= 5;
    }

    msg = msg_tbl[wp->msgId];
    if (msg != NULL) {
        msg = msgSearch(msg);
    } else {
        msg = str_802FA0CC;
    }

    {
        u16 msgWidth = FontGetMessageWidth(msg);
        s32 alpha;

        scale = float_1_8041e7cc;
        if (gp->language != 0) {
            if ((f32)msgWidth / (width - float_100_8041e7d8) >= float_1_8041e7cc) {
                scale = (width - float_100_8041e7d8) / (f32)msgWidth;
            }
        }

        if (wp->alpha < 0) {
            alpha = 0;
        } else if (wp->alpha > 0xFF) {
            alpha = 0xFF;
        } else {
            alpha = wp->alpha;
        }
        FontDrawStart();
        color = dat_8041e7c0;
        color.a = alpha;
        FontDrawColor(&color);

        msgWidth = FontGetMessageWidth(msg);
        PSMTXTrans(fontTrans,
                   (width - scale * (f32)msgWidth) * float_0p5_8041e7dc + float_neg15_8041e7d0,
                   float_neg105_8041e7d4 - (height - float_24_8041e7e0) * float_0p5_8041e7dc,
                   float_0_8041e7c4);
        PSMTXScale(fontScale, scale, float_1_8041e7cc, float_1_8041e7cc);
        PSMTXConcat(fontTrans, fontScale, fontTrans);
        FontDrawStringMtx(fontTrans, msg);
    }
}

USER_FUNC(evt_telop_entry) {
    s32* args = event->args;

    if (isFirstCall) {
        event->userdata[0] = 0;
        memset(wp, 0, sizeof(TelopWork));
        wp->msgId = evtGetValue(event, args[0]);
        if (swGet(wp->msgId + 0x28)) {
            return EVT_RETURN_DONE;
        }
        swSet(wp->msgId + 0x28);
    }

    switch (event->userdata[0]) {
        case 0:
            wp->start = event->lifetime;
            event->userdata[0]++;
            break;

        case 1: {
            u32 elapsed = ((s32)event->lifetime - (s32)wp->start) / (OS_TIMER_CLOCK / 1000);

            if (elapsed < 1000) {
                wp->alpha = (s32)intplGetValue(0xB, elapsed, 1000, float_neg256_8041e7e4, float_256_8041e7e8);
            } else {
                wp->start = event->lifetime;
                event->userdata[0]++;
            }
            dispEntry(CAMERA_2D, float_300_8041e7ec, 0, telop, NULL);
            break;
        }

        case 2: {
            u32 elapsed = ((s32)event->lifetime - (s32)wp->start) / (OS_TIMER_CLOCK / 1000);

            if (elapsed > 5000) {
                wp->start = event->lifetime;
                event->userdata[0]++;
            }
            dispEntry(CAMERA_2D, float_300_8041e7ec, 0, telop, NULL);
            break;
        }

        case 3: {
            u32 elapsed = ((s32)event->lifetime - (s32)wp->start) / (OS_TIMER_CLOCK / 1000);

            if (elapsed < 1000) {
                wp->alpha = (s32)intplGetValue(0xB, elapsed, 1000, float_256_8041e7e8, float_neg256_8041e7e4);
            } else {
                event->userdata[0]++;
            }
            dispEntry(CAMERA_2D, float_300_8041e7ec, 0, telop, NULL);
            break;
        }

        case 4:
            return EVT_RETURN_DONE;

        default:
            break;
    }

    return EVT_RETURN_BLOCK;
}
