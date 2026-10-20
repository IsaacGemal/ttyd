#include "window/win_badge.h"
#include "window/win_root.h"
#include "window/win_mario.h"
#include "window/win_main.h"
#include "win/win_main.h"
#include "mario/mario_pouch.h"
#include "driver/animdrv.h"
#include "driver/msgdrv.h"
#include "manager/fontmgr.h"
#include "pmario_sound.h"
#include "data/item_data.h"
#include <stdio.h>
#include <stdarg.h>

// Extended WindowWork fields used by this unit. win_main.h only declares the
// first 0x40 bytes, so the rest are reached through raw offsets.
#define WW_S32(w, off) (*(s32*)((u8*)(w) + (off)))
#define WW_F32(w, off) (*(f32*)((u8*)(w) + (off)))
// Indexed float-field read: the plain WW_F32 macro groups the constant first and MWCC folds 0x3F4
// into the lfs displacement; retail emits `slwi; addi r0, idx*4, 0x3F4; lfsx f, work, r0`.
#define WW_F32X(w, idx, off) (((f32*)((u8*)(w) + ((idx) << 2) + (off)))[0])
// win texture ids of the BP pips
typedef enum {
    WIN_TEX_BP_ON = 0x1C,
    WIN_TEX_BP_OFF = 0x1D,
} WinBadgeTex;

// .sdata2: the colours the two draw functions use
const GXColor dat_8041d348 = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor dat_8041d34c = { 0x00, 0x00, 0x00, 0xFF };
const GXColor dat_8041d350 = { 0x00, 0x00, 0x00, 0x80 };
const GXColor dat_8041d354 = { 0x80, 0x80, 0x80, 0xFF };
const GXColor dat_8041d358 = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor dat_8041d35c = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor dat_8041d360 = { 0x7F, 0x7F, 0x7F, 0xFF };
const GXColor dat_8041d364 = { 0x00, 0x00, 0x00, 0xFF };

extern ItemData itemDataTable[];

// 288 B
void winMakeEquipList(WindowWork* work) {
    s32 count = 0;
    PouchData* pouch = pouchGetPtr();
    u8* entry = (u8*)work;
    s32 i;

    for (i = 0; i < MAX_BADGES; i++) {
        if (pouch->equippedBadges[i] != 0) {
            *(s32*)(entry + 0x404) = 1;
            count++;
            *(s32*)(entry + 0x408) = i;
            *(s32*)(entry + 0x40C) = pouch->equippedBadges[i];
            entry += 0xC;
        }
    }
    work->equipListCount = count;
}

// 312 B
void winBadgeInit(WindowWork* work) {
    work->badgeState = 0;
    work->badgeTab = 0;
    work->badgeSel[0] = 0;
    work->badgeScrollRow[0] = 0;
    work->badgeSel[1] = 0;
    work->badgeScrollRow[1] = 0;
    winMakeEquipList(work);
}

// 464 B
void winBadgeInit2(WindowWork* work) {
    work->targetX = -270.0f;
    work->targetY = (f32)(0x7D - work->badgeTab * 0x28);
    work->badgeScrollPos[0] = work->badgeScrollTarget[0] = 184.0f * (f32)work->badgeScrollRow[0];
    work->badgeScrollPos[1] = work->badgeScrollTarget[1] = 184.0f * (f32)work->badgeScrollRow[1];
    winMsgEntry(work, 0, work->badgeTab == 0 ? "msg_menu_badge_all" : "msg_menu_badge_equip", 0);
    winMakeEquipList(work);
}

// 4 B
void winBadgeExit(void) {
}

