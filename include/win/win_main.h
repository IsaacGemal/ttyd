#pragma once

#include <dolphin/types.h>
#include "evt/evt_cmd.h"
#include "sdk/texPalette.h"

// The pause menu's work area. Each submenu keeps its state in its own band, laid out one after another
// rather than overlaid: root 0x1C-0x184, win_mario 0x160-0x198, win_party 0x19C-0x208, win_item
// 0x20C-0x3DA (plus 0x1208/0x120C), win_badge 0x3DC-0xD64, win_log 0xD68-0x1204. win_root.c and
// win_log.c also view it through their own overlay structs (WinRoot, LogWin).
typedef struct WindowWork {
    u16 flags;                  // 0x0, 0x2000 = famicom (8-bit) Mario
    u8 align_2[2];              // 0x2
    u32 buttonTrg;              // 0x4
    u32 buttonRep;              // 0x8
    u32 dirTrg;                 // 0xC
    u32 dirRep;                 // 0x10
    s32 lectureStep;            // 0x14, win_main winLecture*: step counter
    EventEntry* lectureEvent;   // 0x18, win_main winLectureOn: the evt_lecture_msg event
    s32 openTabId;              // 0x1C, winMain: root tab id the menu opens on (0 = root, 1-4 = shortcut buttons)
    s32 menuState;              // 0x20, winMain state machine
    s32 rootState;              // 0x24, win_root state machine
    struct FileEntry* winTpl;   // 0x28, w/<lang>/win.tpl
    struct FileEntry* mailTpl;  // 0x2C, win_item: mail texture file
    struct FileEntry* mapTpl;   // 0x30, win_log: map texture file
    struct smartEntry* fbCopy;  // 0x34, win_root: framebuffer copy drawn behind the menu
    s32 fbCopyReady;            // 0x38, win_root: set once fbCopy holds a frame
    EventEntry* currentEvent;   // 0x3C, winMain: the evt_item_use / evt_diary_read event it waits on
    // win_root's tab selection (0x40), tab count (0x44), WinRootTab[5] (0x48) and WinRootRec[5] (0xC0) live
    // here. sort_7_2_func/sort_7_3_func address the tattle list at 0x1050 as &field_40[0x1010], and only
    // that u8-array base matches, so the array has to keep starting at 0x40.
    u8 field_40[0x124 - 0x40];  // 0x40
    s32 savedState;             // 0x124, win_badge/win_item/win_log: menu state to return to after help text
    u8 field_128[0x150 - 0x128]; // 0x128
    f32 posX;                   // 0x150, cursor position, animates toward targetX (win_root, win_item, win_badge)
    f32 posY;                   // 0x154, animates toward targetY
    f32 targetX;                // 0x158, cursor target, set by every submenu
    f32 targetY;                // 0x15C
    s32 marioLinkIdx;           // 0x160, win_mario: current linkDt entry (the cursor on the Mario page)
    u8 field_164[0x180 - 0x164]; // 0x164
    s32 sortReversed;           // 0x180, win_root's item sort: 0 = ascending pouchSortItem types, 1 = the _r ones (toggled by each sort)
    u8 field_184[0x188 - 0x184]; // 0x184
    s32 marioPose;              // 0x188, win_mario's Mario pose (mario_change rebuilds it; mapGX reads its anim)
    s32 marioHoldTimer;         // 0x18C, win_mario: frames left before the pose's anim is driven again (30 after a loop)
    s32 marioState;             // 0x190, win_mario: 0 = link cursor, 1 = super action cursor
    s32 superActionSel;         // 0x194, win_mario: cursor among the owned super actions
    s32 superActionCount;       // 0x198, win_mario: star powers owned (bit count of mStarPowersObtained)
    u8 field_19C[0x1A0 - 0x19C]; // 0x19C
    s32 partyPose[7];           // 0x1A0, win_party: pose handle per party slot
    s32 partyId[7];             // 0x1BC, win_party: party member id per slot (PARTNER_* order)
    s32 selPartyDt;             // 0x1D8, win_party: index into winPartyDt for the selected slot
    s32 partySel;               // 0x1DC, win_party: selected slot
    s32 partyCount;             // 0x1E0, partners joined (winPartyInit counts them; win_root, win_item read it)
    f32 partyAngle;             // 0x1E4, win_party: current ring angle
    f32 partyAngleTarget;       // 0x1E8
    u8 field_1EC[0x1FC - 0x1EC]; // 0x1EC
    s32 lastPartySel;           // 0x1FC, win_party: slot before the last move (win_root compares it)
    s32 partyState;             // 0x200, win_party: state machine switch
    s32 wazaCount;              // 0x204, win_party: moves shown (attack level + 2)
    s32 wazaSel;                // 0x208, win_party: cursor among the moves
    s32 itemState;              // 0x20C, win_item state machine switch
    s32 itemTab;                // 0x210, win_item: 0 = held items (STR_MOCHI_ITEM), 1 = key items (STR_MOCHI_DAIJI)
    s32 itemSel[2];             // 0x214, win_item: cursor per tab (win_root's sort_1_* and sort_2_* read [0] and [1])
    s32 itemScrollRow[2];       // 0x21C, win_item: first visible row per tab
    u8 field_224[0x254 - 0x224]; // 0x224, win_item's float positions (book and list slide)
    s32 mailCount;              // 0x254, received mail (pouchReceiveMailCount)
    s32 mailSel;                // 0x258, mail cursor
    s32 mailPage;               // 0x25C, mailSel / 5
    u8 field_260[0x268 - 0x260]; // 0x260, f32 positions
    s32 mailScroll;             // 0x268, first visible line of the open mail
    u16 mailLines;              // 0x26C, line count of the open mail (FontGetMessageWidthLine, +10 with a picture)
    u8 field_26E[0x2D4 - 0x26E]; // 0x26E, mail index pairs (sort base), Luigi book state at 0x2C8-0x2D0
    s32 useItemId;              // 0x2D4, item picked for use (win_item case 300; win_main hands it to the event)
    s32 useItemSlot;            // 0x2D8, its pouch index (pouchRemoveItemIndex)
    s32 useTargetSel;           // 0x2DC, target: 0 = Mario, else index into the joined partners
    s32 useTimer;               // 0x2E0, frames since the item was used (removed after framerate * 2)
    u8 field_2E4[0x3DA - 0x2E4]; // 0x2E4, key-item skip list (u16[0x79] at 0x2E8, see ItemWorkTail)
    s16 skipCount;              // 0x3DA, entries in that list
    s32 badgeState;             // 0x3DC, win_badge: state machine switch
    s32 badgeTab;               // 0x3E0, win_badge: 0 = all badges, 1 = equipped
    s32 badgeSel[2];            // 0x3E4, win_badge: cursor per tab
    s32 badgeScrollRow[2];      // 0x3EC, win_badge: first visible row per tab
    f32 badgeScrollPos[2];      // 0x3F4, animates toward badgeScrollTarget
    f32 badgeScrollTarget[2];   // 0x3FC
    u8 field_404[0xD64 - 0x404]; // 0x404, equipped-badge list, 0xC bytes per entry (win_root sorts it)
    s32 equipListCount;         // 0xD64
    u8 field_D68[0xE1C - 0xD68]; // 0xD68
    s32 mapMarioPose;           // 0xE1C, win_log map's Mario pose (LogWin.pose; mario_change rebuilds it)
    u8 field_E20[0x1208 - 0xE20]; // 0xE20
    s32 useTargetWin[2];        // 0x1208, win_item: winMgr windows opened to pick who an item is used on
} WindowWork;

