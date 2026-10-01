#include "pmario_sound.h"
#include "win/win_main.h"
#include "data/item_data.h"
#include "driver/camdrv.h"
#include "driver/msgdrv.h"
#include "manager/fontmgr.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include <dolphin/types.h>
#include <string.h>
#include <stdio.h>
#include "driver/windowdrv.h"
#include "driver/mapdrv.h"
#include "system.h"
#include "window/win_root.h"
#include "data/mail_data.h"
#include "window/win_item.h"
#include "window/win_badge.h"
#include "window/win_mario.h"
#include "window/win_party.h"
#include "window/win_log.h"
#include "battle/battle_monosiri.h"
#include "manager/evtmgr_cmd.h"

extern WinRootVecs str_802F3FE8;
extern const GXColor dat_8041ced8;
extern const GXColor dat_8041cedc;
extern const GXColor dat_8041cee0;
extern const GXColor dat_8041cee4;
extern const GXColor dat_8041d014;
extern const f32 float_2_8041d090;
extern const f32 float_neg1p3963_8041d0dc;
extern const f32 float_neg0p5_8041d0e0;
extern const GXColor dat_8041cee8;
extern const GXColor dat_8041ceec;
extern const GXColor dat_8041cef0;
extern const GXColor dat_8041cef4;
extern const GXColor dat_8041cef8;
extern const GXColor dat_8041cefc;
extern const GXColor float_neg0p0047648_8041cf00;
extern const GXColor dat_8041cf04;
extern const GXColor dat_8041cf08;
extern const GXColor dat_8041cf0c;
extern const GXColor dat_8041cf10;
extern const GXColor dat_8041cf14;
extern const GXColor dat_8041cf18;
extern const GXColor dat_8041cf1c;
extern const GXColor dat_8041cf20;
extern const GXColor dat_8041cf24;
extern const f32 float_18_8041d1b8;
extern const f32 float_320_8041d074;
extern const f32 float_570_8041d1bc;
extern const f32 float_288_8041d16c;
extern const f32 float_neg285_8041d1c0;
extern const f32 float_160_8041d1c4;
extern const f32 float_neg160_8041d1c8;
extern const f32 float_285_8041d1cc;
extern const f32 float_144_8041d1d0;
extern const f32 float_neg144_8041d1d4;
extern const f32 float_250_8041d1a4;
extern const f32 float_10_8041d1a0;
extern const f32 float_500_8041d1a8;
extern const f32 float_80_8041d0b0;
extern const f32 float_20_8041d0c0;
extern const f32 float_neg130_8041d1ac;
extern const f32 float_16_8041d0e4;
extern const f32 float_120_8041d09c;
extern const f32 float_8_8041d0a0;
extern const f32 float_240_8041d0ac;
extern const f32 float_32_8041d0a4;
extern const f32 float_4_8041d07c;
extern const f32 float_14_8041d180;
extern const f32 float_24_8041d0c8;
extern const GXColor dat_8041cf28;
extern const GXColor dat_8041cf2c;
extern const GXColor dat_8041cf30;
extern const GXColor dat_8041cf34;
extern const GXColor dat_8041cf38;
extern const GXColor dat_8041cf3c;
extern const GXColor dat_8041cf40;
extern const GXColor dat_8041cf44;

// Extended fields of WindowWork used by this unit. win_main.h only declares the
// first 0x40 bytes, so the rest are reached through raw offsets below.
#define WW_S32(w, off) (*(s32*)((u8*)(w) + (off)))
#define WW_U32(w, off) (*(u32*)((u8*)(w) + (off)))
#define WW_PTR(w, off) (*(void**)((u8*)(w) + (off)))
#define WW_F32(w, off) (*(f32*)((u8*)(w) + (off)))

typedef struct SortTable {
    const char* name;                 // 0x0
    void (*func)(WindowWork* work);    // 0x4
} SortTable;

extern SortTable* sort_data[8];
extern const f32 float_neg1000_8041d0cc;
extern const f32 float_0_8041d068;
extern const f32 float_neg500_8041d0d0;
extern const f32 float_300_8041d0d4;
extern const f32 float_neg100_8041d0d8;
extern const f32 float_neg600_8041d0fc;
extern const f32 float_30_8041d100;
extern const f32 float_40_8041d0e8;
extern const f32 float_0p125_8041d104;
extern const f32 float_0p5_8041d06c;
extern const f32 float_1_8041d070;
extern ItemData itemDataTable[];

static char msgbuf[0x3C];

// This unit owns this .rodata block (the strings below are already here). The two
// int->float conversion doubles come out as anonymous @NNN@ pool entries rather
// than these named symbols -- an MWCC limitation also seen in win_badge/win_item.
const f64 double_to_int_802f44d0 = 4503601774854144.0;
const f64 double_to_int_mask_802f44c8 = 4503599627370496.0;
const char str_802F4510[] = "menu_monosiri_%s";
const char str_802F44F0[] = "btl_un_hatena";
const char str_802F4500[] = "btl_un_rampell";

static const char* const tbl[] = {
    "nara", "gor",  "tik", "shiga", "hei", "nok", "stn", "gon", "win",
    "hou",  "mri",  "tou", "usu",   "gra", "jin", "muj", "dou", "hom",
    "eki",  "rsh",  "pik", "sin",   "bom", "moo", "aji", "las",
};

const GXColor dat_8041d064 = {0xFF, 0xFF, 0xFF, 0xFF};


s32 winSortWait(WindowWork* work) {
    return WW_S32(work, 0x17C) != 0;
}

// Full-screen textured quad. Values/branches match; MWCC keeps w/h and the 0/1
// constants in different FPRs than the original (target reloads float_0/float_1
// per vertex, ours caches them) and the u16->f32 magic double is pooled as
// @1158@ instead of double_to_int_mask_802f44c8.
// NONMATCHING (88.73%): FPR allocation and the pooled conversion constant differ.
void unk_8014d338(s32 unused, WindowWork* work) {
    GXTexObj texObj;
    f32 w;
    f32 h;

    if (WW_S32(work, 0x38) != 0) {
        GXSetFog(0, float_0_8041d068, float_0_8041d068, float_0_8041d068, float_0_8041d068,
                 dat_8041d064);
        GXSetNumChans(0);
        GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
        GXSetNumTevStages(1);
        GXSetTevOrder(0, 0, 0, 0xFF);
        GXSetTevOp(0, 3);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
        GXSetBlendMode(1, 4, 5, 0);
        GXSetZCompLoc(1);
        GXSetAlphaCompare(7, 0, 0, 7, 0);
        GXSetZMode(0, 7, 0);
        GXLoadPosMtxImm(camGetPtr(8)->view, 0);
        GXSetCurrentMtx(0);
        GXInitTexObj(&texObj, (void*)*(u32*)((u8*)work + 0x34), gp->fbWidth, gp->efbHeight, 4, 0, 0, 0);
        GXLoadTexObj(&texObj, 0);
        GXSetCullMode(0);
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxDesc(13, 1);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 13, 1, 4, 0);
        GXBegin(0x80, 0, 4);
        w = (f32)gp->fbWidth;
        h = (f32)gp->efbHeight;
        GXPosition3f32(-w * float_0p5_8041d06c, -h * float_0p5_8041d06c, float_0_8041d068);
        GXTexCoord2f32(float_0_8041d068, float_1_8041d070);
        GXPosition3f32(w * float_0p5_8041d06c, -h * float_0p5_8041d06c, float_0_8041d068);
        GXTexCoord2f32(float_1_8041d070, float_1_8041d070);
        GXPosition3f32(w * float_0p5_8041d06c, h * float_0p5_8041d06c, float_0_8041d068);
        GXTexCoord2f32(float_1_8041d070, float_0_8041d068);
        GXPosition3f32(-w * float_0p5_8041d06c, h * float_0p5_8041d06c, float_0_8041d068);
        GXTexCoord2f32(float_0_8041d068, float_0_8041d068);
    }
}