// 516 B
void mario_change(WindowWork* work, s32 badgeId) {
    const char* anim;
    const char* s1;
    const char* s2;
    s32 badge147;
    s32 badge148;

    badge147 = pouchEquipCheckBadge(0x147);
    badge148 = pouchEquipCheckBadge(0x148);

    if (work->flags & 0x2000) {
        if (badge147 && !badge148) {
            s1 = "S_2";
            s2 = "W_15";
        } else if (!badge147 && badge148) {
            s1 = "S_3";
            s2 = "W_17";
        } else if (badge147 && badge148) {
            s1 = "S_4";
            s2 = "W_18";
        } else {
            s1 = "S_1";
            s2 = "W_1";
        }
        if (work->poseId != -1) {
            animPoseRelease(work->poseId);
            work->poseId = animPoseEntry("d_mario", 0);
            animPoseSetAnim(work->poseId, s1, 1);
            animPoseSetMaterialFlagOn(work->poseId, 0x1800);
        }
        if (work->poseId2 != -1) {
            animPoseRelease(work->poseId2);
            work->poseId2 = animPoseEntry("d_mario", 0);
            animPoseSetAnim(work->poseId2, s2, 1);
            animPoseSetMaterialFlagOn(work->poseId2, 0x1800);
        }
    } else {
        if (badge147 && !badge148) {
            anim = "a_mario_l";
        } else if (!badge147 && badge148) {
            anim = "a_mario_w";
        } else if (badge147 && badge148) {
            anim = "a_mario_wl";
        } else {
            anim = "a_mario";
        }
        if (work->poseId != -1) {
            animPoseRelease(work->poseId);
            work->poseId = animPoseEntry(anim, 0);
            animPoseSetAnim(work->poseId, "M_S_1", 1);
            animPoseSetMaterialFlagOn(work->poseId, 0x1800);
        }
        if (work->poseId2 != -1) {
            animPoseRelease(work->poseId2);
            work->poseId2 = animPoseEntry(anim, 0);
            animPoseSetAnim(work->poseId2, "M_W_1", 1);
            animPoseSetMaterialFlagOn(work->poseId2, 0x1800);
        }
    }
}

