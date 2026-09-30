#include "evt/evt_cmd.h"
#include "evt/evt_telop.h"
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

// .sdata2 constants this translation unit owns (named by their original address).
extern const GXColor dat_8041e7b8;
extern const GXColor dat_8041e7bc;
extern const GXColor dat_8041e7c0;
#ifdef MUST_MATCH
// volatile: the quad loop reloads the FP constants; non-volatile scored 92% vs 99.6%.
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

// .rodata objects this translation unit owns, after the message strings.
const GXColorS10 zero_802fa0c4 = { 0, 0, 0, 0 };
const char str_802FA0CC[] = "\201H\201H\201H";

#ifdef MUST_MATCH
// Reproduce the original's by-value struct temporaries (local below, copy above).
static inline GXColor telopMakeColor(u8 a) {
    GXColor c = dat_8041e7c0;
    c.a = a;
    return c;
}

static inline GXColorS10 telopMakeColorS10(s16 a) {
    GXColorS10 c = zero_802fa0c4;
    c.a = a;
    return c;
}
#endif

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

// (1) the by-value GXColor temporaries in the quad loop and for FontDrawColor land in
//     different stack slots (loop temps 0x10/0xc vs 0x14/0x10, colour copy 0x14 vs 0xc);
// (2) the font PSMTXTrans y/scale expression keeps 0.5 in f6 and msgWidth in f5, the
//     original the other way round.
// NONMATCHING (99.91%): the two points above. The ratio register/branch differences were fixed
// by `#pragma opt_common_subs off` plus an empty then-block (see below).
#pragma opt_common_subs off // needed: without it the ratio's int->float mask lands in f1, not f2
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
    s32 alpha;

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
#ifdef MUST_MATCH
    {
        f32 s = float_0p2_8041e7c8;
        PSMTXScale(texScale, s, s, float_1_8041e7cc);
    }
#else
    PSMTXScale(texScale, float_0p2_8041e7c8, float_0p2_8041e7c8, float_1_8041e7cc);
#endif
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

#ifdef MUST_MATCH
    GXSetTevColorS10(GX_TEVREG0, telopMakeColorS10(wp->alpha));
#else
    colorS10 = zero_802fa0c4;
    colorS10.a = wp->alpha;
    GXSetTevColorS10(GX_TEVREG0, colorS10);
#endif

    effGetTexObj(0x81, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    effGetTexObj(0x80, &texObj);
#ifdef MUST_MATCH
    {
        f32 z = float_0_8041e7c4;
        GXInitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, z, z, z, GX_FALSE, GX_FALSE, GX_ANISO_1);
    }
#else
    GXInitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, float_0_8041e7c4, float_0_8041e7c4, float_0_8041e7c4, GX_FALSE, GX_FALSE, GX_ANISO_1);
#endif
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

        scale = float_1_8041e7cc;
#ifdef MUST_MATCH
        // Match-only: the empty then-block makes MWCC lay the assignment out of line, emitting the
        // original's `bge body; b join`; the natural `>=` test gives a `cror`/`bne` pair instead.
        if ((u32)gp->language != 0) {
            if ((f32)msgWidth / (width - float_100_8041e7d8) < scale) {
                scale = scale;
            } else {
                scale = (width - float_100_8041e7d8) / (f32)msgWidth;
            }
        }
#else
        if (gp->language != 0) {
            if ((f32)msgWidth / (width - float_100_8041e7d8) >= scale) {
                scale = (width - float_100_8041e7d8) / (f32)msgWidth;
            }
        }
#endif

        alpha = wp->alpha;
        if (alpha < 0) {
            alpha = 0;
        } else {
            alpha = alpha > 0xFF ? 0xFF : alpha;
        }
        FontDrawStart();
#ifdef MUST_MATCH
        color = telopMakeColor(alpha);
#else
        color = dat_8041e7c0;
        color.a = alpha;
#endif
        FontDrawColor(&color);

        msgWidth = FontGetMessageWidth(msg);
        PSMTXTrans(fontTrans,
                   (width - scale * (f32)msgWidth) * float_0p5_8041e7dc + float_neg15_8041e7d0,
                   float_neg105_8041e7d4 - (height - float_24_8041e7e0) * float_0p5_8041e7dc,
                   float_0_8041e7c4);
#ifdef MUST_MATCH
        {
            f32 one = float_1_8041e7cc;
            PSMTXScale(fontScale, scale, one, one);
        }
#else
        PSMTXScale(fontScale, scale, float_1_8041e7cc, float_1_8041e7cc);
#endif
        PSMTXConcat(fontTrans, fontScale, fontTrans);
        FontDrawStringMtx(fontTrans, msg);
    }
}