void unk_8014d5dc(s32 unused, WindowWork* work) {
    GXSetTexCopySrc(0, 0, gp->fbWidth, gp->efbHeight);
    GXSetTexCopyDst(gp->fbWidth, gp->efbHeight, GX_TF_RGB565, GX_FALSE);
    GXCopyTex(**(void***)((u8*)work + 0x34), GX_FALSE);
    GXTexModeSync();
    WW_S32(work, 0x38) = 1;
    N_mapDispOff();
    camDispOff(4);
}

void winMsgEntry(WindowWork* work, s32 a1, const char* str, s32 a3) {
    u16 width;
    BOOL changed = FALSE;

    if (str != NULL) {
        if (WW_PTR(work, 0x144) != (void*)str) {
            changed = TRUE;
        } else if (strcmp(msgbuf, str) != 0) {
            changed = TRUE;
        }
        if (changed) {
            WW_S32(work, 0x138) = a1;
            WW_PTR(work, 0x13C) = (void*)str;
            WW_S32(work, 0x140) = a3;
            FontGetMessageWidthLine((const char*)msgSearch((const char*)WW_PTR(work, 0x13C)), &width);
            WW_S32(work, 0x148) = 0;
            WW_S32(work, 0x14C) = width + 1;
            WW_PTR(work, 0x144) = (void*)str;
            strncpy(msgbuf, str, 0x3C);
        }
    }
}

// The original passes structs by value / takes font positions by value; the
// shared headers declare pointers, so copy under MUST_MATCH (same as win_item.c).
// The control flow and every call argument match; MWCC's stack-slot assignment
// (our `winTexSet` by-value copies land in the outgoing-argument area, so the
// frame is 0x630 instead of 0x620), the int->float pool doubles and the FPR
// allocation in the arrow blocks still differ. See REPORT.md.
// NONMATCHING (69.15%): stack slots, pool doubles and arrow FPRs, see above.
void winMsgDisp(WindowWork* work, f32 x, f32 y) {
    char buf[0x400];
    Vec* vp = (Vec*)&str_802F3FE8;
    GXColor white = dat_8041cf28;
    GXColor black = dat_8041cf2c;
    GXColor color = dat_8041cf30;
    f32 f30;
    f32 f31;
    f32 f26;
    f32 f27;
    Vec tex_t;
    Vec tex_s;
    Vec icon1_t;
    Vec icon1_s;
    Vec fe_pos;
    Vec fe_s;
    Vec fs_pos;
    Vec fs_s;
    Vec fl_pos;
    Vec fl_s;
    Vec i1_t;
    Vec i1_s;
    Vec i2_t;
    Vec i2_s;
    Vec i3_t;
    Vec i3_s;
    Vec i4_t;
    Vec i4_s;
    Vec fin_t;
    Vec fin_s;
    GXColor c_icon1;
    GXColor c_fe;
    GXColor c_fs;
    GXColor c_fl;
    GXColor c_i1;
    GXColor c_i2;
    GXColor c_i3;
    GXColor c_i4;
    GXColor c_fin;

    f31 = (x - float_250_8041d1a4 + float_10_8041d1a0) - float_10_8041d1a0;
    f30 = float_40_8041d0e8 + y;
#ifdef MUST_MATCH
    ((void (*)(s32, GXColor*, f32, f32, f32, f32, f32))windowDispGX_Waku_col)(
        0, &color, f31, f30, float_500_8041d1a8, float_80_8041d0b0, float_20_8041d0c0);
#else
    windowDispGX_Waku_col(0, color, f31, f30, float_500_8041d1a8, float_80_8041d0b0,
                          float_20_8041d0c0);
#endif
    if (WW_S32(work, 0x138) != 0 && WW_S32(work, 0x138) > 0) {
        winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
        tex_t = *(Vec*)((u8*)vp + 0x9C);
        tex_s = *(Vec*)((u8*)vp + 0xA8);
        tex_t.x = f31;
        tex_t.y = f30;
        winTexSet(0x25, tex_t, tex_s, white);

        winIconInit();
        icon1_t = *(Vec*)((u8*)vp + 0xB4);
        icon1_t.x = f31;
        icon1_t.y = f30;
        icon1_s = *(Vec*)((u8*)vp + 0xC0);
        c_icon1 = white;
        {
#ifdef MUST_MATCH
            Vec tcopy = icon1_t;
            winIconSet(*(u16*)&itemDataTable[WW_S32(work, 0x138)].icon_id, &tcopy, &icon1_s, &c_icon1);
#else
            winIconSet(*(u16*)&itemDataTable[WW_S32(work, 0x138)].icon_id, &icon1_t, &icon1_s, &c_icon1);
#endif
        }

        if (WW_PTR(work, 0x140) != NULL) {
            const char* msg;

            f31 = (WW_S32(work, 0x138) == -2) ? float_neg130_8041d1ac : float_0_8041d068;
            msg = msgSearch((const char*)WW_PTR(work, 0x140));
            f26 = (f32)FontGetMessageWidth(msg);
            f27 = float_8_8041d0a0 + (float_16_8041d0e4 + f30);
            winNameGX(work, 1, (x - float_120_8041d09c) + f31, f27, float_240_8041d0ac,
                      float_32_8041d0a4);
            winFontInit();
            fe_pos = *(Vec*)((u8*)vp + 0xCC);
            fe_pos.x = f31 + ((float_240_8041d0ac - f26) * float_0p5_8041d06c + (x - float_120_8041d09c));
            fe_pos.y = f27 - float_4_8041d07c;
            fe_s = *(Vec*)((u8*)vp + 0xD8);
            c_fe = white;
#ifdef MUST_MATCH
            {
                Vec pcopy = fe_pos;
                winFontSetEdge(&pcopy, &fe_s, &c_fe, msg);
            }
#else
            winFontSetEdge(&fe_pos, &fe_s, &c_fe, msg);
#endif
        }
    }

    if (WW_PTR(work, 0x13C) != NULL) {
        f30 = f30 - float_14_8041d180;
        GXSetScissor(0, (u32)(float_240_8041d0ac - f30), 0x260, 0x38);
        if (strcmp((const char*)WW_PTR(work, 0x13C), (const char*)((u8*)vp + 0x5D0)) == 0) {
            s16 num = *(s16*)((u8*)lotteryGetPtr() + 0x20); // lottery_info field past the header

            sprintf(buf, msgSearch((const char*)WW_PTR(work, 0x13C)), num);
            winFontInit();
            fs_pos = *(Vec*)((u8*)vp + 0xE4);
            fs_pos.x = (float_40_8041d0e8 + (x - float_250_8041d1a4)) - float_10_8041d1a0;
            fs_pos.y = f30 + (f32)(WW_S32(work, 0x148) * 0x1C);
            fs_s = *(Vec*)((u8*)vp + 0xF0);
            c_fs = black;
#ifdef MUST_MATCH
            {
                Vec pcopy = fs_pos;
                winFontSet(&pcopy, &fs_s, &c_fs, buf);
            }
#else
            winFontSet(&fs_pos, &fs_s, &c_fs, buf);
#endif
        } else {
            winFontInit();
            fl_pos = *(Vec*)((u8*)vp + 0xFC);
            fl_pos.x = (float_40_8041d0e8 + (x - float_250_8041d1a4)) - float_10_8041d1a0;
            fl_pos.y = f30 + (f32)(WW_S32(work, 0x148) * 0x1C);
            fl_s = *(Vec*)((u8*)vp + 0x108);
            c_fl = black;
#ifdef MUST_MATCH
            {
                Vec pcopy = fl_pos;
                winFontSetLabel(&pcopy, &fl_s, &c_fl, (const char*)WW_PTR(work, 0x13C));
            }
#else
            winFontSetLabel(&fl_pos, &fl_s, &c_fl, (const char*)WW_PTR(work, 0x13C));
#endif
        }
        GXSetScissor(0, 0, 0x260, 0x1E0);
    }

    if (WW_S32(work, 0x14C) > 2) {
        winIconInit();
        if (WW_S32(work, 0x148) == 0) {
            if ((gp->retraceCount & 0x1F) < 0x14) {
                i1_t = *(Vec*)((u8*)vp + 0x114);
                i1_t.x = (float_250_8041d1a4 + x) - float_10_8041d1a0;
                i1_t.y = (float_40_8041d0e8 + (y - float_40_8041d0e8)) - float_24_8041d0c8;
                i1_s = *(Vec*)((u8*)vp + 0x120);
                c_i1 = dat_8041cf34;
#ifdef MUST_MATCH
                {
                    Vec tcopy = i1_t;
                    winIconSet(0x1B6, &tcopy, &i1_s, &c_i1);
                }
#else
                winIconSet(0x1B6, &i1_t, &i1_s, &c_i1);
#endif
            }
        } else if (WW_S32(work, 0x148) / 2 == (WW_S32(work, 0x14C) + 1) / 2 - 1) {
            if ((gp->retraceCount & 0x1F) < 0x14) {
                i2_t = *(Vec*)((u8*)vp + 0x12C);
                i2_t.x = (float_250_8041d1a4 + x) - float_10_8041d1a0;
                i2_t.y = float_24_8041d0c8 + (float_40_8041d0e8 + (y - float_40_8041d0e8));
                i2_s = *(Vec*)((u8*)vp + 0x138);
                c_i2 = dat_8041cf38;
#ifdef MUST_MATCH
                {
                    Vec tcopy = i2_t;
                    winIconSet(0x1B5, &tcopy, &i2_s, &c_i2);
                }
#else
                winIconSet(0x1B5, &i2_t, &i2_s, &c_i2);
#endif
            }
        } else {
            if ((gp->retraceCount & 0x1F) < 0x14) {
                i3_t = *(Vec*)((u8*)vp + 0x144);
                i3_t.x = (float_250_8041d1a4 + x) - float_10_8041d1a0;
                i3_t.y = float_24_8041d0c8 + (float_40_8041d0e8 + (y - float_40_8041d0e8));
                i3_s = *(Vec*)((u8*)vp + 0x150);
                c_i3 = dat_8041cf3c;
#ifdef MUST_MATCH
                {
                    Vec tcopy = i3_t;
                    winIconSet(0x1B5, &tcopy, &i3_s, &c_i3);
                }
#else
                winIconSet(0x1B5, &i3_t, &i3_s, &c_i3);
#endif

                i4_t = *(Vec*)((u8*)vp + 0x15C);
                i4_t.x = (float_250_8041d1a4 + x) - float_10_8041d1a0;
                i4_t.y = (float_40_8041d0e8 + (y - float_40_8041d0e8)) - float_24_8041d0c8;
                i4_s = *(Vec*)((u8*)vp + 0x168);
                c_i4 = dat_8041cf40;
#ifdef MUST_MATCH
                {
                    Vec tcopy = i4_t;
                    winIconSet(0x1B6, &tcopy, &i4_s, &c_i4);
                }
#else
                winIconSet(0x1B6, &i4_t, &i4_s, &c_i4);
#endif
            }
        }
        fin_t = *(Vec*)((u8*)vp + 0x174);
        fin_t.x = (float_250_8041d1a4 + x) - float_10_8041d1a0;
        fin_t.y = float_40_8041d0e8 + (y - float_40_8041d0e8);
        fin_s = *(Vec*)((u8*)vp + 0x180);
        c_fin = dat_8041cf44;
#ifdef MUST_MATCH
        {
            Vec tcopy = fin_t;
            winIconSet(0x70, &tcopy, &fin_s, &c_fin);
        }
#else
        winIconSet(0x70, &fin_t, &fin_s, &c_fin);
#endif
    }
}

