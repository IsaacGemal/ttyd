#include "window/win_mario.h"
#include "window/win_root.h"
#include "driver/animdrv.h"
#include "mario/mario.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "driver/msgdrv.h"
#include "manager/fontmgr.h"
#include "pmario_sound.h"
#include "battle/battle_database_common.h"
#include "battle/battle_seq_end.h"
#include "data/item_icon_view.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

// .sdata2/.sdata objects this unit reads but does not define
// .sdata2 anim names shared with winMarioInit
const char str_M_S_1[6] = "M_S_1";
const char str_8041D580[4] = "S_1";
const char str_M_H_1[6] = "M_H_1";
const char str_R_1[4] = "R_1";
const char str_M_R_1[6] = "M_R_1";
const char str_M_H_4[6] = "M_H_4";
const char str_M_H_7[6] = "M_H_7";

// .rodata / .sdata2 objects this unit reads but does not define yet

// .data table: one entry per window "link" (see linkDt in win_mario.s)
typedef struct LinkDt {
    s16 targetX;          // 0x0
    s16 targetY;          // 0x2
    const char* poseAnim; // 0x4, pose anim when the window is not famicom (flags & 0x2000)
    const char* field_8;  // 0x8
    const char* starPoseAnim; // 0xC, famicom pose anim (compared with "S_1" to pick the S or R set)
    u16 reqItem;          // 0x10, pouch item that must be owned to land here (0 = always)
    u16 field_12;         // 0x12
    const char* helpMsg;  // 0x14
    u8 next[4];           // 0x18, four 1-byte "next" indices, one per d-pad direction
} LinkDt;

extern LinkDt linkDt[];
extern BattleWeapon* superActionTable[];

// .rodata strings, before the code: their jumptable pools land after them
const char str_msg_menu_mario_lv[20] = "msg_menu_mario_lv\x00\x00\x00";
const char str_msg_menu_mario_star_r[24] = "msg_menu_mario_star_r\x00\x00\x00";
const char str_msg_menu_mario_hp[20] = "msg_menu_mario_hp\x00\x00\x00";
const char str_msg_menu_mario_fp[20] = "msg_menu_mario_fp\x00\x00\x00";
const char str_msg_menu_mario_bp[20] = "msg_menu_mario_bp\x00\x00\x00";
const char str_msg_slit[12] = "msg_slit\x00\x00\x00\x00";
const char str_msg_plane[12] = "msg_plane\x00\x00\x00";
const char str_msg_ship[12] = "msg_ship\x00\x00\x00\x00";
const char str_msg_roll[12] = "msg_roll\x00\x00\x00\x00";
const char str_msg_menu_star_power[20] = "msg_menu_star_power\x00";
const char str_msg_menu_mario_waza[20] = "msg_menu_mario_waza\x00";
const char str_msg_menu_mario_star_p[24] = "msg_menu_mario_star_p\x00\x00\x00";
const char str_msg_menu_mario_coin[20] = "msg_menu_mario_coin\x00";
const char str_msg_menu_mario_kakera[24] = "msg_menu_mario_kakera\x00\x00\x00";
const char str_msg_menu_mario_scoin[24] = "msg_menu_mario_scoin\x00\x00\x00\x00";
const char str_msg_menu_mario_time[20] = "msg_menu_mario_time\x00";
const char str_msg_hammer_1[16] = "msg_hammer_1\x00\x00\x00\x00";
const char str_msg_hammer_2[16] = "msg_hammer_2\x00\x00\x00\x00";
const char str_msg_hammer_3[16] = "msg_hammer_3\x00\x00\x00\x00";
const char str_msg_boots_1[12] = "msg_boots_1\x00";
const char str_msg_boots_2[12] = "msg_boots_2\x00";
const char str_msg_boots_3[12] = "msg_boots_3\x00";

// .sdata lookup tables (window "help" message ids per upgrade level)
const char* hammer_help[4] = { NULL, str_msg_hammer_1, str_msg_hammer_2, str_msg_hammer_3 };
const char* boots_help[4] = { NULL, str_msg_boots_1, str_msg_boots_2, str_msg_boots_3 };


void winMarioInit(WindowWork* work) {
    PouchData* pouch = pouchGetPtr();
    const char* anim;
    s32 i;

    if (work->flags & 0x2000) {
        anim = str_8041D580;
        switch (marioGetColor()) {
        case 0:
            anim = str_8041D580;
            break;
        case 1:
            anim = "S_2";
            break;
        case 2:
            anim = "S_3";
            break;
        case 3:
            anim = "S_4";
            break;
        }
        work->marioPose = animPoseEntry("d_mario", 0);
        animPoseSetAnim(work->marioPose, anim, 1);
    } else {
        anim = "a_mario";
        switch (marioGetColor()) {
        case 0:
            anim = "a_mario";
            break;
        case 1:
            anim = "a_mario_l";
            break;
        case 2:
            anim = "a_mario_w";
            break;
        case 3:
            anim = "a_mario_wl";
            break;
        }
        work->marioPose = animPoseEntry(anim, 0);
        animPoseSetAnim(work->marioPose, str_M_S_1, 1);
    }

    animPoseSetMaterialFlagOn(work->marioPose, 0x1800);
    if (marioGetPtr()->flags & 0x40000000) {
        animPoseSetMaterialFlagOn(work->marioPose, 0xA000000);
    }

    work->marioHoldTimer = 0;
    work->marioState = 0;
    work->superActionSel = 0;
    work->superActionCount = 0;

    for (i = 0; i < 8; i++) {
        if (pouch->mStarPowersObtained & (1 << i)) {
            work->superActionCount += 1;
        }
    }
}

