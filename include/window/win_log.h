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
    f32 e28; // 0xE28
    f32 e2c; // 0xE2C
    f32 e30; // 0xE30
    f32 e34; // 0xE34
    f32 mapScrollX; // 0xE38
    f32 mapScrollY; // 0xE3C
    s32 stoneNum; // 0xE40
    EffectEntry* stones[7]; // 0xE44
    s32 stoneSel; // 0xE60
    f32 e64; // 0xE64
    f32 e68; // 0xE68
    f32 e6c; // 0xE6C
    f32 e70; // 0xE70
    f32 e74; // 0xE74
    f32 e78; // 0xE78
    f32 e7c; // 0xE7C
    f32 e80; // 0xE80
    f32 e84; // 0xE84
    f32 e88; // 0xE88
    f32 e8c; // 0xE8C
    f32 e90; // 0xE90
    s32 cardCnt; // 0xE94
    s32 cardSel; // 0xE98
    s32 cardPage; // 0xE9C
    f32 ea0; // 0xEA0
    f32 ea4; // 0xEA4
    f32 ea8; // 0xEA8
    f32 eac; // 0xEAC
    f32 eb0; // 0xEB0
    f32 eb4; // 0xEB4
    s16 cardIcon[100]; // 0xEB8
    f32 f80; // 0xF80
    f32 f84; // 0xF84
    f32 f88; // 0xF88
    f32 f8c; // 0xF8C
    s32 badgeCnt; // 0xF90
    s32 badgeSel; // 0xF94
    s32 badgePage; // 0xF98
    f32 f9c; // 0xF9C
    f32 fa0; // 0xFA0
    f32 fa4; // 0xFA4
    f32 fa8; // 0xFA8
    f32 fac; // 0xFAC
    f32 fb0; // 0xFB0
    s16 badgeIcon[58]; // 0xFB4
    f32 x1028; // 0x1028
    f32 x102c; // 0x102C
    f32 x1030; // 0x1030
    f32 x1034; // 0x1034
    s32 charCnt; // 0x1038
    s32 seenCnt; // 0x103C
    s32 charSel; // 0x1040
    s32 charPage; // 0x1044
    f32 x1048; // 0x1048
    f32 x104c; // 0x104C
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