// NONMATCHING (90.37%): GX quad vertex loop; MWCC CSEs the position constants
// into f1/f2/f3 and reloads f0 only, while the original reloads f0 per vertex.
// Non-const externs made it worse (88.47%).
void winZClear(void) {
    Mtx mtx;

    PSMTXTrans(mtx, float_0_8041d068, float_0_8041d068, float_neg1000_8041d0cc);
    PSMTXConcat(camGetCurPtr()->view, mtx, mtx);
    GXLoadPosMtxImm(mtx, 0);
    GXSetCurrentMtx(0);
    GXSetAlphaUpdate(0);
    GXSetColorUpdate(0);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0xFF, 0xFF, 4);
    GXSetTevOp(0, 4);
    GXSetNumTexGens(0);
    GXSetZMode(1, 7, 1);
    GXSetBlendMode(0, 4, 5, 0);
    GXSetZCompLoc(1);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxDesc(9, 1);
    GXBegin(0x80, 0, 4);
    GXPosition3f32(float_neg500_8041d0d0, float_300_8041d0d4, float_0_8041d068);
    GXPosition3f32(float_0_8041d068, float_300_8041d0d4, float_0_8041d068);
    GXPosition3f32(float_0_8041d068, float_neg100_8041d0d8, float_0_8041d068);
    GXPosition3f32(float_neg500_8041d0d0, float_neg100_8041d0d8, float_0_8041d068);
    GXEnd();
    GXSetColorUpdate(1);
}

int compare_func1(void* a, void* b) {
    if (*(u8*)a > *(u8*)b) {
        return 1;
    } else if (*(u8*)a < *(u8*)b) {
        return -1;
    } else {
        return 0;
    }
}

int compare_func1_r(void* a, void* b) {
    if (*(u8*)a < *(u8*)b) {
        return 1;
    } else if (*(u8*)a > *(u8*)b) {
        return -1;
    } else {
        return 0;
    }
}

int compare_func5(void* a, void* b) {
    if (*(u8*)a > *(u8*)b) {
        return 1;
    } else if (*(u8*)a < *(u8*)b) {
        return -1;
    } else {
        return 0;
    }
}

int compare_func5_r(void* a, void* b) {
    if (*(u8*)a < *(u8*)b) {
        return 1;
    } else if (*(u8*)a > *(u8*)b) {
        return -1;
    } else {
        return 0;
    }
}

void winSortEntry(WindowWork* work, s32 idx, f32 a, f32 b) {
    SortTable* p = sort_data[idx];
    s32 count = 0;

    while (p->name != NULL) {
        p++;
        count++;
    }

    WW_S32(work, 0x184) = idx;
    WW_S32(work, 0x178) = count;
    WW_S32(work, 0x174) = 0;
    WW_S32(work, 0x180) = 0;
    WW_F32(work, 0x164) = float_neg1000_8041d0cc;
    WW_F32(work, 0x16C) = a;
    WW_F32(work, 0x168) = b;
    WW_F32(work, 0x170) = b;
    WW_S32(work, 0x17C) = 1;
}

int N_compare_func4_2(void* a, void* b) {
    s32 id_a = (s16)*(s32*)((u8*)a + 8);
    s32 id_b = (s16)*(s32*)((u8*)b + 8);
    s16 va = itemDataTable[id_a].type_sort_order;
    s16 vb = itemDataTable[id_b].type_sort_order;

    if (va > vb) {
        return 1;
    } else if (va < vb) {
        return -1;
    } else {
        return 0;
    }
}

int N_compare_func4_2_r(void* a, void* b) {
    s32 id_a = (s16)*(s32*)((u8*)a + 8);
    s32 id_b = (s16)*(s32*)((u8*)b + 8);
    s16 va = itemDataTable[id_a].type_sort_order;
    s16 vb = itemDataTable[id_b].type_sort_order;

    if (va < vb) {
        return 1;
    } else if (va > vb) {
        return -1;
    } else {
        return 0;
    }
}

// NONMATCHING (80.65%): the instruction multiset matches the original; only MWCC's
// scheduling differs (the original hoists `lwz r4, 0x8(r4)` above the prologue
// saves, ours emits it after them).
int N_compare_func4_1(void* a, void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_b = (s16)*(s32*)((u8*)b + 8);
    s16 id_a = (s16)*(s32*)((u8*)a + 8);

    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_a, (char*)buf_b);
}

// NONMATCHING (80.65%): same schedule-only difference as N_compare_func4_1 (ids
// are s16, the original `extsh`s both).
int N_compare_func4_1_r(void* a, void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_b = (s16)*(s32*)((u8*)b + 8);
    s16 id_a = (s16)*(s32*)((u8*)a + 8);

    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_b, (char*)buf_a);
}

int compare_func5_2(void* a, void* b) {
    s16 id_a = (s16)(*(s16*)a + 0xF0);
    s16 id_b = (s16)(*(s16*)b + 0xF0);
    s16 va = itemDataTable[id_a].type_sort_order;
    s16 vb = itemDataTable[id_b].type_sort_order;

    if (va > vb) {
        return 1;
    } else if (va < vb) {
        return -1;
    } else {
        return 0;
    }
}