// 1752 B
// 100%. Two shapes are needed to reproduce the original's register allocation/scheduling:
//   - the condition is written inline (`itemDataTable[pouchHaveBadge(idx)].bp_cost`) so the `lwz`/`cmpw`
//     land availableBP in r0 and bp_cost in r3 instead of r4/r0.
//   - the sort key's 0x3E4 pointer is held in a named `selPtr`, which puts `sel` in r28 (not r27).
// The `void* arg0` copy is a scheduling aid only (see MUST_MATCH below): it materialises `work` into r3
// before the `if`, so the `mr r3, r31` for winSortEntry is not sunk into the `lfs` delay slot.
s32 winBadgeMain(WindowWork* work) {
    PouchData* pouch = pouchGetPtr();
    s32 sel;
    s32 count;

    switch (work->badgeState) {
    case 0:
        if (work->buttonTrg & 0x200) {
            psndSFXOn((const char*)0x20013);
            return -1;
        }
        if (work->buttonTrg & 0x1000) {
            return -2;
        }
        if (work->dirRep & 0x3000) {
            work->badgeTab = 1 - work->badgeTab;
            psndSFXOn((const char*)0x20005);
            if (work->badgeTab != 0) {
                winMakeEquipList(work);
            }
        } else if (work->buttonTrg & 0x100) {
            if (work->badgeTab == 0) {
                if (pouchGetHaveBadgeCnt() == 0) {
                    goto end;
                }
            }
            if (work->badgeTab == 1) {
                if (work->equipListCount == 0) {
                    goto end;
                }
            }
            psndSFXOn((const char*)0x20012);
            work->badgeState = 10;
            goto end;
        } else if (work->buttonTrg & 0x10) {
#ifdef MUST_MATCH
            void* arg0 = work; // match-only: forces `mr r3, r31` before the idx branch (schedule)
#endif
            s32 idx = 3;
            if (work->badgeTab == 0) {
                idx = 2;
            }
#ifdef MUST_MATCH
            winSortEntry((WindowWork*)arg0, idx, -310.0f, 150.0f);
#else
            winSortEntry(work, idx, -310.0f, 150.0f);
#endif
            work->savedState = work->badgeState;
            psndSFXOn((const char*)0x20012);
            work->badgeState = 1000;
        }
        work->targetX = -270.0f;
        work->targetY = (f32)(0x7D - work->badgeTab * 0x28);
        winMsgEntry(work, 0, work->badgeTab == 0 ? "msg_menu_badge_all" : "msg_menu_badge_equip", 0);
        goto end;

    case 10:
        if (work->badgeTab == 0) {
            count = pouchGetHaveBadgeCnt();
        } else {
            count = work->equipListCount;
        }
        sel = work->badgeTab;
        {
            s32 rows = (count + 7) / 8;

            if (work->buttonTrg & 0x100) {
                if (sel == 0) {
                    s32 idx = work->badgeSel[sel];
                    if (pouchEquipCheckBadgeIndex(idx)) {
                        psndSFXOn((const char*)0x20039);
                        pouchUnEquipBadgeIndex(idx);
                    } else {
                        if (pouch->availableBP >= itemDataTable[pouchHaveBadge(idx)].bp_cost) {
                            psndSFXOn((const char*)0x20038);
                            pouchEquipBadgeIndex(idx);
                        } else {
                            psndSFXOn((const char*)0x20014);
                        }
                    }
                    pouchReviseMarioParam();
                    pouchRevisePartyParam();
                    mario_change(work, pouchHaveBadge(idx));
                } else {
                    s32* selPtr = (s32*)((u8*)work + 0x3E4) + sel;
                    s32 entryIdx = selPtr[0];
                    s32 idx = ((s32*)((u8*)work))[entryIdx * 3 + 0x102];
                    if (((s32*)((u8*)work))[entryIdx * 3 + 0x101] != 0) {
                        psndSFXOn((const char*)0x20039);
                        pouchUnEquipBadgeIndex(idx);
                        ((s32*)((u8*)work + 0x404))[selPtr[0] * 3] = 0;
                    } else {
                        if (pouch->availableBP >= itemDataTable[pouchHaveBadge(idx)].bp_cost) {
                            psndSFXOn((const char*)0x20038);
                            pouchEquipBadgeIndex(idx);
                            ((s32*)((u8*)work + 0x404))[selPtr[0] * 3] = 1;
                        }
                    }
                    pouchReviseMarioParam();
                    pouchRevisePartyParam();
                    mario_change(work, pouchHaveBadge(idx));
                }
            } else if (work->buttonTrg & 0x200) {
                psndSFXOn((const char*)0x20013);
                work->badgeState = 0;
            } else if (work->buttonTrg & 0x1000) {
                return -2;
            } else if (work->buttonRep & 0x40) {
                ((s32*)((u8*)work + 0x3EC))[sel] = ((s32*)((u8*)work + 0x3EC))[sel] - 1;
                if (((s32*)((u8*)work + 0x3EC))[sel] < 0) {
                    ((s32*)((u8*)work + 0x3EC))[sel] = 0;
                }
                work->badgeSel[sel] = ((s32*)((u8*)work + 0x3EC))[sel] * 8;
                psndSFXOn((const char*)0x20035);
            } else if (work->buttonRep & 0x20) {
                ((s32*)((u8*)work + 0x3EC))[sel] = ((s32*)((u8*)work + 0x3EC))[sel] + 1;
                if (((s32*)((u8*)work + 0x3EC))[sel] >= rows) {
                    ((s32*)((u8*)work + 0x3EC))[sel] = rows - 1;
                }
                work->badgeSel[sel] = ((s32*)((u8*)work + 0x3EC))[sel] * 8;
                psndSFXOn((const char*)0x20035);
            } else if (work->dirRep & 0x1000) {
                work->badgeSel[sel] = work->badgeSel[sel] - 1;
                psndSFXOn((const char*)0x20035);
            } else if (work->dirRep & 0x2000) {
                work->badgeSel[sel] = work->badgeSel[sel] + 1;
                psndSFXOn((const char*)0x20035);
            } else if (work->buttonTrg & 0x10) {
#ifdef MUST_MATCH
                void* arg0 = work; // match-only: forces `mr r3, r31` before the idx branch (schedule)
#endif
                s32 idx = 3;
                if (sel == 0) {
                    idx = 2;
                }
#ifdef MUST_MATCH
                winSortEntry((WindowWork*)arg0, idx, -310.0f, 150.0f);
#else
                winSortEntry(work, idx, -310.0f, 150.0f);
#endif
                work->savedState = work->badgeState;
                psndSFXOn((const char*)0x20012);
                work->badgeState = 1000;
            }

            if (work->badgeSel[sel] < 0) {
                work->badgeSel[sel] = count - 1;
            }
            if (work->badgeSel[sel] >= count) {
                work->badgeSel[sel] = 0;
            }
            ((s32*)((u8*)work + 0x3EC))[sel] = work->badgeSel[sel] / 8;
            work->targetX = -110.0f;
            work->targetY = (f32)(0x6E - (work->badgeSel[sel] % 8) * 0x17);
            {
                s32 msgId;
                if (work->badgeTab == 0) {
                    msgId = pouchHaveBadge(work->badgeSel[sel]);
                } else {
                    msgId = ((s32*)((u8*)work + 0x40C))[work->badgeSel[sel] * 3];
                }
                winMsgEntry(work, msgId, itemDataTable[msgId].menu_description, 0);
            }
        }
        goto end;

    case 1000:
        if (!winSortWait(work)) {
            work->badgeState = work->savedState;
        }
        if (work->buttonTrg & 0x1000) {
            return -2;
        }
        break;
    }

end:
    return 0;
}