void winMarioInit2(WindowWork* work) {
    f32 x;
    f32 y;

    work->marioLinkIdx = 0;
    x = (f32)linkDt[work->marioLinkIdx].targetX;
    work->targetX = x;
    work->targetX = x;
    y = (f32)linkDt[work->marioLinkIdx].targetY;
    work->targetY = y;
    work->targetY = y;

    switch (work->marioLinkIdx) {
    case 2:
        winMsgEntry(work, 0, hammer_help[pouchGetHammerLv()], 0);
        break;
    case 3:
        winMsgEntry(work, 0, boots_help[pouchGetJumpLv()], 0);
        break;
    default:
        winMsgEntry(work, 0, linkDt[work->marioLinkIdx].helpMsg, 0);
        break;
    }

    work->marioHoldTimer = 0;
    work->marioState = 0;
    work->superActionSel = 0;
}

void winMarioExit(WindowWork* work) {
    animPoseRelease(work->marioPose);
}

// The tail walks its own link pointer (`d2`) so MWCC colours it in r30 and the tail
// `anim` in r31, as the target does; the early name is held in `d2` as well (the
// match-only cast is guarded by MUST_MATCH below). The int->float magic still
// pools as a local @NNN@ rather than double_to_int_802f5e78; that symbol name does
// not affect the match score.
s32 winMarioMain(WindowWork* work) {
    LinkDt* d;
    const char* anim;
    LinkDt* d2;
    f32 f2;

    switch (work->marioState) {
    case 0:
        if (work->buttonTrg & 0x100) {
            if (work->marioLinkIdx == 0xc) {
                work->marioState = 1;
                work->superActionSel = 0;
                psndSFXOn((const char*)0x20012);
            }
            goto tail;
        }
#ifdef MUST_MATCH
        // Match shape: the early name must land in the tail pointer's register
        // (r30, `d2`); the plain `anim` form under #else scores 99.90%.
#define SET_ANIM(s) (d2 = (LinkDt*)(s))
#else
#define SET_ANIM(s) (anim = (s))
#endif
        if (work->buttonTrg & 0x200) {
            psndSFXOn((const char*)0x20013);
            if (work->flags & 0x2000) {
                if (strcmp(str_8041D580, str_8041D580) == 0) {
                    switch (marioGetColor()) {
                    case 0:
                        SET_ANIM(str_8041D580);
                        break;
                    case 1:
                        SET_ANIM("S_2");
                        break;
                    case 2:
                        SET_ANIM("S_3");
                        break;
                    case 3:
                        SET_ANIM("S_4");
                        break;
                    }
                } else {
                    switch (marioGetColor()) {
                    case 0:
                        SET_ANIM(str_R_1);
                        break;
                    case 1:
                        SET_ANIM("R_15");
                        break;
                    case 2:
                        SET_ANIM("R_17");
                        break;
                    case 3:
                        SET_ANIM("R_18");
                        break;
                    }
                }
            } else {
                SET_ANIM(str_M_S_1);
            }
#ifdef MUST_MATCH
            animPoseSetAnim(work->marioPose, (const char*)d2, 0);
#else
            animPoseSetAnim(work->marioPose, anim, 0);
#endif
#undef SET_ANIM
            return -1;
        }
        if (work->buttonTrg & 0x1000) {
            return -2;
        }

#define MARIO_TRAVERSE(off)                                                                          \
    do {                                                                                             \
        d = &linkDt[work->marioLinkIdx];                                                             \
        work->marioLinkIdx = d->next[off];                                                           \
        d = &linkDt[work->marioLinkIdx];                                                             \
        while (1) {                                                                                  \
            if (work->marioLinkIdx == 0xb || work->marioLinkIdx == 0xc) {                            \
                if (work->superActionCount != 0) {                                                   \
                    break;                                                                           \
                }                                                                                    \
            } else {                                                                                 \
                if (d->reqItem == 0) {                                                               \
                    break;                                                                           \
                }                                                                                    \
                if ((s32)pouchCheckItem(d->reqItem) != 0) {                                          \
                    break;                                                                           \
                }                                                                                    \
            }                                                                                        \
            work->marioLinkIdx = d->next[off];                                                       \
            d = &linkDt[work->marioLinkIdx];                                                         \
        }                                                                                            \
    } while (0)

        if (work->dirRep & 0x1000) {
            MARIO_TRAVERSE(0);
            psndSFXOn((const char*)0x20005);
            goto tail;
        }
        if (work->dirRep & 0x2000) {
            MARIO_TRAVERSE(1);
            psndSFXOn((const char*)0x20005);
            goto tail;
        }
        if (work->dirRep & 0x4000) {
            MARIO_TRAVERSE(2);
            psndSFXOn((const char*)0x20005);
            goto tail;
        }
        if (work->dirRep & 0x8000) {
            MARIO_TRAVERSE(3);
            psndSFXOn((const char*)0x20005);
        }

#undef MARIO_TRAVERSE

    tail:
        d2 = &linkDt[work->marioLinkIdx];
        work->targetX = (f32)d2->targetX;
        work->targetY = (f32)d2->targetY;

        switch (work->marioLinkIdx) {
        case 2:
            if (work->flags & 0x2000) {
                if (strcmp(d2->starPoseAnim, str_8041D580) == 0) {
                    switch (marioGetColor()) {
                    case 0:
                        anim = str_8041D580;
                        break;
                    case 1:
                        anim = "S_2";
                        break;
                    case 2:
                        anim = "S_3";
                        break;
                    case 3:
                        anim = "S_4";
                        break;
                    }
                } else {
                    switch (marioGetColor()) {
                    case 0:
                        anim = str_R_1;
                        break;
                    case 1:
                        anim = "R_15";
                        break;
                    case 2:
                        anim = "R_17";
                        break;
                    case 3:
                        anim = "R_18";
                        break;
                    }
                }
            } else {
                anim = hammer_pose[pouchGetHammerLv()];
            }

            animPoseSetAnim(work->marioPose, anim, 0);
            if (work->marioHoldTimer == 0) {
                if (animPoseGetLoopTimes(work->marioPose) >= 0.89f) {
                    work->marioHoldTimer = 30;
                }
            } else {
                work->marioHoldTimer -= 1;
            }
            if (work->flags & 0x2000) {
                work->marioHoldTimer = 0;
            }
                    break;
        default:
            work->marioHoldTimer = 0;
            if (work->flags & 0x2000) {
                if (strcmp(d2->starPoseAnim, str_8041D580) == 0) {
                    switch (marioGetColor()) {
                    case 0:
                        anim = str_8041D580;
                        break;
                    case 1:
                        anim = "S_2";
                        break;
                    case 2:
                        anim = "S_3";
                        break;
                    case 3:
                        anim = "S_4";
                        break;
                    }
                } else {
                    switch (marioGetColor()) {
                    case 0:
                        anim = str_R_1;
                        break;
                    case 1:
                        anim = "R_15";
                        break;
                    case 2:
                        anim = "R_17";
                        break;
                    case 3:
                        anim = "R_18";
                        break;
                    }
                }
            } else {
                anim = d2->poseAnim;
            }
            animPoseSetAnim(work->marioPose, anim, 0);
                    break;
        }

        switch (work->marioLinkIdx) {
        case 2:
            winMsgEntry(work, 0, hammer_help[pouchGetHammerLv()], 0);
            break;
        case 3:
            winMsgEntry(work, 0, boots_help[pouchGetJumpLv()], 0);
            break;
        default:
            winMsgEntry(work, 0, d2->helpMsg, 0);
            break;
        }
        break;
    case 1:
        if (work->dirRep & 0x1000) {
            work->superActionSel -= 1;
            if (work->superActionSel < 0) {
                work->superActionSel = work->superActionCount - 1;
            }
            psndSFXOn((const char*)0x20005);
        } else if (work->dirRep & 0x2000) {
            work->superActionSel += 1;
            if (work->superActionSel >= work->superActionCount) {
                work->superActionSel = 0;
            }
            psndSFXOn((const char*)0x20005);
        } else if (work->buttonTrg & 0x200) {
            work->marioState = 0;
            psndSFXOn((const char*)0x20013);
        } else if (work->buttonTrg & 0x1000) {
            return -2;
        }
        work->targetX = -280.0f;
        f2 = (f32)work->superActionSel;
        work->targetY = 10.0f + ((120.0f - 26.0f * f2) - 12.0f);
        winMsgEntry(work, 0, superActionTable[work->superActionSel]->description, 0);
        break;
    }
    return 0;
}