int compare_func5_2_r(void* a, void* b) {
    s16 id_a = (s16)(*(s16*)a + 0xF0);
    s16 id_b = (s16)(*(s16*)b + 0xF0);
    s16 va = itemDataTable[id_a].type_sort_order;
    s16 vb = itemDataTable[id_b].type_sort_order;

    if (va < vb) {
        return 1;
    } else if (va > vb) {
        return -1;
    } else {
        return 0;
    }
}

int compare_func6_2(void* a, void* b) {
    s16 id_a = (s16)(*(s16*)a + 0xB3);
    s16 id_b = (s16)(*(s16*)b + 0xB3);
    s16 va = itemDataTable[id_a].type_sort_order;
    s16 vb = itemDataTable[id_b].type_sort_order;

    if (va > vb) {
        return 1;
    } else if (va < vb) {
        return -1;
    } else {
        return 0;
    }
}

int compare_func6_2_r(void* a, void* b) {
    s16 id_a = (s16)(*(s16*)a + 0xB3);
    s16 id_b = (s16)(*(s16*)b + 0xB3);
    s16 va = itemDataTable[id_a].type_sort_order;
    s16 vb = itemDataTable[id_b].type_sort_order;

    if (va < vb) {
        return 1;
    } else if (va > vb) {
        return -1;
    } else {
        return 0;
    }
}

// NONMATCHING (69.03%): 2 instructions differ, 5 missing, 5 extra.
int compare_func5_1(void* a, void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_a = (s16)(*(s16*)a + 0xF0);
    s16 id_b = (s16)(*(s16*)b + 0xF0);

    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_a, (char*)buf_b);
}

// NONMATCHING (69.03%): 2 instructions differ, 5 missing, 5 extra.
int compare_func5_1_r(void* a, void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_a = (s16)(*(s16*)a + 0xF0);
    s16 id_b = (s16)(*(s16*)b + 0xF0);

    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_b, (char*)buf_a);
}

// NONMATCHING (69.03%): 2 instructions differ, 5 missing, 5 extra.
int compare_func6_1(void* a, void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_a = (s16)(*(s16*)a + 0xB3);
    s16 id_b = (s16)(*(s16*)b + 0xB3);

    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_a, (char*)buf_b);
}

// NONMATCHING (69.03%): 2 instructions differ, 5 missing, 5 extra.
int compare_func6_1_r(void* a, void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_a = (s16)(*(s16*)a + 0xB3);
    s16 id_b = (s16)(*(s16*)b + 0xB3);

    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_b, (char*)buf_a);
}

// NONMATCHING (92.86%): 1 missing, 1 extra.
int compare_func4(void* a, void* b) {
    void* ma = mailGetPtr(*(u8*)((u8*)a + 1));
    void* mb = mailGetPtr(*(u8*)((u8*)b + 1));
    const char* sa = (const char*)msgSearch(*(const char**)((u8*)ma + 8));
    const char* sb = (const char*)msgSearch(*(const char**)((u8*)mb + 8));

    return strcmp(sa, sb);
}

// NONMATCHING (92.31%): 1 missing, 1 extra.
int compare_func4_r(void* a, void* b) {
    void* ma = mailGetPtr(*(u8*)((u8*)a + 1));
    void* mb = mailGetPtr(*(u8*)((u8*)b + 1));
    const char* sa = (const char*)msgSearch(*(const char**)((u8*)ma + 8));
    const char* sb = (const char*)msgSearch(*(const char**)((u8*)mb + 8));

    return strcmp(sb, sa);
}

void sort_7_1_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0x1050, WW_S32(work, 0x1038), 2, compare_func1);
    } else {
        qqsort((u8*)work + 0x1050, WW_S32(work, 0x1038), 2, compare_func1_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x1040) = 0;
    WW_S32(work, 0x1044) = WW_S32(work, 0x1040) / 16;
}

void sort_7_3_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func1);
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func3);
    } else {
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func1_r);
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func3_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x1040) = 0;
    WW_S32(work, 0x1044) = WW_S32(work, 0x1040) / 16;
}

void sort_8_1_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0x26E, WW_S32(work, 0x254), 2, compare_func4);
    } else {
        qqsort((u8*)work + 0x26E, WW_S32(work, 0x254), 2, compare_func4_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x258) = 0;
    WW_S32(work, 0x25C) = WW_S32(work, 0x258) / 5;
}

void sort_8_2_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0x26E, WW_S32(work, 0x254), 2, compare_func5);
    } else {
        qqsort((u8*)work + 0x26E, WW_S32(work, 0x254), 2, compare_func5_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x258) = 0;
    WW_S32(work, 0x25C) = WW_S32(work, 0x258) / 5;
}

int compare_func3_r(void* a, void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    TattleEntry* ea = battleGetUnitMonosiriPtr(*(u8*)((u8*)a + 1));
    TattleEntry* eb = battleGetUnitMonosiriPtr(*(u8*)((u8*)b + 1));

    unk_800d06b4((char*)msgSearch(ea->enemyTag), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(eb->enemyTag), (char*)buf_b);

    if (*(u8*)((u8*)a + 1) == 0x4D || *(u8*)((u8*)a + 1) == 0x87) {
        if (evtGetValue(0, 0xF5DE0180) < 0xD2) {
            unk_800d06b4((char*)msgSearch(str_802F44F0), (char*)buf_a);
        } else {
            unk_800d06b4((char*)msgSearch(str_802F4500), (char*)buf_a);
        }
    }
    if (*(u8*)((u8*)b + 1) == 0x4D || *(u8*)((u8*)b + 1) == 0x87) {
        if (evtGetValue(0, 0xF5DE0180) < 0xD2) {
            unk_800d06b4((char*)msgSearch(str_802F44F0), (char*)buf_b);
        } else {
            unk_800d06b4((char*)msgSearch(str_802F4500), (char*)buf_b);
        }
    }

    return strcmp((char*)buf_b, (char*)buf_a);
}

int compare_func3(void* a, void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    TattleEntry* ea = battleGetUnitMonosiriPtr(*(u8*)((u8*)a + 1));
    TattleEntry* eb = battleGetUnitMonosiriPtr(*(u8*)((u8*)b + 1));

    unk_800d06b4((char*)msgSearch(ea->enemyTag), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(eb->enemyTag), (char*)buf_b);

    if (*(u8*)((u8*)a + 1) == 0x4D || *(u8*)((u8*)a + 1) == 0x87) {
        if (evtGetValue(0, 0xF5DE0180) < 0xD2) {
            unk_800d06b4((char*)msgSearch(str_802F44F0), (char*)buf_a);
        } else {
            unk_800d06b4((char*)msgSearch(str_802F4500), (char*)buf_a);
        }
    }
    if (*(u8*)((u8*)b + 1) == 0x4D || *(u8*)((u8*)b + 1) == 0x87) {
        if (evtGetValue(0, 0xF5DE0180) < 0xD2) {
            unk_800d06b4((char*)msgSearch(str_802F44F0), (char*)buf_b);
        } else {
            unk_800d06b4((char*)msgSearch(str_802F4500), (char*)buf_b);
        }
    }

    return strcmp((char*)buf_a, (char*)buf_b);
}

void sort_7_2_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func1);
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func2);
    } else {
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func1_r);
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func2_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x1040) = 0;
    WW_S32(work, 0x1044) = WW_S32(work, 0x1040) / 16;
}

