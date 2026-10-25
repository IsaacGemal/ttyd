#pragma once

#include <dolphin/types.h>
#include "evt/evt_cmd.h"
#include "sdk/texPalette.h"

// win_root's menu tabs (WindowWork.tabs).
typedef struct WinRootTab {
    s32 id;          // 0x0
    f32 x;           // 0x4
    f32 y;           // 0x8
    s32 state;       // 0xC
    s32 counter;     // 0x10
    const char* str; // 0x14
} WinRootTab;

// win_root's submenu window records (WindowWork.recs).
typedef struct WinRootRec {
    s32 a; // 0x0
    f32 b; // 0x4
    f32 c; // 0x8
    s32 d; // 0xC
    s32 e; // 0x10
} WinRootRec;

// win_badge's equipped-badge list entry (WindowWork.equipList), built by winMakeEquipList.
typedef struct WinEquipRec {
    s32 equipped; // 0x0, 1 = equipped (win_badge toggles it)
    s32 badgeIdx; // 0x4, pouch badge index (winMakeEquipList's i; pouchEquipBadgeIndex, pouchHaveBadge)
    s32 badgeId;  // 0x8, badge item id (itemDataTable index in win_badge and win_root's sorts)
} WinEquipRec;

// The pause menu's work area. Each submenu keeps its state in its own band, laid out one after another
// rather than overlaid: root 0x1C-0x184, win_mario 0x160-0x198, win_party 0x19C-0x208, win_item
// 0x20C-0x3DA (plus 0x1208/0x120C), win_badge 0x3DC-0xD64, win_log 0xD68-0x1204. win_log's band is
// declared in LogWin (window/win_log.h), an overlay of the whole struct that win_log and win_root use.
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
    s32 tabSel;                 // 0x40, win_root: index of the tab under the cursor
    s32 tabCount;               // 0x44, win_root: tabs in use
    WinRootTab tabs[5];         // 0x48
    WinRootRec recs[5];         // 0xC0, one per submenu window; b/c are its position (each submenu's Disp reads its own)
    s32 savedState;             // 0x124, win_badge/win_item/win_log: menu state to return to after help text
    f32 msgX;                   // 0x128, win_root: winMsgDisp's x (winMsgMain slides it in from -800)
    f32 msgY;                   // 0x12C, winMsgDisp's y
    s32 msgState;               // 0x130, win_root: winMsgMain state
    s32 msgStep;                // 0x134, winMsgMain: the slide's intplGetValue step (0..16)
    s32 msgItemId;              // 0x138, winMsgEntry's a1: > 0 an item whose icon winMsgDisp draws; -2 moves the name box left
    const char* msgTag;         // 0x13C, win_root: winMsgEntry's str, the message winMsgDisp shows
    const char* nameTag;        // 0x140, winMsgEntry's a3, a second message winMsgDisp draws in a winNameGX box (NULL for none)
    const char* lastMsgTag;     // 0x144, the last str winMsgEntry took (it skips a repeat)
    s32 msgLine;                // 0x148, first line shown; winMsgMain scrolls it two lines at a time
    s32 msgLines;               // 0x14C, line count from FontGetMessageWidthLine, plus one
    f32 posX;                   // 0x150, cursor position, animates toward targetX (win_root, win_item, win_badge)
    f32 posY;                   // 0x154, animates toward targetY
    f32 targetX;                // 0x158, cursor target, set by every submenu
    f32 targetY;                // 0x15C
    s32 marioLinkIdx;           // 0x160, win_mario: current linkDt entry (the cursor on the Mario page)
    f32 sortX;                  // 0x164, win_root's sort menu (winSortEntry, winSortMain, winSortGX): its position,
    f32 sortY;                  // 0x168  easing towards sortTargetX/Y
    f32 sortTargetX;            // 0x16C
    f32 sortTargetY;            // 0x170
    s32 sortSel;                // 0x174, cursor row
    s32 sortCount;              // 0x178, rows in sort_data[sortKind]
    s32 sortWait;               // 0x17C
    s32 sortReverseNext;        // 0x180, win_root's sorts: 0 = next press uses the ascending comparator, 1 = the _r one (flipped by each press, cleared by winSortEntry)
    s32 sortKind;               // 0x184, win_root: sort_data index
    s32 marioPose;              // 0x188, win_mario's Mario pose (mario_change rebuilds it; mapGX reads its anim)
    s32 marioHoldTimer;         // 0x18C, win_mario: frames left before the pose's anim is driven again (30 after a loop)
    s32 marioState;             // 0x190, win_mario: 0 = link cursor, 1 = super action cursor
    s32 superActionSel;         // 0x194, win_mario: cursor among the owned super actions
    s32 superActionCount;       // 0x198, win_mario: star powers owned (bit count of mStarPowersObtained)
    s32 partyChange;            // 0x19C, win_party: 1 when A closed the menu; win_root then returns -2 if partySel moved
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
    f32 itemScrollPos[2];       // 0x224, win_item: list scroll per tab (item_disp reads [itemTab])
    f32 itemScrollTarget[2];    // 0x22C, per tab: 190 * itemScrollRow, which itemScrollPos eases towards (winItemMain2)
    f32 bookX;                  // 0x234, the Luigi book window's position (winBookGX), easing towards bookTargetX/Y
    f32 bookTargetX;            // 0x238
    f32 bookY;                  // 0x23C
    f32 bookTargetY;            // 0x240
    f32 mailX;                  // 0x244, the mail window's position (winMailGX), easing towards mailTargetX/Y
    f32 mailTargetX;            // 0x248
    f32 mailY;                  // 0x24C
    f32 mailTargetY;            // 0x250
    s32 mailCount;              // 0x254, received mail (pouchReceiveMailCount)
    s32 mailSel;                // 0x258, mail cursor
    s32 mailPage;               // 0x25C, mailSel / 5
    f32 mailPagePos;            // 0x260, win_item: the mail list's scroll, easing towards mailPageTarget
    f32 mailPageTarget;         // 0x264, mailPage * 200
    s32 mailScroll;             // 0x268, first visible line of the open mail
    u16 mailLines;              // 0x26C, line count of the open mail (FontGetMessageWidthLine, +10 with a picture)
    u8 mailList[0x2C8 - 0x26E]; // 0x26E, byte pairs: display number, then mail id (pouch mEmailIds); win_root sorts it
    s32 bookId;                 // 0x2C8, win_item/win_root: the open Luigi book (luigi_book_list index)
    s32 bookPages;              // 0x2CC, its page count (two lines a page)
    s32 bookPage;               // 0x2D0, the page shown
    s32 useItemId;              // 0x2D4, item picked for use (win_item case 300; win_main hands it to the event)
    s32 useItemSlot;            // 0x2D8, its pouch index (pouchRemoveItemIndex)
    s32 useTargetSel;           // 0x2DC, target: 0 = Mario, else index into the joined partners
    s32 useTimer;               // 0x2E0, frames since the item was used (removed after framerate * 2)
    u8 field_2E4[0x2E8 - 0x2E4]; // 0x2E4
    s16 skipList[0x79];         // 0x2E8, win_item: key items shown (winMakeSkipList: those not in menu_skip_list)
    s16 skipCount;              // 0x3DA, entries in skipList
    s32 badgeState;             // 0x3DC, win_badge: state machine switch
    s32 badgeTab;               // 0x3E0, win_badge: 0 = all badges, 1 = equipped
    s32 badgeSel[2];            // 0x3E4, win_badge: cursor per tab
    s32 badgeScrollRow[2];      // 0x3EC, win_badge: first visible row per tab
    f32 badgeScrollPos[2];      // 0x3F4, animates toward badgeScrollTarget
    f32 badgeScrollTarget[2];   // 0x3FC
    WinEquipRec equipList[200]; // 0x404, winMakeEquipList; win_root sorts it
    s32 equipListCount;         // 0xD64
    u8 field_D68[0xE1C - 0xD68]; // 0xD68
    s32 mapMarioPose;           // 0xE1C, win_log map's Mario pose (LogWin.pose; mario_change rebuilds it)
    u8 field_E20[0x1050 - 0xE20]; // 0xE20
    u8 logChars[0x1200 - 0x1050]; // 0x1050, win_log's tattle list (LogWin.chars); win_root's sort_7_2/7_3 sort it
    u8 field_1200[0x1208 - 0x1200]; // 0x1200
    s32 useTargetWin[2];        // 0x1208, win_item: winMgr windows opened to pick who an item is used on
} WindowWork;

