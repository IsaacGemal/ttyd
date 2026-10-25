#pragma once

#include <dolphin/types.h>
#include "battle/battle_unit.h"

// Tattle ("monosiri") viewer window (win_log in the original; no source file yet).
void* monosiriInit(BattleUnitType kind, f32 x, f32 y);
void monosiriMain(void* work);
void monosiriClose(void* work);
BOOL monosiriCloseWait(void* work);
void monosiriExit(void* work);
#include "driver/dispdrv.h"
void monosiri_disp(CameraId camId, void* param);
void capture(CameraId camId, void* param);
typedef struct WindowWork WindowWork;

typedef struct FileEntry FileEntry;
typedef struct EffectEntry EffectEntry;

typedef struct LogRow {
    s32 type;         // 0x00
    f32 curX;         // 0x04
    f32 curY;         // 0x08
    f32 targetX;      // 0x0C
    f32 targetY;      // 0x10
    s32 phase;        // 0x14
    s32 timer;        // 0x18
    const char* name; // 0x1C
} LogRow;
typedef struct LogChar {
    u8 pad;
    u8 id;
} LogChar;
// win_log's view of WindowWork: the shared header fields it uses, then the log band at 0xD68-0x1204.
// win_root's sorts and book pages read the card, badge and tattle counts here too.
typedef struct LogWin {
    u16 flags; // 0x0
    u8 pad_2[0x2];
    u32 buttonTrg; // 0x4
    u32 buttonRep; // 0x8
    u32 dirTrg; // 0xC
    u32 dirRep; // 0x10
    u8 pad_14[0x1C];
    FileEntry* mapTpl; // 0x30
    u8 pad_34[0xF0];
    s32 savedState; // 0x124
    u8 pad_128[0x30];
    f32 targetX; // 0x158
    f32 targetY; // 0x15C
    u8 pad_160[0xC08];
    s32 state; // 0xD68
    s32 sel; // 0xD6C
    s32 count; // 0xD70
    LogRow rows[5]; // 0xD74
    s32 mapSel; // 0xE14
    s32 mapPick; // 0xE18
    s32 mapMarioPose; // 0xE1C
    f32 mapCx; // 0xE20
    f32 mapCy; // 0xE24
    f32 mapTargetX; // 0xE28, mapGX's position (win_log's Disp), easing towards the target
    f32 mapX; // 0xE2C
    f32 mapTargetY; // 0xE30
    f32 mapY; // 0xE34
    f32 mapScrollX; // 0xE38
    f32 mapScrollY; // 0xE3C
    s32 stoneNum; // 0xE40
    EffectEntry* stones[7]; // 0xE44
    s32 stoneSel; // 0xE60
    f32 stoneTargetX; // 0xE64, the crystal star ring's position
    f32 stoneX; // 0xE68
    f32 stoneTargetY; // 0xE6C
    f32 stoneY; // 0xE70
    f32 stoneTargetAlpha; // 0xE74, the ring backdrop's alpha
    f32 stoneAlpha; // 0xE78
    f32 stoneTargetAngle; // 0xE7C, the ring's rotation, 360/7 degrees per stoneSel
    f32 stoneAngle; // 0xE80
    f32 cardX; // 0xE84, the card window (winHakoGX 0), easing towards its target
    f32 cardTargetX; // 0xE88
    f32 cardY; // 0xE8C
    f32 cardTargetY; // 0xE90
    s32 cardCnt; // 0xE94
    s32 cardSel; // 0xE98
    s32 cardPage; // 0xE9C
    f32 cardScrollPos; // 0xEA0, card list scroll, cardPage * 0xE8 (win_root's book pages read it)
    f32 cardScrollTarget; // 0xEA4
    f32 cardIconX; // 0xEA8, the selected card's picture frame (texture 0x95) and icon
    f32 cardIconTargetX; // 0xEAC
    f32 cardIconY; // 0xEB0
    f32 cardIconTargetY; // 0xEB4
    s16 cardIcon[100]; // 0xEB8
    f32 badgeX; // 0xF80, the badge window (winHakoGX 1)
    f32 badgeTargetX; // 0xF84
    f32 badgeY; // 0xF88
    f32 badgeTargetY; // 0xF8C
    s32 badgeCnt; // 0xF90
    s32 badgeSel; // 0xF94
    s32 badgePage; // 0xF98
    f32 badgeScrollPos; // 0xF9C, badge list scroll, badgePage * 0xE8
    f32 badgeScrollTarget; // 0xFA0
    f32 badgeIconX; // 0xFA4, the selected badge's picture frame (texture 0x96) and icon
    f32 badgeIconTargetX; // 0xFA8
    f32 badgeIconY; // 0xFAC
    f32 badgeIconTargetY; // 0xFB0
    s16 badgeIcon[58]; // 0xFB4
    f32 field_0x1028; // 0x1028
    f32 field_0x102C; // 0x102C
    f32 field_0x1030; // 0x1030
    f32 field_0x1034; // 0x1034
    s32 charCnt; // 0x1038
    s32 seenCnt; // 0x103C
    s32 charSel; // 0x1040
    s32 charPage; // 0x1044
    f32 field_0x1048; // 0x1048
    f32 field_0x104C; // 0x104C
    LogChar chars[216]; // 0x1050
    void* mono; // 0x1200
    u8 pad_1204[0xC];
} LogWin;

void monoshiriGX(WindowWork* work, f32 x, f32 y);
void winLogDisp(CameraId camId, WindowWork* work, s32 idx);
void mapGX(WindowWork* work, f32 x, f32 y);
const char* winGetMapTplName(void);

void winLogMain2(WindowWork* work);
s32 winLogMain(WindowWork* work);
void winLogExit(WindowWork* work);
void winLogInit2(WindowWork* work);
void winLogInit(WindowWork* work);

// The win_log unit's read-only rodata base. Match-only non-const declaration: it stops MWCC hoisting the
// vec3 loads out of monoshiriGX's and winLogDisp's loops (the data is in fact read-only).
#ifdef MUST_MATCH
extern char str_msg_menu_kiroku_map[];
#else
extern const char str_msg_menu_kiroku_map[];
#endif

// The 4-byte "S_1" anim name in .sdata2, addressed with @sda21 by the original. Declared with its real size
// so MWCC treats it as small data; an unsized array is loaded with lis/addi instead.
extern const char str_8041D810[4];