// Two `menu_monosiri_%s` lookups over the 26-entry enemy-code table.
// NONMATCHING (94.86%): only MWCC register allocation (table base/tag/index FPRs)
// and the compiler-generated table symbol differ.
int compare_func2_r(void* a, void* b) {
    u8 buf_a[64];
    u8 buf_b[64];
    TattleEntry* ea = battleGetUnitMonosiriPtr(*(u8*)((u8*)a + 1));
    TattleEntry* eb = battleGetUnitMonosiriPtr(*(u8*)((u8*)b + 1));
    const char* tag_a = ea->locationTag;
    const char* tag_b;
    s32 i;
    s32 j;

    for (i = 0; (u32)i < 0x1A; i++) {
        sprintf((char*)buf_a, str_802F4510, tbl[i]);
        if (strcmp((char*)buf_a, tag_a) == 0) {
            break;
        }
    }
    if ((u32)i >= 0x1A) {
        i = 0;
    }

    tag_b = eb->locationTag;
    for (j = 0; (u32)j < 0x1A; j++) {
        sprintf((char*)buf_b, str_802F4510, tbl[j]);
        if (strcmp((char*)buf_b, tag_b) == 0) {
            break;
        }
    }
    if ((u32)j >= 0x1A) {
        j = 0;
    }

    if (i < j) {
        return 1;
    } else if (i > j) {
        return -1;
    } else {
        return 0;
    }
}

// Two `menu_monosiri_%s` lookups over the 26-entry enemy-code table.
// NONMATCHING (94.86%): only MWCC register allocation (table base/tag/index FPRs)
// and the compiler-generated table symbol differ.
int compare_func2(void* a, void* b) {
    u8 buf_a[64];
    u8 buf_b[64];
    TattleEntry* ea = battleGetUnitMonosiriPtr(*(u8*)((u8*)a + 1));
    TattleEntry* eb = battleGetUnitMonosiriPtr(*(u8*)((u8*)b + 1));
    const char* tag_a = ea->locationTag;
    const char* tag_b;
    s32 i;
    s32 j;

    for (i = 0; (u32)i < 0x1A; i++) {
        sprintf((char*)buf_a, str_802F4510, tbl[i]);
        if (strcmp((char*)buf_a, tag_a) == 0) {
            break;
        }
    }
    if ((u32)i >= 0x1A) {
        i = 0;
    }

    tag_b = eb->locationTag;
    for (j = 0; (u32)j < 0x1A; j++) {
        sprintf((char*)buf_b, str_802F4510, tbl[j]);
        if (strcmp((char*)buf_b, tag_b) == 0) {
            break;
        }
    }
    if ((u32)j >= 0x1A) {
        j = 0;
    }

    if (i > j) {
        return 1;
    } else if (i < j) {
        return -1;
    } else {
        return 0;
    }
}

void sort_6_1_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0xFB4, WW_S32(work, 0xF90), 2, compare_func6_1);
    } else {
        qqsort((u8*)work + 0xFB4, WW_S32(work, 0xF90), 2, compare_func6_1_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0xF94) = 0;
    WW_S32(work, 0xF98) = WW_S32(work, 0xF94) / 28;
}

void sort_6_2_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0xFB4, WW_S32(work, 0xF90), 2, compare_func6_2);
    } else {
        qqsort((u8*)work + 0xFB4, WW_S32(work, 0xF90), 2, compare_func6_2_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0xF94) = 0;
    WW_S32(work, 0xF98) = WW_S32(work, 0xF94) / 28;
}

void sort_5_2_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0xEB8, WW_S32(work, 0xE94), 2, compare_func5_2);
    } else {
        qqsort((u8*)work + 0xEB8, WW_S32(work, 0xE94), 2, compare_func5_2_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0xE98) = 0;
    WW_S32(work, 0xE9C) = WW_S32(work, 0xE98) / 28;
}

void sort_5_1_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0xEB8, WW_S32(work, 0xE94), 2, compare_func5_1);
    } else {
        qqsort((u8*)work + 0xEB8, WW_S32(work, 0xE94), 2, compare_func5_1_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0xE98) = 0;
    WW_S32(work, 0xE9C) = WW_S32(work, 0xE98) / 28;
}

void sort_4_2_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0x404, WW_S32(work, 0xD64), 0xC, N_compare_func4_2);
    } else {
        qqsort((u8*)work + 0x404, WW_S32(work, 0xD64), 0xC, N_compare_func4_2_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x3E8) = 0;
    WW_S32(work, 0x3F0) = WW_S32(work, 0x3E8) / 8;
}

void sort_4_1_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0x404, WW_S32(work, 0xD64), 0xC, N_compare_func4_1);
    } else {
        qqsort((u8*)work + 0x404, WW_S32(work, 0xD64), 0xC, N_compare_func4_1_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x3E8) = 0;
    WW_S32(work, 0x3F0) = WW_S32(work, 0x3E8) / 8;
}

void sort_3_1_func(WindowWork* work) {
    s32 badge = WW_S32(work, 0x3E4);
    pouchGetHaveBadgeCnt();
    pouchHaveBadge(badge);
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 4 : 11);
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeEquipList(work);
    WW_S32(work, 0x3E4) = 0;
    WW_S32(work, 0x3EC) = WW_S32(work, 0x3E4) / 8;
}

void sort_3_2_func(WindowWork* work) {
    s32 badge = WW_S32(work, 0x3E4);
    pouchGetHaveBadgeCnt();
    pouchHaveBadge(badge);
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 5 : 12);
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeEquipList(work);
    WW_S32(work, 0x3E4) = 0;
    WW_S32(work, 0x3EC) = WW_S32(work, 0x3E4) / 8;
}

void sort_3_3_func(WindowWork* work) {
    s32 badge = WW_S32(work, 0x3E4);
    pouchGetHaveBadgeCnt();
    pouchHaveBadge(badge);
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 6 : 13);
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 6 : 13);
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeEquipList(work);
    WW_S32(work, 0x3E4) = 0;
    WW_S32(work, 0x3EC) = WW_S32(work, 0x3E4) / 8;
}

void sort_2_2_func(WindowWork* work) {
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 3 : 10);
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeSkipList(work);
    WW_S32(work, 0x218) = 0;
    WW_S32(work, 0x220) = WW_S32(work, 0x218) / 10;
}

void sort_2_1_func(WindowWork* work) {
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 2 : 9);
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeSkipList(work);
    WW_S32(work, 0x218) = 0;
    WW_S32(work, 0x220) = WW_S32(work, 0x218) / 10;
}

void sort_1_2_func(WindowWork* work) {
    s32 item = WW_S32(work, 0x214);
    pouchGetHaveItemCnt();
    pouchHaveItem(item);
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 1 : 8);
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeSkipList(work);
    WW_S32(work, 0x214) = 0;
    WW_S32(work, 0x21C) = WW_S32(work, 0x214) / 10;
}