// .sdata2 colour constants (referenced by fukidashi/winMarioDisp)

void winMarioMain2(WindowWork* work) {
    (void)work;
}

char* winZenkakuStr(s32 num) {
    signed char buf[0x100];
    const char* tbl[10] = {
        "\x82\x4F", "\x82\x50", "\x82\x51", "\x82\x52", "\x82\x53",
        "\x82\x54", "\x82\x55", "\x82\x56", "\x82\x57", "\x82\x58",
    };
    static char tmp2[0x100];
    u32 i;

    if (num < 0) {
        strcpy(tmp2, "\x81\x7C");
        num = -num;
    } else {
        strcpy(tmp2, "");
    }
    sprintf((char*)buf, "%d", num);
    for (i = 0; i < strlen((char*)buf); i++) {
        strcat(tmp2, tbl[buf[i] - 0x30]);
    }
    return tmp2;
}

char* winZenkakuPadStr(s32 num, s32 width) {
    signed char buf[0x100];
    const char* tbl[10] = {
        "\x82\x4F", "\x82\x50", "\x82\x51", "\x82\x52", "\x82\x53",
        "\x82\x54", "\x82\x55", "\x82\x56", "\x82\x57", "\x82\x58",
    };
    static char tmp[0x100];
    s32 i;

    if (num < 0) {
        strcpy(tmp, "\x81\x7C");
        num = -num;
    } else {
        strcpy(tmp, "");
    }
    sprintf((char*)buf, "%d", num);
    for (i = 0; i < width; i++) {
        if (i < width - strlen((char*)buf)) {
            strcat(tmp, "\x82\x4F");
        } else {
            strcat(tmp, tbl[buf[i - (width - (s32)strlen((char*)buf))] - 0x30]);
        }
    }
    return tmp;
}