typedef struct WindowPartyDt {
    s32 partnerId;            // 0x0, PARTNER_* id (win_party, win_item)
    s32 iconId;               // 0x4, icon drawn beside the partner (win_item winIconSet)
    const char* nameMsg;      // 0x8, msg key for the partner's name
    const char* descMsg;      // 0xC, msg key for the partner's description
    const char* model;        // 0x10, animPose model shown in win_party ("EFF_m_kuriko", "c_vivian", ...)
    const char* anim;         // 0x14, its idle animation ("W_1", Vivian "PTR_S_1")
    const char* famicomModel; // 0x18, model used when flags & 0x2000 ("d_mario")
    const char* famicomAnim;  // 0x1C, its animation ("R_2", ...)
    struct BattleWeapon** waza; // 0x20, the partner's four moves
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
void winFontSet(Vec pos, Vec scale, GXColor color, const char* format, ...);
void winFontSetLabel(Vec pos, Vec scale, GXColor color, const char* tag);
void winFontSetEdge(Vec pos, Vec scale, GXColor color, const char* format, ...);
void winFontSetR(Vec pos, Vec scale, GXColor color, const char* format, ...);
void winIconSet(s32 iconId, Vec translate, Vec scale, GXColor color);
void winTexInit_x2(TEXPalette* texture);
void unk_80177008(u32 textureId, Vec translate, Vec scale, GXColor color);
void winTexSet_x2(u32 textureId, u32 textureId2, Vec translate, Vec scale, GXColor color);
void winTexSetRot(u32 textureId, Vec translate, Vec scale, f32 rot, GXColor color);