void sort_1_1_func(WindowWork* work) {
    s32 item = WW_S32(work, 0x214);
    pouchGetHaveItemCnt();
    pouchHaveItem(item);
    {
        s32 type = 7;
        if (WW_S32(work, 0x180) == 0) {
            type = 0;
        }
        pouchSortItem(type);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeSkipList(work);
    WW_S32(work, 0x214) = 0;
    WW_S32(work, 0x21C) = WW_S32(work, 0x214) / 10;
}

// The two trailing lerps use separate deltas so MWCC keeps the first fmadd's
// difference/accumulator in the original's FPRs.
void winSortMain(WindowWork* work) {
    u32 trg = work->buttonTrg;
    u32 rep = work->dirRep;

    if (WW_S32(work, 0x17C) == 0) {
        trg = 0;
        rep = 0;
    }

    if (trg & 0x200) {
        psndSFXOn((const char*)0x20013);
        WW_S32(work, 0x17C) = 0;
        WW_F32(work, 0x16C) = float_neg600_8041d0fc;
    } else if (rep & 0x1000) {
        psndSFXOn((const char*)0x20005);
        WW_S32(work, 0x174) = WW_S32(work, 0x174) - 1;
        if (WW_S32(work, 0x174) < 0) {
            WW_S32(work, 0x174) = WW_S32(work, 0x178) - 1;
        }
    } else if (rep & 0x2000) {
        psndSFXOn((const char*)0x20005);
        WW_S32(work, 0x174) = WW_S32(work, 0x174) + 1;
        if (WW_S32(work, 0x174) >= WW_S32(work, 0x178)) {
            WW_S32(work, 0x174) = 0;
        }
    } else if (trg & 0x100) {
        psndSFXOn((const char*)0x20012);
        sort_data[WW_S32(work, 0x184)][WW_S32(work, 0x174)].func(work);
    } else if (trg & 0x1000) {
        WW_S32(work, 0x17C) = 0;
        WW_F32(work, 0x16C) = float_neg600_8041d0fc;
    }

    if (WW_S32(work, 0x17C) != 0) {
        BOOL changed = FALSE;
        const char* msg;
        u16 width;

        WW_F32(work, 0x158) = float_30_8041d100 + WW_F32(work, 0x16C);
        WW_F32(work, 0x15C) = (WW_F32(work, 0x170) - float_40_8041d0e8) -
                              (f32)(WW_S32(work, 0x174) * 0x28);
        msg = sort_data[WW_S32(work, 0x184)][WW_S32(work, 0x174)].name;
        if (msg != NULL) {
            if (WW_PTR(work, 0x144) != (void*)msg) {
                changed = TRUE;
            } else if (strcmp(msgbuf, msg) != 0) {
                changed = TRUE;
            }
            if (changed) {
                WW_S32(work, 0x138) = 0;
                WW_PTR(work, 0x13C) = (void*)msg;
                WW_S32(work, 0x140) = 0;
                FontGetMessageWidthLine((const char*)msgSearch((const char*)WW_PTR(work, 0x13C)), &width);
                WW_S32(work, 0x148) = 0;
                WW_S32(work, 0x14C) = width + 1;
                WW_PTR(work, 0x144) = (void*)msg;
                strncpy(msgbuf, msg, 0x3C);
            }
        }
    }

    {
#ifdef MUST_MATCH
        // Two deltas (the natural form shares one): a single temp makes MWCC swap
        // the first fmadd's difference and accumulator registers.
        f32 d1;
        f32 d2;
        d1 = WW_F32(work, 0x16C) - WW_F32(work, 0x164);
        WW_F32(work, 0x164) += d1 * float_0p125_8041d104;
        d2 = WW_F32(work, 0x170) - WW_F32(work, 0x168);
        WW_F32(work, 0x168) += d2 * float_0p125_8041d104;
#else
        WW_F32(work, 0x164) += float_0p125_8041d104 * (WW_F32(work, 0x16C) - WW_F32(work, 0x164));
        WW_F32(work, 0x168) += float_0p125_8041d104 * (WW_F32(work, 0x170) - WW_F32(work, 0x168));
#endif
    }
}

extern const f32 float_neg800_8041d1b0;
extern const f32 float_neg170_8041d1b4;

void winMsgMain(WindowWork* work) {
    switch (WW_S32(work, 0x130)) {
        case 0:
            WW_F32(work, 0x128) = float_neg800_8041d1b0;
            WW_F32(work, 0x12C) = float_neg170_8041d1b4;
            WW_S32(work, 0x134) = 0;
            WW_S32(work, 0x130) = WW_S32(work, 0x130) + 1;
            break;
        case 1: {
            s32 step = WW_S32(work, 0x134);
            WW_S32(work, 0x134) = step + 1;
            WW_F32(work, 0x128) = intplGetValue(0xB, step, 0x10, float_neg800_8041d1b0, float_0_8041d068);
            if (WW_S32(work, 0x134) > 0x10) {
                WW_S32(work, 0x130) = WW_S32(work, 0x130) + 1;
            }
            break;
        }
        case 2:
            if (keyGetDirTrg(0) & 0x100) {
                WW_S32(work, 0x148) = WW_S32(work, 0x148) - 2;
                if (WW_S32(work, 0x148) < 0) {
                    WW_S32(work, 0x148) = 0;
                } else {
                    psndSFXOn((const char*)0x20006);
                }
            } else if (keyGetDirTrg(0) & 0x200) {
                if (WW_S32(work, 0x148) / 2 < (WW_S32(work, 0x14C) + 1) / 2 - 1) {
                    WW_S32(work, 0x148) = WW_S32(work, 0x148) + 2;
                    psndSFXOn((const char*)0x20006);
                }
            }
            break;
        case 10:
            WW_S32(work, 0x134) = 0;
            WW_S32(work, 0x130) = WW_S32(work, 0x130) + 1;
            break;
        case 11: {
            s32 step = WW_S32(work, 0x134);
            WW_S32(work, 0x134) = step + 1;
            WW_F32(work, 0x128) = intplGetValue(0xB, step, 0x10, float_0_8041d068, float_neg800_8041d1b0);
            if (WW_S32(work, 0x134) > 0x10) {
                WW_S32(work, 0x130) = WW_S32(work, 0x130) + 1;
            }
            break;
        }
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 12:
        default:
            break;
    }
}

extern const f32 float_800_8041d1d8;
extern const f32 float_640_8041d1dc;
extern const f32 float_neg640_8041d1e0;

// 5-page switch over work->0xcc. The explicit empty case -1 below (a redirect
// target of cases 11 and 41) is what makes MWCC pick 0 as the [0,10) search-tree
// pivot; without it the tree pivots on 1.
void winBgMain(WindowWork* work) {
    WindowWork* p = work;
    s32 i;

    for (i = 0; i < 5; i++) {
        switch (WW_S32(p, 0xCC)) {
            case 0:
                WW_F32(p, 0xC4) = float_800_8041d1d8;
                WW_F32(p, 0xC8) = float_0_8041d068;
                WW_S32(p, 0xD0) = 0;
                WW_S32(p, 0xCC) = WW_S32(p, 0xCC) + 1;
                break;
            case 1: {
                s32 step = WW_S32(p, 0xD0);
                WW_S32(p, 0xD0) = step + 1;
                WW_F32(p, 0xC4) = intplGetValue(0xB, step, 0x10, float_800_8041d1d8, float_0_8041d068);
                if (WW_S32(p, 0xD0) > 0x10) {
                    WW_S32(p, 0xCC) = 0x64;
                }
                break;
            }
            case 10:
                WW_F32(p, 0xC8) = float_0_8041d068;
                WW_S32(p, 0xD0) = 0;
                WW_S32(p, 0xCC) = WW_S32(p, 0xCC) + 1;
                break;
            case 11: {
                s32 step = WW_S32(p, 0xD0);
                WW_S32(p, 0xD0) = step + 1;
                WW_F32(p, 0xC4) = intplGetValue(0xB, step, 0x10, float_0_8041d068, float_800_8041d1d8);
                if (WW_S32(p, 0xD0) > 0x10) {
                    WW_S32(p, 0xCC) = -1;
                }
                break;
            }
            case 20:
                WW_F32(p, 0xC8) = float_0_8041d068;
                WW_S32(p, 0xD0) = 0;
                WW_S32(p, 0xCC) = WW_S32(p, 0xCC) + 1;
                break;
            case 21: {
                s32 step = WW_S32(p, 0xD0);
                WW_S32(p, 0xD0) = step + 1;
                WW_F32(p, 0xC4) = intplGetValue(0xB, step, 0x18, float_640_8041d1dc, float_0_8041d068);
                if (WW_S32(p, 0xD0) > 0x18) {
                    WW_S32(p, 0xCC) = 0x64;
                }
                break;
            }
            case 30:
                WW_F32(p, 0xC8) = float_0_8041d068;
                WW_S32(p, 0xD0) = 0;
                WW_S32(p, 0xCC) = WW_S32(p, 0xCC) + 1;
                break;
            case 31: {
                s32 step = WW_S32(p, 0xD0);
                WW_S32(p, 0xD0) = step + 1;
                WW_F32(p, 0xC4) = intplGetValue(0xB, step, 0x18, float_neg640_8041d1e0, float_0_8041d068);
                if (WW_S32(p, 0xD0) > 0x18) {
                    WW_S32(p, 0xCC) = 0x64;
                }
                break;
            }
            case 40:
                WW_F32(p, 0xC8) = float_0_8041d068;
                WW_S32(p, 0xD0) = 0;
                WW_S32(p, 0xCC) = WW_S32(p, 0xCC) + 1;
                break;
            case 41: {
                s32 step = WW_S32(p, 0xD0);
                WW_S32(p, 0xD0) = step + 1;
                WW_F32(p, 0xC4) = intplGetValue(0xB, step, 0x18, float_0_8041d068, float_neg640_8041d1e0);
                if (WW_S32(p, 0xD0) > 0x18) {
                    WW_S32(p, 0xCC) = -1;
                }
                break;
            }
#ifdef MUST_MATCH
            /* MUST_MATCH: -1 is a redirect target of cases 11/41; the empty case makes
               MWCC emit the retail search-tree pivot (0 then 2) instead of 1 then 0. */
            case -1:
#endif
            case 100:
                break;
            default:
                break;
        }
        p = (WindowWork*)((u8*)p + 0x14);
    }
}

// NONMATCHING (84.78%): bubble sort of the 0xc-byte records at work+0x404. pb now
// starts as work + j*0xc like the original; still, the original spills the three
// swap temporaries to 0x14/0x18/0x1c and uses different volatile registers.
void sort_4_3_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        s32 i;
        u8* pa = (u8*)work;

        for (i = 0; i < WW_S32(work, 0xD64) - 1; i++) {
            s32 j = i + 1;
            u8* pb = (u8*)work + j * 0xC;

            while (j < WW_S32(work, 0xD64)) {
                ItemData* da = &itemDataTable[(s16)*(u32*)(pa + 0x40C)];
                ItemData* db = &itemDataTable[(s16)*(u32*)(pb + 0x40C)];
                s32 swap = 0;

                if (da->bp_cost > db->bp_cost) {
                    swap = 1;
                } else if (da->bp_cost == db->bp_cost && da->type_sort_order > db->type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    u32 t0 = *(u32*)(pa + 0x404);
                    u32 t1 = *(u32*)(pa + 0x408);
                    u32 t2 = *(u32*)(pa + 0x40C);
                    *(u32*)(pa + 0x404) = *(u32*)(pb + 0x404);
                    *(u32*)(pa + 0x408) = *(u32*)(pb + 0x408);
                    *(u32*)(pa + 0x40C) = *(u32*)(pb + 0x40C);
                    *(u32*)(pb + 0x404) = t0;
                    *(u32*)(pb + 0x408) = t1;
                    *(u32*)(pb + 0x40C) = t2;
                }
                pb += 0xC;
                j++;
            }
            pa += 0xC;
        }
    } else {
        s32 i;
        u8* pa = (u8*)work;

        for (i = 0; i < WW_S32(work, 0xD64) - 1; i++) {
            s32 j = i + 1;
            u8* pb = (u8*)work + j * 0xC;

            while (j < WW_S32(work, 0xD64)) {
                ItemData* da = &itemDataTable[(s16)*(u32*)(pa + 0x40C)];
                ItemData* db = &itemDataTable[(s16)*(u32*)(pb + 0x40C)];
                s32 swap = 0;

                if (da->bp_cost < db->bp_cost) {
                    swap = 1;
                } else if (da->bp_cost == db->bp_cost && da->type_sort_order < db->type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    u32 t0 = *(u32*)(pa + 0x404);
                    u32 t1 = *(u32*)(pa + 0x408);
                    u32 t2 = *(u32*)(pa + 0x40C);
                    *(u32*)(pa + 0x404) = *(u32*)(pb + 0x404);
                    *(u32*)(pa + 0x408) = *(u32*)(pb + 0x408);
                    *(u32*)(pa + 0x40C) = *(u32*)(pb + 0x40C);
                    *(u32*)(pb + 0x404) = t0;
                    *(u32*)(pb + 0x408) = t1;
                    *(u32*)(pb + 0x40C) = t2;
                }
                pb += 0xC;
                j++;
            }
            pa += 0xC;
        }
    }

    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x3E8) = 0;
    WW_S32(work, 0x3F0) = WW_S32(work, 0x3E8) / 8;
}