// 136 B
void winBadgeMain2(WindowWork* work) {
    work->badgeScrollTarget[0] = 184.0f * (f32)work->badgeScrollRow[0];
    work->badgeScrollPos[0] += (work->badgeScrollTarget[0] - work->badgeScrollPos[0]) / 4.0f;
    work->badgeScrollTarget[1] = 184.0f * (f32)work->badgeScrollRow[1];
    work->badgeScrollPos[1] += (work->badgeScrollTarget[1] - work->badgeScrollPos[1]) / 4.0f;
}

// The window draw functions take their Vec and GXColor arguments by value. win_main.h declares the font and
// icon ones with pointers, which compiles to the same callee.
#ifdef MUST_MATCH
// Match-only: called through by-value casts (caller copies plus a pointer, ABI-identical).
#define winFontSetV ((void (*)(Vec, Vec, GXColor, const char*, ...))winFontSet)
#define winFontSetRV ((void (*)(Vec, Vec, GXColor, const char*, ...))winFontSetR)
#define winIconSetV ((void (*)(s32, Vec, Vec, GXColor))winIconSet)
#else
static void winBadgeFontV(void (*fn)(Vec*, Vec*, GXColor*, const char*, ...), Vec pos, Vec scale,
                          GXColor color, const char* format, va_list args) {
    char text[0x200];

    vsprintf(text, format, args);
    fn(&pos, &scale, &color, "%s", text);
}
static void winFontSetV(Vec pos, Vec scale, GXColor color, const char* format, ...) {
    va_list args;

    va_start(args, format);
    winBadgeFontV(winFontSet, pos, scale, color, format, args);
    va_end(args);
}
static void winFontSetRV(Vec pos, Vec scale, GXColor color, const char* format, ...) {
    va_list args;

    va_start(args, format);
    winBadgeFontV(winFontSetR, pos, scale, color, format, args);
    va_end(args);
}
static inline void winIconSetV(s32 id, Vec pos, Vec scale, GXColor color) {
    winIconSet(id, &pos, &scale, &color);
}
#endif

#define WIN_BADGE_TEX(w) (**(TEXPalette***)(*(u8**)((u8*)(w) + 0x28) + 0xA0))