const GXColor fukiShadowColor = { 0x00, 0x00, 0x00, 0x80 };
const GXColor fukiBubbleColor = { 0xE1, 0xE1, 0xE1, 0xFE };
const GXColor fukiIconColor = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor fukiShadowColorR = { 0x00, 0x00, 0x00, 0x80 };
const GXColor fukiBubbleColorR = { 0xE1, 0xE1, 0xE1, 0xFE };
const GXColor fukiIconColorR = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor winWhiteColor = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor winBlackColor = { 0x00, 0x00, 0x00, 0xFF };

// The window draw functions take their Vec and GXColor arguments by value (win_main.h
// declares pointers, which compiles to the same callee).
#ifdef MUST_MATCH
// Match-only: called through by-value casts (caller copies plus a pointer, ABI-identical).
#define winFontSetV ((void (*)(Vec, Vec, GXColor, const char*, ...))winFontSet)
#define winFontSetRV ((void (*)(Vec, Vec, GXColor, const char*, ...))winFontSetR)
#define winFontSetEdgeV ((void (*)(Vec, Vec, GXColor, const char*, ...))winFontSetEdge)
#define winFontSetLabelV ((void (*)(Vec, Vec, GXColor, const char*))winFontSetLabel)
#define winIconSetV ((void (*)(s32, Vec, Vec, GXColor))winIconSet)
#else
static void winMarioFontV(void (*fn)(Vec*, Vec*, GXColor*, const char*, ...), Vec pos, Vec scale,
                          GXColor color, const char* format, va_list args) {
    char text[0x200];

    vsprintf(text, format, args);
    fn(&pos, &scale, &color, "%s", text);
}
static void winFontSetV(Vec pos, Vec scale, GXColor color, const char* format, ...) {
    va_list args;

    va_start(args, format);
    winMarioFontV(winFontSet, pos, scale, color, format, args);
    va_end(args);
}
static void winFontSetRV(Vec pos, Vec scale, GXColor color, const char* format, ...) {
    va_list args;

    va_start(args, format);
    winMarioFontV(winFontSetR, pos, scale, color, format, args);
    va_end(args);
}
static void winFontSetEdgeV(Vec pos, Vec scale, GXColor color, const char* format, ...) {
    va_list args;

    va_start(args, format);
    winMarioFontV(winFontSetEdge, pos, scale, color, format, args);
    va_end(args);
}
static inline void winFontSetLabelV(Vec pos, Vec scale, GXColor color, const char* tag) {
    winFontSetLabel(&pos, &scale, &color, tag);
}
static inline void winIconSetV(s32 id, Vec pos, Vec scale, GXColor color) {
    winIconSet(id, &pos, &scale, &color);
}
#endif