// NONMATCHING (98.13%): same bubble sort as sort_4_3 but over the s16 array at
// work+0xeb8 (index +0xF0). Loops/branches match; only MWCC's register assignment
// differs (original: i=r12, j=r31, pb=r6; ours: i=r6, j=r8, pb=r9).
void sort_5_3_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        s32 i;
        u8* pa = (u8*)work;

        for (i = 0; i < WW_S32(work, 0xE94) - 1; i++) {
            s32 j = i + 1;
            u8* pb = (u8*)work + j * 2;

            while (j < WW_S32(work, 0xE94)) {
                s16 la = *(s16*)(pa + 0xEB8);
                s16 lb = *(s16*)(pb + 0xEB8);
                ItemData* da = &itemDataTable[(s16)((s32)la + 0xF0)];
                ItemData* db = &itemDataTable[(s16)((s32)lb + 0xF0)];
                s32 swap = 0;

                if (da->bp_cost > db->bp_cost) {
                    swap = 1;
                } else if (da->bp_cost == db->bp_cost && da->type_sort_order > db->type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    *(s16*)(pa + 0xEB8) = lb;
                    *(s16*)(pb + 0xEB8) = la;
                }
                pb += 2;
                j++;
            }
            pa += 2;
        }
    } else {
        s32 i;
        u8* pa = (u8*)work;

        for (i = 0; i < WW_S32(work, 0xE94) - 1; i++) {
            s32 j = i + 1;
            u8* pb = (u8*)work + j * 2;

            while (j < WW_S32(work, 0xE94)) {
                s16 la = *(s16*)(pa + 0xEB8);
                s16 lb = *(s16*)(pb + 0xEB8);
                ItemData* da = &itemDataTable[(s16)((s32)la + 0xF0)];
                ItemData* db = &itemDataTable[(s16)((s32)lb + 0xF0)];
                s32 swap = 0;

                if (da->bp_cost < db->bp_cost) {
                    swap = 1;
                } else if (da->bp_cost == db->bp_cost && da->type_sort_order < db->type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    *(s16*)(pa + 0xEB8) = lb;
                    *(s16*)(pb + 0xEB8) = la;
                }
                pb += 2;
                j++;
            }
            pa += 2;
        }
    }

    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0xE98) = 0;
    WW_S32(work, 0xE9C) = WW_S32(work, 0xE98) / 28;
}

// First loop and inter-call sequence match; still differs in the scheduling of
// the dat_8041ced8 load and the stack slots MWCC picks for the translate/scale
// temporaries in the second loop and trailing winTexSet calls.
// NONMATCHING (63.77%): see above.
void winRootDisp(CameraId camId, WindowWork* work) {
    WinRootVecs* vp = &str_802F3FE8;
    GXColor bgcolor = dat_8041ced8;
    Vec translate;
    Vec scale;
    s32 i;
    s32 j;
    u8* p;

    if (WW_S32(work, 0x24) == 0) {
        return;
    }

    for (i = 4; i >= 0; i--) {
        u8* e = (u8*)work + i * 0x14;
        s32 state = WW_S32(e, 0xCC);

        if (state == -1) {
            continue;
        }
        if (i > 0 && WW_S32(e, 0xB8) == 0x64) {
            continue;
        }
        if (i > 0 && (u32)(state - 0xA) <= 1) {
            continue;
        }

        switch (WW_S32(e, 0xC0)) {
            case 0:
                winMarioDisp(camId, work, i);
                break;
            case 1:
                winPartyDisp(camId, work, i);
                break;
            case 2:
                winItemDisp(camId, work, i);
                break;
            case 3:
                winBadgeDisp(camId, work, i);
                break;
            case 4:
                winLogDisp(camId, work, i);
                break;
        }
    }

    p = (u8*)work;
    for (j = 0; j < WW_S32(work, 0x44); j++) {
        GXColor color;

        if (WW_S32(work, 0x40) == j) {
            color = dat_8041cee0;
        } else {
            color = dat_8041cee4;
        }

        winTexInit_x2(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
        translate = vp->v4;
        scale = vp->v5;
        translate.x = WW_F32(p, 0x4C);
        translate.y = WW_F32(p, 0x50);
        winTexSet_x2(WW_S32(p, 0x48) + 1, 0xB2, translate, scale, color);
        p += 0x18;
    }

    winMailDisp(work);
    winMsgDisp(work, WW_F32(work, 0x128), WW_F32(work, 0x12C));
    winSortGX(work);

    if (!(work->flags & 0x8000)) {
        winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));

        translate = vp->v0;
        scale = vp->v1;
        translate.x = WW_F32(work, 0x150) + 2.0f;
        translate.y = WW_F32(work, 0x154) - 2.0f;
        winTexSet(0, translate, scale, dat_8041cedc);

        translate = vp->v2;
        scale = vp->v3;
        translate.x = WW_F32(work, 0x150);
        translate.y = WW_F32(work, 0x154);
        winTexSet(0, translate, scale, bgcolor);
    }
}