// Draws the badge screen: the "all"/"equipped" tabs, the BP panel with one pip per BP, then the list.
void winBadgeDisp(CameraId camId, void* param, s32 idx) {
    WindowWork* work = param;
    GXColor white = dat_8041d348;
    GXColor color = dat_8041d34c;
    PouchData* pouch = pouchGetPtr();
    f32 tabX;
    f32 tabY;
    f32 tabTop;
    f32 x = WW_F32((u8*)work + idx * 0x14, 0xC4);
    s32 i;
    f32 y = WW_F32((u8*)work + idx * 0x14, 0xC8);
    (void)camId;

    winBgGX(work, 3, x, y);
    winKirinukiGX(work, 0, x - 100.0f, 20.0f + (120.0f + y), 365.0f, 210.0f);

    tabTop = 105.0f + y;
    tabX = (x - 190.0f) - 4.0f;
    tabY = 4.0f + tabTop;

    for (i = 0; i < 2; i++) {
        if (i == work->badgeTab) {
            winTexInit(WIN_BADGE_TEX(work));
            winTexSet(0xB4, (Vec){x - 190.0f, 20.0f + (tabTop - (f32)(i * 0x28)), 0.0f}, (Vec){1.0f, 1.0f, 1.0f},
                      dat_8041d350);
            winTexInit_x2(WIN_BADGE_TEX(work));
            winTexSet_x2(i + 0x1A, 0xB4, (Vec){tabX, 20.0f + (tabY - (f32)(i * 0x28)), 0.0f}, (Vec){1.0f, 1.0f, 1.0f},
                         white);
        } else {
            winTexInit_x2(WIN_BADGE_TEX(work));
            winTexSet_x2(i + 0x1A, 0xB4, (Vec){x - 190.0f, 20.0f + (tabTop - (f32)(i * 0x28)), 0.0f},
                         (Vec){1.0f, 1.0f, 1.0f}, dat_8041d354);
        }
    }

    winKirinukiGX(work, 1, x - 260.0f, 20.0f + (40.0f + y), 150.0f,
                  (f32)(0x9E - (10 - (pouch->maximumBP + 9) / 10) * 0xD));

    winFontInit();
    winFontSetV((Vec){x - 220.0f, (20.0f + (40.0f + y)) - 4.0f, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, color, "\202a\202o");
    winFontSetRV((Vec){(x - 175.0f) - 18.0f, (20.0f + (40.0f + y)) - 4.0f, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, color, "%s",
                 winZenkakuStr(pouch->maximumBP));
    winFontSetV((Vec){x - 180.0f,
                      (20.0f + (y - 108.0f) - 18.0f) + (f32)((10 - (pouch->maximumBP + 9) / 10) * 0xD), 0.0f},
                (Vec){0.5f, 0.5f, 0.5f}, color, msgSearch("msg_menu_badge_nokori"));
    winFontSetRV((Vec){(x - 145.0f) - 16.0f,
                       (20.0f + (y - 102.0f) - 18.0f) + (f32)((10 - (pouch->maximumBP + 9) / 10) * 0xD), 0.0f},
                 (Vec){0.8f, 0.8f, 0.8f}, color, "%s", winZenkakuStr(pouch->availableBP));

    // one pip per BP, ten to a row: the available ones, then the used ones
    winTexInit(WIN_BADGE_TEX(work));
    {
        f32 ry = 28.0f + y;
        f32 rx = x - 242.0f;

        for (i = 0; i < pouch->availableBP; i++) {
            winTexSet(WIN_TEX_BP_ON, (Vec){rx + (f32)((i % 10) * 0xD), 20.0f + (ry - (f32)((i / 10) * 0xD)) - 18.0f, 0.0f},
                      (Vec){1.0f, 1.0f, 1.0f}, white);
        }
        for (i = pouch->availableBP; i < pouch->maximumBP; i++) {
            winTexSet(WIN_TEX_BP_OFF,
                      (Vec){(x - 242.0f) + (f32)((i % 10) * 0xD), 20.0f + (ry - (f32)((i / 10) * 0xD)) - 18.0f, 0.0f},
                      (Vec){1.0f, 1.0f, 1.0f}, white);
        }
    }
    badge_disp(work, x, y);
}

// The badge list: per row the equipped marker, icon, name, BP cost and one pip per BP, then the scroll
// arrows and the sort / BP-needed captions.
void badge_disp(WindowWork* work, f32 x, f32 y) {
    GXColor iconColor = dat_8041d358;
    GXColor color;
    GXColor black;
    GXColor white;
    f32 rowY;
    f32 markX;
    f32 costX;
    f32 pipX;
    f32 drawY;
    f32 top;
    f32 scroll;
    u8* entry;
    s32 ofs;
    s32 i;
    s32 gray;
    s32 count;

    if (work->badgeTab == 0) {
        count = pouchGetHaveBadgeCnt();
    } else {
        count = work->equipListCount;
    }

    GXSetScissor(0, 0x77, 0x260, 0xB8);
    top = 110.0f + y;
    markX = 82.0f + x;
    costX = 142.0f + x;
    pipX = 160.0f + x;

    for (i = 0, ofs = 0, entry = (u8*)work; i < count; entry += 0xC, ofs += 0x17, i++) {
        s32 badge;
        s32 equipped;
        f32 textY;

        if (work->badgeTab == 0) {
            badge = pouchHaveBadge(i);
        } else {
            badge = *(s32*)(entry + 0x40C);
        }
        scroll = WW_F32X(work, work->badgeTab, 0x3F4);
        if (work->badgeTab == 0) {
            equipped = pouchEquipCheckBadgeIndex(i);
        } else {
            equipped = *(s32*)(entry + 0x404);
        }
        rowY = top - (f32)ofs;
        drawY = scroll + rowY;
        if (drawY > 240.0f) {
            continue;
        }
        if (drawY < -240.0f) {
            break;
        }

        if (equipped) {
            winTexInit(WIN_BADGE_TEX(work));
            winTexSet(0x1E, (Vec){markX, drawY, 0.0f}, (Vec){44.0f, 1.0f, 1.0f}, dat_8041d35c);
        }

        if (equipped == 0 && itemDataTable[badge].bp_cost > pouchGetPtr()->availableBP) {
            gray = 1;
            color = dat_8041d360;
        } else {
            gray = 0;
            color = dat_8041d364;
        }
        if (!gray) {
            winIconInit();
        } else {
            winIconGrayInit();
        }
        winIconSetV(*(u16*)&itemDataTable[badge].icon_id, (Vec){x - 75.0f, drawY, 0.0f}, (Vec){0.54f, 0.54f, 0.54f},
                    iconColor);
        winFontInit();
        winFontSetV((Vec){x - 55.0f, textY = scroll + (12.0f + rowY), 0.0f}, (Vec){0.9f, 0.9f, 0.9f}, color, itemDataTable[badge].id);
        {
            char buf[0x40];

            sprintf(buf, "%s", winZenkakuStr(itemDataTable[badge].bp_cost));
            winFontSetV((Vec){costX - (f32)((u16)FontGetMessageWidth(buf) / 2), textY, 0.0f}, (Vec){0.9f, 0.9f, 0.9f},
                        color, buf);
        }
        winTexInit(WIN_BADGE_TEX(work));
        {
            s32 j;

            for (j = 0; j < itemDataTable[badge].bp_cost; j++) {
                winTexSet(equipped ? WIN_TEX_BP_ON : WIN_TEX_BP_OFF, (Vec){pipX + (f32)(j * 0xD), drawY, 0.0f}, (Vec){1.0f, 1.0f, 1.0f},
                          iconColor);
            }
        }
    }

    GXSetScissor(0, 0, 0x260, 0x1E0);
    white = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
    black = (GXColor){ 0x00, 0x00, 0x00, 0xFF };
    if (work->badgeScrollRow[work->badgeTab] > 0) {
        f32 arrowX;

        winTexInit(WIN_BADGE_TEX(work));
        arrowX = 250.0f + x;
        winTexSet(0x17, (Vec){arrowX, 20.0f + (18.0f + top), 0.0f}, (Vec){-1.0f, -1.0f, -1.0f}, white);
        winIconInit();
        winIconSetV(0x7E, (Vec){arrowX, 20.0f + (113.0f + y), 0.0f}, (Vec){0.6f, 0.6f, 0.6f}, white);
    }
    if (count > (work->badgeScrollRow[work->badgeTab] << 3) + 8) {
        f32 arrowX;

        winTexInit(WIN_BADGE_TEX(work));
        arrowX = 250.0f + x;
        winTexSet(0x17, (Vec){arrowX, 20.0f + ((y - 90.0f) - 15.0f), 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, white);
        winIconInit();
        winIconSetV(0x80, (Vec){arrowX, 20.0f + (y - 90.0f), 0.0f}, (Vec){0.6f, 0.6f, 0.6f}, white);
    }
    {
        f32 w = 0.7f * (f32)FontGetMessageWidth(msgSearch("msg_menu_sort_narabikae"));
        f32 sortX;

        winFontInit();
        sortX = x + (220.0f - w);
        winFontSetV((Vec){sortX, 20.0f + ((y - 92.0f) - 10.0f), 0.0f}, (Vec){0.7f, 0.7f, 0.7f}, black,
                    msgSearch("msg_menu_sort_narabikae"));
        winIconInit();
        winIconSetV(0x217, (Vec){sortX - 30.0f, 20.0f + ((y - 100.0f) - 10.0f), 0.0f}, (Vec){0.6f, 0.6f, 0.6f}, white);
    }
    winFontInit();
    winFontSetV((Vec){120.0f + x, 135.0f + y, 0.0f}, (Vec){0.5f, 0.5f, 0.5f}, black, msgSearch("msg_menu_badge_hituyou"));
}