// One speech bubble: the bubble texture twice (shadow, then body) and the item icon.
// fukiDrawL is the bubble left of the item (Mail/Map/... key items), fukiDrawR the
// mirrored one for the hammer and boots.
static inline void fukiDrawL(u16 iconId, f32 x, f32 y, f32 dx) {
    winTexSet(0xB1, (Vec){(x - dx) + 5.0f, y - 5.0f, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, fukiShadowColor);
    winTexSet(0xB1, (Vec){x - dx, y, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, fukiBubbleColor);
    winIconInit();
    winIconSetV(iconId, (Vec){(x - dx) + 6.0f, y, 0.0f}, (Vec){0.7f, 0.7f, 0.7f}, fukiIconColor);
}

static inline void fukiDrawR(u16 iconId, f32 x, f32 y, f32 dx) {
    winTexSet(0xB1, (Vec){(x - dx) + 5.0f, y - 5.0f, 0.0f}, (Vec){-0.8f, 0.8f, 0.8f}, fukiShadowColorR);
    winTexSet(0xB1, (Vec){x - dx, y, 0.0f}, (Vec){-0.8f, 0.8f, 0.8f}, fukiBubbleColorR);
    winIconInit();
    winIconSetV(iconId, (Vec){(x - dx) - 6.0f, y, 0.0f}, (Vec){0.7f, 0.7f, 0.7f}, fukiIconColorR);
}

void fukidashi(WindowWork* work, u32 type, f32 x, f32 y) {
    switch (type) {
    case 2:
        if (pouchGetHammerLv() > 0) {
            u16 iconId = itemDataTable[pouchGetHammerLv() + 8].icon_id;
            winTexInit(*(TEXPalette**)work->winTpl->data);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
            fukiDrawR(iconId, x, 72.0f + y, 230.0f);
        }
        break;
    case 3:
        if (pouchGetJumpLv() > 0) {
            u16 iconId = itemDataTable[pouchGetJumpLv() + 5].icon_id;
            winTexInit(*(TEXPalette**)work->winTpl->data);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
            fukiDrawR(iconId, x, 20.0f + y, 210.0f);
        }
        break;
    case 7:
        if ((s32)pouchCheckItem(2) != 0) {
            u16 iconId = itemDataTable[2].icon_id;
            winTexInit(*(TEXPalette**)work->winTpl->data);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
            fukiDrawL(iconId, x, 90.0f + y, 80.0f);
        }
        break;
    case 8:
        if ((s32)pouchCheckItem(4) != 0) {
            u16 iconId = itemDataTable[4].icon_id;
            winTexInit(*(TEXPalette**)work->winTpl->data);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
            fukiDrawL(iconId, x, 20.0f + y, 80.0f);
        }
        break;
    case 9:
        if ((s32)pouchCheckItem(5) != 0) {
            u16 iconId = itemDataTable[5].icon_id;
            winTexInit(*(TEXPalette**)work->winTpl->data);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
            fukiDrawL(iconId, x, 130.0f + y, 30.0f);
        }
        break;
    case 10:
        if ((s32)pouchCheckItem(3) != 0) {
            u16 iconId = itemDataTable[3].icon_id;
            winTexInit(*(TEXPalette**)work->winTpl->data);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
            fukiDrawL(iconId, x, 60.0f + y, 30.0f);
        }
        break;
    default:
        break;
    }
}

const char* hammer_pose[4] = { NULL, str_M_H_1, str_M_H_4, str_M_H_7 };

#define POSE (work->marioPose)

void winMarioDisp(CameraId camId, void* param, s32 idx) {
    WindowWork* work = param;
    // Draws the Mario status panel: name/level/rank, HP/FP/BP, star pieces and
    // shine sprites, the play-time clock and the collected star powers.
    PouchData* pouch = pouchGetPtr();
    static s32 pri[6] = { 3, 2, 8, 10, 7, 9 };
    GXColor unusedWhite, unusedBlack, loopWhite, loopBlack, white, black;
    f32 spRowY, hpIconY, spIconX, rowTexX, headerY, rowX, spIconY, fpRowY, bpRowY, listLabelX;
    f32 listIconX, statX, maxX, colonX, fpIconY, fpTextY, statIconX, listTop, hpTextY, rawW;
    f32 listLeft, nameX, timeRowY, x, coinRowY, y, listValueX, slashX, spLabelY, pieceRowY;
    f32 shineRowY, halfTagW, wazaTitleX;
    s32 i;
    (void)camId;

    unusedWhite = winWhiteColor;
    white = winWhiteColor;
    loopWhite = winWhiteColor;
    unusedBlack = winBlackColor;
    black = winBlackColor;
    loopBlack = winBlackColor;
    x = ((f32*)((u8*)work + 0xC4))[idx * 5];
    y = ((f32*)((u8*)work + 0xC8))[idx * 5];

    winBgGX(work, 0, x, y);
    winNameGX(work, 2, x - 265.0f, 154.0f + y, 190.0f, 24.0f);
    nameX = 10.0f + (x - 265.0f);
    winFontInit();
    headerY = 152.0f + y;
    winFontSetEdgeV((Vec){nameX, headerY, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, white, msgSearch("name_mario"));
    nameX += 0.8f * (f32)(u16)FontGetMessageWidth(msgSearch("name_mario"));
    nameX += 20.0f;
    winIconInit();
    winIconSetV(0x19E, (Vec){nameX, 142.0f + y, 0.0f}, (Vec){0.6f, 0.6f, 0.6f}, white);
    nameX += 20.0f;
    winFontInit();
    winFontSetEdgeV((Vec){nameX, headerY, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, white, msgSearch("msg_menu_mario_level"));
    {
        char buf[0x100];
        sprintf(buf, "%s", winZenkakuStr(pouch->level));
        winFontInit();
        winFontSetEdgeV((Vec){(x - 265.0f + 190.0f)
                - 0.8f * (f32)(u16)FontGetMessageWidth(buf)
                - 10.0f, headerY, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, white, buf);
    }
    winTexInit(*(TEXPalette**)work->winTpl->data);
    winTexSet(0xB0, (Vec){x - 255.0f, 112.0f + y, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, white);
    winFontInit();
    winFontSetV((Vec){x - 240.0f, 124.0f + y, 0.0f}, (Vec){0.7f, 0.7f, 0.7f}, black, msgSearch(BattleGetRankNameLabel(pouch->level)));

    for (i = 0; i < 6; i++) {
        if (work->marioLinkIdx != pri[i]) {
            fukidashi(work, pri[i], x, y);
        }
    }
    fukidashi(work, work->marioLinkIdx, x, y);

    winKirinukiGX(work, 0, x - 270.0f, y - 10.0f,
                  264.0f, 110.0f);

    rowX = 50.0f + (x - 270.0f);
    rowTexX = rowX - 2.0f;
    for (i = 0; i < 3; i++) {
        GXColor colTbl[3] = { { 0xCB, 0x8C, 0xA2, 0xFE }, { 0xCB, 0xBE, 0x89, 0xFE }, { 0x94, 0xB9, 0xC7, 0xFE } };
        GXColor hiliteTbl[3] = { { 0xA7, 0x3C, 0x3C, 0xFF }, { 0x37, 0x69, 0x1E, 0xFF }, { 0x0F, 0x21, 0x7A, 0xFF } };
        const char* rawTbl[3] = { "\x82g\x82o", "\x82""e\x82o", "\x82""a\x82o" };
        const char* tagTbl[3] = { "msg_menu_mario_ruby_hp", "msg_menu_mario_ruby_fp", "msg_menu_mario_ruby_bp" };
        rawW = (f32)(u16)FontGetMessageWidth(rawTbl[i]);
        halfTagW = 0.5f * (f32)(u16)FontGetMessageWidth(msgSearch(tagTbl[i]));
        winTexInit(*(TEXPalette**)work->winTpl->data);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
        winTexSet(0xAF, (Vec){rowTexX, ((y - 38.0f) - (f32)(i * 34)) - 4.0f, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, colTbl[i]);
        winFontInit();
        winFontSetV((Vec){rowX - rawW / 2.0f, 12.0f + ((y - 38.0f) - (f32)(i * 34)), 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, loopBlack, rawTbl[i]);
        winFontSetV((Vec){rowX - halfTagW / 2.0f, 21.0f + ((y - 38.0f) - (f32)(i * 34)), 0.0f}, (Vec){0.5f, 0.5f, 0.5f}, hiliteTbl[i], msgSearch(tagTbl[i]));
    }
    winIconInit();
    statIconX = 110.0f + (x - 270.0f);
    hpIconY = 4.0f + (y - 38.0f);
    winIconSetV(0x19F, (Vec){statIconX, hpIconY, 0.0f}, (Vec){0.82f, 0.82f, 0.82f}, white);
    fpRowY = (y - 38.0f) - 34.0f;
    fpIconY = 4.0f + fpRowY;
    winIconSetV(0x1A0, (Vec){statIconX, fpIconY, 0.0f}, (Vec){0.82f, 0.82f, 0.82f}, white);
    bpRowY = (y - 38.0f) - 68.0f;
    winIconSetV(0x1A1, (Vec){statIconX, 4.0f + bpRowY, 0.0f}, (Vec){0.82f, 0.82f, 0.82f}, white);
    winFontInit();
    statX = 130.0f + (x - 270.0f);
    hpTextY = 4.0f + (12.0f + (y - 38.0f));
    winFontSetRV((Vec){statX, hpTextY, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, black, "%s", winZenkakuStr(pouch->currentHP));
    fpTextY = 4.0f + (12.0f + fpRowY);
    winFontSetRV((Vec){statX, fpTextY, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, black, "%s", winZenkakuStr(pouch->currentFP));
    winFontSetRV((Vec){statX, 4.0f + (12.0f + bpRowY), 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, black, "%s", winZenkakuStr(pouch->maximumBP));
    winTexInit(*(TEXPalette**)work->winTpl->data);
    slashX = 190.0f + (x - 270.0f);
    winTexSet(0x10, (Vec){slashX, hpIconY, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, white);
    winTexSet(0x10, (Vec){slashX, fpIconY, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, white);
    winFontInit();
    maxX = 200.0f + (x - 270.0f);
    winFontSetV((Vec){maxX, hpTextY, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, black, "%s", winZenkakuStr(pouch->maximumHP));
    winFontSetV((Vec){maxX, fpTextY, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, black, "%s", winZenkakuStr(pouch->maximumFP));

    if (work->superActionCount > 0) {
        winKirinukiGX(work, 0, 10.0f + x,
                      (20.0f + (130.0f + y)) - 20.0f,
                      260.0f, 80.0f);
        winIconInit();
        spIconX = 10.0f + (40.0f + x);
        spIconY = 106.0f + y;
        winIconSetV(0x20F, (Vec){spIconX, spIconY, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, white);
        spLabelY = 70.0f + y;
        winIconSetV(0x197, (Vec){spIconX, spLabelY, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, white);
        winFontInit();
        winFontSetV((Vec){10.0f + (70.0f + x), 10.0f + spLabelY, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, black, msgSearch("msg_menu_mario_special"));
        winIconInit();
        for (i = 0; i < work->superActionCount; i++) {
            if (i < pouch->currentSP / 100) {
                winIconSetV(0x1B7 + i, (Vec){(10.0f + (80.0f + x)) + (f32)(i * 22), spIconY, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, loopWhite);
            } else {
                winIconSetV(0x1BF, (Vec){(10.0f + (80.0f + x)) + (f32)(i * 22), spIconY, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, loopWhite);
            }
        }
    }

    listLeft = 10.0f + x;
    winKirinukiGX(work, 0, listLeft,
                  10.0f + (20.0f + y),
                  260.0f, 150.0f);
    winIconInit();
    listIconX = 10.0f + (20.0f + x);
    winIconSetV(0x18C, (Vec){listIconX, 10.0f + y, 0.0f}, (Vec){0.62f, 0.62f, 0.62f}, white);
    winIconSetV(0x18B, (Vec){listIconX, 10.0f + (y - 28.0f), 0.0f}, (Vec){0.62f, 0.62f, 0.62f}, white);
    winIconSetV(0x18D, (Vec){listIconX, 10.0f + (y - 56.0f), 0.0f}, (Vec){0.62f, 0.62f, 0.62f}, white);
    winIconSetV(0x102, (Vec){listIconX, 10.0f + (y - 84.0f), 0.0f}, (Vec){0.62f, 0.62f, 0.62f}, white);
    winIconSetV(0x1A2, (Vec){listIconX, 10.0f + (y - 112.0f), 0.0f}, (Vec){0.62f, 0.62f, 0.62f}, white);

    winFontInit();
    listLabelX = 10.0f + (35.0f + x);
    listTop = 12.0f + y;
    spRowY = 10.0f + listTop;
    winFontSetV((Vec){listLabelX, spRowY, 0.0f}, (Vec){0.9f, 0.9f, 0.9f}, black, msgSearch("msg_menu_mario_num_sp"));
    coinRowY = 10.0f + (listTop - 28.0f);
    winFontSetV((Vec){listLabelX, coinRowY, 0.0f}, (Vec){0.9f, 0.9f, 0.9f}, black, msgSearch("msg_menu_mario_num_coin"));
    pieceRowY = 10.0f + (listTop - 56.0f);
    winFontSetV((Vec){listLabelX, pieceRowY, 0.0f}, (Vec){0.9f, 0.9f, 0.9f}, black, msgSearch("msg_menu_mario_num_hoshi"));
    shineRowY = 10.0f + (listTop - 84.0f);
    winFontSetV((Vec){listLabelX, shineRowY, 0.0f}, (Vec){0.9f, 0.9f, 0.9f}, black, msgSearch("msg_menu_mario_num_super_coin"));
    timeRowY = 10.0f + (listTop - 112.0f);
    winFontSetV((Vec){listLabelX, timeRowY, 0.0f}, (Vec){0.9f, 0.9f, 0.9f}, black, msgSearch("msg_menu_mario_num_playtime"));

    listValueX = 2.0f + (8.0f + (205.0f + x));
    winFontSetRV((Vec){listValueX, spRowY, 0.0f}, (Vec){0.9f, 0.9f, 0.9f}, black, "%s", winZenkakuStr(pouch->mStarPoints));
    winFontSetRV((Vec){listValueX, coinRowY, 0.0f}, (Vec){0.9f, 0.9f, 0.9f}, black, "%s", winZenkakuStr(pouch->coins));
    winFontSetRV((Vec){listValueX, pieceRowY, 0.0f}, (Vec){0.9f, 0.9f, 0.9f}, black, "%s", winZenkakuStr(pouch->mStarPieceCount));
    winFontSetRV((Vec){listValueX, shineRowY, 0.0f}, (Vec){0.9f, 0.9f, 0.9f}, black, "%s", winZenkakuStr(pouch->mShineSpriteCount));

    {
        s64 secs;
        u32 hours;
        u32 mins;
        secs = gp->retraceLocalTime / (s64)(*(volatile u32*)0x800000F8 >> 2);
        if ((u64)secs > 359999) {
            hours = 99;
            mins = 59;
        } else {
            u64 rest = secs;
            hours = rest / 3600;
            rest %= 3600;
            mins = rest / 60;
        }
        winFontSetRV((Vec){2.0f + (2.0f + ((8.0f + (172.0f + x)) - 16.0f)), timeRowY, 0.0f}, (Vec){0.9f, 0.9f, 0.9f}, black, "%s", winZenkakuPadStr(hours, 2));
        winFontSetRV((Vec){listValueX, timeRowY, 0.0f}, (Vec){0.9f, 0.9f, 0.9f}, black, "%s", winZenkakuPadStr(mins, 2));
    }

    winTexInit(*(TEXPalette**)work->winTpl->data);
    for (i = 0; i < 4; i++) {
        winTexSet(0x11, (Vec){10.0f + (190.0f + x), 10.0f + (y - (f32)(i * 28)), 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, loopWhite);
    }
    colonX = 220.0f + x;
    winTexSet(0x12, (Vec){colonX, (10.0f + (2.0f + (y - 112.0f))) - 2.0f, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, white);

    {
        AnimData* anim;
        void* p;
        f32 frac;
        animPoseGetAnimPosePtr(POSE);
        anim = animPoseGetAnimDataPtr(POSE);
        if (work->marioHoldTimer == 0) {
            p = (void*)anim->clip;
            frac = (f32)(gp->retraceCount % (s32)*(f32*)((u8*)p + 8));
            animPoseSetLocalTime(POSE, frac);
        }
        animPoseMain(POSE);
        winKageGX(work, x - 150.0f, y, 0.0f, 1.0f, &(GXColor){0x00, 0x00, 0x00, 0x80});
        {
            Mtx mtx;
            PSMTXTrans(mtx, x - 150.0f, y, 0.0f);
            winZClear();
            animPoseDrawMtx(POSE, mtx, 2, 0.0f, 5.0f);
        }
    }

    if (work->marioState != 0) {
        winWazaGX(work, 0, x - 270.0f, 160.0f,
                  270.0f, 254.0f);
        winFontInit();
        wazaTitleX = 90.0f + x - 270.0f;
        winFontSetEdgeV((Vec){wazaTitleX, 155.0f, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, white, msgSearch("msg_menu_mario_sac_waza"));
        winFontSetEdgeV((Vec){colonX - 270.0f, 155.0f, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, white, "\x82r\x82o");
        for (i = 0; i < work->superActionCount; i++) {
            char buf[0x20];
            f32 drop;
            winFontSetLabelV((Vec){10.0f + (listLeft - 270.0f), (f32)(0x82 - i * 26), 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, loopBlack, superActionTable[i]->name);
            sprintf(buf, "%s", winZenkakuStr(superActionTable[i]->baseSpCost));
            drop = (f32)(0x82 - i * 26);
            winFontSetV((Vec){(228.0f + x - 270.0f) - (f32)((u16)FontGetMessageWidth(buf) >> 1), drop, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, loopBlack, buf);
            winFontSetV((Vec){4.0f + (17.0f + colonX - 270.0f), (f32)(0x78 - i * 26), 0.0f}, (Vec){0.5f, 0.5f, 0.5f}, loopBlack, "\x82r\x82o");
        }
    }

#undef POSE
}


// ---- named data, defined after the code so MWCC does not fold it ----
#pragma explicit_zero_data on
#pragma force_active on

// .data: link table
LinkDt linkDt[18] = {
    { (s16)0xFEF4, (s16)0x008E, str_M_S_1, NULL, str_8041D580, 0x0000, 0x0000, str_msg_menu_mario_lv, { 0x06, 0x01, 0x0B, 0x09 } },
    { (s16)0xFEF4, (s16)0x0076, str_M_S_1, NULL, str_8041D580, 0x0000, 0x0000, str_msg_menu_mario_star_r, { 0x00, 0x02, 0x0B, 0x07 } },
    { (s16)0xFEF0, (s16)0x0044, str_M_H_1, NULL, str_R_1, 0x0009, 0x0000, NULL, { 0x01, 0x03, 0x0B, 0x07 } },
    { (s16)0xFF0E, (s16)0x0018, str_M_R_1, NULL, str_R_1, 0x0006, 0x0000, NULL, { 0x02, 0x04, 0x0B, 0x08 } },
    { (s16)0xFEF2, (s16)0xFFDE, str_M_S_1, NULL, str_8041D580, 0x0000, 0x0000, str_msg_menu_mario_hp, { 0x03, 0x05, 0x0D, 0x0D } },
    { (s16)0xFEF2, (s16)0xFFBC, str_M_S_1, NULL, str_8041D580, 0x0000, 0x0000, str_msg_menu_mario_fp, { 0x04, 0x06, 0x0D, 0x0D } },
    { (s16)0xFEF2, (s16)0xFF9A, str_M_S_1, NULL, str_8041D580, 0x0000, 0x0000, str_msg_menu_mario_bp, { 0x05, 0x00, 0x0D, 0x0D } },
    { (s16)0xFF9C, (s16)0x005A, str_M_S_1, str_M_S_1, str_8041D580, 0x0002, 0x0000, str_msg_slit, { 0x09, 0x0A, 0x02, 0x09 } },
    { (s16)0xFF9C, (s16)0x0014, str_M_S_1, str_M_S_1, str_8041D580, 0x0004, 0x0000, str_msg_plane, { 0x0A, 0x04, 0x03, 0x0A } },
    { (s16)0xFFCE, (s16)0x0082, str_M_S_1, str_M_S_1, str_8041D580, 0x0005, 0x0000, str_msg_ship, { 0x00, 0x07, 0x00, 0x0B } },
    { (s16)0xFFCE, (s16)0x003C, str_M_S_1, str_M_S_1, str_8041D580, 0x0003, 0x0000, str_msg_roll, { 0x07, 0x08, 0x08, 0x0C } },
    { (s16)0x0014, (s16)0x006A, str_M_S_1, NULL, str_8041D580, 0x0000, 0x0000, str_msg_menu_star_power, { 0x11, 0x0C, 0x09, 0x00 } },
    { (s16)0x0014, (s16)0x0046, str_M_S_1, NULL, str_8041D580, 0x0000, 0x0000, str_msg_menu_mario_waza, { 0x0B, 0x0D, 0x0A, 0x00 } },
    { (s16)0x0003, (s16)0x000A, str_M_S_1, NULL, str_8041D580, 0x0000, 0x0000, str_msg_menu_mario_star_p, { 0x0C, 0x0E, 0x04, 0x04 } },
    { (s16)0x0003, (s16)0xFFEE, str_M_S_1, NULL, str_8041D580, 0x0000, 0x0000, str_msg_menu_mario_coin, { 0x0D, 0x0F, 0x04, 0x04 } },
    { (s16)0x0003, (s16)0xFFD2, str_M_S_1, NULL, str_8041D580, 0x0000, 0x0000, str_msg_menu_mario_kakera, { 0x0E, 0x10, 0x04, 0x04 } },
    { (s16)0x0003, (s16)0xFFB6, str_M_S_1, NULL, str_8041D580, 0x0000, 0x0000, str_msg_menu_mario_scoin, { 0x0F, 0x11, 0x04, 0x04 } },
    { (s16)0x0003, (s16)0xFF9A, str_M_S_1, NULL, str_8041D580, 0x0000, 0x0000, str_msg_menu_mario_time, { 0x10, 0x0B, 0x04, 0x04 } },
};

#pragma force_active off
#pragma explicit_zero_data reset
