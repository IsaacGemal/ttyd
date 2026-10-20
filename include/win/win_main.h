#pragma once

#include <dolphin/types.h>
#include "evt/evt_cmd.h"
#include "sdk/texPalette.h"

typedef struct WindowWork {
    u16 flags;                  // 0x0
    u8 align_2[2];              // 0x2
    u32 buttonTrg;              // 0x4
    u32 buttonRep;              // 0x8
    u32 dirTrg;                 // 0xC
    u32 dirRep;                 // 0x10
    u8 field_0x14[0x20 - 0x14]; // 0x14
    s32 menuState;              // 0x20, TODO: rename?
    u8 field_0x24[0x3C - 0x24]; // 0x24
    EventEntry* currentEvent;   // 0x3C
    u8 field_40[0x124 - 0x40];  // 0x40
    s32 savedState;             // 0x124, win_badge/win_item: menu state to return to
    u8 field_128[0x150 - 0x128]; // 0x128
    f32 posX;                   // 0x150, animates toward targetX (win_root, win_item, win_badge)
    f32 posY;                   // 0x154, animates toward targetY
    f32 targetX;                // 0x158
    f32 targetY;                // 0x15C
    u8 field_160[0x188 - 0x160]; // 0x160
    s32 poseId;                 // 0x188, pose handle (win_mario, win_badge, win_log)
    u8 field_18C[0x21C - 0x18C]; // 0x18C
    s32 itemScrollRow[2];       // 0x21C, win_item: first visible row per tab
    u8 field_224[0x3DC - 0x224]; // 0x224
    s32 badgeState;             // 0x3DC, win_badge: state machine switch
    s32 badgeTab;               // 0x3E0, win_badge: 0 = all badges, 1 = equipped
    s32 badgeSel[2];            // 0x3E4, win_badge: cursor per tab
    s32 badgeScrollRow[2];      // 0x3EC, win_badge: first visible row per tab
    f32 badgeScrollPos[2];      // 0x3F4, animates toward badgeScrollTarget
    f32 badgeScrollTarget[2];   // 0x3FC
    u8 field_404[0xD64 - 0x404]; // 0x404, equipped-badge list, 0xC bytes per entry (win_root sorts it)
    s32 equipListCount;         // 0xD64
    u8 field_D68[0xE1C - 0xD68]; // 0xD68
    s32 poseId2;                // 0xE1C, second pose handle (win_badge, win_log)
    u8 field_E20[0x1210 - 0xE20]; // 0xE20
} WindowWork;

typedef struct WindowPartyDt {
    s32 partnerId;          // 0x0, PARTNER_* id (win_party, win_item)
    s32 iconId;             // 0x4, icon drawn beside the partner (win_item winIconSetV)
    const char* nameMsg;    // 0x8, msg key for the partner's name
    const char* descMsg;    // 0xC, msg key for the partner's description
    const char* effModel;   // 0x10, effect model played with the partner
    const char* waitAnim;   // 0x14, idle animation ("W_1" for every partner)
    const char* field_0x18; // 0x18
    const char* field_0x1C; // 0x1C
    const char* waza;       // 0x20, the partner's four moves (u32 partyWeapon pointers)
} WindowPartyDt;            // TODO: rename

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