// All GX state, matrices and quad match except GXSetFog's 0.0f argument copies,
// the pooled int->float double (local @754@ vs double_to_int_mask_802f44c8) and
// the vertex FIFO stores (original reloads f0, MWCC keeps f1/f2/f3).
// NONMATCHING (90.93%): see above.
void winKageGX(WindowWork* work, void* color, f32 x, f32 y, f32 w, f32 h) {
    GXTexObj texObj;
    Mtx mtx4;
    Mtx mtx1;
    Mtx mtx2;
    Mtx mtx3;
    f32 scaleY;

    GXSetFog(0, float_0_8041d068, float_0_8041d068, float_0_8041d068, float_0_8041d068, dat_8041d014);
    GXSetBlendMode(1, 4, 5, 0);
    GXSetZCompLoc(1);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetZMode(0, 7, 0);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 2, 8, 0xF);
    GXSetTevAlphaIn(0, 7, 1, 4, 7);
    GXSetTevSwapMode(0, 0, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, 0x7A);
    GXInitTexObjLOD(&texObj, 1, 1, float_0_8041d068, float_0_8041d068, float_0_8041d068, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetTevColor(1, *(GXColor*)color);
    PSMTXTrans(mtx1, x, y, w);
    scaleY = h * (float_2_8041d090 * (f32)GXGetTexObjHeight(&texObj));
    PSMTXScale(mtx2, h * (float_2_8041d090 * (f32)GXGetTexObjWidth(&texObj)), scaleY, float_1_8041d070);
    PSMTXRotRad(mtx3, 0x78, float_neg1p3963_8041d0dc);
    PSMTXConcat(mtx1, mtx3, mtx4);
    PSMTXConcat(mtx4, mtx2, mtx4);
    PSMTXConcat(camGetPtr(CAMERA_2D)->view, mtx4, mtx4);
    GXLoadPosMtxImm(mtx4, 0);
    GXSetCurrentMtx(0);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXBegin(0x80, 0, 4);
    GXPosition3f32(float_neg0p5_8041d0e0, float_0p5_8041d06c, float_0_8041d068);
    GXTexCoord2f32(float_0_8041d068, float_0_8041d068);
    GXPosition3f32(float_neg0p5_8041d0e0, float_neg0p5_8041d0e0, float_0_8041d068);
    GXTexCoord2f32(float_0_8041d068, float_1_8041d070);
    GXPosition3f32(float_0p5_8041d06c, float_neg0p5_8041d0e0, float_0_8041d068);
    GXTexCoord2f32(float_1_8041d070, float_1_8041d070);
    GXPosition3f32(float_0p5_8041d06c, float_0p5_8041d06c, float_0_8041d068);
    GXTexCoord2f32(float_1_8041d070, float_0_8041d068);
}

// NONMATCHING (88.97%): everything matches except the vertex FIFO stores
// (original reloads each float constant into f0, MWCC keeps them in f1-f4) and
// the two pooled int->float doubles (local @848@ vs double_to_int_mask_802f44c8).
void winBgGX(WindowWork* work, s32 type, f32 x, f32 y) {
    GXTexObj texObj;
    Mtx mtx;
    f32 scaleY;

    switch (type) {
        case 0:
            GXSetTevColor(1, dat_8041cee8);
            GXSetTevColor(2, dat_8041ceec);
            GXSetTevColor(3, dat_8041cef0);
            break;
        case 1:
            GXSetTevColor(1, dat_8041cef4);
            GXSetTevColor(2, dat_8041cef8);
            GXSetTevColor(3, dat_8041cefc);
            break;
        case 2:
            GXSetTevColor(1, float_neg0p0047648_8041cf00);
            GXSetTevColor(2, dat_8041cf04);
            GXSetTevColor(3, dat_8041cf08);
            break;
        case 3:
            GXSetTevColor(1, dat_8041cf0c);
            GXSetTevColor(2, dat_8041cf10);
            GXSetTevColor(3, dat_8041cf14);
            break;
        case 4:
            GXSetTevColor(1, dat_8041cf18);
            GXSetTevColor(2, dat_8041cf1c);
            GXSetTevColor(3, dat_8041cf20);
            break;
    }

    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 7, 0);
    GXSetFog(0, float_0_8041d068, float_0_8041d068, float_0_8041d068, float_0_8041d068, dat_8041cf24);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 2);
    GXSetTevAlphaIn(0, 7, 7, 7, 4);
    GXSetTevSwapMode(0, 0, 0);

    PSMTXTrans(mtx, x, float_18_8041d1b8 + y, float_0_8041d068);
    PSMTXConcat(camGetPtr(CAMERA_2D)->view, mtx, mtx);
    GXLoadPosMtxImm(mtx, 0);
    GXSetCurrentMtx(0);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);

    TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, 7);
    GXInitTexObjLOD(&texObj, 0, 0, float_0_8041d068, float_0_8041d068, float_0_8041d068, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    scaleY = float_320_8041d074 / (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx, float_570_8041d1bc / (f32)GXGetTexObjWidth(&texObj), scaleY, float_1_8041d070);
    GXLoadTexMtxImm(mtx, 0x1E, 1);

    GXBegin(0x80, 0, 4);
    GXPosition3f32(float_neg285_8041d1c0, float_160_8041d1c4, float_0_8041d068);
    GXTexCoord2f32(float_0_8041d068, float_0_8041d068);
    GXPosition3f32(float_neg285_8041d1c0, float_neg160_8041d1c8, float_0_8041d068);
    GXTexCoord2f32(float_0_8041d068, float_1_8041d070);
    GXPosition3f32(float_0_8041d068, float_neg160_8041d1c8, float_0_8041d068);
    GXTexCoord2f32(float_1_8041d070, float_1_8041d070);
    GXPosition3f32(float_0_8041d068, float_160_8041d1c4, float_0_8041d068);
    GXTexCoord2f32(float_1_8041d070, float_0_8041d068);

    GXBegin(0x80, 0, 4);
    GXPosition3f32(float_0_8041d068, float_160_8041d1c4, float_0_8041d068);
    GXTexCoord2f32(float_1_8041d070, float_0_8041d068);
    GXPosition3f32(float_0_8041d068, float_neg160_8041d1c8, float_0_8041d068);
    GXTexCoord2f32(float_1_8041d070, float_1_8041d070);
    GXPosition3f32(float_285_8041d1cc, float_neg160_8041d1c8, float_0_8041d068);
    GXTexCoord2f32(float_0_8041d068, float_1_8041d070);
    GXPosition3f32(float_285_8041d1cc, float_160_8041d1c4, float_0_8041d068);
    GXTexCoord2f32(float_0_8041d068, float_0_8041d068);

    TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, 6);
    GXInitTexObjLOD(&texObj, 0, 0, float_0_8041d068, float_0_8041d068, float_0_8041d068, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    scaleY = float_288_8041d16c / (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx, float_570_8041d1bc / (f32)GXGetTexObjWidth(&texObj), scaleY, float_1_8041d070);
    GXLoadTexMtxImm(mtx, 0x1E, 1);

    GXSetTevColorIn(0, 4, 6, 8, 0xF);
    GXSetTevAlphaIn(0, 7, 7, 7, 3);

    GXBegin(0x80, 0, 4);
    GXPosition3f32(float_neg285_8041d1c0, float_144_8041d1d0, float_0_8041d068);
    GXTexCoord2f32(float_0_8041d068, float_0_8041d068);
    GXPosition3f32(float_neg285_8041d1c0, float_neg144_8041d1d4, float_0_8041d068);
    GXTexCoord2f32(float_0_8041d068, float_1_8041d070);
    GXPosition3f32(float_285_8041d1cc, float_neg144_8041d1d4, float_0_8041d068);
    GXTexCoord2f32(float_1_8041d070, float_1_8041d070);
    GXPosition3f32(float_285_8041d1cc, float_144_8041d1d0, float_0_8041d068);
    GXTexCoord2f32(float_1_8041d070, float_0_8041d068);
}