typedef struct WindowPartyDt {
    s32 partnerId;            // 0x0, PARTNER_* id (win_party, win_item)
    s32 iconId;               // 0x4, icon drawn beside the partner (win_item winIconSetV)
    const char* nameMsg;      // 0x8, msg key for the partner's name
    const char* descMsg;      // 0xC, msg key for the partner's description
    const char* model;        // 0x10, animPose model shown in win_party ("EFF_m_kuriko", "c_vivian", ...)
    const char* anim;         // 0x14, its idle animation ("W_1", Vivian "PTR_S_1")
    const char* famicomModel; // 0x18, model used when flags & 0x2000 ("d_mario")
    const char* famicomAnim;  // 0x1C, its animation ("R_2", ...)
    const char* waza;         // 0x20, the partner's four moves (u32 partyWeapon pointers)
} WindowPartyDt;

WindowWork* winGetPtr(void);
void winInit(void);
void winReInit(void);
BOOL winCheck(void);
void winOpenEnable(void);
void winOpenDisable(void);
USER_FUNC(itemUseFunc);

void winMain(void);

void winTexInit(TEXPalette* texture);
void winTexSet(u32 textureId, Vec translate, Vec scale, GXColor color);
void winIconInit(void);
void winFontInit(void);
void winFontSet(Vec* pos, Vec* scale, GXColor* color, const char* format, ...);
void winFontSetLabel(Vec* pos, Vec* scale, GXColor* color, const char* tag);
void winFontSetEdge(Vec* pos, Vec* scale, GXColor* color, const char* format, ...);
void winFontSetR(Vec* pos, Vec* scale, GXColor* color, const char* format, ...);
void winIconSet(s32 iconId, Vec* translate, Vec* scale, GXColor* color);
void winTexInit_x2(TEXPalette* texture);
void unk_80177008(u32 textureId, Vec translate, Vec scale, GXColor color);
void winTexSet_x2(u32 textureId, u32 textureId2, Vec translate, Vec scale, GXColor color);
void winTexSetRot(u32 textureId, Vec translate, Vec scale, f32 rot, GXColor color);
